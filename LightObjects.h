// Author: Ben Foley
// Starting code provided by Kevin Smith


#pragma once

#include "ofApp.h"
#include "Ray.h"
#include "SceneObjects.h"


/*
	Luminaire
	- Light source with volume
*/
class Luminaire : public Sphere {
public:
	Luminaire(glm::vec3 p, float i, float r) {
		position = p;
		intensity = i;
		radius = r;
	}
	Luminaire(glm::vec3 p, float i) {
		position = p;
		intensity = i;
		radius = 3;
	}
	Luminaire() {}

	void draw() {
		ofSetColor(ofColor::white);
		ofDrawSphere(glm::vec3(position.x, -position.y, position.z), radius);
	}

	// Data
	//
	float intensity;
};


/*
	Light Object
*/
class Light : public SceneObject {
public:
	Light(glm::vec3 p, float i) { position = p; intensity = i; }
	Light() {}
	void draw() {
		ofSetColor(ofColor::yellow);
		ofDrawSphere(glm::vec3(position.x, -position.y, position.z), 1);
	}
	bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal) {
		return (glm::intersectRaySphere(ray.p, ray.d, position, .1, point, normal));
	}

	// Data
	//
	float intensity;
};

/*
	Ambient Light Object
*/
class AmbientLight : public Light {
public:
	AmbientLight(ofColor diffuseColor = ofColor::white) { this->diffuseColor = diffuseColor; }
	void draw() { }
};


/*
	Cone Light ==============================================================================
*/
class ConeLight : public Light {
public:
	ConeLight(glm::vec3 p, float i, glm::vec3 d, float angle, float falloff) {
		position = p;
		intensity = i;
		dir_vec = d;
		angle_cutoff = angle;
		falloff_radius = falloff;
	}
	ConeLight() {}

	void draw() {
		ofSetColor(ofColor::orange);
		ofDrawSphere(glm::vec3(position.x, -position.y, position.z), 1);
		ofDrawLine((5 * glm::normalize(dir_vec) + glm::vec3(position.x, -position.y, position.z)), glm::vec3(position.x, -position.y, position.z));
		//float cone_radius = 7 * glm::sin(angle_cutoff);
		//ofDrawCone(glm::vec3(position.x, -position.y, position.z), cone_radius, -10);
	}

	// Direction of cone light
	glm::vec3 dir_vec;
	// Angle of cut off from direction vector
	float angle_cutoff;
	// fall off radius
	float falloff_radius;
};



//============================================================================================


