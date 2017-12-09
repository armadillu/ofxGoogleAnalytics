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
#include "ofxSimpleHttpResponse.h"

#define UUID_FILENAME (cfg.appNameRaw + "_UUID.txt")

#define GA_URL_ENDPOINT "http://www.google-analytics.com/collect?"
#define GA_DEBUG_URL_ENDPOINT "https://www.google-analytics.com/debug/collect?"


#define OFX_GA_CHECKS() if(!enabled) return;											\
						if(!isSetup){													\
							ofLogError() <<	"ofxGoogleAnalytics: call setUp() first!";	\
							return;														\
						}



class ofxSimpleHttp;

class ofxGoogleAnalytics{

	const bool debugAnalytics = false;
	const std::string ofxGAVersion = "1.0";

	public:

		struct AnalyticsResponse{
			bool ok;
			std::string status;
			int httpStatus;
		};

		ofxGoogleAnalytics();
		~ofxGoogleAnalytics();

		//make sure to call setup from the Main Thread or it will crash
		//as it makes some GL calls to get GPU info
		void setup(std::string googleTrackingID,
				   std::string appName = "",
				   std::string appVersion = "",
				   std::string appID = "",
				   std::string appInstallerID = "");

		void update();
		void draw(int x, int y);

		ofxSimpleHttp* getHttp(); //in case you want to configure things like proxy etc

		void setEnabled(bool enabled_){ enabled = enabled_;}

		//these works for "App" type reports views
		//where a "Screen" is the content unit.
		//you can also use "web" reports, in which url's are the "content unit"
		//which helps in apps with hierarchical organizations; see below
		void sendScreenView(std::string screenName);
		void sendException(std::string description, bool fatal);
		void sendEvent(std::string category,
					   std::string action,
					   int value = 0,
					   std::string label = "",
					   bool interactive = true
					   );

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
		void sendPageView(std::string documentPath, std::string documentTitle = "");

		//manually send it whenever you want, or schedule it with setShouldReportFramerates
		void sendFrameRateReport(); //see results in GA website in "Behvaior/App Speed"
		void setShouldReportFramerates(bool); //see "Behvaior/App Speed" in the GA web interface
		void setFramerateReportInterval(float sec);

		//if you want to time different parts of your code and report that, you can do this here
		//and look in the "Behvaior/App Speed" section of he web interface
		//this will be automatically re-sent every time we restart the session.
		void sendCustomTimeMeasurement(std::string timingCategory, std::string timingVariable, int timeInMs, std::string timingLabel = "");

		void setCustomUserAgent(std::string ua); //will be url encoded!

		void setVerbose(bool);

		void setUserID(std::string userName);
		void setSendSimpleBenchmarks(bool doBench);

		void setIP(std::string ipAddress);

		void setRandomizeUUID(bool);
		void setMaxRequestsPerSession(int n); //should not be > 500 or google will drop data

		void setSendToGoogleInterval(float interval);	//how often can we contact google?
														//used to throttle requests

		//you address those by ID [0..20]
		//you must assign a name to each ID key from the Google Analytics admin interface
		//(Custom Definitions)
		//https://developers.google.com/analytics/devguides/platform/customdimsmets
		void sendCustomMetric(int metricID/*1..20*/, float value);

		//1..4 are auto filled in by the addon, reporting
		//
		//	1	OpenFrameworks Version
		//	2	CPU
		//	3	GPU
		//	4	Computer Model
		//	5	OS
		//YOU MUST SETUP THIS in your google analytics account
		// https://farm8.staticflickr.com/7648/16874457272_9bb0d95d1b_o_d.png

		void sendCustomDimension(int dimensionID/*6..20*/, std::string value);


		//if you want to be notified of ok / ko
		ofEvent<AnalyticsResponse> gaResponse;

	private:

		struct AnalyticsConfig{
			std::string trackingID;
			std::string currentUUID;
			std::string appName;
			std::string appNameRaw;
			std::string appID;
			std::string appVersion;
			std::string appInstallerID;
		};

		enum AnalyticsHitType{
			AnalyticsScreenView,
			AnalyticsEvent,
			AnalyticsException,
			AnalyticsTiming,
			AnalyticsPageview
		};

		struct RequestQueueItem{
			std::string queryString;
			bool blocking;
		};

		AnalyticsConfig cfg;

		// http response from ofxSimpleHttp
		void googleResponse(ofxSimpleHttpResponse &response);


		void sendCustomDimensionInternal(int dimensionID/*0..20*/, std::string value);
		ofxSimpleHttp * http;

		bool verbose;
		bool enabled;
		bool isSetup;
		bool startedFirstSession;

		bool randomizeUUID;

		bool reportFrameRates;
		float reportFrameRatesInterval; //in sec
		float reportTime; //in sec

		float time;
		float sendInterval;

		bool doBenchmarks;

		int maxRequestsPerSession;

		std::string lastUserScreen;

		int requestCounter;
		std::vector<RequestQueueItem> requestQueue;

		std::string userID;
		std::string ipAddress;

		// utils
		void endSession(bool restart = false);
		void startSession(bool restart = false);

		std::string basicQuery(AnalyticsHitType type);
		std::string getNewUUID();
		std::string loadUUID();
		std::string generateUUID();
		std::string getUserAgent();

		void enqueueRequest(std::string queryString, bool blocking = false);
		void sendRequest(RequestQueueItem item);

		float simpleFloatBench();
		float simpleIntegerBench();
		float simpleSinCosBench();
		float simpleSqrtBench();

		std::string getComputerModel();
		std::string getComputerCPU();
		std::string getComputerGPU();
		std::string getComputerPlatform();

		void reportHardwareAsEvent();

		// unique to this computer
		std::string gpuName;
		std::string cpuName;
		std::string modelName;
		std::string ofVersion;
		std::string computerPlatform;

		ofMutex mutex;
		std::map<int,std::string> customDimensions;
		void sendAllUserDefinedCustomDimensions();

		ofMutex sendMutex;
};

#endif /* defined(__emptyExample__ofxGoogleAnalytics__) */
