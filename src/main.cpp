#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGlutWindow.h"
#include "wtypes.h"
#include <iostream>
#include "ofxXmlSettings.h"
#include <locale>
#include "ofxOpenCv.h"

using namespace cv;

//========================================================================
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);

	horizontal = desktop.right;
	vertical = desktop.bottom;
}
//========================================================================
int main(){
	setlocale(LC_ALL, "Russian");

	ofxXmlSettings XML;
	XML.loadFile("settings.xml");
	XML.pushTag("settings");

	int winMode = XML.getValue("window", 0);

	int wdth = XML.getAttribute("window", "width", 640);
	int hght = XML.getAttribute("window", "height", 420);
	//GetDesktopResolution(wdth, hght);
	XML.popTag();

	ofAppGlutWindow window;

	switch (winMode) {
	case 0:
		GetDesktopResolution(wdth, hght);
		ofSetupOpenGL(&window, wdth, hght, OF_FULLSCREEN);			// <-------- setup the GL context
		break;
	case 1:
		ofSetupOpenGL(&window, wdth, hght, OF_WINDOW);					// <-------- setup the GL context
		break;
	default:
		ofSetupOpenGL(&window, wdth, hght, OF_WINDOW);			// <-------- setup the GL context
	}

	ofSetWindowTitle("Aug_real");

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
