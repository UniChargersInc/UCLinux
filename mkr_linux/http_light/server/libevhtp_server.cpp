#include "udorx_server_config.h"
#if USE_LIBEVENT


#include "libevhtp_server.h"
#include "webservice/WebService.h"

#ifdef WIN32
#include <ws2tcpip.h>  //inet_ntop
#else
#include <arpa/inet.h> //-> linux inet_ntop
#endif
#include <google/protobuf/stubs/strutil.h>  //GlobalReplaceSubstring
void m_printf(const char *format, ...);

#if MKR_USE_INTERNAL_MAP_TILES_SERVICE
int tile_server_init(void);
int tile_server_process_request(evhtp_request_t * request, std::string &req_path, std::string &req_ext);
#endif

/*
https://stackoverflow.com/questions/1321878/how-to-prevent-favicon-ico-requests 
https://en.wikipedia.org/wiki/Favicon
*/

//extern const char* server_key ;
//extern const char* server_val;
//extern const char* welcome_msg;
//
//extern const char* content_type;
//extern const char* content_disp;
//extern const char* content_len;
//
//WebSession* GetWebSessionByID(WebSession *pws);

void process_main_application_request(evhtp_request_t * req, struct app* app, evthr_t* thread, evhtp_connection_t * conn);

//static evthr_t *get_request_thr(evhtp_request_t * request) 
//{
//	evhtp_connection_t * htpconn;
//	evthr_t            * thread;
//
//	htpconn = evhtp_request_get_connection(request);
//	thread = htpconn->thread;
//
//	return thread;
//}
namespace evhtp {
	namespace evhtphelp {
		void set_response_attr(evhtp_request_t * req, const char* key, const char* val, char kalloc)
		{
			// last two pars are very important!! 
			evhtp_headers_add_header(req->headers_out, evhtp_header_new(key, val, kalloc, 1));
		}
		void set_response_printf(evhtp_request_t * req, string str, evhtp_res code)
		{   // we pass str as a value -> it should be copied !!!
			evbuffer_add_printf(req->buffer_out, str.c_str());
			evhtp_send_reply(req, /*EVHTP_RES_OK*/code);

			//set_response_buf(req, str.data(), str.size());
		}
		void set_response_json(evhtp_request_t * req, string &str, evhtp_res code)
		{
			set_response_buf(req, str.data(), str.size(), code);
		}

		static void FreeBuf_CB(void const *data, std::size_t datalen, void *extra)
		{
			delete[] data;
		}
		void set_response_buf(evhtp_request_t * req, void const *data, std::size_t bytes, evhtp_res code)
		{
			unsigned char *buf = new unsigned char[bytes];
			if (buf)
			{
				memcpy(buf, data, bytes);
				if (evbuffer_add_reference(req->buffer_out, buf, bytes, &FreeBuf_CB, nullptr) == -1)
				{
					delete buf;
					//throw HttpRequestException("Failed to make response.");
				}
			}
			evhtp_send_reply(req, /*EVHTP_RES_OK*/code);
		}
		
//		string getWebRoot(evhtp_request_t * req, evhtp_connection_t * conn)
//		{
//			char                 tmp[32];  // to keep ip address
//			struct sockaddr_in * sin;
//
//			sin = (struct sockaddr_in *)conn->saddr;
//
//			evutil_inet_ntop(AF_INET, &sin->sin_addr, tmp, sizeof(tmp));    //1897181194=0x7114B00A -> http://10.176.20.113:5555/
//			string path = "";
//			//16777343 = 0x100007F   ->127.0.0.1
//			unsigned int ip_addr = 0;
//#ifdef WIN32
//			ip_addr = sin->sin_addr.S_un.S_addr;
//#else
//			ip_addr = sin->sin_addr.s_addr;
//#endif
//
//			{
//				path = string(Mkr_RootDir);
//			}
//			//	m_printf("process_request path:peer:%s (%s)\r\n", tmp, req->uri->path->full);
//			return path;
//		}

#if 0
		//example of how to make a a http client request  
		//https://chromium.googlesource.com/external/github.com/ellzey/libevhtp/+/1.2.8/examples/test_proxy.c
		int make_request(evbase_t         * evbase,
			evthr_t          * evthr,
			const char * const host,
			const short        port,
			const char * const path,
			evhtp_headers_t  * headers,
			evhtp_callback_cb  cb,
			void             * arg) {
			evhtp_connection_t * conn;
			evhtp_request_t    * request;
			conn = evhtp_connection_new(evbase, host, port);
			conn->thread = evthr;
			request = evhtp_request_new(cb, arg);
			evhtp_headers_add_header(request->headers_out,
				evhtp_header_new("Host", "localhost", 0, 0));
			evhtp_headers_add_header(request->headers_out,
				evhtp_header_new("User-Agent", "libevhtp", 0, 0));
			evhtp_headers_add_header(request->headers_out,
				evhtp_header_new("Connection", "close", 0, 0));
			evhtp_headers_add_headers(request->headers_out, headers);
			printf("Making backend request...\n");
			evhtp_make_request(conn, request, htp_method_GET, path);
			printf("Ok.\n");
			return 0;
		}

		//call this method from void app_process_request(evhtp_request_t * req, void * arg)
		make_request(evthr_get_base(req->conn->thread),
			req->conn->thread,
			"127.0.0.1", 80,
			req->uri->path->full,
			req->headers_in, backend_cb, req);
#endif

		
		int header_cb(evhtp_kv_t *kv, void *arg)
		{
			if (!kv)
				return 0;
			m_printf("Key:%s Val:%s\r\n", kv->key, kv->val);
			return 1;
		}
		bool isStrContainsSpace(string &str)
		{
			for (auto& ch : str)
			{
				if (ch == '%' || isspace(ch))
					return true;
			}
			return false;
		}

	}	//namespace evhtphelp

	
	void EvhServer::app_process_request(evhtp_request_t * req, void * arg) {

		struct app         * app;
		evthr_t            * thread;
		evhtp_connection_t * conn;


		conn = evhtp_request_get_connection(req);
		thread = conn->thread;
		app = (struct app *)evthr_get_aux(thread);

		if (app && app->parent && app->parent->server)
		{
			application_cb_t cb = ((EvhServer*)app->parent->server)->getAppCb();
			if (cb)
			{
				cb(req, app, thread, conn);
			}
		}

	}

	void EvhServer::app_init_thread(evhtp_t * htp, evthr_t * thread, void * arg) {
		struct app_parent * app_parent;
		struct app        * app;

		app_parent = (struct app_parent *)arg;
		app = (struct app        *) calloc(sizeof(struct app), 1);

		app->parent = app_parent;
		app->evbase = evthr_get_base(thread);

		evthr_set_aux(thread, app);
	}

	int EvhServer::start_libevhtp(EvhServer* server, void *pSSL)
	{

		//--------------------------------------------------------
			/*SSL_load_error_strings();
			SSL_library_init();*/

			//-------------------------------------------------------

#if MKR_USE_INTERNAL_MAP_TILES_SERVICE
		tile_server_init();
#endif

		if (pSSL)
		{
			evhtp_ssl_cfg_t  *sslcfg = (evhtp_ssl_cfg_t *)pSSL;
			memset(sslcfg, 0, sizeof(evhtp_ssl_cfg_t));
		}

		evbase = event_base_new();
		evhtp = evhtp_new(evbase, NULL);
		app_p = (struct app_parent *)calloc(sizeof(struct app_parent), 1);

		app_p->evhtp = evhtp;
		app_p->evbase = evbase;
		app_p->server = server;
		//init ssl
		if (pSSL)
		{
			evhtp_ssl_cfg_t  *sslcfg = (evhtp_ssl_cfg_t *)pSSL;
			sslcfg->pemfile = mkr_mbedtls_x509_crt_cert_file;
			sslcfg->privfile = mkr_mbedtls_pk_cert_keyfile;
			sslcfg->scache_type = evhtp_ssl_scache_type_internal;
			sslcfg->scache_timeout = 5000;
			m_printf("Initialize SSL.");

			evhtp_ssl_init(evhtp, sslcfg);
		}
		evhtp_set_gencb(evhtp, app_process_request, NULL);
		evhtp_use_threads(evhtp, app_init_thread, 4, app_p);

		uint16_t port = 0;
		if (pSSL)
		{
			port = HTTPS_LIGHT_SERVER_PORT;
		}
		else
		{
			port = HTTP_LIGHT_SERVER_PORT;
		}
		int rv = evhtp_bind_socket(evhtp, HTTP_LIGHT_SERVER_IPADDR, port, 1024);
		if (rv < 0)
		{
			m_printf("**BIND Error %s %d errno=%d.", HTTP_LIGHT_SERVER_IPADDR, port, errno);
			/*if (errno == EACCES)
			{
			}*/
			//errno = 98  -> address already in use
			//https://www.liquidweb.com/kb/error-98address-already-in-use-make_sock-could-not-bind-to-address-0-0-0-080-on-ubuntu-solved/
			// Evaluate which process(es) may be using port 443 with the following: ss -plnt sport eq :443
		}
		else {
			m_printf("GOTO Loop on server side %d.", port);
			event_base_loop(evbase, 0);
			m_printf("EXIT Loop on server side %d.", port);
		}

		return 0;
	}
	int EvhServer::stop_libevhtp()
	{
		if (evhtp)
		{
			evhtp_unbind_socket(evhtp);
			evhtp_free(evhtp);
			evhtp = NULL;
		}

		if (evbase)
		{
			event_base_loopbreak(evbase);
			event_base_free(evbase);
			evbase = NULL;
		}

		if (app_p)
		{
			free(app_p);
			app_p = NULL;
		}
		return 1;
	}

	static std::shared_ptr<EvhServer> ev_server = nullptr;

	int start_libevhtp_main()
	{
		void *pSSL = NULL;
#if MKR_USE_SSL	
		evhtp_ssl_cfg_t  sslcfg;
		pSSL = &sslcfg;
#endif
		ev_server = std::make_shared<EvhServer>();
		ev_server->setAppCb(process_main_application_request);
		ev_server->start_libevhtp(ev_server.get(), pSSL);
		return 0;
	}

	//https://stackoverflow.com/questions/29219314/how-to-break-out-libevents-dispatch-loop
	int stop_libevhtp_main()
	{
		if (ev_server != nullptr)
		{
			ev_server->stop_libevhtp();

			ev_server.reset();
		}
		m_printf("stop_libevhtp_main..Done.");
		return 0;
	}
}//namespace evhtp 
#endif  //#if USE_LIBEVENT