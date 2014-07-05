#ofxGoogleAnalytics
Simple of addon to track app usage through Google Analytics. To use it, do the following:

	ofxGoogleAnalytics ga;
	
	void setup(){
	
		ga.setup(	"UA-51706745-1",	//google track ID
			 		"myAppname",		//app name
					 "0.1",				//app version
					 "myAppID",			//ap id
					 "myAppInstallerID"	//app installer id
			 );
	}
	
	void update(){
		ga.update();
	}

	//and then, just call:
	
	//screen views
	ga.sendScreenView("screen1");

	//events
	ga.sendEvent("KeyboardEvent", "keyDown", 'a', "someLabel");

	//exceptions
	ga.sendException("Exception1", false); break;
	


## NOTES

It requires [ofxSimpleHttp](https://github.com/armadillu/ofxSimpleHttp).

All the requests are threaded in a single queue so they should not affect your app performance.

It creates an unique UUID the first time that's launched, and it stores it in "data/UUID.txt". this way you can uniquely identify the app/installation across sessions.
