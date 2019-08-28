#include <chrono>
#include <cstdio>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "hitable_list.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "xy_rect.h"
#include "yz_rect.h"
#include "xz_rect.h"
#include "flip_normal.h"
#include "box.h"
#include "translate.h"
#include "constant_medium.h"
#include "bvh_node.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

hitable *final(){
	int nb = 20;
	hitable **list = new hitable*[30];
	hitable **boxlist = new hitable*[10000];
	hitable **boxlist2 = new hitable*[10000];
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));

	// box on the floor
	int b = 0;
	for (size_t i = 0; i < nb; i++) {
		for (size_t j = 0; j < nb; j++) {
			float w = 100;
			float x0 = -1000 + i*w;
			float z0 = -1000 + j*w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100*(drand48()+0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0,y0,z0),vec3(x1,y1,z1), ground);
		}
	}

	int l = 0;
	list[l++] = new bvh_node(boxlist, b, 0, 1);
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
	list[l++] = new xz_rect(113, 443, 127, 432, 554, light);

	vec3 center(400, 400, 200);
	list[l++] = new moving_sphere(center, center+vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
	list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));

	hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));

	boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));

	int nx,ny,nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
	material *emat = new lambertian(new image_texture(tex_data, nx, ny));
	list[l++] = new sphere(vec3(400, 200, 400), 100, emat);

	texture *pertext = new perlin_texture(0.1);
	list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));

	int ns = 1000;
	for (size_t j = 0; j < ns; j++) {
		boxlist2[j] = new sphere(vec3(165*drand48(), 165*drand48(), 165*drand48()), 10, white);
	}
	list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
	return new hitable_list(list, l);
}

int main(){
	auto timeStart = std::chrono::high_resolution_clock::now();

	int nx = 800;
	int ny = 800;
	int ns = 10000;

	std::cout << "P3\n" << nx << " " << ny << "\n255\n";

	hitable *world = final();

	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float disk_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;

	camera cam(lookfrom,lookat,vec3(0,1,0),vfov,float(nx)/float(ny), aperture, disk_to_focus, 0.0, 1.0);

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
