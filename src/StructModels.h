#ifndef _STRUCT_MODELS_H_
#define _STRUCT_MODELS_H_

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

struct structModels {
	std::string url;
	ofxAssimpModelLoader model;
	float scale;
	ofVec3f rotation;
	ofVec3f offset;
	vector<ofVec3f> lights;
	int marker;
	ofImage info;
};

#endif