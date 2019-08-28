#ifndef TEXTUREH
#define TEXTUREH


class texture {
public:
  virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture: public texture {
public:
  constant_texture() {}
  constant_texture(vec3 c): color(c) {}
  vec3 value(float u, float v, const vec3 &p) const{
    return color;
  }
  vec3 color;
};

class checker_texture: public texture {
private:
  /* data */

public:
  checker_texture() {}
  checker_texture(texture *t0, texture *t1): even(t0), odd(t1) { }
  vec3 value(float u, float v, const vec3 &p) const {
    float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
    if (sines < 0) {
      return odd->value(u,v,p);
    } else {
      return even->value(u,v,p);
    }
  }

  texture *odd;
  texture *even;
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
