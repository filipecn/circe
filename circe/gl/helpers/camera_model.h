#ifndef CIRCE_HELPERS_CAMERA_MODEL_H
#define CIRCE_HELPERS_CAMERA_MODEL_H

#include <circe/scene/camera_interface.h>
#include <circe/gl/utils/open_gl.h>

#include <ponos/ponos.h>

#include <iostream>

namespace circe {

class CameraModel {
public:
  CameraModel() = default;
  virtual ~CameraModel() = default;

  static void drawCamera(const CameraInterface &camera) {
    ponos::vec3 dir = normalize(camera.getTarget() - camera.getPosition());
    ponos::vec3 left = normalize(cross(normalize(camera.getUpVector()), dir));
    ponos::vec3 up = normalize(cross(dir, left));

    ponos::point3 nbl, nbr, ntl, ntr;
    ponos::point3 fbl, fbr, ftl, ftr;
    float talpha = tanf(dynamic_cast<const PerspectiveProjection *>(
                            camera.getCameraProjection())
                            ->fov /
                        2.f);
    float tbeta = tanf((dynamic_cast<const PerspectiveProjection *>(
                            camera.getCameraProjection())
                            ->fov /
                        camera.getCameraProjection()->ratio) /
                       2.f);
    nbl = camera.getPosition() + dir * camera.getNear() -
          camera.getNear() * talpha * left - camera.getNear() * tbeta * up;
    nbr = camera.getPosition() + dir * camera.getNear() +
          camera.getNear() * talpha * left - camera.getNear() * tbeta * up;
    ntl = camera.getPosition() + dir * camera.getNear() -
          camera.getNear() * talpha * left + camera.getNear() * tbeta * up;
    ntr = camera.getPosition() + dir * camera.getNear() +
          camera.getNear() * talpha * left + camera.getNear() * tbeta * up;

    fbl = camera.getPosition() + dir * camera.getFar() -
          camera.getFar() * talpha * left - camera.getFar() * tbeta * up;
    fbr = camera.getPosition() + dir * camera.getFar() +
          camera.getFar() * talpha * left - camera.getFar() * tbeta * up;
    ftl = camera.getPosition() + dir * camera.getFar() -
          camera.getFar() * talpha * left + camera.getFar() * tbeta * up;
    ftr = camera.getPosition() + dir * camera.getFar() +
          camera.getFar() * talpha * left + camera.getFar() * tbeta * up;
    glColor3f(0, 0, 0);
//    glBegin(GL_LINES);
//    glVertex(camera.getPosition());
//    glVertex(fbl);
//    glVertex(camera.getPosition());
//    glVertex(fbr);
//    glVertex(camera.getPosition());
//    glVertex(ftl);
//    glVertex(camera.getPosition());
//    glVertex(ftr);
//    glEnd();
//    glBegin(GL_LINE_LOOP);
//    glVertex(nbl);
//    glVertex(nbr);
//    glVertex(ntr);
//    glVertex(ntl);
//    glEnd();
//    glBegin(GL_LINE_LOOP);
//    glBegin(GL_LINE_LOOP);
//    glVertex(fbl);
//    glVertex(fbr);
//    glVertex(ftr);
//    glVertex(ftl);
//    glEnd();
  }
};

} // namespace circe

#endif // CIRCE_HELPERS_CAMERA_MODEL_H
