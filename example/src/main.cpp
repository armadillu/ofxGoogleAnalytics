#include "ofMain.h"
#include "ofApp.h"
#include "ofxTimeMeasurements.h"
//========================================================================
int main( ){

	ofSetupOpenGL(600,400, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:

	TIME_SAMPLE_DISABLE_AVERAGE();

	ofRunApp( new ofApp());

}
