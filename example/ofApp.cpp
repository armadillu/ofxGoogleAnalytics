#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(60);
	
	//add listener to GoogleAnalytics, to get feedback
	ofAddListener(ga.gaResponse, this, &ofApp::googleAnalyticsResponse);

	ga.setup("UA-51706745-1",				//google track ID
			 "ofxGoogleAnalyticsExample",	//app name
			 "0.1",							//app version
			 "myAppID",						//ap id
			 "myAppInstallerID"				//app installer id
			 );
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
	ga.draw(30,30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
		case '1': ga.sendScreenView("screen1"); break;
		case '2': ga.sendScreenView("screen2"); break;
		case '3': ga.sendScreenView("screen3"); break;
		case '4': ga.sendException("Exception1", false); break;
		case '5': ga.sendException("Exception2", false); break;
		case '6': ga.sendException("Exception3", true); break;
		case '7': ga.sendEvent("KeyboardEvent", "pressed7", key, "someLabel"); break;
		case '8': ga.sendEvent("KeyboardEvent", "pressed8", key, "someLabel"); break;
		case '9': ga.sendEvent("KeyboardEvent", "pressed9", key, "someLabel"); break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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