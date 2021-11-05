#include "mkr_sip_handler.hpp"


m_map SipHandler::reg_clients;
std::map<std::string,CallDetails> SipHandler::current_calls;
//=============================================================================
 void SipHandler::set_ip_port_address(sockaddr_in *pA,std::string ip,int port)
{
	pA->sin_family = AF_INET; // IPv4 
	pA->sin_addr.s_addr = inet_addr(ip.c_str());//INADDR_ANY;
	pA->sin_port = htons(port);
}
 void SipHandler::insertCallDetails(std::string cid, CallDetails cd)
{
	int indOf = indexOf(cid,"@");
	string callId = "";
	if (indOf > 0)
		callId = cid.substr(0, indOf);
	else
		callId = cid;
	std::lock_guard<std::mutex> lock(hdl_mutex);
	current_calls[callId]= cd;
}

 bool SipHandler::TRYING_100(Request& req,std::stringstream &ss)      //method to generate trying response
{
	SipRequest &sr = req.sr;
	ss << "SIP/2.0 100 TRYING\r\n";

	//add recieved to topmost via feild
	string upperViaFeild = sr.via[0];
	string recieved = extractSubStr(upperViaFeild, " ", ":");
	upperViaFeild = upperViaFeild;// +";recieved=" + recieved;
	sr.via[0]= upperViaFeild; //via.set(0, upperViaFeild);
#if !USE_SHORT_NAMES
	for (int in = 0; in < sr.via.size(); in++)
		ss << "Via: " << sr.via[in] << "\r\n";

	ss << "From: " << sr.from << "\r\n";
	ss << "To: " << sr.to << "\r\n";
	ss << "Call-ID: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";
	/*ss << "Allow: " << sr.allow << "\r\n";
	ss << "User-Agent: " << sr.userAgent << "\r\n";
	ss << "Supported: " << sr.supported << "\r\n";*/
	ss << "Content-Length: 0" << "\r\n\r\n";
#else
	for (int in = 0; in < sr.via.size(); in++)
		ss << "v: " << sr.via[in] << "\r\n";

	ss << "f: " << sr.from << "\r\n";
	ss << "i: " << sr.to << "\r\n";
	ss << "i: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";
	/*ss << "Allow: " << sr.allow << "\r\n";
	ss << "User-Agent: " << sr.userAgent << "\r\n";
	ss << "Supported: " << sr.supported << "\r\n";*/
	ss << "l: 0" << "\r\n\r\n";
#endif
	return true;
}
 bool SipHandler::forwardRinging(std::string servIp, Request& req, std::stringstream &ss)
{
	SipRequest &sr = req.sr;
	ss << "SIP/2.0 180 Ringing\r\n";

	string v0=sr.via[0];
	int ind0 = indexOf(v0, ",");
	string modifiedVia = v0.substr(ind0 + 1);
	sr.via[0]= modifiedVia;
#if !USE_SHORT_NAMES
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "Via: " << sr.via[in] << "\r\n";

	ss << "From: " << sr.from << "\r\n";
	ss << "To: " << sr.to << "\r\n";
	ss << "Call-ID: " << sr.cid << "\r\n";
	ss << "CSeq: " + sr.cseq << "\r\n";
	if (!sr.contact.empty())
	{
		string modifiedContact = sr.contact.substr(0, indexOf(sr.contact, "@") + 1) + servIp + ">";
		ss << "Contact: " << modifiedContact << "\r\n";
	}
	//ss << "User-Agent: " << sr.userAgent << "\r\n";

	ss << "Content-Length: 0" << "\r\n\r\n";
#else
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "v: " << sr.via[in] << "\r\n";

	ss << "f: " << sr.from << "\r\n";
	ss << "t: " << sr.to << "\r\n";
	ss << "i: " << sr.cid << "\r\n";
	ss << "CSeq: " + sr.cseq << "\r\n";
	if (!sr.contact.empty())
	{
		string modifiedContact = sr.contact.substr(0, indexOf(sr.contact, "@") + 1) + servIp + ">";
		ss << "m: " << modifiedContact << "\r\n";
	}
	//ss << "User-Agent: " << sr.userAgent << "\r\n";

	ss << "l: 0" << "\r\n\r\n";
#endif
	return true;
}
 bool SipHandler::forward_200_OK(std::string servIp, Request& req, std::stringstream &ss)
{
	SipRequest &sr = req.sr;
	ss << "SIP/2.0 200 OK\r\n";

	string v0 = sr.via[0];
	if (indexOf(v0, ",") != -1)
	{
		string modifiedVia = v0.substr(indexOf(v0, ",") + 1);
		sr.via[0] = modifiedVia;
	}
	else
		removeItemFromVector(sr.via, 0);//via.remove(0);

#if !USE_SHORT_NAMES
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "Via: " << sr.via[in] << "\r\n";

	ss << "From: " << sr.from << "\r\n";
	ss << "To: " << sr.to << "\r\n";
	ss << "Call-ID: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";
#else
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "v: " << sr.via[in] << "\r\n";

	ss << "f: " << sr.from << "\r\n";
	ss << "t: " << sr.to << "\r\n";
	ss << "i: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";
#endif
	
	if (indexOf(sr.cseq,"CANCEL")!=-1)     //If this OK is in response to a CANCEL then
	{
		if (!sr.contact.empty())
		{
			string modifiedContact = sr.contact.substr(0, indexOf(sr.contact, "@") + 1) + servIp + ">";
#if !USE_SHORT_NAMES
			ss << "Contact: " << modifiedContact << "\r\n";
#else
			ss << "m: " << modifiedContact << "\r\n";
#endif
			MKR_PRINTF("MC--%s", modifiedContact.c_str());
		}
	}
	//svk Content-Type was missing here !!!!  but pjsip requires it
	if (sr.cont_len != "0" && !sr.cont_type.empty())
	{
#if !USE_SHORT_NAMES
		ss << "Content-Type: " << sr.cont_type << "\r\n";
#else
		ss << "c: " << sr.cont_type << "\r\n";
#endif
	}
#if !USE_SHORT_NAMES
	ss << "Content-Length: " << sr.cont_len << "\r\n\r\n";
#else
	ss << "l: " << sr.cont_len << "\r\n\r\n";
#endif

	if (indexOf(sr.cseq,"BYE")==-1 && indexOf(sr.cseq,"CANCEL")==-1)   //If this OK is not in response to a BYE or a CANCEL then
	{
		ss << req.body;//setSdpPayload(sr, ss);
	}


	return true;
}

 bool SipHandler::forwardACK(std::string servIp, Request& req, std::stringstream &ss)
{
	SipRequest &sr = req.sr;
	ss << req.method << "\r\n";

	//add recieved to topmost via feild
	string upperViaFeild = sr.via[0];
	string recieved = extractSubStr(upperViaFeild, " ", ":");//upperViaFeild.substr(indexOf(upperViaFeild," ") + 1, indexOf(upperViaFeild,":"));
	upperViaFeild = upperViaFeild;// +";recieved=" + recieved;
	sr.via[0] = upperViaFeild;

	//add this server's Via tag
	insertToFront(sr.via, "SIP/2.0/UDP " + servIp + ":" + std::to_string(this->listener_port) + ";branch=z9hG4bK2d4790");
#if !USE_SHORT_NAMES
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "Via: " << sr.via[in] << "\r\n";

	ss << "From: " << sr.from << "\r\n";
	ss << "To: " << sr.to << "\r\n";
	ss << "Call-ID: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";
	if (!sr.contact.empty())
		ss << "Contact: " << sr.contact << "\r\n";
	ss << "Content-Length: 0\r\n\r\n";
#else
	for (int in = 0; in < sr.via.size(); in++)
		ss << "v: " << sr.via[in] << "\r\n";

	ss << "f: " << sr.from << "\r\n";
	ss << "t: " << sr.to << "\r\n";
	ss << "i: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";
	if (!sr.contact.empty())
		ss << "m: " << sr.contact << "\r\n";
	ss << "l: 0\r\n\r\n";
#endif
	return true;
}

 bool SipHandler::forwardBYE(std::string servIp, Request& req, std::stringstream &ss)
{
	SipRequest &sr = req.sr;
	ss << req.method << "\r\n";
	
	//add recieved to topmost via feild
	string upperViaFeild = sr.via[0];
	string recieved = extractSubStr(upperViaFeild, " ", ":");//upperViaFeild.substr(indexOf(upperViaFeild, " ") + 1, indexOf(upperViaFeild, ":"));
	upperViaFeild = upperViaFeild;// +";recieved=" + recieved;
	sr.via[0] = upperViaFeild;

	//add this server's Via tag
	insertToFront(sr.via, "SIP/2.0/UDP " + servIp + ":" + std::to_string(this->listener_port) + ";branch=z9hG4bK2d4790");
#if !USE_SHORT_NAMES
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "Via: " << sr.via[in] << "\r\n";


	ss << "From: " << sr.from << "\r\n";
	ss << "To: " << sr.to << "\r\n";
	ss << "Call-ID: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";

	// String modifiedContact = "<"+ contact.substring(0,contact.indexOf("@")+1)+ servIp+">";  // svk added "<"+   ??? 
	// fwd_res = fwd_res + "Contact: " + modifiedContact + "\r\n";

	ss << "Max-Forwards: " << (strToInt(trim(sr.maxForwards)) - 1) << "\r\n";
	//ss << "User-Agent: " << sr.userAgent << "\r\n";
	ss << "Content-Length: " << sr.cont_len << "\r\n\r\n";
#else
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "v: " << sr.via[in] << "\r\n";
	ss << "f: " << sr.from << "\r\n";
	ss << "t: " << sr.to << "\r\n";
	ss << "i: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";

	// String modifiedContact = "<"+ contact.substring(0,contact.indexOf("@")+1)+ servIp+">";  // svk added "<"+   ??? 
	// fwd_res = fwd_res + "Contact: " + modifiedContact + "\r\n";

	ss << "Max-Forwards: " << (strToInt(trim(sr.maxForwards)) - 1) << "\r\n";
	//ss << "User-Agent: " << sr.userAgent << "\r\n";
	ss << "l: " << sr.cont_len << "\r\n\r\n";
#endif
	return true;
}

 bool SipHandler::forwardCANCEL(std::string servIp, Request& req, std::stringstream &ss)  // TODO it the same as forwardBYE
{
	SipRequest &sr = req.sr;
	ss << req.method << "\r\n";

	//add recieved to topmost via feild
	string upperViaFeild = sr.via[0];
	string recieved = extractSubStr(upperViaFeild, " ", ":");//upperViaFeild.substr(indexOf(upperViaFeild, " ") + 1, indexOf(upperViaFeild, ":"));
	upperViaFeild = upperViaFeild;// +";recieved=" + recieved;
	sr.via[0] = upperViaFeild;

	//add this server's Via tag
	insertToFront(sr.via, "SIP/2.0/UDP " + servIp + ":" + std::to_string(this->listener_port) + ";branch=z9hG4bK2d4790");
#if !USE_SHORT_NAMES
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "Via: " << sr.via[in] << "\r\n";


	ss << "From: " << sr.from << "\r\n";
	ss << "To: " << sr.to << "\r\n";
	ss << "Call-ID: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";

	// String modifiedContact = "<"+ contact.substring(0,contact.indexOf("@")+1)+ servIp+">";  // svk added "<"+   ??? 
	// fwd_res = fwd_res + "Contact: " + modifiedContact + "\r\n";

	ss << "Max-Forwards: " << (strToInt(trim(sr.maxForwards)) - 1) << "\r\n";
	//ss << "User-Agent: " << sr.userAgent << "\r\n";
	ss << "Content-Length: " << sr.cont_len << "\r\n\r\n";
#else
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "v: " << sr.via[in] << "\r\n";


	ss << "f: " << sr.from << "\r\n";
	ss << "t: " << sr.to << "\r\n";
	ss << "i: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";

	// String modifiedContact = "<"+ contact.substring(0,contact.indexOf("@")+1)+ servIp+">";  // svk added "<"+   ??? 
	// fwd_res = fwd_res + "Contact: " + modifiedContact + "\r\n";

	ss << "Max-Forwards: " << (strToInt(trim(sr.maxForwards)) - 1) << "\r\n";
	//ss << "User-Agent: " << sr.userAgent << "\r\n";
	ss << "l: " << sr.cont_len << "\r\n\r\n";
#endif
	return true;
}

 bool SipHandler::forwardRequestTerminated(std::string servIp, Request& req, std::stringstream &ss)
{
	SipRequest &sr = req.sr;
	ss << req.method << "\r\n";

	string v0 = sr.via[0];
	if (indexOf(v0, ",") != -1)
	{
		string modifiedVia = v0.substr(indexOf(v0, ",") + 1);
		sr.via[0] = modifiedVia;
	}
	else
		removeItemFromVector(sr.via, 0);//via.remove(0);

#if !USE_SHORT_NAMES
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "Via: " << sr.via[in] << "\r\n";

	ss << "From: " << sr.from << "\r\n";
	ss << "To: " << sr.to << "\r\n";
	ss << "Call-ID: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";

	
	{
		//string modifiedContact = sr.contact.substr(0, indexOf(sr.contact, "@") + 1) + servIp + ">";
		//ss << "Contact: " << modifiedContact << "\r\n";
	}
	
	ss << "Content-Length: 0" << "\r\n\r\n";
#else
	//add via feilds
	for (int in = 0; in < sr.via.size(); in++)
		ss << "v: " << sr.via[in] << "\r\n";

	ss << "f: " << sr.from << "\r\n";
	ss << "t: " << sr.to << "\r\n";
	ss << "i: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";


	{
		//string modifiedContact = sr.contact.substr(0, indexOf(sr.contact, "@") + 1) + servIp + ">";
		//ss << "Contact: " << modifiedContact << "\r\n";
	}

	ss << "l: 0" << "\r\n\r\n";
#endif
	return true;
}

bool SipHandler::forwardInvite(SipStream *strm,Request& req,std::stringstream &ss)    //to generate fwd response
{
	ss << req.method << "\r\n";
	SipRequest &sr = req.sr;
	//add this server's Via tag
	std::string str = "SIP/2.0/";
	if (strm->getUserData() != NULL)
	{
		// request from WSS
		str += ("WSS mikerel.com" );
	}
	else {
		str += ("UDP " + this->listener_IPADDR);
	}
	str += (":" + std::to_string(this->listener_port) + ";branch=z9hG4bK2d4790");
	insertToFront(sr.via, str);
#if !USE_SHORT_NAMES	
	for (int i = 0; i < sr.via.size(); i++)
		ss << "Via: " << sr.via[i] << "\r\n";

	ss << "From: " << sr.from << "\r\n";
	ss << "To: " << sr.to << "\r\n";
	ss << "Call-ID: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";

	std::string modifiedContact = sr.contact.substr(0, indexOf(sr.contact,"@") + 1) + this->listener_IPADDR + ">";
	ss << "Contact: " << modifiedContact << "\r\n";

	ss << "Content-Type: " << sr.cont_type << "\r\n";
	ss << "Max-Forwards: " << (strToInt(trim(sr.maxForwards)) - 1) << "\r\n";
	//ss << "User-Agent: " << sr.userAgent << "\r\n";
	ss << "Content-Length: " << sr.cont_len << "\r\n\r\n";
#else
	for (int i = 0; i < sr.via.size(); i++)
		ss << "v: " << sr.via[i] << "\r\n";

	ss << "f: " << sr.from << "\r\n";
	ss << "t: " << sr.to << "\r\n";
	ss << "i: " << sr.cid << "\r\n";
	ss << "CSeq: " << sr.cseq << "\r\n";

	std::string modifiedContact = sr.contact.substr(0, indexOf(sr.contact, "@") + 1) + this->listener_IPADDR + ">";
	ss << "m: " << modifiedContact << "\r\n";

	ss << "c: " << sr.cont_type << "\r\n";
	ss << "Max-Forwards: " << (strToInt(trim(sr.maxForwards)) - 1) << "\r\n";
	//ss << "User-Agent: " << sr.userAgent << "\r\n";
	ss << "l: " << sr.cont_len << "\r\n\r\n";
#endif
	ss << req.body;//setSdpPayload(sr, ss);

	return true;
}
	
	int SipHandler::processInviteRequest(SipStream* strm,Request& req)
	{
		//find who is calling
		SipRequest &sr = req.sr;
		string callerNumber = extractSubStr(sr.from, ":", "@");

		//find whom to send this invite
		string line1 = req.method;
		string calleeNumber = extractSubStr(line1, ":", "@");
		ClientSipInfo ci;
		{
			std::lock_guard<std::mutex> lock(hdl_mutex);
			bool rv = containsKey(reg_clients, calleeNumber);
			if (!rv)
			{
				MKR_PRINTF("INVITE %s => %s Exc\r\n", callerNumber.c_str(), calleeNumber.c_str());
				return -1;
			}
			ci = reg_clients[calleeNumber];
		}
		string calleeIp = ci.host;//extractIpOrPort(ci.ipPort, 0);
		/*string*/int calleePort = ci.port;// extractIpOrPort(ci.ipPort, 1);

		MKR_PRINTF("INVITE %s => %s\r\n" , callerNumber.c_str() ,calleeNumber.c_str());

		std::stringstream ss;
		TRYING_100(req,ss);
		pServer->sendResponse(strm, /*pDestAddr*/NULL, ss);  // NULL -> send to the caller IP address

		std::stringstream ss1;
		forwardInvite(strm,req, ss1);
		// set dest address
		sockaddr_in destAddr;
		set_ip_port_address(&destAddr, calleeIp, /*strToInt(calleePort)*/calleePort);
		pServer->sendResponse(strm, &destAddr, ss1);

		CallDetails cd(callerNumber, calleeNumber);
		insertCallDetails(sr.cid, cd);
		return 1;
	}




void SipHandler::set_response_buf(Response &res, const char* buf,int len,int sts)
{
	res.body.assign(buf, len);
	res.status = sts;
}

void SipHandler::set_response_post(Response &res,std::string &str,int sts)
{
	res.body.assign(str.data(), str.size());
	res.status = sts;
}

 void SipHandler::_response_with_html(Request& req, Response& res,string &outStr)
{
	//evhtp_headers_add_header(req->headers_out, evhtp_header_new(server_key, server_val, 0, 0));
	
	//res.set_content(outStr, "text/html");
	//res.status = 200;
}


bool SipHandler::process_register_request(SipStream* strm,Request& req, Response& res)
{
	bool rv = false;
	
		
		std::string sipUri=extractSubStr(req.sr.contact, ":", ";");
		string contact_number="", contact_ipPort="";
		if (indexOf(sipUri,">")==-1)
		{
			//this means there is no > (for Jitsi)
			int ind = indexOf(sipUri, "@");
			contact_number = sipUri.substr(0, ind);//
			contact_ipPort = sipUri.substr(ind+1, sipUri.size()- (ind + 1));
		}
		else
		{
			//this means there is a > (for Phoner)
			int ind = indexOf(sipUri, "@");
			contact_number = sipUri.substr(0, ind);
			contact_ipPort = sipUri.substr(ind + 1, sipUri.size()- (ind + 2));
		}
		ClientSipInfo ci;
		bool isRegistered = false;
		int expires = strToInt(trim(req.sr.expireStr));
		void *udata = NULL;
		bool flag = false, flag1 = false;
		{
			std::lock_guard<std::mutex> lock(hdl_mutex);
			isRegistered=containsKey(reg_clients, contact_number);
		
			if (/* !isRegistered && */ expires > 0)    //register
			{
				udata = strm->getUserData();
				flag = true;
				ci.clientName = contact_number;
				/*if (udata == NULL) {
					ci.host = extractIpOrPort(contact_ipPort, 0);
					std::string port = extractIpOrPort(contact_ipPort, 1);
					if (is_digits(port))
						ci.port = std::stoi(port);
					else
						ci.port = -1;
				}
				else {
					ci.host = this->clientIP;
					ci.port = this->clientPort;
				}*/

				ci.host = this->clientIP;
				ci.port = this->clientPort;

				insertToMMap(reg_clients, contact_number, ci);
				
			}
			else if (isRegistered && expires == 0)   //unregister
			{
				flag1 = true;
				eraseFromMMap(reg_clients, contact_number);
			}
		}
		if(flag)
			MKR_PRINTF("Dev %s Registered at IPPORT=%s ci:(%s:%d) transport:%s\r\n", contact_number.c_str(), contact_ipPort.c_str(), ci.host.c_str(), ci.port, udata == NULL ? "udp" : "ws");
		else if(flag1)
			MKR_PRINTF("Dev %s UNREGISTERED\r\n", contact_number.c_str());


		// prepare response
		
		////send OK response to caller
		
		stringstream ss;
		
		ss << "SIP/2.0 200 OK" << "\r\n";
#if !USE_SHORT_NAMES
		ss << "Via: " << req.sr.via[0] << "\r\n";
		ss << "From: " << req.sr.from << "\r\n";
		ss << "To: " << req.sr.to << "\r\n";
		ss << "Call-ID: " << req.sr.cid << "\r\n";
		ss << "CSeq: " << req.sr.cseq << "\r\n";
	
		//websocket JsSIP.js require Contact
		ss << "Contact: " << req.sr.contact << "\r\n";

		ss << "Content-Length: 0\r\n\r\n";                
#else
		if(req.sr.via.size()>0)
			ss << "v: " << req.sr.via[0] << "\r\n";
		ss << "f: " << req.sr.from << "\r\n";
		ss << "t: " << req.sr.to << "\r\n";
		ss << "i: " << req.sr.cid << "\r\n";
		ss << "CSeq: " << req.sr.cseq << "\r\n";

		//websocket JsSIP.js require Contact
		ss << "m: " << req.sr.contact << "\r\n";

		ss << "l: 0\r\n\r\n";
#endif

		//sockaddr_in destAddr;
		//set_ip_port_address(&destAddr, ci.host, (ci.port-1) /*strToInt(trim(frw.fwdPort))*/);
		pServer->sendResponse(strm, /*&destAddr*/NULL, ss);  // NULL -> send to the caller IP address

	return rv;
}
 bool SipHandler::extractForwardInfo(SipRequest &sr,bool from, ForwardInfo  &frw,const char *title)
{
	std::string& str = from ? sr.from : sr.to;
	frw.fwdNumber = extractSubStr(str, ":", "@");
	ClientSipInfo ci;
	{
		std::lock_guard<std::mutex> lock(hdl_mutex);
		if (!containsKey(reg_clients, frw.fwdNumber))
		{
			MKR_PRINTF("%s null: %s\r\n", title, frw.fwdNumber.c_str());
			return false;
		}
		ci = reg_clients[frw.fwdNumber];
	}
	frw.fwdIp = ci.host;//extractIpOrPort(si.ipPort, 0);
	frw.fwdPort = ci.port;//extractIpOrPort(si.ipPort, 1);
	MKR_PRINTF("%s forwarded to %s  addr=%s:%d\r\n",title, frw.fwdNumber.c_str(), frw.fwdIp.c_str(), frw.fwdPort);
	return true;
}
bool SipHandler::process_180_Ringing(SipStream* strm, Request& req, Response& res)
{
	ForwardInfo frw;
	SipRequest &sr = req.sr;
	bool rv=extractForwardInfo(sr,true, frw, "Ringing");
	if (!rv) {
		return rv;
	}
		
		
		//forward ringing
		std::stringstream ss;
		forwardRinging(voip_listener_IPADDR,req,ss);
		sockaddr_in destAddr;
		set_ip_port_address(&destAddr, frw.fwdIp, frw.fwdPort /*strToInt(trim(frw.fwdPort))*/);
		pServer->sendResponse(strm, &destAddr, ss);
				
	
	return rv;
}
bool SipHandler::process_200_OK(SipStream* strm, Request& req, Response& res)
{
	ForwardInfo frw;
	SipRequest &sr = req.sr;
	bool rv = extractForwardInfo(sr, true, frw, "200_OK");
	if (!rv) {
		return rv;
	}
	//
	std::stringstream ss;
	forward_200_OK(voip_listener_IPADDR, req, ss);
	sockaddr_in destAddr;
	set_ip_port_address(&destAddr, frw.fwdIp, frw.fwdPort /*strToInt(trim(frw.fwdPort))*/);

	pServer->sendResponse(strm, &destAddr, ss);

	return rv;
}

 bool SipHandler::processAckRequest(SipStream* strm, Request& req)
{
	ForwardInfo frw;
	SipRequest &sr = req.sr;
	bool rv = extractForwardInfo(sr, false, frw, "ACK");
	if (!rv) {
		return rv;
	}
	//forward 
	std::stringstream ss;
	forwardACK(voip_listener_IPADDR, req, ss);
	sockaddr_in destAddr;
	set_ip_port_address(&destAddr, frw.fwdIp, frw.fwdPort /*strToInt(trim(frw.fwdPort))*/);
	pServer->sendResponse(strm, &destAddr, ss);

	return rv;
}

 void SipHandler::remove_from_current_calls(std::string sr_cid)
{
	int indOf = indexOf(sr_cid, "@");
	string callId = "";
	if (indOf > 0)
		callId = sr_cid.substr(0, indOf);
	else
		callId = sr_cid;
	std::lock_guard<std::mutex> lock(hdl_mutex);
	if (current_calls.find(callId) != current_calls.end())
	{
		CallDetails cd = current_calls[callId];
		MKR_PRINTF("call  %s => %s  has been ENDED.\r\n", cd.callerNumber.c_str(), cd.calleeNumber.c_str());
		current_calls.erase(callId);//remove(callId);
	}
}

 bool SipHandler::processByeRequest(SipStream* strm, Request& req)
{
	ForwardInfo frw;
	SipRequest &sr = req.sr;
	bool rv = extractForwardInfo(sr, false, frw, "BYE");
	if (!rv) {
		return rv;
	}
	//forward ringing
	std::stringstream ss;
	forwardBYE(voip_listener_IPADDR, req, ss);
	sockaddr_in destAddr;
	set_ip_port_address(&destAddr, frw.fwdIp, frw.fwdPort /* strToInt(trim(frw.fwdPort))*/ );
	pServer->sendResponse(strm, &destAddr, ss);

	//remove this call from CURRENTCALLS
	remove_from_current_calls(sr.cid);

	return rv;
}

 bool SipHandler::processCancelRequest(SipStream* strm, Request& req)
{
	ForwardInfo frw;
	SipRequest &sr = req.sr;
	bool rv = extractForwardInfo(sr, false, frw, "CANCEL");
	if (!rv) {
		return rv;
	}
	//forward 
	std::stringstream ss;
	forwardCANCEL(voip_listener_IPADDR, req, ss);
	sockaddr_in destAddr;
	set_ip_port_address(&destAddr, frw.fwdIp, frw.fwdPort /*strToInt(trim(frw.fwdPort))*/ );
	pServer->sendResponse(strm, &destAddr, ss);

	//remove this call from CURRENTCALLS
	remove_from_current_calls(sr.cid);

	return rv;
}

 bool SipHandler::process_487_Request(SipStream* strm, Request& req)
{
	ForwardInfo frw;
	SipRequest &sr = req.sr;
	bool rv = extractForwardInfo(sr, true, frw, "487_Req");
	if (!rv) {
		return rv;
	}
	//forward ringing
	std::stringstream ss;
	forwardRequestTerminated(voip_listener_IPADDR, req, ss);
	sockaddr_in destAddr;
	set_ip_port_address(&destAddr, frw.fwdIp, frw.fwdPort /*strToInt(trim(frw.fwdPort))*/);
	pServer->sendResponse(strm, &destAddr, ss);

	return rv;
}

 bool SipHandler::process_486_Request(SipStream* strm, Request& req)
{
	ForwardInfo frw;
	SipRequest &sr = req.sr;
	bool rv = extractForwardInfo(sr, true, frw, "486_Busy");
	if (!rv) {
		return rv;
	}
	//forward ringing
	std::stringstream ss;
	forwardRequestTerminated(voip_listener_IPADDR, req, ss);
	sockaddr_in destAddr;
	set_ip_port_address(&destAddr, frw.fwdIp, frw.fwdPort /*strToInt(trim(frw.fwdPort))*/);
	pServer->sendResponse(strm, &destAddr, ss);

	//remove this call from CURRENTCALLS
	remove_from_current_calls(sr.cid);

	return rv;
}

void SipHandler::dump_headers(Request& req)
{
	/*M_HTTP_PRINTF(">>>>line1: %s", req.method.c_str());
	SipRequest &sr = req.sr;
	M_HTTP_PRINTF("%s", sr.contact.c_str());

	M_HTTP_PRINTF(">>>>>>>>>>>>>>\r\n");*/
	
}

 int SipHandler::get_client_address(SipStream* strm)
{
	SOCKADDR *pAddr = NULL;
	pAddr = strm->getClientAddr();
	
	this->clientIP = "";
	this->clientPort = 0;
	
	if (pAddr) {
		struct sockaddr_in *pAddr1 = (struct sockaddr_in *)pAddr;
		char *ip = inet_ntoa(pAddr1->sin_addr);
		this->clientIP = std::string(ip);
		this->clientPort = ntohs(pAddr1->sin_port);

		int length = 0;
		const char *pCh = strm->getInBuffer(length);
		MKR_PRINTF("\r\n<<Req<< from %s:%d length=%d\r\n", this->clientIP.c_str(), clientPort, length);
		
	}
		return 1;
}


