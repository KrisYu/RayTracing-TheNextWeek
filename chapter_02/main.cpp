#include <chrono>
#include <iomanip>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aabb.h"
#include "bvh_node.h"


vec3 color(const ray& r, hitable *world, int depth){
	hit_record rec;
	if (world->hit(r, 0.001, MAXFLOAT, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		} else {
			return vec3(0, 0, 0);
		}
	} else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y()+1.0); // map y from [-1,1] to [0,1]
		return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5, 0.7, 1.0); // linear interpolation
	}
}

hitable *random_scene(){
	int n = 500;
	hitable **list = new hitable*[n+1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5,0.5,0.5)));
	int i = 1;

	for (int a = -5; a < 5; a++) {
		for (int b = -5; b < 5; b++) {
			float choose_mat = drand48();
			vec3 center(a + 0.9*drand48(), 0.2, b + 0.9*drand48());
			if ((center-vec3(4,0.2,0)).length() > 0.9) {
				if (choose_mat < 0.8) { // diffuse
					list[i++] = new moving_sphere(center, center + vec3(0, 0.5*drand48(), 0),
					0.0, 1.0, 0.2, new lambertian(vec3(drand48()*drand48(),drand48()*drand48(),drand48()*drand48())));
				}
				 else if (choose_mat < 0.95) { //metal
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1+drand48()),0.5*(1+drand48()),0.5*(1+drand48())), 0.5*drand48()));
				}
				else { //glass
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0,1,0),1.0,new dielectric(1.5));
	list[i++] = new sphere(vec3(-4,1,0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4,1,0),1.0, new metal(vec3(0.7,0.6,0.5),0.0));

	// return new hitable_list(list, i);

	return new bvh_node(list, i, 0, 1);;
}



int main(){
	auto timeStart = std::chrono::high_resolution_clock::now();

	int nx = 800;
	int ny = 400;
	int ns = 100;

	std::cout << "P3\n" << nx << " " << ny << "\n255\n";


	hitable *world = random_scene();


	vec3 lookfrom(13,2,3);
	vec3 lookat(0,0,0);
	float dist_to_focus = 10.0;
	float aperture = 0.1;

	camera cam(lookfrom, lookat,vec3(0,1,0), 20 ,float(nx)/float(ny),
	 aperture, dist_to_focus, 0.0, 1.0);

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
  std::cerr << "\rDone:" << std::setprecision(2) << passedTime / 1000 << "(sec)\n" << std::endl;

}
