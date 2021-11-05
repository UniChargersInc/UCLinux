#pragma once

#include "sip_server_config.h"
#include "sip_lib_stream.hpp"

#ifdef WIN32
//#include <ws2tcpip.h>  //inet_ntop
#else
#include <arpa/inet.h> //-> linux inet_ntop
#endif
#include <google/protobuf/stubs/strutil.h>  //GlobalReplaceSubstring
//#include "sip_helpers.h"

using namespace std;

struct ForwardInfo {
	string fwdNumber = "", fwdIp = "";// , fwdPort = "";
	int fwdPort = 0;
};

#if 0
// this is taken from sip_msg.c to implement short messaging
struct pjsip_hdr_name_info_t
{
	const char *fullname;
	int len;
	const char *sname;
};

const pjsip_hdr_name_info_t pjsip_hdr_names[] =
{
	//{ "Accept",		    6,  NULL },   // PJSIP_H_ACCEPT,
	//{ "Accept-Encoding",    15, NULL },   // PJSIP_H_ACCEPT_ENCODING,
	//{ "Accept-Language",    15, NULL },   // PJSIP_H_ACCEPT_LANGUAGE,
	//{ "Alert-Info",	    10, NULL },   // PJSIP_H_ALERT_INFO,
	//{ "Allow",		    5,  NULL },   // PJSIP_H_ALLOW,
	//{ "Authentication-Info",19, NULL },   // PJSIP_H_AUTHENTICATION_INFO,
	//{ "Authorization",	    13, NULL },   // PJSIP_H_AUTHORIZATION,
	{ "Call-ID",	    7,  "i" },    // PJSIP_H_CALL_ID,
	//{ "Call-Info",	    9,  NULL },   // PJSIP_H_CALL_INFO,
	{ "Contact",	    7,  "m" },    // PJSIP_H_CONTACT,
	//{ "Content-Disposition",19, NULL },   // PJSIP_H_CONTENT_DISPOSITION,
	{ "Content-Encoding",   16, "e" },    // PJSIP_H_CONTENT_ENCODING,
	//{ "Content-Language",   16, NULL },   // PJSIP_H_CONTENT_LANGUAGE,
	{ "Content-Length",	    14, "l" },    // PJSIP_H_CONTENT_LENGTH,
	{ "Content-Type",	    12, "c" },    // PJSIP_H_CONTENT_TYPE,
	//{ "CSeq",		     4, NULL },   // PJSIP_H_CSEQ,
	//{ "Date",		     4, NULL },   // PJSIP_H_DATE,
	//{ "Error-Info",	    10, NULL },   // PJSIP_H_ERROR_INFO,
	//{ "Expires",	     7, NULL },   // PJSIP_H_EXPIRES,
	{ "From",		     4, "f" },    // PJSIP_H_FROM,
	{ "Subject",	     7, "s" },    // PJSIP_H_SUBJECT,
	{ "Supported",	     9, "k" },    // PJSIP_H_SUPPORTED,
	{ "To",		     2, "t" },    // PJSIP_H_TO,
	{ "Via",		     3, "v" },    // PJSIP_H_VIA,
	
};
#endif


//=============================================================================
class SipHandler
{
public:
#if USE_UDP_SIP_TRANSPORT
	SipHandler(std::string domain,int port,UdpSipServer *pServer) 
#else
	SipHandler(std::string domain, int port, tcp_sip_lib::TcpSipServer *pServer)
#endif
	{

		this->pServer = pServer;
		this->listener_port = port;
		this->listener_IPADDR = domain;

		clientIP="";
		clientPort=0;
	}
	void set_ip_port_address(sockaddr_in *pA, std::string ip, int port);
	 void insertCallDetails(std::string cid, CallDetails cd);
	
	 bool TRYING_100(Request& req, std::stringstream &ss);
	 bool forwardRinging(std::string servIp, Request& req, std::stringstream &ss);
	 bool forward_200_OK(std::string servIp, Request& req, std::stringstream &ss);

	 bool forwardACK(std::string servIp, Request& req, std::stringstream &ss);

	 bool forwardBYE(std::string servIp, Request& req, std::stringstream &ss);

	 bool forwardCANCEL(std::string servIp, Request& req, std::stringstream &ss);

	 bool forwardRequestTerminated(std::string servIp, Request& req, std::stringstream &ss);

	 bool forwardInvite(SipStream* strm,Request& req,std::stringstream &ss);

	int processInviteRequest(SipStream* strm, Request& req);

	void set_response_buf(Response &res, const char* buf, int len, int sts);

	void set_response_post(Response &res, std::string &str, int sts);

	void _response_with_html(Request& req, Response& res, string &outStr);


	 bool process_register_request(SipStream* strm, Request& req, Response& res);
	bool extractForwardInfo(SipRequest &sr, bool from, ForwardInfo  &frw, const char *title);
	 bool process_180_Ringing(SipStream* strm, Request& req, Response& res);
	bool process_200_OK(SipStream* strm, Request& req, Response& res);

	bool processAckRequest(SipStream* strm, Request& req);

	 void remove_from_current_calls(std::string sr_cid);

	 bool processByeRequest(SipStream* strm, Request& req);

	 bool processCancelRequest(SipStream* strm, Request& req);

	 bool process_487_Request(SipStream* strm, Request& req);

	 bool process_486_Request(SipStream* strm, Request& req);

	 void dump_headers(Request& req);

	int get_client_address(SipStream* strm);
	private:
		static  m_map reg_clients;
		static  std::map<std::string, CallDetails> current_calls;

		std::mutex hdl_mutex;  // to protect access to the above statics

		std::string listener_IPADDR;
		int listener_port; //voip_listener_IPADDR_SERVER_PORT

		std::string clientIP;
		int clientPort;
#if USE_UDP_SIP_TRANSPORT
		UdpSipServer *pServer = NULL;
#else
		
		TcpSipServer *pServer = NULL;


#endif
};


