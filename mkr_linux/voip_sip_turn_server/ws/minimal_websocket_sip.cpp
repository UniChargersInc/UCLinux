
#include <wsock/libwebsockets.h>
#include <wsock/private-libwebsockets.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "../src/mkr_sip_handler.hpp"
#include "../src/sip_server_config.h"


class WsSipStream : public TcpSocketStream
{
public:	
	WsSipStream(socket_t socket) : TcpSocketStream(socket) {}
	~WsSipStream() {}

	int write(const char* ptr, size_t size)
	{
		
		//return send(sock_, ptr, size, 0);
		return 0;
	}

	int write(const char* ptr)
	{
		return write(ptr, strlen(ptr));
	}
};

class WsUdpSipServer :public TcpSipServer
{
public:
	WsUdpSipServer() {  }
	~WsUdpSipServer(){ libwebsocket_context_destroy(context); }
	static int callback_sip(struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason,
		void *user,
		void *in,
		size_t len);
	int process_callback_sip(struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason,
		void *in,
		size_t len);
	int process_callback_close(struct libwebsocket_context *context,
		struct libwebsocket *wsi);
	int init();
	//void run(int *terminate);
	virtual bool listen(const char* host, int port, int *terminate, int socket_flags = 0);
	virtual void sendResponse(SipStream& strm, struct sockaddr_in *pDestAddr, std::stringstream&);
	
private:
	
	struct libwebsocket_context *context;
	
};
static WsUdpSipServer *wsServer = NULL;


static struct libwebsocket_protocols protocols[] = {

	{
		"sip",
		WsUdpSipServer::callback_sip,
		sizeof(/*struct libws_mqtt_data*/void_p_t),
		0,
#ifdef LWS_FEATURE_PROTOCOLS_HAS_ID_FIELD
		1,
#endif
		NULL,
		0
	},
#ifdef LWS_FEATURE_PROTOCOLS_HAS_ID_FIELD
	{ NULL, NULL, 0, 0, 0, NULL, 0}
#else
	{ NULL, NULL, 0, 0, NULL, 0}
#endif
};
int WsUdpSipServer::process_callback_close(struct libwebsocket_context *context, struct libwebsocket *wsi)
{
	MKR_PRINTF("LWS_CALLBACK_CLOSED\r\n");
	
	if(removeActConn(wsi))
		MKR_PRINTF("LWSI removed from list %d\r\n", wsi->sock);
	return 0;
}
int WsUdpSipServer::process_callback_sip(struct libwebsocket_context *context,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason,
	void *in,
	size_t len)
{
	int count;
	
	size_t pos;
	uint8_t *buf;
	int rc;
	uint8_t byte;


	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED: {
		
		MKR_PRINTF("LWS_CALLBACK_ESTABLISHED add to list %d\r\n", wsi->sock);
		insert2ActConn(wsi);
	}
	break;

	case LWS_CALLBACK_CLOSED:
		return process_callback_close(context, wsi);
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
	{
		MKR_PRINTF("\r\nLWS_CALLBACK_WRITEABLE wsi sock=%d\r\n", wsi->sock);
		// send our asynchronous message
		auto it = std::find_if(conn_list.begin(), conn_list.end(), [wsi](const ActiveConnection& c) {
			return (c.wsi == wsi && !c.data_to_send.empty());
			});
		if (it != conn_list.end()) 
		{
			/*		 * libwebsockets requires that the payload has
					 * LWS_SEND_BUFFER_PRE_PADDING space available before the
					 * actual data and LWS_SEND_BUFFER_POST_PADDING afterwards.
					 * We've already made the payload big enough to allow this,
					 * but need to move it into position here. */

			int size_to_send = it->data_to_send.size();
			unsigned char *buf = new unsigned char[LWS_SEND_BUFFER_PRE_PADDING + size_to_send + LWS_SEND_BUFFER_POST_PADDING];
			memmove(&buf[LWS_SEND_BUFFER_PRE_PADDING], (unsigned char*)it->data_to_send.c_str(), size_to_send);
			
			int count=libwebsocket_write(wsi, (buf+ LWS_SEND_BUFFER_PRE_PADDING), it->data_to_send.size(), /*LWS_WRITE_TEXT*/ LWS_WRITE_BINARY);
			MKR_PRINTF("\r\nsent to wsi_sock %d size=%d sent=%d\r\n", wsi->sock, it->data_to_send.size(),count);
			if (count == it->data_to_send.size())
			{
				MKR_PRINTF("%s\r\n", it->data_to_send.c_str());
				it->data_to_send = "";
			}
			delete[] buf;
		}
		
	}
			break;

	case LWS_CALLBACK_RECEIVE:
		{
		MKR_PRINTF("\r\nLWS_CALLBACK_RECEIVE: %d str:%s wsi sock=%d\r\n", len, (const char*)in, wsi->sock);
			WsSipStream strm(wsi->sock);
			Request req;
			Response res;
			strm.setUserData(wsi);                       // it will be used to find a response recipient
			strm.setInBuffer((const char*)in, len);
			SOCKADDR *pAddr = (SOCKADDR *)&wsi->mkr_sock_address_in;
			strm.setClientAddr(pAddr, sizeof(struct sockaddr_in));
			process_request((WsSipStream*)&strm,req,res);
			

			//test
			/*std::stringstream ss;
			std::string str((const char*)in, len);
			ss << str;
			sendResponse(strm, NULL, ss);*/
		}
	

		break;

	default:
		break;
	}
	return 0;
}

int WsUdpSipServer::callback_sip(struct libwebsocket_context *context,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason,
	void *user,
	void *in,
	size_t len)
{
	if (user != NULL)
	{
		//UdpSipServer* p = (UdpSipServer*)context->user_space;
		WsUdpSipServer *_this = (WsUdpSipServer*)context->user_space;   //dynamic_cast<WsUdpSipServer*>(p);
		if(_this)
			_this->process_callback_sip(context, wsi, reason, in, len);
	}
	
	return 0;
}

int WsUdpSipServer::init ()
{
	struct lws_context_creation_info info;
	struct libwebsocket_protocols *p;
	int n = 0, logs = LLL_ERR | LLL_WARN | LLL_NOTICE;

	int protocol_count;
	lws_set_log_level(logs, NULL);

	/* Count valid protocols */
	for (protocol_count = 0; protocols[protocol_count].name; protocol_count++);
	p = (struct libwebsocket_protocols *) calloc(protocol_count + 1, sizeof(struct libwebsocket_protocols));

	int i = 0;

	for (i = 0; protocols[i].name; i++) {
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
	}

	memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
	info.port = voip_ws_listener_PORT;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;

#if defined(LWS_OPENSSL_SUPPORT)
	info.ssl_ca_filepath = "/home/mkr/service/SSL_Certificate/mikerel.crt";
	info.ssl_cert_filepath = "/home/mkr/service/SSL_Certificate/mikerel.crt";
	info.ssl_private_key_filepath = "/home/mkr/service/SSL_Certificate/mikerel.key";
#ifndef LWS_IS_OLD
	//info.ssl_cipher_list = listener->ciphers;
#endif
	//if (listener->require_certificate) {
	//	info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	//}
#endif


#ifndef LWS_IS_OLD
	info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
#endif

	info.user = this;

	this->context = libwebsocket_create_context(&info);
	if (!this->context) {
		lwsl_err("lws init failed\n");
		return 1;
	}


	return 0;
}

void WsUdpSipServer::sendResponse(SipStream& strm, struct sockaddr_in *pDestAddr, std::stringstream& ss)
{
	struct libwebsocket *wsi = (struct libwebsocket *)strm.getUserData();
	if (wsi != NULL)
	{
		
		if (pDestAddr == NULL)
		{
			// we just send the response to the client address
			for (ActiveConnection &x : conn_list)
			{
				struct libwebsocket *wsi1 = (struct libwebsocket*)x.wsi;
				if (wsi1 == wsi)
				{
					x.data_to_send = ss.str();
					
					MKR_PRINTF("sendResponse_0: %d  wsi sock=%d\r\n", x.data_to_send.size(),  wsi->sock);
					libwebsocket_callback_on_writable(this->context, wsi);
					return;
				}
			}
		}
		else {
			// here we need to find wsi with address matches to pDestAddr 
			for (ActiveConnection &x : conn_list)
			{
				struct libwebsocket *wsi1 = (struct libwebsocket*)x.wsi;
				if (wsi1)
				{
					struct sockaddr_in *pAddr = &wsi1->mkr_sock_address_in;
					if (compareAddresses(pDestAddr, pAddr))
					{
						x.data_to_send = ss.str();

						MKR_PRINTF("sendResponse_0: %d  wsi sock=%d\r\n", x.data_to_send.size(), wsi1->sock);
						libwebsocket_callback_on_writable(this->context, wsi1);
						return;
					}
				}
			}
		}
		
	}
	return ;
}
bool WsUdpSipServer::listen(const char* host, int port, int *terminate, int socket_flags)
{
	int n = 0;
	while (n >= 0 && *terminate) {
		n = libwebsocket_service(context, 0);
		if (n == 0)
#ifdef WIN32
			Sleep(10);
#else
			usleep(10 * 1000);
#endif
	}
	return false;
}

int main_ws_sip(int *terminate)
{
#if 0
	WsUdpSipServer svr;
	SipHandler hdl(voip_ws_listener_IPADDR, voip_ws_listener_PORT, &svr);
	wsServer = &svr;
	
	svr.init();
	svr.get("/", [&](SipStream* strm, const Request& req, Response& res) {

		int ind = -1;

		// IMPORTANT req.method comes with "\r\n" at the end -> remove them
		//M_HTTP_PRINTF("line1: %s", req.method.c_str());
		//dump_headers((Request&)req);
		hdl.get_client_address(strm);
		removeCharsFromString((std::string &)req.method, "\r\n");
		string line1 = req.method;
		if (contain_substr(line1, "REGISTER", ind))
		{
			hdl.process_register_request(strm, (Request&)req, res);
		}
		else if (contain_substr(line1, "INVITE", ind))
		{
			hdl.processInviteRequest(strm, (Request&)req);
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
			//MKR_PRINTF("****Request has not been processed\r\n");
		}

		});

	svr.set_error_handler([](const auto &strm, const auto& req, auto& res) {
		// send response to error
		});

	//MKR_PRINTF("ws_listener start  %s:%d\r\n", voip_ws_listener_IPADDR, voip_ws_listener_PORT);
	svr.listen(0,0,terminate,0);
#endif
	return 0;
}