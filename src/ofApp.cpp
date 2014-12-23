#include "ofApp.h"

using namespace ofxCv;
using namespace cv;
//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofEnableSmoothing();

	XML.loadFile("settings.xml");
	XML.pushTag("settings");

	string str_debug = XML.getValue("debug", "NO");
	if (str_debug == "YES") debug_mode = true;
	else debug_mode = false;

	camWidth = 640;	// try to grab at this size. 
	camHeight = 480;
	
	//we can now get back a list of devices. 
	camGrabber = new ofVideoGrabber();
	devices = camGrabber->listDevices();
	for (int i = 0; i < devices.size(); i++){
		cout << devices[i].id << ": " << devices[i].deviceName;
		if (devices[i].bAvailable){
			cout << endl;
		}
		else{
			cout << " - unavailable " << endl;
		}
	}
	cout << "End list cameras." << endl << endl;

	setupARmarker();

	drawPadding = false;

	gui = new ofxUICanvas();
	gui->addLabel("Camera", OFX_UI_FONT_LARGE);
	gui->addSpacer();

	for (int i = 0; i < devices.size(); i++){
		gui->addLabelButton(/*ofToString(devices[i].id) + ": " + */devices[i].deviceName, false, true);
	}

	gui->autoSizeToFitWidgets();
	ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);

	numCam = XML.getValue("camera", -1);
	if ((numCam >= 0) && (numCam < devices.size())) {
		camGrabber = new ofVideoGrabber();
		camGrabber->setDeviceID(numCam);
		camGrabber->setDesiredFrameRate(60);
		camGrabber->setVerbose(true);
		camGrabber->initGrabber(camWidth, camHeight);
	}

	if (!debug_mode) {
		gui->toggleVisible();
	}

	camCvFrame.allocate(camWidth, camHeight);

	XML.popTag();
}

void ofApp::setupARmarker() {
	//cout << "Intro setup 2" << endl;
	vector<string> urlModels;
	vector<float> scaleModels;
	vector<ofVec3f> rotModels;
	vector<int> numMarkers;
	vector<ofVec3f> offset;
	vector<vector<ofVec3f>> lights;
	vector<string> infos;

	correct_mode = false;

	XML.pushTag("models");

	for (int j = 0; j < XML.getNumTags("model"); j++) {
		int marker = XML.getAttribute("model", "marker", -1, j);

		XML.pushTag("model", j);
		string url = XML.getValue("url", " ");
		float scale = XML.getValue("scale", 1.0);
		ofVec3f rot;
		rot.set(XML.getAttribute("rotation", "x", 0), XML.getAttribute("rotation", "y", 0), XML.getAttribute("rotation", "z", 0));
		ofVec3f off;
		off.set(XML.getAttribute("offset", "x", 0), XML.getAttribute("offset", "y", 0), XML.getAttribute("offset", "z", 0));

		vector<ofVec3f> light;
		for (int i = 0; i < XML.getNumTags("light"); i++) {
			ofVec3f pos;
			pos.set(XML.getAttribute("light", "x", 0, i), XML.getAttribute("light", "y", 0, i), XML.getAttribute("light", "z", 0, i));
			light.push_back(pos);
		}
		string info = XML.getValue("info", "models\\info.png");

		infos.push_back(info);
		urlModels.push_back(url);
		scaleModels.push_back(scale);
		rotModels.push_back(rot);
		numMarkers.push_back(marker);
		offset.push_back(off);
		lights.push_back(light);

		XML.popTag();
	}

	XML.popTag();

	int th = XML.getValue("threshold", 85);
	string optimizStr = XML.getValue("optimization", "NO");
	bool opt = false;
	if (optimizStr == "YES") opt = true;

	markerAR.setup(camCvFrame.getWidth(), camCvFrame.getHeight(), th, urlModels, scaleModels, rotModels, numMarkers, offset, lights, infos, opt);
}

//--------------------------------------------------------------
void ofApp::update(){
	camGrabber->update();
	if (camGrabber->isFrameNew()) {
		//int totalPixels = camWidth*camHeight * 3;
		//pixels = camGrabber->getPixels();
		//for (int i = 0; i < totalPixels; i++){
		//	camCvFrame[i] = 255 - pixels[i];
		//}
		camCvFrame.setFromPixels(camGrabber->getPixels(), camWidth, camHeight);// , GL_RGB);
	}

	markerAR.update(camCvFrame, camGrabber->isFrameNew());

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofEnableAlphaBlending();
	// Prepare screen
	ofSetupScreen();
	// Start blend
	glEnable(GL_BLEND);
	// Blending function for everything (NB: happens in parent class)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//ofPushMatrix();
	ofSetHexColor(0xffffff);

	camCvFrame.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	if (correct_mode) {
		ofxCvGrayscaleImage img = markerAR.getGreyThresImage();
		img.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	}
	
	//glBegin(GL_POINTS);

	markerAR.draw();

	glEnd();
	// END TO DO.
	ofSetupScreen();

	ofPushMatrix();
	//gui.draw();
	if (debug_mode) {
		int m_fps = ofGetFrameRate();
		if (m_fps >= 30) ofSetColor(0, 255, 0);
		else if (m_fps >= 15) ofSetColor(255, 255, 0);
		else ofSetColor(255, 0, 0);
		ofDrawBitmapString(ofToString(m_fps) + " fps", ofGetWindowWidth() - 75, 15);

		ofSetColor(255, 255, 255);
		ofDrawBitmapString("Debug mode", ofGetWindowWidth() - 100, ofGetWindowHeight() - 15);
		
		ofSetHexColor(0xFFFF00);
		ofDrawBitmapString("Threshold: " + ofToString(markerAR.getThreshold()), ofGetWindowWidth() - 130, 35);

		ofSetHexColor(0x666666);
		ofDrawBitmapString(ofToString(markerAR.getNumDetectedMarkers()) + " marker(s) found", ofGetWindowWidth() - 150, 55);
	}

	ofSetColor(255, 255, 255);
	ofDrawBitmapString("DEVELOPMENT BUILD", 15, ofGetWindowHeight() - 15);
	ofPopMatrix();
	// End blend
	glDisable(GL_BLEND);
}

void ofApp::exit() {
	delete gui;
}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();

	if (kind == OFX_UI_WIDGET_LABELBUTTON)
	{
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		cout << name << "\t value: " << button->getValue() << endl;
		
		for (int i = 0; i < devices.size(); i++) {
			if (name == devices[i].deviceName) {
				delete camGrabber;
				camGrabber = new ofVideoGrabber();
				camGrabber->setDeviceID(i);
				camGrabber->setDesiredFrameRate(60);
				camGrabber->setVerbose(true);
				camGrabber->initGrabber(camWidth, camHeight);
				
				numCam = i;
				XML.pushTag("settings");
				XML.setValue("camera", numCam);
				XML.popTag();
				XML.saveFile();
				break;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if ((key == 'T') || (key == 't')) {
		correct_mode = !correct_mode;
	}

	if (key == OF_KEY_UP) {
		markerAR.setThreshold(markerAR.getThreshold() + 1);
	}
	else if (key == OF_KEY_DOWN) {
		markerAR.setThreshold(markerAR.getThreshold() - 1);
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
