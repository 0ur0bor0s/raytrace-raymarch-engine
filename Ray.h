// Author: Ben Foley
// Starting code provided by Kevin Smith


#pragma once


//  General Purpose Ray class 
//
class Ray {
public:
	Ray(glm::vec3 p, glm::vec3 d) { this->p = p; this->d = d; }
	//void draw(float t) { ofDrawLine(p, p + t * d); }
	void draw(float t) {
		glm::vec3 pos = glm::vec3(p.x, -p.y, p.z);
		glm::vec3 dir = glm::vec3(d.x, -d.y, d.z);
		ofDrawLine(pos, pos + t * dir);
	}

	glm::vec3 evalPoint(float t) {
		return (p + t * d);
	}

	glm::vec3 p, d;
};