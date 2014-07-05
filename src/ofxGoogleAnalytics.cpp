//
//  ofxGoogleAnalytics.cpp
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 04/07/14.
//
//

#include "ofxGoogleAnalytics.h"

ofxGoogleAnalytics::ofxGoogleAnalytics(){

	http.setVerbose(true);
	http.setUserAgent("ofxGoogleAnalytics");

	//add download listener
	ofAddListener(http.httpResponse, this, &ofxGoogleAnalytics::googleResponse);

	cfg.currentUUID = loadUUID();
	if ( cfg.currentUUID.size() == 0 ){ //need to create one!
		cfg.currentUUID = generateUUID();
		ofLogNotice() << "ofxGoogleAnalytics: Creating a new UUID for this app: " << cfg.currentUUID << endl;
	}else{
		ofLogNotice() << "ofxGoogleAnalytics: Loaded UUID for this app: " << cfg.currentUUID << endl;
	}
}


void ofxGoogleAnalytics::setup(string googleTrackingID_, string appName, string appVersion,
							   string appID, string appInstallerID){

	cfg.trackingID = googleTrackingID_;
	cfg.appName = appName;
	cfg.appVersion = appVersion;
	cfg.appID = appID;
	cfg.appInstallerID = appInstallerID;
}


void ofxGoogleAnalytics::update(){
	http.update();
}

void ofxGoogleAnalytics::draw(int x, int y){
	http.draw(x, y);
}


void ofxGoogleAnalytics::sendEvent(string category, string action, int value, string label){

	string query = basicQuery(AnalyticsEvent);
	query += "&ec=" + UriEncode(category);
	query += "&ea=" + UriEncode(action);
	if (label.size()) query += "&el=" + UriEncode(label);
	query += "&ev=" + ofToString(value);
	sendRequest(query);
}


void ofxGoogleAnalytics::sendScreenView(string screenName){

	string query = basicQuery(AnalyticsScreenView);
	if (cfg.appName.size()) query += "&an=" + UriEncode(cfg.appName);
	if (cfg.appVersion.size()) query += "&av=" + UriEncode(cfg.appVersion);
	if (cfg.appID.size()) query += "&aid=" + UriEncode(cfg.appID);
	if (cfg.appInstallerID.size()) query += "&aiid=" + UriEncode(cfg.appInstallerID);
	query += "&cd=" + UriEncode(screenName);
	sendRequest(query);
}


void ofxGoogleAnalytics::sendException(string description, bool fatal){

	string query = basicQuery(AnalyticsException);
	query += "&exd=" + UriEncode(description);
	query += "&exdf=" + string(fatal ? "1" : "0");
	sendRequest(query);
}


string ofxGoogleAnalytics::basicQuery(AnalyticsHitType type){

	string q = string("v=1&tid=" + cfg.trackingID + "&cid=" + cfg.currentUUID);

	switch (type) {
		case AnalyticsScreenView: q+= "&t=screenview";break;
		case AnalyticsEvent: q+= "&t=event";break;
		case AnalyticsException: q+= "&t=exception";break;
	}
	return q;
}


void ofxGoogleAnalytics::sendRequest(string queryString){

	string randomize = "&z=" + ofToString((int)ofRandom(99999));
	string url = GA_URL_ENDPOINT + queryString + randomize;

	http.fetchURL(url, true);
}


void ofxGoogleAnalytics::googleResponse(ofxSimpleHttpResponse &res){

	AnalyticsResponse r;
	if (res.status < 300 && res.status > 200){
		r.status = "ok - " + ofToString(res.status);;
	}else{
		r.status = "ko - " + ofToString(res.status);
	}
	ofNotifyEvent( gaResponse, r, this );
	res.print();
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
