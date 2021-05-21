// Author: Ben Foley
// Starting code provided by Kevin Smith


#include "ofApp.h"
#include "SceneObjects.h"

// Intersect Ray with Plane  (wrapper on glm::intersect*
//
bool Plane::intersect(const Ray &ray, glm::vec3 & point, glm::vec3 & normalAtIntersect) {
	float dist;
	bool insidePlane = false;
	bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal, dist);
	if (hit) {
		Ray r = ray;
		point = r.evalPoint(dist);
		normalAtIntersect = this->normal;

		glm::vec2 xrange = glm::vec2(position.x - width / 2, position.x + width / 2);
		glm::vec2 zrange = glm::vec2(position.z - height / 2, position.z + height / 2);
		if (point.x < xrange[1] && point.x > xrange[0] && point.z < zrange[1] && point.z > zrange[0]) {
			insidePlane = true;
		}
	}
	return insidePlane;
}

