/*
 * Copyright (c) 2017 FilipeCN
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <circe/gl/scene/bvh.h>

#include <algorithm>
#include <ponos/structures/bvh.h>
#include <vector>

namespace circe::gl {

BVH::BVH(SceneMeshObjectSPtr m) {
  sceneMesh = m;
  std::vector<BVHElement> buildData;
  for (size_t i = 0; i < sceneMesh->mesh()->rawMesh()->meshDescriptor.count;
       i++)
    buildData.emplace_back(
        BVHElement(i, sceneMesh->mesh()->rawMesh()->elementBBox(i)));
  uint32_t totalNodes = 0;
  orderedElements.reserve(sceneMesh->mesh()->rawMesh()->meshDescriptor.count);
  root = recursiveBuild(buildData, 0,
                        sceneMesh->mesh()->rawMesh()->meshDescriptor.count,
                        &totalNodes, orderedElements);
  nodes.resize(totalNodes);
  for (uint32_t i = 0; i < totalNodes; i++)
    new (&nodes[i]) LinearBVHNode;
  uint32_t offset = 0;
  flattenBVHTree(root, &offset);
}

BVH::BVHNode *BVH::recursiveBuild(std::vector<BVHElement> &buildData,
                                  uint32_t start, uint32_t end,
                                  uint32_t *totalNodes,
                                  std::vector<uint32_t> &orderedElements) {
  (*totalNodes)++;
  BVHNode *node = new BVHNode();
  ponos::bbox3 bbox;
  for (uint32_t i = start; i < end; ++i)
    bbox = ponos::make_union(bbox, buildData[i].bounds);
  // compute all bounds
  uint32_t nElements = end - start;
  if (nElements == 1) {
    // create leaf node
    uint32_t firstElementOffset = orderedElements.size();
    for (uint32_t i = start; i < end; i++) {
      uint32_t elementNum = buildData[i].ind;
      orderedElements.emplace_back(elementNum);
    }
    node->initLeaf(firstElementOffset, nElements, bbox);
  } else {
    // compute bound of primitives
    ponos::bbox3 centroidBounds;
    for (uint32_t i = start; i < end; i++)
      centroidBounds = ponos::make_union(centroidBounds, buildData[i].centroid);
    int dim = centroidBounds.maxExtent();
    // partition primitives
    uint32_t mid = (start + end) / 2;
    if (centroidBounds.upper[dim] == centroidBounds.lower[dim]) {
      node->initInterior(
          dim,
          recursiveBuild(buildData, start, mid, totalNodes, orderedElements),
          recursiveBuild(buildData, mid, end, totalNodes, orderedElements));
      return node;
    }
    // partition into equally sized subsets
    std::nth_element(&buildData[start], &buildData[mid],
                     &buildData[end - 1] + 1, Comparepoints(dim));
    node->initInterior(
        dim, recursiveBuild(buildData, start, mid, totalNodes, orderedElements),
        recursiveBuild(buildData, mid, end, totalNodes, orderedElements));
  }
  return node;
}

uint32_t BVH::flattenBVHTree(BVHNode *node, uint32_t *offset) {
  LinearBVHNode *linearNode = &nodes[*offset];
  linearNode->bounds = node->bounds;
  uint32_t myOffset = (*offset)++;
  if (node->nElements > 0) {
    linearNode->elementsOffset = node->firstElementOffset;
    linearNode->nElements = node->nElements;
  } else {
    linearNode->axis = node->splitAxis;
    linearNode->nElements = 0;
    flattenBVHTree(node->children[0], offset);
    linearNode->secondChildOffset = flattenBVHTree(node->children[1], offset);
  }
  return myOffset;
}

int BVH::intersect(const ponos::Ray3 &ray, float *t) {
  UNUSED_VARIABLE(t);
  if (!nodes.size())
    return false;
  ponos::Transform inv = ponos::inverse(sceneMesh->transform);
  ponos::Ray3 r = inv(ray);
  int hit = 0;
  ponos::vec3 invDir(1.f / r.d.x, 1.f / r.d.y, 1.f / r.d.z);
  uint32_t dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};
  uint32_t todoOffset = 0, nodeNum = 0;
  uint32_t todo[64];
  while (true) {
    LinearBVHNode *node = &nodes[nodeNum];
    if (intersect(node->bounds, r, invDir, dirIsNeg)) {
      if (node->nElements > 0) {
        // intersect ray with primitives
        for (uint32_t i = 0; i < node->nElements; i++) {
          ponos::point3 v0 = sceneMesh->mesh()->rawMesh()->positionElement(
              orderedElements[node->elementsOffset + i], 0);
          ponos::point3 v1 = sceneMesh->mesh()->rawMesh()->positionElement(
              orderedElements[node->elementsOffset + i], 1);
          ponos::point3 v2 = sceneMesh->mesh()->rawMesh()->positionElement(
              orderedElements[node->elementsOffset + i], 2);
          if (ponos::triangle_ray_intersection(v0, v1, v2, r))
            hit++;
        }
        if (todoOffset == 0)
          break;
        nodeNum = todo[--todoOffset];
      } else {
        if (dirIsNeg[node->axis]) {
          todo[todoOffset++] = nodeNum + 1;
          nodeNum = node->secondChildOffset;
        } else {
          todo[todoOffset++] = node->secondChildOffset;
          nodeNum++;
        }
      }
    } else {
      if (todoOffset == 0)
        break;
      nodeNum = todo[--todoOffset];
    }
  }
  return hit;
}

bool BVH::intersect(const ponos::bbox3 &bounds, const ponos::Ray3 &ray,
                    const ponos::vec3 &invDir,
                    const uint32_t dirIsNeg[3]) const {
  float hit1, hit2;
  return ponos::bbox_ray_intersection(bounds, ray, hit1, hit2);
  float tmin = (bounds[dirIsNeg[0]].x - ray.o.x) * invDir.x;
  float tmax = (bounds[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x;
  float tymin = (bounds[dirIsNeg[1]].y - ray.o.y) * invDir.y;
  float tymax = (bounds[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y;
  if ((tmin < tymax) || (tymin > tmax))
    return false;
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;
  float tzmin = (bounds[dirIsNeg[2]].z - ray.o.z) * invDir.z;
  float tzmax = (bounds[1 - dirIsNeg[2]].z - ray.o.z) * invDir.z;
  if ((tmin < tzmax) || (tzmin > tmax))
    return false;
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  return tmax > 0;
}

bool BVH::isInside(const ponos::point3 &p) {
  ponos::Ray3 r(p, ponos::vec3(1.2, 1.1, 0.1));
  ponos::Ray3 r2(p, ponos::vec3(0.2, -1.1, 0.1));

  return intersect(r, nullptr) % 2 && intersect(r2, nullptr) % 2;

  int hit = 0, hit2 = 0;
  for (size_t i = 0; i < sceneMesh->mesh()->rawMesh()->meshDescriptor.count;
       i++) {
    ponos::point3 v0 = sceneMesh->mesh()->rawMesh()->positionElement(i, 0);
    ponos::point3 v1 = sceneMesh->mesh()->rawMesh()->positionElement(i, 1);
    ponos::point3 v2 = sceneMesh->mesh()->rawMesh()->positionElement(i, 2);
    if (ponos::triangle_ray_intersection(v0, v1, v2, r))
      hit++;
    if (ponos::triangle_ray_intersection(v0, v1, v2, r2))
      hit2++;
  }
  return hit % 2 && hit2 % 2;
}

} // namespace circe
