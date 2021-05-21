// Author: Ben Foley
// Starting code provided by Kevin Smith


#pragma once

#include <random>

#include "ofApp.h"
#include "Ray.h"
#include "SceneObjects.h"
#include "CamObjects.h"
#include "LightObjects.h"
#include "glm/gtx/perpendicular.hpp"


typedef struct {
	float r;
	float g;
	float b;
} Rgb;


enum RenderAlgo {
	raytrace,
	pathtrace,
	raymarch
};

/*
	Ray Tracer object
*/
class RayTracer {
public:
	// Constructor
	RayTracer();

	// Update scene objects
	void addSceneObject(SceneObject *o);
	void addLight(Light *l);
	void addLuminaire(Luminaire *l);

	// Render functions
	void render();

	// Return scene object references
	vector<SceneObject*> getSceneObjects();

	vector<ConeLight*> cone_refs;
	RenderCam render_cam; 	        // Render camera

	// Function to turn shadows on and off
	void setShadow(const bool &s);

	bool path_trace;
	float focal_dist;
	uint32_t dof_samples;
	uint32_t max_depth;
	float apeture_size;

	RenderAlgo ra = RenderAlgo::raymarch;

private:
	ofColor texture_lookup(const ofImage &texture, float u, float v);
	bool inShadow(Ray r);
	ofColor phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power);
	ofColor rayColor(float u, float v);
	
	// Dof
	ofColor blurRayColor(float u, float v, float eye_radius, uint32_t num_sample);
	ofColor rayColorFromRay(Ray r);
	
	// Path tracing
	void colorToRgb(Rgb &r, ofColor &c);
	void pathTrace(Rgb &clr, Ray r, uint32_t depth, std::mt19937 &e2, std::uniform_real_distribution<> &dist);

	// SDF scene loop used for Ray Marching
	float sceneSDF(const glm::vec3 &p, int &obj_index);
	
	// Ray Marching algorithm
	bool rayMarch(const Ray &r, glm::vec3 &p, int &obj_index);
	ofColor rayMarchLoop(const Ray &r);
	glm::vec3 getNormalRM(const glm::vec3 &p);

	AmbientLight ambient_light;
	vector<SceneObject*> objects; 	// Vector of pointers to scene objects
	vector<Light*> light_refs;
	vector<Luminaire*> lumin_refs;
	ofImage final_image; 	// Image object that will be used to draw image and save to disk
	ofColor background_color = ofColor::black;

	// Ray march data
	uint32_t max_ray_steps = 500;
	float distance_threshold = 0.0001;
	float max_distance = 1000;

	// Boolean to set shadowing
	bool bshadow;
};