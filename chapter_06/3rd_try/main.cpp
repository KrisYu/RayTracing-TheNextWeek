#include <chrono>
#include <cstdio>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "xy_rect.h"

vec3 color(const ray& r, hitable *world, int depth){
	hit_record rec;
	if (world->hit(r, 0.001, MAXFLOAT, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation * color(scattered, world, depth + 1);
		} else {
			return emitted;
		}
	} else {
		return vec3(0, 0, 0);
	}
}

hitable *simple_light(){
	texture *pertext = new perlin_texture(4);
	hitable **list = new hitable*[4];

	list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(new constant_texture(vec3(1, 1, 1))));
	list[1] = new sphere(vec3(0,2,0), 2, new lambertian(new constant_texture(vec3(1, 1, 1))));
  list[2] = new sphere(vec3(0, 7, 0), 2,new diffuse_light(new constant_texture(vec3(4,4,4))));
  list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4,4,4))));

	return new hitable_list(list, 4);
}

int main(){
	auto timeStart = std::chrono::high_resolution_clock::now();

	int nx = 800;
	int ny = 400;
	int ns = 1000;

	std::cout << "P3\n" << nx << " " << ny << "\n255\n";

	hitable *world = simple_light();
  camera cam(vec3(25,2,0),vec3(0,0,0),vec3(0,1,0),25,float(nx)/float(ny));

	for(int j = ny - 1; j >= 0; j--){
		for (int i = 0; i < nx; i++){
			vec3 col(0, 0, 0);

			for (int s = 0; s < ns; s++) {
				float u = float(i + drand48()) / float(nx);
				float v = float(j + drand48()) / float(ny);
				ray r = cam.get_ray(u, v);
			  col += color(r, world, 0);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
      int ir = std::min(255, std::max(0, int(255.99 * col[0])));
      int ig = std::min(255, std::max(0, int(255.99 * col[1])));
      int ib = std::min(255, std::max(0, int(255.99 * col[2])));
			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}
	auto timeEnd = std::chrono::high_resolution_clock::now();
	auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
	fprintf(stderr, "\rDone: %.2f (sec)\n", passedTime / 1000);


}
