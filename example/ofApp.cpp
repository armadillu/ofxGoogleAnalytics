#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(22);
	ofSetFrameRate(60);

	currentScreen = 1;

	ga = new ofxGoogleAnalytics();

	//add listener to GoogleAnalytics, to get feedback
	ofAddListener(ga->gaResponse, this, &ofApp::googleAnalyticsResponse);


	ga->setup("UA-51706745-1",				//google track ID
			 "ofxGoogleAnalyticsExample",	//app name
			 "0.1",							//app version
			 "myAppID",						//ap id
			 "myAppInstallerID"				//app installer id
			 );

	ga->setShouldReportFramerates(true); //send timing tracking info
	ga->setFramerateReportInterval(5); //every 60 sec, report app's framerate

	ga->setUserID("armadillu"); //you can set a random user string for the GA session

}

//get feedback on our request
void ofApp::googleAnalyticsResponse(ofxGoogleAnalytics::AnalyticsResponse &r){

}

//--------------------------------------------------------------
void ofApp::update(){

	ga->update();
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofColor color;
	color.r = (currentScreen == 1 ? 60 : 0);
	color.g = (currentScreen == 2 ? 60 : 0);
	color.b = (currentScreen == 3 ? 60 : 0);
	ofBackground(color);

	ga->draw(30,30);

	ofSetColor(200);
	ofDrawBitmapString("Screen: " + ofToString(currentScreen) + "\n"
					   "press 1-3 to report different screenViews\n"
					   "press 4-6 to report Excpetions\n"
					   "press 7-9 to report Events\n"
					   "press q,a,z to report PageViews\n"
					   "press SPACEBAR to report a simple benchmark\n",
					   20, ofGetHeight() - 84);
}


void ofApp::exit(){
	delete ga; //deleting the GA object closes the current session
	ga = NULL;
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
		case '1': ga->sendScreenView(currentScreenS); break;
		case '2': ga->sendScreenView(currentScreenS); break;
		case '3': ga->sendScreenView(currentScreenS); break;
		case '4': ga->sendException("Exception1", false); break;
		case '5': ga->sendException("Exception2", false); break;
		case '6': ga->sendException("Exception3", true); break;
		case '7': ga->sendEvent("KeyboardEvent", "pressed7", key, "someLabel"); break;
		case '8': ga->sendEvent("KeyboardEvent", "pressed8", key, "someLabel"); break;
		case '9': ga->sendEvent("KeyboardEvent", "pressed9", key, "someLabel"); break;
		case 'q': ga->sendPageView("keyboardKeys/row1/q", "my page for Q key"); break;
		case 'a': ga->sendPageView("keyboardKeys/row2/a", "my page for A key"); break;
		case 'z': ga->sendPageView("keyboardKeys/row3/z", "my page for Z key"); break;

		case ' ':{
			//measure how long it takes to calc 9000000 sinf()'s
			float t = ofGetElapsedTimef();
			for(int i = 0; i < 9000000; i++){
				float r = sinf(i);
			}
			t = ofGetElapsedTimef() - t;
			//send that timing
			ga->sendCustomTimeMeasurement("SimpleBenchMark", "9000000 sinf()", t * 1000);
		}break;
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