//
//  ofxGoogleAnalytics.cpp
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 04/07/14.
//
//

#include "ofxGoogleAnalytics.h"

#ifdef TARGET_OSX
#include <sys/types.h>
#include <sys/sysctl.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/Graphics/IOFrameBufferShared.h>
#endif

#ifdef TARGET_WIN32
#ifdef _WIN32
#if(!defined(FREE_WINDOWS))
#include <intrin.h>
#endif
#include <windows.h>
#endif
#endif

ofxGoogleAnalytics::ofxGoogleAnalytics(){

	requestCounter = 0;
	isSetup = false;
	doBenchmarks = true;
	enabled = true;
	reportFrameRates = true;
	reportFrameRatesInterval = 60;
	sendInterval = 0.1; //dont send stuff to google any faster than this
	randomizeUUID = false;
	maxRequestsPerSession = 450;
	verbose = false;

	http = new ofxSimpleHttp();
	http->setCancelPendingDownloadsOnDestruction(false); //when deleted, wait until all pending stuff is sent
	http->setCancelCurrentDownloadOnDestruction(false);
	http->setUserAgent(getUserAgent());
}


ofxGoogleAnalytics::~ofxGoogleAnalytics(){
	if(isSetup){
		ofLogNotice("ofxGoogleAnalytics") << "Closing session for good! " << requestQueue.size() << " requests pending executing...";

		endSession(false);
		mutex.lock();
		while(requestQueue.size()){ //lets send all what's pending right now
			sendRequest(requestQueue[0]);
			requestQueue.erase(requestQueue.begin());
		}
		mutex.unlock();
		ofLogNotice("ofxGoogleAnalytics") << "all pending requests executed!";
		delete http;
	}
}

void ofxGoogleAnalytics::setup(string googleTrackingID_, string appName, string appVersion,
							   string appID, string appInstallerID){
	
	cfg.trackingID = googleTrackingID_;
	cfg.appName = UriEncode(appName);
	cfg.appID = UriEncode(appID);
	cfg.appNameRaw = appName;
	cfg.appInstallerID = UriEncode(appInstallerID);	
	cfg.currentUUID = loadUUID();
	if ( cfg.currentUUID.size() == 0 ){ //need to create one!
		cfg.currentUUID = generateUUID();
		ofLogWarning("ofxGoogleAnalytics") << "Creating a new UUID for this app: " << cfg.currentUUID;
	}else{
		ofLogWarning("ofxGoogleAnalytics") << "Loaded UUID for this app: " << cfg.currentUUID;
	}
	time = ofGetElapsedTimef();

	//HW thingies
	gpuName = getComputerGPU();
	cpuName = getComputerCPU();
	modelName = getComputerModel();
	ofVersion = ofGetVersionInfo();
	ofStringReplace(ofVersion, "\n", "");
	computerPlatform = getComputerPlatform();

	//add download listener
	ofAddListener(http->httpResponse, this, &ofxGoogleAnalytics::googleResponse);
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
			cfg.appVersion = UriEncode(appVersion);
		#endif
	#endif

	isSetup = true;
	startedFirstSession = false;
	reportTime = ofGetElapsedTimef();

	if(doBenchmarks){
		//time some basic operations, send to google to compare HW performance
		float floatB = simpleFloatBench();
		float intB = simpleIntegerBench();
		float sinB = simpleSinCosBench();
		float sqrtB = simpleSqrtBench();

		sendCustomTimeMeasurement("BenchMark", "FloatMathBench", floatB * 1000);
		sendCustomTimeMeasurement("BenchMark", "IntegerMathBench", intB * 1000);
		sendCustomTimeMeasurement("BenchMark", "SinCosBench", sinB * 1000);
		sendCustomTimeMeasurement("BenchMark", "SqrtBench", sqrtB * 1000);
		ofLogNotice("ofxGoogleAnalytics") << "Benchmarks took " << floatB + intB + sinB + sqrtB << "seconds";
	}
}


ofxSimpleHttp* ofxGoogleAnalytics::getHttp(){
	return http;
}

void ofxGoogleAnalytics::setVerbose(bool v){
	verbose = v;
}

void ofxGoogleAnalytics::setMaxRequestsPerSession(int n){
	maxRequestsPerSession = ofClamp(n, 10, 499);
};

void ofxGoogleAnalytics::setSendSimpleBenchmarks(bool d){
	doBenchmarks = d;
}


void ofxGoogleAnalytics::setSendToGoogleInterval(float interval){
	sendInterval = ofClamp(interval, 0.01, FLT_MAX);
}

void ofxGoogleAnalytics::update(){

	if(!enabled) return;
	http->update();
	float now = ofGetElapsedTimef();

	if( now - time > sendInterval){
		time = now;
		mutex.lock();
		if (requestQueue.size()){
			sendRequest(requestQueue[0]);
			requestQueue.erase(requestQueue.begin());
		}
		mutex.unlock();
	}

	if (reportFrameRates){
		if(now - reportTime > reportFrameRatesInterval){
			reportTime = now;
			sendFrameRateReport();
		}
	}
}


void ofxGoogleAnalytics::draw(int x, int y){
	string httpS = http->drawableString();
	mutex.lock();
	ofDrawBitmapString("ofxGoogleAnalytics: " + ofToString(requestQueue.size()) + " Queued Requests" +
					   "\nRequestsThisSession: " + ofToString(requestCounter) + " / " +
					   ofToString(maxRequestsPerSession) + "\n\n" +  httpS
					   , x, y);
	mutex.unlock();

}


void ofxGoogleAnalytics::sendCustomMetric(int ID, float value){
	if (ID <= 20 && ID > 0){
		if(verbose) ofLogNotice("ofxGoogleAnalytics") << "sendCustomMetric(" << ofToString(ID) << ", " << ofToString(value) << ")";
		OFX_GA_CHECKS();
		string query = basicQuery(AnalyticsTiming);
		query += "&cm" + ofToString(ID)+ "=" + ofToString(value);
		enqueueRequest(query);
	}else{
		//https://developers.google.com/analytics/devguides/collection/protocol/v1/parameters#cm[1-9][0-9]*
		ofLogError("ofxGoogleAnalytics") << "metric ID's should be between 0 and 20";
	}
}

void ofxGoogleAnalytics::sendCustomDimension(int ID, string value){
	if (ID <= 20 && ID > 5){
		if(verbose) ofLogNotice("ofxGoogleAnalytics") << "sendCustomDimension(" << ofToString(ID) << ", " << ofToString(value) << ")";
		customDimensions[ID] = value;
		sendCustomDimensionInternal(ID, value);
	}else{
		//https://developers.google.com/analytics/devguides/collection/protocol/v1/parameters#cm[1-9][0-9]*
		ofLogError("ofxGoogleAnalytics") << "Dimension ID's should be between 6 and 20 - 0..5 are used by the addon.";
	}
}

void ofxGoogleAnalytics::sendCustomDimensionInternal(int ID, string value){
	OFX_GA_CHECKS();
	string query = basicQuery(AnalyticsTiming);
	query += "&cd" + ofToString(ID)+ "=" + UriEncode(value);
	enqueueRequest(query);
}


void ofxGoogleAnalytics::setUserID(string userName){
	userID = UriEncode(userName);
}

void ofxGoogleAnalytics::setCustomUserAgent(string ua){
	http->setUserAgent(ua);
}

void ofxGoogleAnalytics::setIP(string ipAddress_){
	ipAddress = ipAddress_;
}

void ofxGoogleAnalytics::setShouldReportFramerates(bool b){
	reportFrameRates = b;
}

void ofxGoogleAnalytics::setRandomizeUUID(bool t){
	randomizeUUID = t;
	if(randomizeUUID) generateUUID();
}

void ofxGoogleAnalytics::setFramerateReportInterval(float sec){
	reportFrameRatesInterval = sec;
}


void ofxGoogleAnalytics::sendAllUserDefinedCustomDimensions(){

	map<int,string>::iterator it = customDimensions.begin();
	while( it != customDimensions.end()){
		sendCustomDimensionInternal(it->first, it->second);
		++it;
	}
}

void ofxGoogleAnalytics::sendFrameRateReport(){
	OFX_GA_CHECKS();
	if(verbose) ofLogNotice("ofxGoogleAnalytics") << "sendFrameRateReport()";
	string query = basicQuery(AnalyticsTiming);
	query += "&utc=AppTimings";
	query += "&utv=FrameRate";
	query += "&utt=" + ofToString((int)(ofGetFrameRate() * 1000)); //to milliseconds
	query += "&ni=1";
	enqueueRequest(query);
}

void ofxGoogleAnalytics::sendCustomTimeMeasurement(string timingCategory, string timingVariable,
							   int timeInMs, string timingLabel){
	OFX_GA_CHECKS();
	if(verbose) ofLogNotice("ofxGoogleAnalytics") << "sendCustomTimeMeasurement(" << ofToString(timingCategory) << ", " << ofToString(timingVariable) << ", " << timeInMs << ")";
	string query = basicQuery(AnalyticsTiming);
	query += "&utc=" + UriEncode(timingCategory);
	query += "&utv=" + UriEncode(timingVariable);
	query += "&utt=" + UriEncode(ofToString((int)(timeInMs)));
	query += "&utl=" + UriEncode(timingLabel);
	query += "&ni=1";
	enqueueRequest(query);
}

void ofxGoogleAnalytics::sendEvent(string category, string action, int value, string label, bool interactive ){

	OFX_GA_CHECKS();
	if(verbose) ofLogNotice("ofxGoogleAnalytics") << "sendEvent(" << ofToString(category) << ", " <<
		ofToString(action) << ", " << ofToString(value) << ", " << ofToString(label) <<
		", " << ofToString(interactive) << ")";

	string query = basicQuery(AnalyticsEvent);
	query += "&ec=" + UriEncode(category);
	if (action.size()) query += "&ea=" + UriEncode(action);
	if (label.size()) query += "&el=" + UriEncode(label);
	query += "&ev=" + ofToString(value);
	query += "&ni=" + string(interactive ? "0" : "1");
	enqueueRequest(query);
}

void ofxGoogleAnalytics::sendScreenView(string screenName){
	OFX_GA_CHECKS();

	if(verbose) ofLogNotice("ofxGoogleAnalytics") << "sendScreenView(" << ofToString(screenName) <<  ")";
	lastUserScreen = screenName;
	string query = basicQuery(AnalyticsScreenView);
	query += "&cd=" + UriEncode(screenName);
	enqueueRequest(query);
}


void ofxGoogleAnalytics::sendPageView(string documentPath, string documentTitle){
	OFX_GA_CHECKS();
	if(verbose) ofLogNotice("ofxGoogleAnalytics") << "sendScreenView(" << ofToString(documentPath) <<
		"," << ofToString(documentTitle) <<  ")";

	string query = basicQuery(AnalyticsPageview);
	if(documentTitle.size() > 0) query += "&dt=" + UriEncode(documentTitle);
	query += "&dp=" + UriEncode("/" + documentPath);
	enqueueRequest(query);
}


void ofxGoogleAnalytics::sendException(string description, bool fatal){
	OFX_GA_CHECKS();
	if(verbose) ofLogNotice("ofxGoogleAnalytics") << "sendException(" << ofToString(description) <<
		"," << ofToString(fatal) <<  ")";

	string query = basicQuery(AnalyticsException);
	query += "&exd=" + UriEncode(description);
	query += "&exf=" + string(fatal ? "1" : "0");
	enqueueRequest(query);
}


void ofxGoogleAnalytics::startSession(bool restart){
	//ofLogNotice("ofxGoogleAnalytics") << "🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏  Start Session  🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏🍏";
	if(randomizeUUID) generateUUID();

	if(verbose) ofLogNotice("ofxGoogleAnalytics") << "startSession()";

	string query = basicQuery(AnalyticsEvent);
	query += "&el=" + UriEncode(string(restart ? "Restart" : "Start") + " ofxGoogleAnalytics Session");
	query += "&sc=start";
	if(restart){
		query += "&ni=1";
	}
	enqueueRequest(query, false/*blocking*/);
}


void ofxGoogleAnalytics::endSession(bool restart){
	//ofLogNotice("ofxGoogleAnalytics") << "🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎 End Session 🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎🍎";

	if(verbose) ofLogNotice("ofxGoogleAnalytics") << "endSession()";
	string query = basicQuery(AnalyticsEvent);
	query += "&el=" + UriEncode(string(restart ? "Close-To-Reopen" : "End") + " ofxGoogleAnalytics Session");
	query += "&sc=end";
	if(restart){
		query += "&ni=1";
	}
	enqueueRequest(query, !restart/*blocking*/);
}


string ofxGoogleAnalytics::basicQuery(AnalyticsHitType type){

//	string ua;
//	if (customUserAgent.size() > 0){
//		ua = customUserAgent;
//	}else{
//		ua = cachedUserAgent;
//	}

	string q;
	q += "v=1";
	q += "&tid=" + cfg.trackingID;
	q += "&cid=" + cfg.currentUUID;

	switch (type) {
		case AnalyticsScreenView: q+= "&t=screenview";break;
		case AnalyticsEvent: q+= "&t=event";break;
		case AnalyticsException: q+= "&t=exception";break;
		case AnalyticsTiming: q+= "&t=timing"; break;
		case AnalyticsPageview: q+= "&t=pageview"; break;
	}

	//q += "&ds=app"; //data source (app|web)
	//q += "&ua=" + ua; //User Agent now set at ofxSimplehttp level
	//q += "&sr=" + ofToString((int)ofGetScreenWidth()) + "x" + ofToString((int)ofGetScreenHeight());
	//q += "&vp=" + ofToString((int)ofGetWidth()) + "x" + ofToString((int)ofGetHeight()); //viewport not viewable in reports?

	q += "&sr=" + ofToString((int)ofGetWidth()) + "x" + ofToString((int)ofGetHeight());
	//sneak in OF version and screen size into the flash version field
	q += "&fl=OF_" + ofVersion; //+ "%20" + ofToString((int)ofGetScreenWidth()) + "x" + ofToString((int)ofGetScreenHeight());

	if (cfg.appName.size()) q += "&an=" + cfg.appName;
	if (cfg.appVersion.size()) q += "&av=" + cfg.appVersion;
	if (cfg.appID.size()) q += "&aid=" + cfg.appID;
	if (cfg.appInstallerID.size()) q += "&aiid=" + cfg.appInstallerID;

	if (userID.size() > 0 ) q += "&uid=" + userID;
	if (ipAddress.size()) q += "&uip=" + ipAddress;

	return q;
}

string ofxGoogleAnalytics::getComputerModel(){

	#ifdef TARGET_OSX
	size_t len = 0;
	::sysctlbyname("hw.model", NULL, &len, NULL, 0);
	std::string model(len-1, '\0');
	::sysctlbyname("hw.model", const_cast<char *>(model.data()), &len, NULL, 0);
	return model;
	#endif
	#ifdef TARGET_WIN32
	return "Windows PC";
	#endif
	return "Unknown Model";
}

string ofxGoogleAnalytics::getComputerCPU(){
	#ifdef TARGET_OSX
	size_t len = 0;
	::sysctlbyname("machdep.cpu.brand_string", NULL, &len, NULL, 0);
	std::string cpu(len-1, '\0');
	::sysctlbyname("machdep.cpu.brand_string", const_cast<char *>(cpu.data()), &len, NULL, 0);
	return cpu;
	#endif

	#ifdef TARGET_WIN32
	char buf[48];
	int result[4];

	__cpuid(result, 0x80000000);

	if(result[0] >= (int)0x80000004) {
		__cpuid((int*)(buf+0), 0x80000002);
		__cpuid((int*)(buf+16), 0x80000003);
		__cpuid((int*)(buf+32), 0x80000004);

		string brand = buf;

		size_t i;
		if((i = brand.find("  ")) != string::npos)
			brand = brand.substr(0, i);

		return brand;
	}
	#endif
	return "Unknown CPU";
}

string ofxGoogleAnalytics::getComputerGPU(){
	if(ofThread::isMainThread()){
		string renderer = string((char*)glGetString(GL_RENDERER));
		return renderer;
	}
	return "unknown GPU";
}

string ofxGoogleAnalytics::getComputerPlatform(){

	ofTargetPlatform platform = ofGetTargetPlatform();
	switch (platform) {
		case OF_TARGET_OSX:{
			#ifdef TARGET_OSX
			string platS;
			SInt32 major = 10, minor = 4, bugfix = 1;
			Gestalt(gestaltSystemVersionBugFix, &bugfix);
			Gestalt(gestaltSystemVersionMajor, &major);
			Gestalt(gestaltSystemVersionMinor, &minor);
			platS = "Macintosh; Mac OS X " + ofToString(major) + "." +
			ofToString(minor) + "." + ofToString(bugfix);
			return platS;
			#endif
		}break;
		case OF_TARGET_WINGCC: return "Windows; GCC"; break;
		case OF_TARGET_WINVS:  return "Windows; Visual Studio"; break;
		case OF_TARGET_IOS: return "iOS"; break;
		case OF_TARGET_ANDROID: return "Android"; break;
		case OF_TARGET_LINUX: return "Linux"; break;
		case OF_TARGET_LINUX64: return "Linux 64"; break;
		case OF_TARGET_LINUXARMV6L: return "Linux ARM v6"; break;
		case OF_TARGET_LINUXARMV7L: return "Linux ARM v7"; break;
	}
	return "Unknown Platform";
}


void ofxGoogleAnalytics::reportHardwareAsEvent(){

	//send hw info as an event
	if(cpuName.size()){
		ofLogNotice("ofxGoogleAnalytics") << "Reporting my CPU '" << cpuName << "'";
		sendEvent("Hardware", "CPU", 0, cpuName, false);
	}

	if(modelName.size()){
		ofLogNotice("ofxGoogleAnalytics") << "Reporting my Computer Model '" << modelName << "'";
		sendEvent("Hardware", "Model", 0, modelName, false);
	}

	if(gpuName.size()){
		ofLogNotice("ofxGoogleAnalytics") << "Reporting my Computer GPU '" << gpuName << "'";
		sendEvent("Hardware", "GPU", 0, gpuName, false);
	}

	if(ofVersion.size()){
		ofLogNotice("ofxGoogleAnalytics") << "Reporting my OF version '" << ofVersion << "'";
		sendEvent("Software", "OF Version", 0, ofVersion, false);
	}

	if(computerPlatform.size()){
		ofLogNotice("ofxGoogleAnalytics") << "Reporting my computer platform '" << computerPlatform << "'";
		sendEvent("Software", "OF Version", 0, computerPlatform, false);
	}

}


void ofxGoogleAnalytics::enqueueRequest(string queryString, bool blocking){

	if(!enabled){
		ofLogError("ofxGoogleAnalytics") << "Can't do that, not enabled!";
		return;
	}

	if(!startedFirstSession){
		startedFirstSession = true;
		startSession(false);

		//send hw info as an event
		reportHardwareAsEvent();

		sendCustomDimensionInternal(1, ofVersion);
		sendCustomDimensionInternal(2, cpuName);
		sendCustomDimensionInternal(3, gpuName);
		sendCustomDimensionInternal(4, modelName);
		sendCustomDimensionInternal(5, computerPlatform);
	}

	if (requestCounter >= maxRequestsPerSession ){ //limit of maxRequestsPerSession requests per session! restart session!
		requestCounter = 0;
		endSession(true); 	//if true(restart), will send regadless, so we overcome the block by #requestCounter
		startSession(true);	//idem
		sendAllUserDefinedCustomDimensions();
	}

	RequestQueueItem item;
	item.queryString = queryString;
	item.blocking = blocking;

	mutex.lock();
	requestQueue.push_back(item);
	mutex.unlock();

	requestCounter++;
}



void ofxGoogleAnalytics::sendRequest(RequestQueueItem item){

	string cacheBuster = "&z=" + ofToString((int)ofRandom(0, 999999));
	string url = string( debugAnalytics ? GA_DEBUG_URL_ENDPOINT: GA_URL_ENDPOINT ) + item.queryString + cacheBuster;

	if (item.blocking){
		ofxSimpleHttpResponse r = http->fetchURLBlocking(url);
		r.print();
	}else{
		http->fetchURL(url, true);
	}
}


void ofxGoogleAnalytics::googleResponse(ofxSimpleHttpResponse &res){

	if(ofGetLogLevel() == OF_LOG_VERBOSE) {
		res.print();
	}
	AnalyticsResponse r;
	r.httpStatus = res.status;
	if (res.status < 300 && res.status >= 200){
		r.ok = true;
		r.status = "ok - " + ofToString(res.status);
	}else{
		r.ok = false;
		r.status = "ko - " + ofToString(res.status) + " " + res.reasonForStatus;
		ofLogError("ofxGoogleAnalytics") << "Google Response not good! " << r.status;
	}
	ofNotifyEvent( gaResponse, r, this );
}


string ofxGoogleAnalytics::getUserAgent(){

	string platform = "(" + computerPlatform + ")";
	string ofVersion = ofToString(ofGetVersionMajor()) + "." + ofToString(ofGetVersionMinor()) +
						"." + ofToString(ofGetVersionPatch());
	return "ofxGoogleAnalytics/" + ofxGAVersion + " " + platform + " OpenFrameworks " + ofVersion;
}

string ofxGoogleAnalytics::loadUUID(){

	string path = ofToDataPath(UUID_FILENAME,true);
	ifstream myfile(path.c_str());
	string UUID;
	if (myfile.is_open()){
		getline( myfile, UUID, '\n' );
	}
	myfile.close();
	return UUID;
}


string ofxGoogleAnalytics::generateUUID(){
	string UUID = getNewUUID();
	if (!randomizeUUID){
		ofstream myfile;
		myfile.open(ofToDataPath(UUID_FILENAME, true).c_str());
		myfile << UUID << "\n";
		myfile.close();
	}
	ofLogNotice("ofxGoogleAnalytics") << "Generating new UUID (" << UUID << ")";
	return UUID;
}

float ofxGoogleAnalytics::simpleFloatBench(){
	float a = 0;
	float t = ofGetElapsedTimef();
	for(int i = 0; i < 50000000; i++){
		a = (2.0f + (i * 0.1f)) / (i * 0.5f + 2.0f - 1.0f) ;
	}
	t = ofGetElapsedTimef() - t + (a - a); //overcome compiler optimizations
	return t;
}

float ofxGoogleAnalytics::simpleIntegerBench(){

	//measure how long it takes to calc 999999 common int operations
	int a = 0;
	float t = ofGetElapsedTimef();
	for(int i = 0; i < 5000000; i++){
		a = (((a + i) * 3 ) / (1 + a * 2) ) / 2 - 1;
	}
	t = ofGetElapsedTimef() - t + float(a - a); //overcome compiler optimizations
	return t;
}


float ofxGoogleAnalytics::simpleSinCosBench(){
	float a = 0;
	float t = ofGetElapsedTimef();
	for(int i = 0; i < 5000000; i++){
		a = (2.0f + sinf(i * 0.1f - 0.3f)) / (cosf(i * 0.5f + 2.0f) + 2.0f) ;
	}
	t = ofGetElapsedTimef() - t + (a - a); //overcome compiler optimizations
	return t;
}

float ofxGoogleAnalytics::simpleSqrtBench(){
	float a = 0;
	float t = ofGetElapsedTimef();
	for(int i = 0; i < 5000000; i++){
		a = (2.0f + powf(i * 0.1f, 1.1f)) / (sqrtf(i * 0.5f + 2.0f) - 1.0f) ;
	}
	t = ofGetElapsedTimef() - t + (a - a); //overcome compiler optimizations
	return t;
}


string ofxGoogleAnalytics::getNewUUID(){
	static char alphabet[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	string s;
	for(int i = 0; i < 8; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
	s += "-";
	for(int i = 0; i < 4; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
	s += "-4";
	for(int i = 0; i < 3; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
	s += "-a";
	for(int i = 0; i < 3; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
	s += "-";
	for(int i = 0; i < 12; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
	return s;
}
