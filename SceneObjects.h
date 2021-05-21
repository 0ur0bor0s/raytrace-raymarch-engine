// Author: Ben Foley
// Starting code provided by Kevin Smith


#pragma once

#include "ofApp.h"
#include "Ray.h"
#include "glm/gtx/intersect.hpp"


//  Base class for any renderable object in the scene
//
class SceneObject {
public:
	virtual void draw() = 0;    // pure virtual funcs - must be overloaded
	virtual bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal) { cout << "SceneObject::intersect" << endl; return false; }

	// Sined distance function
	virtual float sdf(const glm::vec3 &p) { 
		cout << "sdf not implemented for this object." << endl; 
		return 0.0f;
	}

	// any data common to all scene objects goes here
	glm::vec3 position = glm::vec3(0, 0, 0);

	// material properties (we will ultimately replace this with a Material class - TBD)
	//
	ofColor diffuseColor = ofColor::grey;    // default colors - can be changed.
	ofColor specularColor = ofColor::lightGray;
	ofImage *texture_ref = NULL;
	float power;
	glm::vec3 normal;
}; // class SceneObject

//  General purpose sphere  (assume parametric)
//
class Sphere : public SceneObject {
public:
	Sphere(glm::vec3 p, float r, ofColor diffuse, float power) { position = p; radius = r; diffuseColor = diffuse; this->power = power; }
	Sphere() {}

	bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal) {
		return (glm::intersectRaySphere(ray.p, ray.d, position, radius, point, normal));
	}

	void draw() {
		ofSetColor(diffuseColor);
		ofDrawSphere(glm::vec3(position.x, -position.y, position.z), radius);
	}

	// Sphere signed distance function
	float sdf(const glm::vec3 &p) {
		return glm::distance(p, position) - radius;
	}

	float radius = 1.0;
}; // end class Sphere

//  Mesh class (will complete later- this will be a refinement of Mesh from Project 1)
//
class Mesh : public SceneObject {
	bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal) { return false; }
	void draw() { }
}; // Place Mesh


// General purpose plane
class Plane : public SceneObject {
public:
	Plane(glm::vec3 p, glm::vec3 n, float power, string image_filename, bool isTextured, ofColor diffuse = ofColor::darkOliveGreen, float w = 20, float h = 20) {
		position = p; normal = n;
		width = w;
		height = h;
		this->power = power;
		diffuseColor = diffuse;
		this->isTextured = isTextured;

		texture_ref = new ofImage();
		if (!texture_ref->load(image_filename)) {
			cerr << "Could not find image for plane at path: " << image_filename << endl;
		}
		if (normal == glm::vec3(0, -1, 0) || normal == glm::vec3(0, 1, 0))
			plane.rotateDeg(90, 1, 0, 0);
	}
	Plane() {
		normal = glm::vec3(0, 1, 0);
		plane.rotateDeg(90, 1, 0, 0);
	}

	bool intersect(const Ray &ray, glm::vec3 & point, glm::vec3 & normal);
	glm::vec3 getNormal(const glm::vec3 &p) { return this->normal; }
	void draw() {
		ofSetColor(diffuseColor);
		plane.setPosition(glm::vec3(position.x, -position.y, position.z));
		plane.setWidth(width);
		plane.setHeight(height);
		plane.setResolution(4, 4);
		//plane.setOrientation()

		plane.drawWireframe();
	}

	// Signed distance function for floor plane
	float sdf(const glm::vec3 & p) {
		return position.y - p.y;
	}

	ofPlanePrimitive plane;


	float width = 20;
	float height = 20;
	bool isTextured = false;
}; // class Plane


class Torus : public SceneObject {
public:
	Torus() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		rotate_amt = 45.0f;
		rotate_axis = glm::vec3(1.0f, 0.0f, 0.0f);
		t = glm::vec2(5.0f, 2.0f);
	}

	Torus(glm::vec3 p, float radius, float thickness, ofColor diffuse, float power) {
		position = p;
		diffuseColor = diffuse;
		this->power = power;
		rotate_amt = 45.0f;
		rotate_axis = glm::vec3(-0.7f, -0.3f, 0.0f);
		t = glm::vec2(radius, thickness);
	}

	void draw() {
		ofSetColor(diffuseColor);
		ofDrawSphere(glm::vec3(position.x, -position.y, position.z), 1);
	}

	// Signed distance function
	float sdf(const glm::vec3 &p1) {

		// Transform
		glm::mat4 m = glm::translate(glm::mat4(1.0), position);
		glm::mat4 M = glm::rotate(m, glm::radians(rotate_amt), rotate_axis);
		glm::vec3 p = glm::inverse(M) * glm::vec4(p1, 1);

		// Repeat
		glm::vec3 rep_period = glm::vec3(21, 21, 21);
		glm::vec3 p2 = glm::mod(p + 0.5 * rep_period, rep_period) - 0.5 * rep_period;

		// Torus
		glm::vec2 q = glm::vec2(glm::length(glm::vec2(p2.x, p2.z)) - t.x, p2.y);
		return glm::length(q) - t.y;
	}

	void setRotateAmt(const float &r) {
		rotate_amt = r;
	}

	void setRotateAxis(const glm::vec3 &ra) {
		rotate_axis = glm::normalize(ra);
	}

protected:
	float rotate_amt;
	glm::vec3 rotate_axis;
	glm::vec2 t;
}; // class Torus


class TwistedTorus : public Torus {
public:
	TwistedTorus(glm::vec3 p, float radius, float thickness, ofColor diffuse, float power) 
		: Torus(p, radius, thickness, diffuse, power) {
		k = 0.2f;
	}

	void draw() {
		ofSetColor(diffuseColor);
		ofDrawSphere(glm::vec3(position.x, -position.y, position.z), 2);
	}

	float sdf(const glm::vec3 &p) {

		// Transform 
		glm::mat4 m = glm::translate(glm::mat4(1.0), position);
		glm::mat4 M = glm::rotate(m, glm::radians(rotate_amt), rotate_axis);
		glm::vec3 p1 = glm::inverse(M) * glm::vec4(p, 1);

		// Twist
		float c = glm::cos(k * p.y);
		float s = glm::sin(k * p.y);
		glm::mat2 mr = glm::mat2(c, -s, s, c);
		glm::vec3 p2 = glm::vec3(mr * glm::vec2(p1.x, p1.z), p1.y);
		
		// Torus
		glm::vec2 q = glm::vec2(glm::length(glm::vec2(p2.x, p2.z)) - t.x, p2.y);
		return glm::length(q) - t.y;
	}

	void setTwist(float k) {
		this->k = k;
	}

protected:
	float k;

}; // class TwistedTorus


class TwistedRepeatedTorus : public TwistedTorus {
public:
	TwistedRepeatedTorus(glm::vec3 p, float radius, float thickness, ofColor diffuse, float power)
		: TwistedTorus(p, radius, thickness, diffuse, power) {}


	float sdf(const glm::vec3 &p) {

		// Transform 
		glm::mat4 m = glm::translate(glm::mat4(1.0), position);
		glm::mat4 M = glm::rotate(m, glm::radians(rotate_amt), rotate_axis);
		glm::vec3 p1 = glm::inverse(M) * glm::vec4(p, 1);

		// Repeat
		rep_period = glm::vec3(21, 21, 21);
		glm::vec3 p3 = glm::mod(p1 + 0.5*rep_period, rep_period) - 0.5*rep_period;

		// Twist
		float c = glm::cos(k * p.y);
		float s = glm::sin(k * p.y);
		glm::mat2 mr = glm::mat2(c, -s, s, c);
		glm::vec3 p2 = glm::vec3(mr * glm::vec2(p3.x, p3.z), p3.y);

		// Torus
		glm::vec2 q = glm::vec2(glm::length(glm::vec2(p2.x, p2.z)) - t.x, p2.y);
		return glm::length(q) - t.y;

	}

private:
	glm::vec3 rep_period;
}; // class TwistedRepeatedTorus