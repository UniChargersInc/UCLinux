#ifndef MKR_MODELS_H_
#define MKR_MODELS_H_

#include <iostream>
#include <exception>

#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
class Client
{
public:	
	int cid=-1,ResourceID=-1;
	string drivername="";
	string user="", pass="";
	string fullname="", email="";
	string phone="",timestamp="";
	string address = "";
	int status=-1;
	//float lat, lon;
};

class EmailInfo
{
public:
	const char *smtpserver=0;
	const char *from=0;
	const char *to=0;
	const char *subject=0;
	const char *replyto = 0;
	const char *msg = 0;

	const char *attachment_file_name = 0;

	EmailInfo(const char *smtpserver,
	const char *from,
	const char *to,
	const char *subject,
	const char *replyto,
	const char *msg)
	{
		this->smtpserver = smtpserver;
		this->from = from;
		this->to = to;
		this->subject = subject;
		this->replyto = replyto;
		this->msg = msg;
	}
};


#endif