#ofxGoogleAnalytics
Simple of addon to track app usage through Google Analytics. 

![Google Analytics](https://farm4.staticflickr.com/3901/14597174633_c9171b3e68_o.png)

To use it, do the following:

	ofxGoogleAnalytics ga;
	
	void setup(){
	
		ga.setup(	"UA-XXXXXXXX-1",	//google track ID (required)
			 		"myAppname",		//app name (optional)
					 "0.1",				//app version (optional)
					 "myAppID",			//ap id (optional)
					 "myAppInstallerID"	//app installer id (optional)
			 );
	}
	
	void update(){
		ga.update();
	}

	//and then, just call when appropriate
	
	//screen views
	ga.sendScreenView("screen1");

	//events
	ga.sendEvent("KeyboardEvent", "keyDown", 'a', "someLabel");

	//exceptions
	ga.sendException("Exception1", false);
	

## NOTES

It requires [ofxSimpleHttp](https://github.com/armadillu/ofxSimpleHttp).

Made following Google's "[Measurement Protocol Developer Guide](https://developers.google.com/analytics/devguides/collection/protocol/v1/devguide)".

It creates an unique UUID the first time that's launched, and it stores it in "data/UUID.txt" so that you can uniquely identify a unique app/installation instance across sessions.

All the requests are threaded in a single queue so they should not affect your app performance. But the last request does: destructing the ofxGoogleAnalytics holds the main thread for a long as it takes to send the last google request signaling end of a Session.

It tries to report as much as possible: platform, screen size and window size. It will try its best to append "debug" or "release" into the supplied app version.
