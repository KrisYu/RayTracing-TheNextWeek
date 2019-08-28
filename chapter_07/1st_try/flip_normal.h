#ifndef FLIP_NORMAL
#define FLIP_NORMAL

#include "hitable.h"

class flip_normal: public hitable {

public:
  flip_normal(hitable *p): ptr(p) {}
  virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const{
    if (ptr->hit(r, t_min, t_max, rec)) {
      rec.normal = -rec.normal;
      return true;
    } else
      return false;
  }

  hitable *ptr;
};

#endif
