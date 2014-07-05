//
//  ofxGoogleAnalytics.h
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 04/07/14.
//
//

#ifndef __emptyExample__ofxGoogleAnalytics__
#define __emptyExample__ofxGoogleAnalytics__

#include "ofMain.h"
#include "ofxSimpleHttp.h"
#include "uuid.h"
#include "devrand.h"
#include "uriencode.h"

#define UUID_FILENAME "UUID.txt"
#define GA_URL_ENDPOINT "http://www.google-analytics.com/collect?"


class ofxGoogleAnalytics{

	public:

		struct AnalyticsResponse{
			string status;
		};

		ofxGoogleAnalytics();

		void setup(string googleTrackingID,
				   string appName = "",
				   string appVersion = "",
				   string appID = "",
				   string appInstallerID = "");

		void update();
		void draw(int x, int y);

		void sendEvent(string category, string action, int value = 0, string label = "");
		void sendScreenView(string screenName);
		void sendException(string description, bool fatal);

		ofEvent<AnalyticsResponse> gaResponse;

	private:


		struct AnalyticsConfig{
			string trackingID;
			string currentUUID;
			string appName;
			string appID;
			string appVersion;
			string appInstallerID;
		};

		enum AnalyticsHitType{
			AnalyticsScreenView,
			AnalyticsEvent,
			AnalyticsException
		};

		AnalyticsConfig cfg;

		// http response from ofxSimpleHttp
		void googleResponse(ofxSimpleHttpResponse &response);

		ofxSimpleHttp http;

		// utils
		string basicQuery(AnalyticsHitType type);
		string getNewUUID();
		string loadUUID();
		string generateUUID();

		void sendRequest(string queryString);
};

#endif /* defined(__emptyExample__ofxGoogleAnalytics__) */
