// Author: Ben Foley


#include "ofApp.h"
#include "RayTracer.h"
#include <random>

/*
	Ray tracer functions ===========================================================================
*/

//---Constructor----------------------------------------------------
RayTracer::RayTracer() {
	// Allocate image object resolution
	final_image.allocate(2400, 1600, OF_IMAGE_COLOR);
	//final_image.allocate(960, 640, OF_IMAGE_COLOR);

	render_cam = RenderCam();
	ambient_light.intensity = .03;

	// Initialize random numbers
	std::random_device rd;
	std::mt19937 e2(rd());

	bshadow = true;
}

//---Add scene object
void RayTracer::addSceneObject(SceneObject *o) {
	objects.push_back(o);
}

void RayTracer::addLight(Light *l) {
	light_refs.push_back(l);
}

void RayTracer::addLuminaire(Luminaire *l) {
	lumin_refs.push_back(l);
}

void RayTracer::setShadow(const bool &s) {
	bshadow = s;
}


//---Get scene object pointers---------------------------------------
vector<SceneObject*> RayTracer::getSceneObjects() {
	return objects;
}

//---Determine whether a ray to a light source hits other objects----
bool RayTracer::inShadow(Ray r) {
	bool isBlocked = false;
	// iterate through objects
	for (auto obj : objects) {
		// Skip pathtracing luminares
		if (Luminaire *l = dynamic_cast<Luminaire*>(obj))
			continue;

		if (ra == RenderAlgo::raymarch) { // Ray marching
			int index;
			if (rayMarch(r, glm::vec3(), index)) {
				isBlocked = true;
				break;
			}
		}
		else {
			if (obj->intersect(r, glm::vec3(), glm::vec3())) { // Ray tracing
				isBlocked = true;
				break;
			}
		}
	}

	return isBlocked;
}


//---Phong shading calculation---------------------------------------
ofColor RayTracer::phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power) {
	ofColor color = ambient_light.diffuseColor * ambient_light.intensity;

	// Iterate through each light
	if (!light_refs.empty()) {
		for (const auto &light_ref : light_refs) {

			glm::vec3 n = glm::normalize(norm);
			glm::vec3 light_vec = glm::normalize(light_ref->position - p);

			glm::vec3 view_vec = glm::normalize(render_cam.position - p);
			glm::vec3 half_vec = glm::normalize(view_vec + light_vec);

			float lamb_angle = glm::max(0.0f, glm::dot(n, light_vec));
			float phong_angle = pow(glm::max(0.0f, glm::dot(n, half_vec)), power);

			float distance = glm::distance(light_vec, p);
			float intensity = light_ref->intensity / (distance * distance);

			// If not in shadow than calculate color of pixel
			bool shadow = false;
			if (bshadow) {
				if (ra == RenderAlgo::raymarch)
					shadow = inShadow(Ray(p + norm, light_vec));
				else
					shadow = inShadow(Ray(p + (norm * 0.01), light_vec));
			}

			if (!shadow) {
				ofColor lambert = ofColor(
					std::fmin(255.0, diffuse.r * intensity * lamb_angle),
					std::fmin(255.0, diffuse.g * intensity * lamb_angle),
					std::fmin(255.0, diffuse.b * intensity * lamb_angle),
					255);
				ofColor phong = ofColor(
					std::fmin(255.0, specular.r * intensity * phong_angle),
					std::fmin(255.0, specular.g * intensity * phong_angle),
					std::fmin(255.0, specular.b * intensity * phong_angle),
					255);
				color += lambert + phong;
			}
		}
	}

	// Cone lights
	if (!cone_refs.empty()) {
		// Iterate through cone lights
		for (const auto &cone_ref : cone_refs) {

			// Vector pointing to light from point
			glm::vec3 L = glm::normalize(cone_ref->position - p);

			// Direction vector of light cone
			//glm::vec3 dir = glm::normalize(cone_ref->dir_vec);
			glm::vec3 dir = glm::normalize(glm::vec3(-cone_ref->dir_vec.x, cone_ref->dir_vec.y, -cone_ref->dir_vec.z));

			// Angle between direction vector (lightpoint->norm) and light vector to point (lightpoint->point)  
			float spotlight_cos = std::abs(glm::dot(-L, dir));

			// If angle of the position to light vector to the direction vector is greater than half cone angle,
			// then the pixel is in the spotlight and needs to be shaded accordingly
			float current_angle = glm::degrees(spotlight_cos);
			if (spotlight_cos >= glm::radians(cone_ref->angle_cutoff) && !inShadow(Ray(p + (norm * .001), L))) {

				// Fall off of cone light the farther away from the cone the point is
				float falloff = std::pow(glm::max(0.0f, spotlight_cos), cone_ref->falloff_radius);

				// Distance and intensity of the light
				float distance = glm::distance(cone_ref->position, p);
				float intensity = cone_ref->intensity / (distance * distance);

				// Normalized normal
				glm::vec3 n = glm::normalize(norm);

				// Phone shading vectors
				glm::vec3 view_vec = glm::normalize(render_cam.position - p);
				glm::vec3 half_vec = glm::normalize(view_vec + L);

				// Angles for lambert and phone calulcations
				float lamb_angle = glm::max(0.0f, glm::dot(n, L));
				float phong_angle = pow(glm::max(0.0f, glm::dot(n, half_vec)), power);

				ofColor lambert = ofColor(
					std::fmin(255.0, diffuse.r * intensity * falloff * lamb_angle),
					std::fmin(255.0, diffuse.g * intensity * falloff * lamb_angle),
					std::fmin(255.0, diffuse.b * intensity * falloff * lamb_angle),
					255);
				ofColor phong = ofColor(
					std::fmin(255.0, specular.r * intensity * phong_angle),
					std::fmin(255.0, specular.g * intensity * phong_angle),
					std::fmin(255.0, specular.b * intensity * phong_angle),
					255);
				color += lambert + phong;

			}
		}
	}

	//color = glm::min(ofColor(255, 255, 255, 255), color);
	color = ofColor(
		std::fmin(255.0, color.r),
		std::fmin(255.0, color.g),
		std::fmin(255.0, color.b),
		255);

	return color;
} // end phong



//---Look up color of texture pixel given the (u, v) coordinates---------------
ofColor RayTracer::texture_lookup(const ofImage &texture, float u, float v) {
	// Height and width of texture
	uint32_t text_w = texture.getWidth();
	uint32_t text_h = texture.getHeight();

	// Find the i j coordinates
	uint32_t i = glm::round(u * text_w - 0.5);
	uint32_t j = glm::round(v * text_h - 0.5);

	// Return the color of the texture at translated texture coordinates
	return texture.getPixelsRef().getColor(i % text_w, j % text_h);

}


// Takes a pixel and finds that color of that pixel.
// This is a necessary abstraction from render in order to create a blue effect
ofColor RayTracer::rayColor(float u, float v) {

	ofColor c;

	// calculate ray through pixel
	Ray ray = render_cam.getRay(u, v);

	// hit boolean to detect the hitting of an object
	bool hit = false;

	// distance variable to calculate distance
	float distance = std::numeric_limits<float>::infinity();

	// pointer to keep a reference to the closest object to determine which color to draw pixel
	SceneObject *closest_object = nullptr;

	// Iterate through each object
	glm::vec3 closest_intersect, closest_normal;
	for (auto obj_ptr : objects) {
		if (dynamic_cast<Luminaire*>(obj_ptr)) continue;
		glm::vec3 point, normal;

		// Detect intersection with scene object
		if (obj_ptr->intersect(ray, point, normal)) {

			// Set hit pointer to true
			hit = true;

			// determine whether object is closer than the closest_object reference, if so replace
			if (glm::distance(glm::vec3(u, v, 0), point) < distance) {
				distance = glm::distance(glm::vec3(u, v, 0), point);
				closest_object = obj_ptr;
				closest_intersect = point;
				closest_normal = glm::normalize(normal);
			}
		}
	}
	if (hit) { // Draw color of nearest object if ray hit it
		
		ofColor color;
		if (closest_object->texture_ref) { // For plane objects with textures
			 // Dynamically cast object to plane
			if (Plane *p = dynamic_cast<Plane*>(closest_object)) {
				if (p->isTextured == true) {
					// Calculate the u and v normal vector directions

				// Orthogonal unit vectors
					glm::vec3 x = glm::cross(p->normal, glm::vec3(1, 0, 0));
					glm::vec3 y = glm::cross(p->normal, glm::vec3(0, 1, 0));
					glm::vec3 z = glm::cross(p->normal, glm::vec3(0, 0, 1));

					// Determine the unit vector whose magnitude is lesser and set that for the u direction vector
					glm::vec3 max_xy = glm::dot(x, x) < glm::dot(y, y) ? y : x;
					glm::vec3 u_vec = glm::normalize(glm::dot(max_xy, max_xy) < glm::dot(z, z) ? max_xy : z);
					// Cross for the v direction vector
					glm::vec3 v_vec = glm::cross(p->normal, u_vec);

					// Calculate (u,v) coordinates of intersection of plane
					float up = glm::dot(u_vec, closest_intersect) * 0.2;
					float vp = glm::dot(v_vec, closest_intersect) * 0.2;

					// Lookup color of pixel of intersected texture
					ofColor diffuse = texture_lookup(*p->texture_ref, up, vp);

					// Calculate shading with texture color
					c = phong(closest_intersect, closest_normal, diffuse, closest_object->specularColor, closest_object->power);
				}
				else {
					c = phong(closest_intersect, closest_normal, closest_object->diffuseColor, closest_object->specularColor, closest_object->power);
				}
			}
			else {
				cerr << "Could not cast object to plane" << endl;
			}
		}
		else { // Calculate non textured shading
			c = phong(closest_intersect, closest_normal, closest_object->diffuseColor, closest_object->specularColor, closest_object->power);
		}
	}
	else { // draw background color of no ray was hit
		c = background_color;
	}

	return c;
} // end rayColor


void RayTracer::colorToRgb(Rgb &r, ofColor &c) {
	r.r += c.r;
	r.g += c.g;
	r.b += c.b;
	return;
}

// --- PathTrace implementation
// --- Based off of psuedocode located in "Fundamentals of Computer Graphics 4th ed." pg. 619
void RayTracer::pathTrace(Rgb &clr, Ray r, uint32_t depth, std::mt19937 &e2, std::uniform_real_distribution<> &dist) {
	if (depth == max_depth) {
		clr.r = 0; clr.g = 0; clr.b = 0;
		return;
	}

	bool hit = false;
	float distance = std::numeric_limits<float>::infinity();
	SceneObject *closest_object = nullptr;

	// Iterate through each object
	glm::vec3 closest_intersect, closest_normal;
	for (auto obj_ptr : objects) {
		glm::vec3 point, normal;

		// Detect intersection with scene object
		if (obj_ptr->intersect(r, point, normal)) {

			// Set hit pointer to true
			hit = true;

			// determine whether object is closer than the closest_object reference, if so replace
			if (glm::distance(r.p, point) < distance) {
				distance = glm::distance(r.p, point);
				closest_object = obj_ptr;
				closest_intersect = point;
				closest_normal = glm::normalize(normal);
			}
		}
	}
	if (hit) { // Draw color of nearest object if ray hit it

		ofColor color;
		if (closest_object->texture_ref) { // For plane objects with textures
			 // Dynamically cast object to plane
			if (Plane *p = dynamic_cast<Plane*>(closest_object)) {
				if (p->isTextured) {
					// Calculate the u and v normal vector directions

				// Orthogonal unit vectors
					glm::vec3 x = glm::cross(p->normal, glm::vec3(1, 0, 0));
					glm::vec3 y = glm::cross(p->normal, glm::vec3(0, 1, 0));
					glm::vec3 z = glm::cross(p->normal, glm::vec3(0, 0, 1));

					// Determine the unit vector whose magnitude is lesser and set that for the u direction vector
					glm::vec3 max_xy = glm::dot(x, x) < glm::dot(y, y) ? y : x;
					glm::vec3 u_vec = glm::normalize(glm::dot(max_xy, max_xy) < glm::dot(z, z) ? max_xy : z);
					// Cross for the v direction vector
					glm::vec3 v_vec = glm::cross(p->normal, u_vec);

					// Calculate (u,v) coordinates of intersection of plane
					float up = glm::dot(u_vec, closest_intersect) * 0.2;
					float vp = glm::dot(v_vec, closest_intersect) * 0.2;

					// Lookup color of pixel of intersected texture
					ofColor diffuse = texture_lookup(*p->texture_ref, up, vp);

					// Calculate shading with texture color
					ofColor c = phong(closest_intersect, closest_normal, diffuse, closest_object->specularColor, closest_object->power);
					colorToRgb(clr, c);
				}
				else {
					ofColor c = phong(closest_intersect, closest_normal, closest_object->diffuseColor, closest_object->specularColor, closest_object->power);
					colorToRgb(clr, c);
				}

			}
			else {
				cerr << "Could not cast object to plane" << endl;
				return;
			}
		}
		else { // Calculate non textured shading
			if (dynamic_cast<Luminaire*>(closest_object)) {
				if (depth == 0) {
					colorToRgb(clr, ofColor(255, 255, 255));
					return;
				}
				return;
			}
			// non textured color
			ofColor c = phong(closest_intersect, closest_normal, closest_object->diffuseColor, closest_object->specularColor, closest_object->power);
			colorToRgb(clr, c);

		}
	


		// Compute random direction to cast new ray
			
		// Produce random direction vector
		float rand_x = dist(e2) - 1;
		float rand_y = dist(e2) - 1;
		float rand_z = dist(e2) - 1;
		glm::vec3 new_dir = glm::vec3(rand_x, rand_y, rand_z);

		// Compute new direction
		// Cast ray not from the point of intersection but from a point just above to disallow self intersection
		Ray new_ray = Ray(closest_intersect + (new_dir * .01), glm::vec3(rand_x, rand_y, rand_z));

		float choose = static_cast<float>(rand() / static_cast<float>(RAND_MAX));
			

		// Recursively follow new ray
		pathTrace(clr, new_ray, ++depth, e2, dist); 
		
	}
	else { // draw background color of no ray was hit
		//if (depth == 0)
		colorToRgb(clr, background_color);
		return;
	}

} // end pathTrace


// Find color from any given ray
// Used for noise in dof function
ofColor RayTracer::rayColorFromRay(Ray r) {
	ofColor c;

	// hit boolean to detect the hitting of an object
	bool hit = false;

	// distance variable to calculate distance
	float distance = std::numeric_limits<float>::infinity();

	// pointer to keep a reference to the closest object to determine which color to draw pixel
	SceneObject *closest_object = nullptr;

	// Iterate through each object
	glm::vec3 closest_intersect, closest_normal;
	for (auto obj_ptr : objects) {
		glm::vec3 point, normal;

		// Detect intersection with scene object
		if (obj_ptr->intersect(r, point, normal)) {

			// Set hit pointer to true
			hit = true;
			
			// determine whether object is closer than the closest_object reference, if so replace
			if (glm::distance(r.p , point) < distance) {
				distance = glm::distance(r.p, point);
				closest_object = obj_ptr;
				closest_intersect = point;
				closest_normal = glm::normalize(normal);
			}
		}
	}
	if (hit) { // Draw color of nearest object if ray hit it

		ofColor color;
		if (closest_object->texture_ref) { // For plane objects with textures
			 // Dynamically cast object to plane
			if (Plane *p = dynamic_cast<Plane*>(closest_object)) {

				// Calculate the u and v normal vector directions

				// Orthogonal unit vectors
				glm::vec3 x = glm::cross(p->normal, glm::vec3(1, 0, 0));
				glm::vec3 y = glm::cross(p->normal, glm::vec3(0, 1, 0));
				glm::vec3 z = glm::cross(p->normal, glm::vec3(0, 0, 1));

				// Determine the unit vector whose magnitude is lesser and set that for the u direction vector
				glm::vec3 max_xy = glm::dot(x, x) < glm::dot(y, y) ? y : x;
				glm::vec3 u_vec = glm::normalize(glm::dot(max_xy, max_xy) < glm::dot(z, z) ? max_xy : z);
				// Cross for the v direction vector
				glm::vec3 v_vec = glm::cross(p->normal, u_vec);

				// Calculate (u,v) coordinates of intersection of plane
				float up = glm::dot(u_vec, closest_intersect) * 0.2;
				float vp = glm::dot(v_vec, closest_intersect) * 0.2;

				// Lookup color of pixel of intersected texture
				ofColor diffuse = texture_lookup(*p->texture_ref, up, vp);

				// Calculate shading with texture color
				c = phong(closest_intersect, closest_normal, diffuse, closest_object->specularColor, closest_object->power);
			}
			else {
				cerr << "Could not cast object to plane" << endl;
			}
		}
		else { // Calculate non textured shading
			c = phong(closest_intersect, closest_normal, closest_object->diffuseColor, closest_object->specularColor, closest_object->power);
		}
	}
	else { // draw background color of no ray was hit
		c = background_color;
	}

	return c;
} // end rayColorFromRay


//--- Render image with depth of field
//--- Implementation developed by Ben Foley
ofColor RayTracer::blurRayColor(float u, float v, float eye_radius, uint32_t num_sample) {

	// Ray casted to find focla length
	Ray focal_ray = render_cam.getRay(u, v);
	glm::vec3 focal_point = focal_ray.evalPoint(focal_dist);
	

	// Cast random point from the circular apeture (i.e. render cam in this case)

	// Seed random number generator
	srand(static_cast<unsigned> (time(0)));

	float c_r = 0.0f;
	float c_g = 0.0f;
	float c_b = 0.0f;

	int apeture_radius = 2;
	for (int p = 0; p < num_sample; p++) {

		// Box apeture implementation
		//float rand_x = fmin(final_image.getWidth(), fmax(0.0, static_cast<float>(rand() / static_cast<float>(RAND_MAX / (apeture_radius - (-apeture_radius))))));
		//float rand_y = fmin(final_image.getHeight(), fmax(0.0, static_cast<float>(rand() / static_cast<float>(RAND_MAX / (apeture_radius - (-apeture_radius))))));

		//glm::vec3 rand_apeture_pt = glm::vec3(rand_x, rand_y, 0.0) + render_cam.position;

		// Circular apeture implementation
		// Random radius
		float rand_radius = static_cast<float>(rand() / static_cast<float>(RAND_MAX / apeture_radius));
		// Random angle
		float rand_angle = static_cast<float>(rand() / static_cast<float>(RAND_MAX / 360));

		// x and y axis values
		float rand_x = eye_radius * glm::cos(glm::radians(rand_angle));
		float rand_y = eye_radius * glm::sin(glm::radians(rand_angle));

		glm::vec3 rand_apeture_pt = glm::vec3(rand_x, rand_y, 0.0) + render_cam.position;

		Ray sample_ray = Ray(rand_apeture_pt, glm::normalize(focal_point - rand_apeture_pt));
		
		// find the color that the ray finds
		ofColor current_color = rayColorFromRay(sample_ray);
		
		c_r += current_color.r;
		c_g += current_color.g;
		c_b += current_color.b;
	}

	c_r /= num_sample;
	c_g /= num_sample;
	c_b /= num_sample;

	return ofColor(c_r, c_g, c_b);
} // end blueRayColor

// --- RAY -------------------------------------------------------------------
// --- MARCH -----------------------------------------------------------------
// --- FUNCTIONS -------------------------------------------------------------

float RayTracer::sceneSDF(const glm::vec3 &p, int &obj_index) {
	float distance = std::numeric_limits<float>::infinity();
	obj_index = -1;

	// For each object, calculate the distance and give the shortest distance away
	for (int i = 0; i < objects.size(); i++) {
		float obj_dist = objects[i]->sdf(p);
		if (distance > obj_dist) {
			distance = obj_dist;
			obj_index = i;
		}
	}

	return distance;
} // end sceneSDF

bool RayTracer::rayMarch(const Ray &r, glm::vec3 &p, int &obj_index) {
	bool hit = false;
	float dist;
	obj_index = -1;

	p = r.p;
	for (int i = 0; i < max_ray_steps; i++) {
		dist = sceneSDF(p, obj_index);

		if (dist < distance_threshold) {
			hit = true;
			break;
		}
		else if (dist > max_distance) {
			break;
		}
		else {
			p = p + r.d * dist; // move along the ray
		}
	}

	return hit;
} // end rayMarch

ofColor RayTracer::rayMarchLoop(const Ray &r) {
	glm::vec3 point;
	ofColor c;
	int obj_index;

	bool hit = rayMarch(r, point, obj_index);

	if (hit) { // Shade point
		//c = ofColor::white;
		c = phong(point, getNormalRM(point), objects[obj_index]->diffuseColor, objects[obj_index]->specularColor, objects[obj_index]->power);
	}
	else { // Draw background color of no ray was hit
		c = background_color;
	}
	

	return c;
} // end rayMarchLooop


glm::vec3 RayTracer::getNormalRM(const glm::vec3 &p) {
	int obj_index;
	float eps = 0.01f;
	float dp = sceneSDF(p, obj_index);

	glm::vec3 n(dp - sceneSDF(glm::vec3(p.x - eps, p.y, p.z), obj_index),
		dp - sceneSDF(glm::vec3(p.x, p.y - eps, p.z), obj_index),
		dp - sceneSDF(glm::vec3(p.x , p.y, p.z - eps), obj_index));

	return glm::normalize(n);
} // end getNormalRM



//---Render ray traced scene--------------------------------------------------
void RayTracer::render() {
	cout << "Render Started" << endl;

	float before_time = ofGetElapsedTimeMillis();

	// For each pixel row
	for (int j = 0; j < final_image.getHeight(); j++) {
		// For each pixel in column
		for (int i = 0; i < final_image.getWidth(); i++) {

			// Convert each (i,j) into (u,v) (pixels in the rendercam image)
			float u = (i + 0.5) / final_image.getWidth();
			float v = (j + 0.5) / final_image.getHeight();

			ofColor color;


			if (ra == RenderAlgo::pathtrace) { // path trace

				// Random number generator
				// Uses Mersenne Twister number generator because rand() was much too slow
				std::random_device rd;
				std::mt19937 e2(rd());
				std::uniform_real_distribution<> dist(0, 2);

				// Ray casted to find focal length
				Ray ray = render_cam.getRay(u, v);

				Rgb clr = { 0.0f, 0.0f, 0.0f };

				uint32_t depth = 0;
				pathTrace(clr, ray, depth, e2, dist);

				color.r = clr.r / (depth + 2);
				color.g = clr.g / (depth + 2);
				color.b = clr.b / (depth + 2);

				color.r = clr.r;
				color.g = clr.g;
				color.b = clr.b;
			}
			else if (ra == RenderAlgo::raytrace) { // dof
				color = rayColor(u, v);
				//ofColor color = blurRayColor(u, v, 0.3, 1000);
				//color = blurRayColor(u, v, apeture_size, dof_samples);
			}
			else { // Ray march
				Ray ray = render_cam.getRay(u, v);
				color = rayMarchLoop(ray);
			}

			// set final color
			final_image.setColor(i, j, color);
				
		}
	}
	// Save image to disk
	if (!final_image.save("../../images/raytrace_image.png"))
		cerr << "Could not save render file" << endl;

	float after_time = ofGetElapsedTimeMillis();
	cout << "Render time: " << after_time - before_time << "ms" << endl;
} // end render


