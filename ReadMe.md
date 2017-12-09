# ofxGoogleAnalytics

[![Build Status](https://travis-ci.org/armadillu/ofxGoogleAnalytics.svg?branch=master)](https://travis-ci.org/armadillu/ofxGoogleAnalytics)

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

You need to go to Google Analytics and create a tracking-ID for your app to send data to. When doing so, when asked "what would you like to track", you should choose "Mobile App".

ofxGoogleAnalytics creates an unique UUID the first time that's launched, and it stores it in "data/UUID.txt" so that you can uniquely identify a unique app/installation instance across sessions. You can also choose to randomize the UUID per every session (you can also specify how often sessions are restarted).

All the requests are threaded in a single queue so they should not affect your app performance. But the last request does: destructing the ofxGoogleAnalytics holds the main thread for a long as it takes to send the last google request signaling end of a Session.

It tries to report as much as possible: platform, screen size and window size. It will try its best to append "debug" or "release" into the supplied app version. It will also sneak the Open Frameworks version in the "flash version" field.

It can also periodically report the framerate of the app, as a timing event. You can look in the "App Speed" section under "Behavior" in the Google Analytics website. You can enable/disable that feature, and also set how often you want to send those framerate reports. Look into setShouldReportFramerates() & co.

You should define your first 5 Custom Dimensions like this

![](https://farm9.staticflickr.com/8688/16675728087_0fb23cc33f_o_d.png)

And then you will be able to segment your hits by CPU, GPU, OpenFrameworks Version, computer model and platform. Those values are gathered when you first instantiate ofxGoogleAnalytics, and the gathering of those values is mostly only implemented on Mac OS X for now.

It will also, by default, send simple computer benchmarks, by measuring how long it takes for the computer to process several arithmetic tasks. Those tasks are ran once, when you instantiate the ofxGoogleAnalytics object. There are four benchmark sections, integer arithmetics, float arithmetics, sinf() performance, and sqrtf() performance. You can disable the benchmarks by calling setSendSimpleBenchmarks(false) before setting up the object. To see the reports on the GA website, navigate to Behavior:AppSpeed:BenchMark. If you set up the Dimensions as stated above, you should be able to segment all traffic by 




