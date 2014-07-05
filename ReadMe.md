#ofxGoogleAnalytics
Simple of addon to track app usage through Google Analytics. 

![Google Analytics](https://farm4.staticflickr.com/3901/14597174633_c9171b3e68_o.png)

To use it, do the following:

	ofxGoogleAnalytics ga;
	
	void setup(){
	
		ga.setup(	"UA-51706745-1",	//google track ID (required)
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

All the requests are threaded in a single queue so they should not affect your app performance.

It creates an unique UUID the first time that's launched, and it stores it in "data/UUID.txt". this way you can uniquely identify the app/installation across sessions.

Still a very WIP!
