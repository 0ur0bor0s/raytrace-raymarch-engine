// Author: Ben Foley
// Starting code provided by Kevin Smith

#pragma once


#include "ofMain.h"
#include "Ray.h"
#include "ofxGui.h"
#include "SceneObjects.h"
#include "CamObjects.h"
#include "LightObjects.h"
#include "RayTracer.h"
//#include "glm/gtx/intersect.hpp"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofEasyCam easyCam;
		ofCamera camOfRender;
		ofCamera *camRef;

		deque<Sphere> spheres;
		deque<Plane> planes;
		deque<Light> lights;
		deque<Luminaire> luminaires;
		deque<ConeLight> cone_lights;
		deque<Torus> tori;
		deque<TwistedTorus> t_tori;
		deque<TwistedRepeatedTorus> tr_tori;

		float intensity = 500;
		//float intensity = 100;

		ofColor background_color = ofColor::black;
		RayTracer ray_tracer;

		bool isRendered = false;

		// Gui
		ofxPanel gui;
		ofxToggle pathOn;
		ofxIntSlider trace_bounces;
		ofxIntSlider dof_samples;
		ofxFloatSlider focal_distance;
		ofxFloatSlider apeture_size;
		
};
