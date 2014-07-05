#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(22);
	ofSetFrameRate(60);

	currentScreen = 1;

	//add listener to GoogleAnalytics, to get feedback
	ofAddListener(ga.gaResponse, this, &ofApp::googleAnalyticsResponse);

	ga.setup("UA-51706745-1",				//google track ID
			 "ofxGoogleAnalyticsExample",	//app name
			 "0.1",							//app version
			 "myAppID",						//ap id
			 "myAppInstallerID"				//app installer id
			 );

	ga.setShouldReportFramerates(true); //send timing tracking info
	ga.setFramerateReportInterval(5); //every 60 sec, report app's framerate
}

//get feedback on our request
void ofApp::googleAnalyticsResponse(ofxGoogleAnalytics::AnalyticsResponse &r){

}

//--------------------------------------------------------------
void ofApp::update(){

	ga.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofColor color;
	color.r = (currentScreen == 1 ? 60 : 0);
	color.g = (currentScreen == 2 ? 60 : 0);
	color.b = (currentScreen == 3 ? 60 : 0);
	ofBackground(color);

	ga.draw(30,30);

	ofSetColor(200);
	ofDrawBitmapString("Screen: " + ofToString(currentScreen) + "\n"
					   "press 1-3 to report different screenViews\n"
					   "press 4-6 to report Excpetions\n"
					   "press 7-9 to report Events\n",
					   20, ofGetHeight() - 54);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	//update current screen
	switch (key) {
		case '1': currentScreen = 1; break;
		case '2': currentScreen = 2; break;
		case '3': currentScreen = 3; break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	string currentScreenS = "screen"+ofToString(currentScreen);
	switch (key) {
		case '1': ga.sendScreenView(currentScreenS); break;
		case '2': ga.sendScreenView(currentScreenS); break;
		case '3': ga.sendScreenView(currentScreenS); break;
		case '4': ga.sendException("Exception1", false); break;
		case '5': ga.sendException("Exception2", false); break;
		case '6': ga.sendException("Exception3", true); break;
		case '7': ga.sendEvent("KeyboardEvent", "pressed7", currentScreenS, key, "someLabel"); break;
		case '8': ga.sendEvent("KeyboardEvent", "pressed8", currentScreenS,  key, "someLabel"); break;
		case '9': ga.sendEvent("KeyboardEvent", "pressed9", currentScreenS, key, "someLabel"); break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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