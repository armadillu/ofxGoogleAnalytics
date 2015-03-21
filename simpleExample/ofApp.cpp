#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(22);
	ofSetFrameRate(60);
	ofSetVerticalSync(false);

	currentScreen = 1;

	ga.setShouldReportFramerates(true); //send App framerate info as timing events?

	ga.setFramerateReportInterval(600); //if enabled, report app's framerate every 600 sec

	//ga.setUserID("armadillu"); //you can set a random user string for the GA session
								//this will not play nice with randomizing UUID

	ga.setEnabled(true); //enable or disable entirely GA

	//ga.setCustomUserAgent("My Custom User Agent"); //set a custom user agent if you want

	ga.setRandomizeUUID(false); //make each new session be a different user?

	ga.setSendToGoogleInterval(0.0); //your can throttle down how often you send data, just in case

	ga.setup("UA-51706745-1",				//google track ID << REPLACE WITH YOURS!
			 "ofxGoogleAnalytics SimpleExample",	//app name
			 "v1",									//app version
			 "mySimpleAppID",						//ap id
			 "mySimpleAppInstallerID"				//app installer id
			 );

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

	ga.draw(30,40);

	ofSetColor(200);
	ofDrawBitmapString("Screen: " + ofToString(currentScreen) + "\n"
					   "press 1-3 to report different screenViews\n"
					   "press 4-6 to report Exceptions\n"
					   "press 7-9 to report Events\n"
					   "press q,a,z to report PageViews\n",
					   20, ofGetHeight() - 84);
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
		case '7': ga.sendEvent("KeyboardEvent", "pressed7", key, "someLabel"); break;
		case '8': ga.sendEvent("KeyboardEvent", "pressed8", key, "someLabel"); break;
		case '9': ga.sendEvent("KeyboardEvent", "pressed9", key, "someLabel"); break;
		case 'q': ga.sendPageView("keyboardKeys/row1/q", "my page for Q key"); break;
		case 'a': ga.sendPageView("keyboardKeys/row2/a", "my page for A key"); break;
		case 'z': ga.sendPageView("keyboardKeys/row3/z", "my page for Z key"); break;
	}
}
