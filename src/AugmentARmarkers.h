#ifndef _AUGMENT_AR_MARKERS_
#define _AUGMENT_AR_MARKERS_

#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"
//#include "ofx3DModelLoader.h"
#include "ofxAssimpModelLoader.h"
// Basics
#include "ofMain.h"
#include "StructModels.h"

class AugmentARmarkers {

public:
	~AugmentARmarkers();
	void				setup(int _width, int _height, int _threshold, 
								vector<string> _urlModels, 
								vector<float> _scaleModels, 
								vector<ofVec3f> _rotModels, 
								vector<int> _numMarkers, 
								vector<ofVec3f> _offset, 
								vector<vector<ofVec3f>> _lights,
								vector<string> _info,
								bool _optimization);

	void				update(ofxCvColorImage _colorImage, bool _bNewFrame);
	void				draw(int x, int y, int w, int h);
	void				draw();
	ofxCvGrayscaleImage getGreyThresImage();
	int					getThreshold();
	void				setThreshold(int _newThreshold);
	int					getNumDetectedMarkers();

private:
	/* ARToolKitPlus class */
	ofxARToolkitPlus artk;
	int threshold;

	/* OpenCV images */
	ofxCvColorImage colorImage;
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage	grayThres;
	//ofImage		logoHL;

	structModels *models;
	int countModels;
	bool optimization;
	int img_w;
	int img_h;

	vector< vector< ofLight* > > lights;
	vector<float> alphaInfo;
};

#endif