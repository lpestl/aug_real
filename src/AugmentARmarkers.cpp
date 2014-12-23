#include "AugmentARmarkers.h"

//--------------------------------------------------------------
void AugmentARmarkers::setup(int _width, int _height, int _threshold, 
							vector<string> _urlModels, 
							vector<float> _scaleModels, 
							vector<ofVec3f> _rotModels, 
							vector<int> _numMarkers, 
							vector<ofVec3f> _offset,
							vector<vector<ofVec3f>> _lights,
							vector<string> _info,
							bool _optimization){
	//some model / light stuff
	glShadeModel(GL_SMOOTH);

	optimization = _optimization;

	if (optimization) {
		img_w = 640;
		img_h = 480;
	}
	else {
		img_w = ofGetWindowWidth();
		img_h = ofGetWindowHeight();
	}

	colorImage.allocate(img_w, img_h);//_width, _height);
	grayImage.allocate(img_w, img_h);//_width, _height);
	grayThres.allocate(img_w, img_h);//_width, _height);
	// This uses the default camera calibration and marker file

	artk.setup(img_w, img_h);// _width, _height);
	//artk.setup(_width, _height, "myCamParamFile.cal", "markerboard_480-499.cfg");

	//artk.setup(img_w, img_h, "markerRealEye.pat");
	

	// Set the threshold
	// ARTK+ does the thresholding for us
	// We also do it in OpenCV so we can see what it looks like for debugging
	threshold = _threshold; // 85;
	artk.setThreshold(threshold);

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofDisableArbTex(); // we need GL_TEXTURE_2D for our models coords.

	countModels = _urlModels.size();
	models = new structModels[countModels];

	for (int i = 0; i < countModels; i++) {
		models[i].url = _urlModels.at(i);
		models[i].marker = _numMarkers.at(i);
		models[i].scale = _scaleModels.at(i);
		models[i].rotation = _rotModels.at(i);
		models[i].offset = _offset.at(i);
		models[i].lights = _lights.at(i);
		models[i].info.loadImage(_info.at(i));
		
		float aInfo = 255;
		vector<ofLight*> currLights;
		for (int j = 0; j < models[i].lights.size(); j++) {
			currLights.push_back(new ofLight());
		}
		lights.push_back(currLights);

		alphaInfo.push_back(aInfo);
		models[i].model.loadModel(models[i].url, true);
		models[i].model.setRotation(0, models[i].rotation.x, 1, 0, 0);
		models[i].model.setRotation(1, models[i].rotation.y, 0, 1, 0);
		models[i].model.setRotation(2, models[i].rotation.z, 0, 0, 1);
		models[i].model.setPosition(models[i].offset.x, models[i].offset.y, models[i].offset.z);
		models[i].model.setScale(models[i].scale, models[i].scale, models[i].scale);
	}

	//logoHL.loadImage("hiteklab_logo.png");
}

//--------------------------------------------------------------
void AugmentARmarkers::update(ofxCvColorImage _colorImage, bool _bNewFrame) {
	if (_bNewFrame) {
		colorImage.setFromPixels(_colorImage.getPixels(), _colorImage.getWidth(), _colorImage.getHeight());

		// convert our camera image to grayscale
		colorImage.resize(img_w, img_h);
		grayImage = colorImage;
		// apply a threshold so we can see what is going on
		grayThres = grayImage;
		grayThres.threshold(threshold);

		artk.setThreshold(threshold);
		// Pass in the new image pixels to artk
		artk.update(grayImage.getPixels());
	}

	for (int i = 0; i < countModels; i++) {
		models[i].model.update();
	}

}

void AugmentARmarkers::draw() {
	ofSetupScreen();
	glPushMatrix();

	// ARTK draw
	// An easy was to see what is going on
	// Draws the marker location and id number
	//artk.draw(0, 0);

	// ARTK 3D stuff
	// This is another way of drawing objects aligned with the marker
	// First apply the projection matrix once
	artk.applyProjectionMatrix();
	// Find out how many markers have been detected
	int numDetected = artk.getNumDetectedMarkers();
	ofEnableAlphaBlending();
	// Draw for each marker discovered
	for (int i = 0; i<numDetected; i++) {
		for (int j = 0; j < countModels; j++) {
			if (artk.getMarkerIndex(models[j].marker) == i) {
				/// Set the matrix to the perspective of this marker
				// The origin is in the middle of the marker	
				ofMatrix4x4 mat = artk.getMatrix(i);
				artk.applyModelMatrix(i);

				// Draw a line from the center out
				ofNoFill();
				ofSetLineWidth(5);
				ofSetHexColor(0xffffff);
				//		glBegin(GL_LINES);
				//		glVertex3f(0, 0, 0); 
				//		glVertex3f(0, 0, 50);
				//		glEnd();

				// Draw a stack of rectangles by offseting on the z axis
				ofNoFill();
				ofEnableSmoothing();
				ofSetColor(255, 255, 255, 255);
		
				ofEnableBlendMode(OF_BLENDMODE_ALPHA);

				glShadeModel(GL_SMOOTH); //some model / light stuff
				for (int l = 0; l < models[j].lights.size(); l++) {
					lights[j].at(l)->enable();
					lights[j].at(l)->setGlobalPosition(models[j].lights.at(l).x, models[j].lights.at(l).y, models[j].lights.at(l).z);
				}

				glDisable(GL_COLOR_MATERIAL);
				//ofEnableSeparateSpecularLight();
				
				/*ofPushMatrix();
				logoHL.draw(22, -22, -44, 44);
				ofPopMatrix();*/

				ofPushMatrix();
				ofEnableDepthTest();
				models[j].model.drawFaces(); // model.drawFaces();
				//printf("%.3f %.3f %.3f\n--------------\n", models[j].model.getPosition()[0], models[j].model.getPosition()[1], models[j].model.getPosition()[2]);
				ofDisableDepthTest();
				ofPopMatrix();			


				if (ofGetGLProgrammableRenderer()){
					glPushAttrib(GL_ALL_ATTRIB_BITS);
					glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
				}
				glEnable(GL_NORMALIZE);
		
				if (ofGetGLProgrammableRenderer()){
					glPopAttrib();
				}

				glEnable(GL_COLOR_MATERIAL);
				for (int l = 0; l < models[j].lights.size(); l++) {
					lights[j].at(l)->disable();
				}
				ofDisableLighting();
				//ofDisableSeparateSpecularLight();
			}
			else {
				// Set the matrix to the perspective of this marker
				// The origin is in the middle of the marker	
				artk.applyModelMatrix(i);

				// Draw a line from the center out
				ofNoFill();
				ofSetLineWidth(5);
				ofSetHexColor(0xffffff);
				//		glBegin(GL_LINES);
				//		glVertex3f(0, 0, 0); 
				//		glVertex3f(0, 0, 50);
				//		glEnd();

				// Draw a stack of rectangles by offseting on the z axis
				ofNoFill();
				ofEnableSmoothing();
				ofSetColor(255, 255, 0, 50);
				ofEnableBlendMode(OF_BLENDMODE_ALPHA);

				ofPushMatrix();
				ofEnableDepthTest();

				for (int l = 0; l<10; l++) {
					ofRect(-25, -25, 50, 50);
					ofTranslate(0, 0, l * 1);
				}
				
				ofDisableDepthTest();
				ofPopMatrix();
			}
		}
	}
	//artk.applyProjectionMatrix(ofGetWindowWidth(), ofGetWindowHeight());
	glPopMatrix();

	ofSetupScreen();

	//for (int i = 0; i < numDetected; i++) {
		for (int j = 0; j < countModels; j++) {
			if ((artk.getMarkerIndex(models[j].marker) == (numDetected - 1)) && ((numDetected - 1) != -1)) {
				ofSetColor(255, 255, 255, alphaInfo.at(j));
				models[j].info.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
			}
		}
	//}
}

//--------------------------------------------------------------
void AugmentARmarkers::draw(int x, int y, int w, int h){
	
}

ofxCvGrayscaleImage AugmentARmarkers::getGreyThresImage() {
	return grayThres;
}

AugmentARmarkers::~AugmentARmarkers() {

	/*for (int i = lights.size() - 1; i >= 0; i--) {
		delete[] lights.at(i);
	}*/

	delete[] models;
}

int AugmentARmarkers::getThreshold() {
	return threshold;
}

void AugmentARmarkers::setThreshold(int _newThreshold) {
	threshold = _newThreshold;
}

int AugmentARmarkers::getNumDetectedMarkers() {
	return artk.getNumDetectedMarkers();
}