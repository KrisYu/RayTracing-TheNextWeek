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
#include "yz_rect.h"
#include "xz_rect.h"
#include "flip_normal.h"
#include "box.h"
#include "translate.h"

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

hitable *cornell_box(){
	hitable **list = new hitable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

  list[i++] = new flip_normal(new yz_rect(0, 555, 0, 555, 555, green));
  list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
  list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
  list[i++] = new flip_normal(new xz_rect(0, 555, 0, 555, 555, white));
  list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
  list[i++] = new flip_normal(new xy_rect(0, 555, 0, 555, 555, white));
  

	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0),
	vec3(165, 165, 165), white), -18),vec3(130, 0, 65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0),
	vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	return new hitable_list(list, i);
}

int main(){
	auto timeStart = std::chrono::high_resolution_clock::now();

	int nx = 400;
	int ny = 400;
	int ns = 1000;

	std::cout << "P3\n" << nx << " " << ny << "\n255\n";

	hitable *world = cornell_box();

	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float disk_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;

	camera cam(lookfrom,lookat,vec3(0,1,0),vfov,float(nx)/float(ny), aperture, disk_to_focus);

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
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}
	auto timeEnd = std::chrono::high_resolution_clock::now();
	auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
	fprintf(stderr, "\rDone: %.2f (sec)\n", passedTime / 1000);


}
