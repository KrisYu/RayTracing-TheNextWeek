#ifndef TRANSLATEH
#define TRANSLATEH

#include "hitable.h"
#include "ray.h"

class translate: public hitable {
public:
  translate (hitable *p, const vec3& displacement): ptr(p), offset(displacement) {}
  virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const;
  hitable *ptr;
  vec3 offset;
};

bool translate::hit(const ray &r, float t_min, float t_max, hit_record& rec) const{
  ray moved_r(r.origin() - offset, r.direction());
  if (ptr->hit(moved_r, t_min, t_max, rec)) {
    rec.p += offset;
    return true;
  }
  return false;
}

class rotate_y: public hitable {
public:
  rotate_y (hitable *ptr, float angle);
  virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const;
  hitable *ptr;
  float sin_theta;
  float cos_theta;
};

rotate_y::rotate_y(hitable *p, float angle): ptr(p){
  float radians = (M_PI/180.0) * angle;
  sin_theta = sin(radians);
  cos_theta = cos(radians);
}

bool rotate_y::hit(const ray &r, float t_min, float t_max, hit_record& rec) const{
  vec3 origin = r.origin();
  vec3 direction = r.direction();
  origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
  origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];
  direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
  direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];
  ray rotated_ray(origin, direction);
  if (ptr->hit(rotated_ray, t_min, t_max, rec)) {
    vec3 p = rec.p;
    vec3 normal = rec.normal;
    p[0] = cos_theta*rec.p[0] + sin_theta*rec.p[2];
    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];
    normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];
    rec.p = p;
    rec.normal = normal;
    return true;
  }
  return false;
}

#endif
