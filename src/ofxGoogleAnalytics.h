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
#define GA_MAX_REQUESTS_PER_SESSION	480
#define GA_URL_ENDPOINT "http://www.google-analytics.com/collect?"

#define OFX_GA_CHECKS() if(!enabled) return;											\
						if(!isSetup){													\
							ofLogError() <<	"ofxGoogleAnalytics: call setUp() first!";	\
							return;														\
						}

class ofxGoogleAnalytics{

	public:

		struct AnalyticsResponse{
			bool ok;
			string status;
			int httpStatus;
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

		void setEnabled(bool enabled_){ enabled = enabled_;}

		//these works for "App" type reports views
		//where a "Screen" is the content unit.
		//you can also use "web" reports, in which url's are the "content unit"
		//which helps in apps with hierarchical organizations; see below
		void sendScreenView(string screenName);
		void sendException(string description, bool fatal);
		void sendEvent(string category, string action, int value = 0, string label = "");

		//This works on "Web" type reports views
		//you should go to "Admin", set Account and Property, and in View column choose
		//"create new view" and make sure its set to "website"
		//this way you can categorize sections of your app as if they were URLS
		// for example /appModes/createMode/createSomething/
		//	or /appModes/editMode/editSomethingElse/
		//this might be preferable to keep things hierarchical.
		//you can find them in the GA web ui under "Behavior/Site Content/".
		//It's your choice. The downside of "web" reports is that you don't get
		//the event and exception types.
		//You can also have one "Web" and one "App" view reports for
		//the same app;
		void sendPageView(string documentPath, string documentTitle = "");

		//manually send it whenever you want, or schedule it with setShouldReportFramerates
		void sendFrameRateReport(); //see results in GA website in "Behvaior/App Speed"

		//if you want to time different parts of your code and report that, you can do this here
		//and look in the "Behvaior/App Speed" section of he web interface
		void sendCustomTimeMeasurement(string timingCategory, string timingVariable, int timeInMs, string timingLabel = "");

		void setCustomUserAgent(string ua);
		void setShouldReportFramerates(bool); //see "Behvaior/App Speed" in the GA web interface
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
			AnalyticsTiming,
			AnalyticsPageview
		};

		AnalyticsConfig cfg;

		// http response from ofxSimpleHttp
		void googleResponse(ofxSimpleHttpResponse &response);

		ofxSimpleHttp * http;

		bool enabled;
		bool isSetup;

		bool reportFrameRates;
		float reportFrameRatesInterval; //in sec
		float reportTime; //in sec

		string lastUserScreen;

		int requestCounter;
		bool restartingSession;
		vector<string> queuedRequests;

		string customUserAgent;
		string cachedUserAgent;

		string userID;
		string ipAddress;

		map<int,string> customMetrics;

		// utils
		void endSession(bool restart = false);
		void startSession(bool restart = false);

		string basicQuery(AnalyticsHitType type);
		string getNewUUID();
		string loadUUID();
		string generateUUID();
		string getUserAgent();

		void sendRequest(string queryString, bool blocking = false, bool regardless = false);
};

#endif /* defined(__emptyExample__ofxGoogleAnalytics__) */
