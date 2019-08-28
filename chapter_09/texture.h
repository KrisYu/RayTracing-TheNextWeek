#ifndef TEXTUREH
#define TEXTUREH

#include "perlin.h"

class texture {
public:
  virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture: public texture{
public:
  constant_texture(vec3 c): color(c) {}
  vec3 value(float u, float v, const vec3& p) const{
    return color;
  }
  vec3 color;
};


class perlin_texture: public texture{
public:
  perlin_texture(float sc): scale(sc) {}
  vec3 value(float u, float v, const vec3& p) const{
    return vec3(1,1,1) * 0.5 * (1. + sin(scale*p.x() + 5*noise.turb(scale*p)));
  }
  perlin noise;
  float scale;
};

class image_texture: public texture{
public:
  image_texture() { }
  image_texture(unsigned char *pixels, int A, int B): data(pixels), nx(A), ny(B) {}

  vec3 value(float u, float v, const vec3 &p) const{
    int i = u * nx;
    int j = (1-v) * ny - 0.001;
    i = std::min(nx-1, std::max(0, i));
    j = std::min(ny-1, std::max(0, j));
    float r = data[3*i+3*nx*j+0] / 255.0;
    float g = data[3*i+3*nx*j+1] / 255.0;
    float b = data[3*i+3*nx*j+2] / 255.0;
    return vec3(r, g, b);
  }

  unsigned char* data;
  int nx, ny;
};

#endif
