#pragma once

#include "ofMain.h"
#include "ofxGoogleAnalytics.h"

class ofApp : public ofBaseApp{
public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y){};

	ofxGoogleAnalytics ga;

	int currentScreen;

};
