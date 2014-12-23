#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxXmlSettings.h"
#include "ofxUI.h"
#include "AugmentARmarkers.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void setupARmarker();
		void guiEvent(ofxUIEventArgs &e);

		bool debug_mode;

		// Ãנאבבונ הכÿ ךאלונû
		ofVideoGrabber			*camGrabber;
		ofxCvColorImage			camCvFrame;

		vector<ofVideoDevice>	devices;
		int						numCam;

		ofxUICanvas *gui;
		bool drawPadding;
		
		int					camWidth;
		int					camHeight;
		
		ofxXmlSettings XML;
		AugmentARmarkers markerAR;
		bool correct_mode;
};
