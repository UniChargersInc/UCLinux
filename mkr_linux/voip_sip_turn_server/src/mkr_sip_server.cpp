
#include "mkr_sip_handler.hpp"
#include "sip_server_config.h"



using namespace std;

#if	USE_UDP_SIP_TRANSPORT
UdpSipServer mServer;

#else
TcpSipServer mServer;
#endif

int start_mkr_voip_server(int *terminate)
{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	SSLHttpServer svr(SERVER_CERT_FILE, SERVER_PRIVATE_KEY_FILE);
#endif


		
		
	mServer.get("/", [](SipStream* strm,const Request& req, Response& res) {
		
		int ind = -1;
		SipHandler hdl(voip_listener_IPADDR, voip_listener_IPADDR_SERVER_PORT, &mServer);
		// IMPORTANT req.method comes with "\r\n" at the end -> remove them
		//M_HTTP_PRINTF("line1: %s", req.method.c_str());
		//dump_headers((Request&)req);
		hdl.get_client_address(strm);
		removeCharsFromString((std::string &)req.method, "\r\n");
		string line1 = req.method;

		//sanity check
		if (req.sr.via.size() == 0 || (req.sr.from.empty() && req.sr.to.empty()))
		{
			std::stringstream ss;
			ss << "SIP/2.0 400 Bad Request" << "\r\n";
			mServer.sendResponse(strm, NULL, ss);
			//MKR_PRINTF("400 Bad Request..");
			return;
		}

		if (contain_substr(line1 , "REGISTER",ind))
		{
			hdl.process_register_request(strm,(Request&)req, res);
		}
		else if (contain_substr(line1, "INVITE", ind))
		{
			hdl.processInviteRequest(strm,(Request& ) req);
		}
		else if (contain_substr(line1, "SIP/2.0 200 OK", ind))
		{
			hdl.process_200_OK(strm, (Request&)req, res);  //recieved OK from callee
		}
		else if (contain_substr(line1, "SIP/2.0 180 Ringing", ind))
		{
			hdl.process_180_Ringing(strm, (Request&)req, res);
		}
		else if (contain_substr(line1, "ACK", ind))
		{
			hdl.processAckRequest(strm, (Request&)req);
		}
		else if (contain_substr(line1, "BYE", ind))
		{
			hdl.processByeRequest(strm, (Request&)req);
		}
		else if (contain_substr(line1, "CANCEL", ind))
		{
			hdl.processCancelRequest(strm, (Request&)req);
		}
		else if (contain_substr(line1, "487 Request", ind))
		{
			hdl.process_487_Request(strm, (Request&)req);
		}
		else if (contain_substr(line1, "486 Busy", ind))
		{
			hdl.process_486_Request(strm, (Request&)req);
		}
		else {
			std::string str = "Request: " + line1 + " dropped\r\n";
			MKR_PRINTF(str.c_str());
		}
		
	});

	mServer.set_error_handler([](const auto &strm,const auto& req, auto& res) {
		// send response to error
	});

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	svr.listen(HTTP_LIGHT_SERVER_IPADDR, HTTPS_LIGHT_SERVER_PORT, terminate);
#else
	MKR_PRINTF( "UdorX VoIP Sip Server on %s:%d\r\n", voip_listener_IPADDR, voip_listener_IPADDR_SERVER_PORT);
//	svr.test_sip_request();
	mServer.listen(voip_listener_IPADDR, voip_listener_IPADDR_SERVER_PORT, terminate);
#endif 
	return 0;
}

int stop_mkr_voip_server(void)
{
	//if (mServer)
	{
		mServer.stop();
	}
	return true;
}


