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

The tracking works for a few days, and then it stops registering... It work in the "Realtime" view. but then its somehow discarded by google. I am currently trying different things to fix this. A session can only hold 500 actions, so I'm closing and opening a new sessions after 500 requests. I am also testing a new random userID for every app launch, just in case. I'm also queueing the requests and sending them staggered at a specified interval to avoid overwhelming google. This is hard to test bc it only starts ignoring the events after a 2-3 days of good tracking. Any ideas appreciated!


