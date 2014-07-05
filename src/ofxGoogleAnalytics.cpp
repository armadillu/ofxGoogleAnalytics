//
//  ofxGoogleAnalytics.cpp
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 04/07/14.
//
//

#include "ofxGoogleAnalytics.h"

ofxGoogleAnalytics::ofxGoogleAnalytics(){

	requestCounter = 0;
	isSetup = false;
	reportFrameRates = false;
	reportFrameRatesInterval = 60;
	cachedUserAgent = getUserAgent();

	http = new ofxSimpleHttp();
	http->setVerbose(true);
	http->setUserAgent(cachedUserAgent);
	http->setCancelCurrentDownloadOnDestruction(false);

	//add download listener
	ofAddListener(http->httpResponse, this, &ofxGoogleAnalytics::googleResponse);

	cfg.currentUUID = loadUUID();
	if ( cfg.currentUUID.size() == 0 ){ //need to create one!
		cfg.currentUUID = generateUUID();
		ofLogNotice() << "ofxGoogleAnalytics: Creating a new UUID for this app: " << cfg.currentUUID << endl;
	}else{
		ofLogNotice() << "ofxGoogleAnalytics: Loaded UUID for this app: " << cfg.currentUUID << endl;
	}
}


ofxGoogleAnalytics::~ofxGoogleAnalytics(){
	if(isSetup){
		endSession();
	}
}

void ofxGoogleAnalytics::setup(string googleTrackingID_, string appName, string appVersion,
							   string appID, string appInstallerID){

	cfg.trackingID = googleTrackingID_;
	cfg.appName = UriEncode(appName);

	#ifdef TARGET_OSX
		//this is ghetto TODO!
		//http://stackoverflow.com/questions/3223753/is-there-a-macro-that-xcode-automatically-sets-in-debug-builds
		#if !defined(__OPTIMIZE__) //
			cfg.appVersion = UriEncode(appVersion + " Debug");
		#else
			cfg.appVersion = UriEncode(appVersion + " Release");
		#endif
	#else
		#ifdef TARGET_WINDOWS
			#if defined(_DEBUG)
			cfg.appVersion = UriEncode(appVersion + " Debug");
			#else
			cfg.appVersion = UriEncode(appVersion + " Release");
			#endif
		#else
			//!osx && !win
			cfg.appVersion = UriEncode(appVersion );
		#endif
	#endif

	cfg.appID = UriEncode(appID);
	cfg.appInstallerID = UriEncode(appInstallerID);

	isSetup = true;
	startSession();
	reportTime = ofGetElapsedTimef();
}

void ofxGoogleAnalytics::update(){
	http->update();
	if (reportFrameRates){
		if(ofGetElapsedTimef() - reportTime > reportFrameRatesInterval){
			reportTime = ofGetElapsedTimef();
			sendFrameRateReport();
		}
	}
}

void ofxGoogleAnalytics::draw(int x, int y){
	http->draw(x, y);
}


//void ofxGoogleAnalytics::setCustomMetric(int ID, string name, int value){
//	if (ID < 20 && ID >= 0){
//		customMetrics[ID] = name;
//	}else{
//		//https://developers.google.com/analytics/devguides/collection/protocol/v1/parameters#cm[1-9][0-9]*
//		ofLogError() << "metric ID's should be between 0 and 20";
//	}
//}

void ofxGoogleAnalytics::setUserID(string userName){
	userID = UriEncode(userName);
}

void ofxGoogleAnalytics::setCustomUserAgent(string ua){
	customUserAgent = UriEncode(ua);
	http->setUserAgent(customUserAgent);
}

void ofxGoogleAnalytics::setIP(string ipAddress_){
	ipAddress = UriEncode(ipAddress_);
}

void ofxGoogleAnalytics::setShouldReportFramerates(bool b){
	reportFrameRates = b;
}


void ofxGoogleAnalytics::setFramerateReportInterval(float sec){
	reportFrameRatesInterval = sec;
}


void ofxGoogleAnalytics::sendFrameRateReport(){
	string query = basicQuery(AnalyticsTiming);
	query += "&utc=AppTiming";
	query += "&utv=FrameRate";
	query += "&utt=" + UriEncode(ofToString((int)(ofGetFrameRate() * 1000)) ); //to seconds
	query += "&ni=1";
	sendRequest(query);
}

void ofxGoogleAnalytics::sendEvent(string category, string action, int value, string label){

	string query = basicQuery(AnalyticsEvent);
	query += "&ec=" + UriEncode(category);
	query += "&ea=" + UriEncode(action);
	if(lastUserScreen.size() > 0) query += "&cd=" + UriEncode(lastUserScreen);
	if (label.size()) query += "&el=" + UriEncode(label);
	query += "&ev=" + ofToString(value);
	sendRequest(query);
}


void ofxGoogleAnalytics::sendScreenView(string screenName){

	lastUserScreen = screenName;
	string query = basicQuery(AnalyticsScreenView);
	query += "&cd=" + UriEncode(screenName);
	sendRequest(query);
}


void ofxGoogleAnalytics::sendException(string description, bool fatal){

	string query = basicQuery(AnalyticsException);
	query += "&exd=" + UriEncode(description);
	query += "&exf=" + string(fatal ? "1" : "0");
	sendRequest(query);
}


void ofxGoogleAnalytics::startSession(){
	string query = basicQuery(AnalyticsScreenView);
	query += "&cd=" + UriEncode("Start ofxGoogleAnalytics");
	query += "&sc=start";
	sendRequest(query);
}


void ofxGoogleAnalytics::endSession(){
	string query = basicQuery(AnalyticsScreenView);
	query += "&cd=" + UriEncode("End ofxGoogleAnalytics");
	query += "&sc=end";
	sendRequest(query, true/*blocking*/);
}


string ofxGoogleAnalytics::basicQuery(AnalyticsHitType type){

	string ua;
	if (customUserAgent.size() > 0){
		ua = customUserAgent;
	}else{
		ua = cachedUserAgent;
	}

	string q;
	q += "v=1";
	q += "&tid=" + cfg.trackingID;
	q += "&cid=" + cfg.currentUUID;

	switch (type) {
		case AnalyticsScreenView: q+= "&t=screenview";break;
		case AnalyticsEvent: q+= "&t=event";break;
		case AnalyticsException: q+= "&t=exception";break;
		case AnalyticsTiming: q+= "&t=timing"; break;
	}

	//q += "&ua=" + ua; //User Agent now set at ofxSimplehttp level

	//q += "&sr=" + ofToString((int)ofGetScreenWidth()) + "x" + ofToString((int)ofGetScreenHeight());
	//q += "&vp=" + ofToString((int)ofGetWidth()) + "x" + ofToString((int)ofGetHeight()); //viewport not viewable in reports?
	q += "&sr=" + ofToString((int)ofGetWidth()) + "x" + ofToString((int)ofGetHeight());

	if (cfg.appName.size()) q += "&an=" + cfg.appName;
	if (cfg.appVersion.size()) q += "&av=" + cfg.appVersion;
	if (cfg.appID.size()) q += "&aid=" + cfg.appID;
	if (cfg.appInstallerID.size()) q += "&aiid=" + cfg.appInstallerID;

	if (userID.size() > 0 ) q += "&uid=" + userID;
	if (ipAddress.size()) q += "&uip=" + ipAddress;

	return q;
}


void ofxGoogleAnalytics::sendRequest(string queryString, bool blocking){
	string randomize = "&z=" + ofToString((int)ofRandom(99999));
	string sessionControl = "";
	string url = GA_URL_ENDPOINT + queryString + randomize;
	if (blocking){
		ofxSimpleHttpResponse r = http->fetchURLBlocking(url);
		r.print();
	}else{
		http->fetchURL(url, true);
	}
	requestCounter++;
}


void ofxGoogleAnalytics::googleResponse(ofxSimpleHttpResponse &res){

	res.print();
	AnalyticsResponse r;
	if (res.status < 300 && res.status > 200){
		r.status = "ok - " + ofToString(res.status);;
	}else{
		r.status = "ko - " + ofToString(res.status);
	}
	ofNotifyEvent( gaResponse, r, this );
}

string ofxGoogleAnalytics::getUserAgent(){

	ofTargetPlatform platform = ofGetTargetPlatform();
	string platS;

	switch (platform) {
		case OF_TARGET_OSX:{
			#ifdef TARGET_OSX
			SInt32 major = 10, minor = 4, bugfix = 1;
			Gestalt(gestaltSystemVersionBugFix, &bugfix);
			Gestalt(gestaltSystemVersionMajor, &major);
			Gestalt(gestaltSystemVersionMinor, &minor);
			string cpu = ofSystem("uname -m"); //this is quite ghetto TODO
			string os = ofSystem("uname");
			string platform = os + " " + cpu ;
			ofStringReplace(platform, "\n", "");
			ofStringReplace(platform, "\xff", "");

			platS = "(Macintosh; " + platform + "; Mac OS X " + ofToString(major) + "_" +
					ofToString(minor) + "_" + ofToString(bugfix) + ")";
			#endif
			}break;
		case OF_TARGET_WINGCC: platS = "(Windows 8; GCC)"; break;
		case OF_TARGET_WINVS:  platS = "(Windows 8; Visual Studio)"; break;
		case OF_TARGET_IOS: platS = "(iOS 8)"; break;
		case OF_TARGET_ANDROID: platS = "(Android)"; break;
		case OF_TARGET_LINUX: platS = "(Linux)"; break;
		case OF_TARGET_LINUX64: platS = "(Linux 64)"; break;
		case OF_TARGET_LINUXARMV6L: platS = "(Linux ARM v6)"; break;
		case OF_TARGET_LINUXARMV7L: platS = "(Linux ARM v7)"; break;
		default:  platS = "(Unknown Platform)"; break;
	}

	
	string ofVersion = ofToString(ofGetVersionMajor()) + "." + ofToString(ofGetVersionMinor()) +
						"." + ofToString(ofGetVersionPatch());
	string all = "ofxGoogleAnalytics + OpenFrameworks " + ofVersion + "; " + platS;
	return UriEncode(all);
}

string ofxGoogleAnalytics::loadUUID(){

	ifstream myfile(ofToDataPath(UUID_FILENAME,true).c_str());
	string UUID;
	if (myfile.is_open()){
		getline( myfile, UUID, '\n' );
	}
	myfile.close();
	return UUID;
}


string ofxGoogleAnalytics::generateUUID(){
	string UUID = getNewUUID();
	ofstream myfile;
	myfile.open(ofToDataPath(UUID_FILENAME, true).c_str());
	myfile << UUID << "\n";
	myfile.close();
	return UUID;
}


string ofxGoogleAnalytics::getNewUUID(){
    using kashmir::uuid_t;
    using kashmir::system::DevRand;
    DevRand devrandom;
    uuid_t uuid;
	devrandom >> uuid;
	stringstream ss;
	ss << uuid;
	string s = ss.str();
	return s;
}
