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
		~ofxGoogleAnalytics();

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
		void sendFrameRateReport();

		void setCustomUserAgent(string ua);
		void setShouldReportFramerates(bool);
		void setFramerateReportInterval(float sec);
		void setUserID(string userName);
		void setIP(string ipAddress);
		//void setCustomMetric(int ID, string name, int value);

		//if you want to be notified of ok / ko
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
			AnalyticsException,
			AnalyticsTiming
		};

		AnalyticsConfig cfg;

		// http response from ofxSimpleHttp
		void googleResponse(ofxSimpleHttpResponse &response);

		ofxSimpleHttp * http;

		bool isSetup;

		bool reportFrameRates;
		float reportFrameRatesInterval; //in sec
		float reportTime; //in sec

		string lastUserScreen;

		int requestCounter;
		string customUserAgent;
		string cachedUserAgent;

		string userID;
		string ipAddress;

		map<int,string> customMetrics;

		// utils
		void endSession();
		void startSession();

		string basicQuery(AnalyticsHitType type);
		string getNewUUID();
		string loadUUID();
		string generateUUID();
		string getUserAgent();

		void sendRequest(string queryString, bool blocking = false);
};

#endif /* defined(__emptyExample__ofxGoogleAnalytics__) */
