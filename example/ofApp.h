#pragma once

#include "ofMain.h"
#include "ofxGoogleAnalytics.h"
#include "ofxRemoteUIServer.h"
#include "ofxTimeMeasurements.h"

class ofApp : public ofBaseApp{
public:

	void setup();
	void update();
	void draw();

	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxGoogleAnalytics * ga;

	int currentScreen;

	void googleAnalyticsResponse(ofxGoogleAnalytics::AnalyticsResponse &response);

	void remoteUIClientDidSomething(RemoteUIServerCallBackArg & arg);

	bool sendScreenViews;
	bool sendExceptions;
	bool sendEvents;
	bool sendPage;
	bool sendFramerate;

	bool randomizeUUID;

	float time ;
	float timeRandomness;

	float sendInterval;
	int maxRequestsPerSession;
	float sendToGoogleRate;
};
