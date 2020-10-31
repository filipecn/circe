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

#include <circe/io/utils.h>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
//#include <ply.h>
#include <tiny_obj_loader.h>

#include <cstring>

namespace circe {

void loadOBJ(const std::string &filename, ponos::RawMesh *mesh) {
  if (!mesh)
    return;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  bool r = false;
//      tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
  if (!r) {
    std::cerr << err << std::endl;
    return;
  }
  mesh->positions = std::vector<float>(attrib.vertices);
  mesh->normals = std::vector<float>(attrib.normals);
  mesh->texcoords = std::vector<float>(attrib.texcoords);
  mesh->indices.resize(shapes[0].mesh.indices.size());
  memcpy(&mesh->indices[0], &shapes[0].mesh.indices[0],
         shapes[0].mesh.indices.size() * sizeof(tinyobj::index_t));
  mesh->positionDescriptor.elementSize = 3;
  mesh->positionDescriptor.count =
      mesh->positions.size() / mesh->positionDescriptor.elementSize;
  if (mesh->normals.size()) {
    mesh->normalDescriptor.elementSize = 3;
    mesh->normalDescriptor.count =
        mesh->normals.size() / mesh->normalDescriptor.elementSize;
  } else
    mesh->normalDescriptor.count = mesh->normalDescriptor.elementSize = 0;
  if (mesh->texcoords.size()) {
    mesh->texcoordDescriptor.elementSize = 2;
    mesh->texcoordDescriptor.count =
        mesh->texcoords.size() / mesh->texcoordDescriptor.elementSize;
  } else
    mesh->texcoordDescriptor.count = mesh->texcoordDescriptor.elementSize = 0;
  mesh->meshDescriptor.elementSize = 3;
  mesh->meshDescriptor.count =
      mesh->indices.size() / mesh->meshDescriptor.elementSize;
  mesh->primitiveType = ponos::GeometricPrimitiveType::TRIANGLES;
  mesh->computeBBox();
  // mesh->splitIndexData();
  // mesh->buildInterleavedData();
  /* tiny obj use
  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
          // Loop over faces(polygon)
          size_t index_offset = 0;
          for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                  int fv = shapes[s].mesh.num_face_vertices[f];
                  // Loop over vertices in the face.
                  for (size_t v = 0; v < fv; v++) {
                          // access to vertex
                          tinyobj::index_t idx =
  shapes[s].mesh.indices[index_offset + v];
                          float vx = attrib.vertices[3*idx.vertex_index+0];
                          float vy = attrib.vertices[3*idx.vertex_index+1];
                          float vz = attrib.vertices[3*idx.vertex_index+2];
                          float nx = attrib.normals[3*idx.normal_index+0];
                          float ny = attrib.normals[3*idx.normal_index+1];
                          float nz = attrib.normals[3*idx.normal_index+2];
                          float tx = attrib.texcoords[2*idx.texcoord_index+0];
                          float ty = attrib.texcoords[2*idx.texcoord_index+1];
                  }
                  index_offset += fv;

                  // per-face material
                  shapes[s].mesh.material_ids[f];
          }
  }*/
}

//void loadPLY(const std::string &filename, ponos::RawMesh *mesh) {
//  if (!mesh)
//    return;
//  typedef struct Vertex {
//    float x, y, z; /* the usual 3-space position of a vertex */
//  } Vertex;
//  typedef struct Face {
//    unsigned char intensity; /* this user attaches intensity to faces */
//    unsigned char nverts;    /* number of vertex indices in list */
//    int *verts;              /* vertex index list */
//  } Face;
//  // const char *elem_names[] = {
//  /* list of the kinds of elements in the user's object */
//  //    "vertex", "face"};
//  char plyPropertyX[] = {'x', '\0'};
//  char plyPropertyY[] = {'y', '\0'};
//  char plyPropertyZ[] = {'z', '\0'};
//  PlyProperty vert_props[] = {
//      /* list of property information for a vertex */
//      {plyPropertyX, PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, x), 0, 0, 0, 0},
//      {plyPropertyY, PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, y), 0, 0, 0, 0},
//      {plyPropertyZ, PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, z), 0, 0, 0, 0},
//  };
//  char plyPropertyIntensity[50];
//  std::strcpy(plyPropertyIntensity, "intensity");
//  char plyPropertyVertexIndices[50];
//  std::strcpy(plyPropertyVertexIndices, "vertex_indices");
//  PlyProperty face_props[] = {
//      /* list of property information for a vertex */
//      {plyPropertyIntensity, PLY_UCHAR, PLY_UCHAR, offsetof(Face, intensity), 0,
//       0, 0, 0},
//      {plyPropertyVertexIndices, PLY_INT, PLY_INT, offsetof(Face, verts), 1,
//       PLY_UCHAR, PLY_UCHAR, offsetof(Face, nverts)},
//  };
//
//  int i, j, k;
//  PlyFile *ply;
//  int nelems;
//  char **elist;
//  int file_type;
//  float version;
//  int nprops;
//  int num_elems;
//  PlyProperty **plist;
//  Vertex **vlist;
//  Face **flist;
//  char *elem_name;
//  int num_comments;
//  char **comments;
//  int num_obj_info;
//  char **obj_info;
//  char vertexString[7];
//  std::strcpy(vertexString, "vertex");
//  char faceString[5];
//  std::strcpy(faceString, "face");
//  /* open a PLY file for reading */
//  char _filename[100];
//  std::strcpy(_filename, filename.c_str());
//  ply = ply_open_for_reading(_filename, &nelems, &elist, &file_type, &version);
//  /* print what we found out about the file */
//  printf("version %f\n", version);
//  printf("type %d\n", file_type);
//  for (i = 0; i < nelems; i++) {
//    /* get the description of the first element */
//    elem_name = elist[i];
//    plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);
//    /* print the name of the element, for debugging */
//    printf("element %s %d\n", elem_name, num_elems);
//    /* if we're on vertex elements, read them in */
//    if (equal_strings(vertexString, elem_name)) {
//      /* create a vertex list to hold all the vertices */
//      vlist = (Vertex **)malloc(sizeof(Vertex *) * num_elems);
//      /* set up for getting vertex elements */
//      ply_get_property(ply, elem_name, &vert_props[0]);
//      ply_get_property(ply, elem_name, &vert_props[1]);
//      ply_get_property(ply, elem_name, &vert_props[2]);
//      mesh->positionDescriptor.count = num_elems;
//      /* grab all the vertex elements */
//      for (j = 0; j < num_elems; j++) {
//        /* grab and element from the file */
//        vlist[j] = (Vertex *)malloc(sizeof(Vertex));
//        ply_get_element(ply, (void *)vlist[j]);
//        /* print out vertex x,y,z for debugging */
//        printf("vertex: %g %g %g\n", vlist[j]->x, vlist[j]->y, vlist[j]->z);
//        mesh->positionDescriptor.elementSize = 3;
//        mesh->positions.emplace_back(vlist[j]->x);
//        mesh->positions.emplace_back(vlist[j]->y);
//        mesh->positions.emplace_back(vlist[j]->z);
//      }
//    }
//    /* if we're on face elements, read them in */
//    if (equal_strings(faceString, elem_name)) {
//      /* create a list to hold all the face elements */
//      flist = (Face **)malloc(sizeof(Face *) * num_elems);
//      /* set up for getting face elements */
//      ply_get_property(ply, elem_name, &face_props[0]);
//      ply_get_property(ply, elem_name, &face_props[1]);
//      mesh->meshDescriptor.count = num_elems;
//      /* grab all the face elements */
//      for (j = 0; j < num_elems; j++) {
//        /* grab and element from the file */
//        flist[j] = (Face *)malloc(sizeof(Face));
//        ply_get_element(ply, (void *)flist[j]);
//        /* print out face info, for debugging */
//        printf("face: %d, list = ", flist[j]->intensity);
//        mesh->meshDescriptor.elementSize = flist[j]->nverts;
//        for (k = 0; k < flist[j]->nverts; k++) {
//          printf("%d ", flist[j]->verts[k]);
//          ponos::RawMesh::IndexData d;
//          d.texcoordIndex = 0;
//          d.normalIndex = 0;
//          d.positionIndex = flist[j]->verts[k];
//          mesh->indices.emplace_back(d);
//        }
//        printf("\n");
//      }
//    }
//    /* print out the properties we got, for debugging */
//    for (j = 0; j < nprops; j++)
//      printf("property %s\n", plist[j]->name);
//  }
//  /* grab and print out the comments in the file */
//  comments = ply_get_comments(ply, &num_comments);
//  for (i = 0; i < num_comments; i++)
//    printf("comment = '%s'\n", comments[i]);
//  /* grab and print out the object information */
//  obj_info = ply_get_obj_info(ply, &num_obj_info);
//  for (i = 0; i < num_obj_info; i++)
//    printf("obj_info = '%s'\n", obj_info[i]);
//  /* close the PLY file */
//  ply_close(ply);
//  mesh->computeBBox();
//  std::cout << mesh->positions.size() / 3 << std::endl;
//  std::cout << mesh->meshDescriptor.count << std::endl;
//  std::cout << mesh->meshDescriptor.elementSize << std::endl;
//  std::cout << mesh->indices.size() << std::endl;
//}

} // circe namespace
