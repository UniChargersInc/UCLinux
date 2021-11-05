#ifndef WEB_SER_REQUEST_H
#define WEB_SER_REQUEST_H

#include <MkrLib.h>
#include <CommonUtils/my_utils.h>

#include <mutex>
using namespace std;


struct WebSession
{
	string user = "";
	string pass = "";
	string lang = "en";
	int    cid = -1;         //ClientID = -1;
	int64_t sessionid = -1;

};

#endif