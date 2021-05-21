// Author: Ben Foley
// Starting code provided by Kevin Smith

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(background_color);
	//cam.setDistance(30);
	camOfRender.setPosition(ray_tracer.render_cam.position);
	camOfRender.lookAt(ray_tracer.render_cam.aim);
	camOfRender.setNearClip(.1);
	
	// Set easycam
	camRef = &camOfRender;

	// Turn shadows on or off
	ray_tracer.setShadow(false);
	
	// Set scene objects
	//spheres.push_back(Sphere(glm::vec3(0.0f, 0.0f, -40.0f), 7.0, ofColor::blue, 500.0f));
	//spheres.push_back(Sphere(glm::vec3(0.0f, 3.0f, -29.0f), 5.0, ofColor::red, 600.0f));
	//spheres.push_back(Sphere(glm::vec3(0.0f, 3.0f, -27.0f), 5.0, ofColor::red, 600.0f));
	//spheres.push_back(Sphere(glm::vec3(-6.0f, 2.0f, -23.0f), 4.0, ofColor::green, 400.0f));
	//planes.push_back(Plane(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 500, "../../textures/wood.jpg", false, ofColor::lightCoral, 100.0, 200.0));

	//tori.push_back(Torus(glm::vec3(15.0f , 1.0f, -30.0f), 6.0, 3.0, ofColor::paleGreen, 500.0f));
	//tori.back().setRotateAxis(glm::vec3(-0.7f, 0.0f, -0.3f));
	

	//t_tori.push_back(TwistedTorus(glm::vec3(-13.0f, 1.0f, -25.0f), 5.0, 2.0, ofColor::aquamarine, 500.0f));
	//t_tori.back().setRotateAmt(90);
	//t_tori.back().setRotateAxis(glm::vec3(0.0f, 0.5f, 0.5f));
	//t_tori.back().setTwist(0.35f);
	

	//tori.push_back(Torus(glm::vec3(0.0f, -1.5f, -25.0f), 4.0, 2.0, ofColor::paleGreen, 500.0f));
	//tori.push_back(Torus(glm::vec3(15.0f, -1.5f, -25.0f), 4.0, 2.0, ofColor::paleGreen, 500.0f));
	//tori.back().setRotateAxis(glm::vec3(-0.7f, 0.0f, -0.3f));


	//t_tori.push_back(TwistedTorus(glm::vec3(0.0f, -1.5f, -25.0f), 4.0, 2.0, ofColor::aquamarine, 500.0f));
	//t_tori.back().setTwist(10.0f);
	//t_tori.back().setTwist(0.3f);

	tr_tori.push_back(TwistedRepeatedTorus(glm::vec3(-4.0f, -1.5f, -25.0f), 4.0f, 2.0f, ofColor::aquamarine, 500.0f));
	//tr_tori.back().setRotateAmt(90.0f);
	//tr_tori.back().setTwist(1.0f);
	tr_tori.back().setTwist(0.2f);


	
	//spheres.push_back(Sphere(glm::vec3(8, 3, -7), 5.0, ofColor::purple, 600));

	
	//planes.push_back(Plane(glm::vec3(0, -100, 0), glm::vec3(0, 1, 0), 500, "../../textures/stone.jpg", ofColor::blue, 100.0, 200.0));
	//planes.push_back(Plane(glm::vec3(0, 0, -50), glm::vec3(0, 0, 1), 500, "../../textures/stone.jpg", true, ofColor::lightCoral, 30.0, 10.0));
	//planes.push_back(Plane(glm::vec3(30, 0, -50), glm::vec3(-1, 0, 0), 500, "../../textures/sample1.jpg", true, ofColor::lightCoral, 30.0, 50.0));
	//planes.push_back(Plane(glm::vec3(-30, 0, -40), glm::vec3(0.3, 0, 0.7), 500, "../../textures/stone.jpg", true, ofColor::lightCoral, 70.0, 50.0));
	//planes.push_back(Plane(glm::vec3(30, 0, -40), glm::vec3(-0.3, 0, 0.7), 500, "../../textures/stone.jpg", true, ofColor::lightCoral, 70.0, 50.0));
	//planes.push_back(Plane(glm::vec3(0, 10, 10), glm::vec3(0, 0, -1), 500, "../../textures/wood.jpg", false, ofColor::white, 100.0, 200.0));


	//luminaires.push_back(Luminaire(glm::vec3(-25, -30, -17), 10, 20));
	//luminaires.push_back(Luminaire(glm::vec3(-10, -5, -25), 10));
	//luminaires.push_back(Luminaire(glm::vec3(10, -5, -25), 10));


	// for ray marcher
	//lights.push_back(Light(glm::vec3(15, -7, -27), intensity));	
	//lights.push_back(Light(glm::vec3(-5, -5, -17), intensity));
	//lights.push_back(Light(glm::vec3(10, -5, -24), intensity));
	
	//lights.push_back(Light(glm::vec3(0.0, -7.0, -10.0), intensity));
	lights.push_back(Light(glm::vec3(0.0, 0.0, 5.0), intensity));


	
	// midlight
	//cone_lights.push_back(ConeLight(glm::vec3(0, -20, -25), 500, glm::vec3(0, -1, 0), 40.0, 40.0));
	//cone_lights.push_back(ConeLight(glm::vec3(0, -30, -30), 1000, glm::vec3(0, -1, 0), 40.0, 44.0));


	// back left light
	//cone_lights.push_back(ConeLight(glm::vec3(-26, -20, -50), 3000, glm::vec3(0, -1, 0), 50.0, 35.0));

	// side cone light
	//cone_lights.push_back(ConeLight(glm::vec3(-17, 5.5, -20), 2000, glm::vec3(0.9, 0, -0.4), 40.0, 35.0));



	// Add spheres to raytraced scene
	if (!spheres.empty()) {
		for (auto &sphere : spheres) {
			ray_tracer.addSceneObject(&sphere);
		}
	}

	// Add plane
	if (!planes.empty()) {
		for (auto &plane : planes) {
			ray_tracer.addSceneObject(&plane);
		}
	}

	// Add torus
	if (!tori.empty()) {
		for (auto &torus : tori) {
			ray_tracer.addSceneObject(&torus);
		}
	}

	// Add Twisted Torus
	if (!t_tori.empty()) {
		for (auto &t_torus : t_tori) {
			ray_tracer.addSceneObject(&t_torus);
		}
	}

	// Add Twisted Repeated Torus
	if (!tr_tori.empty()) {
		for (auto &tr_torus : tr_tori) {
			ray_tracer.addSceneObject(&tr_torus);
		}
	}


	// Add lights
	if (!lights.empty()) {
		for (auto &light : lights) {
			ray_tracer.addLight(&light);
		}
	}

	// Cone lights
	if (!cone_lights.empty()) {
		for (auto &clight : cone_lights) {
			ray_tracer.cone_refs.push_back(&clight);
		}
	}

	// Luminaire for path tracing
	if (!luminaires.empty()) {
		for (auto &lumin : luminaires) {
			ray_tracer.addSceneObject(&lumin);
		}
	}

	// Set up gui
	gui.setup();
	gui.add(pathOn.setup("Pathtrace(On)/DOF(Off)", false));
	gui.add(trace_bounces.setup("Pathtrace Bounces", 10, 1, 10000));
	gui.add(dof_samples.setup("DOF samples", 180, 50, 10000));
	gui.add(focal_distance.setup("Focal Distance", 37, 10, 100));
	gui.add(apeture_size.setup("Apeture Size", 0.3, 0.1, 2.0));
	
}


//--------------------------------------------------------------
void ofApp::update(){
	ray_tracer.path_trace = pathOn;
	ray_tracer.max_depth = trace_bounces;
	ray_tracer.focal_dist = focal_distance;
	ray_tracer.dof_samples = dof_samples;
	ray_tracer.apeture_size = apeture_size;
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	camRef->begin();
	ofNoFill();

	// Draw focal distance
	ofDrawSphere(glm::vec3(0, 0, 20), 3.0f);

	// Draw Scene objects
	for (auto s : ray_tracer.getSceneObjects()) {
		s->draw();
	}

	// Draw light position representation as spheres
	for (auto l : lights) {
		l.draw();
	}

	// Draw cone lights
	for (auto cl : cone_lights) {
		cl.draw();
	}

	ray_tracer.render_cam.view.draw();

	camRef->end();

	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case OF_KEY_F1:
		camRef = &easyCam;
		break;
	case OF_KEY_F2:
		camRef = &camOfRender;
		break;
	case 'r':
	case 'R':
		// Render scene when r is pressed
		ray_tracer.render();
		isRendered = true;
		cout << "rendered in images folder" << endl;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
