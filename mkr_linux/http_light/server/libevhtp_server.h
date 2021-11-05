#ifndef LIBEVHTP_SERVER_H
#define LIBEVHTP_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "udorx_server_config.h"
#if USE_LIBEVENT
#include <evhtp.h>

#include <MkrLib.h>
#include <memory>

using namespace std;

#include "http_content_type.h"
using namespace Network;
void process_p_web_request(evhtp_request_t * req); // using protobytes
void process_web_request(evhtp_request_t * req);
struct app_parent {
	evhtp_t  * evhtp;
	evbase_t * evbase;
	//	char     * redis_host;
	//	uint16_t   redis_port;

	void* server;
};

struct app {
	struct app_parent * parent;
	evbase_t          * evbase;
	//	redisAsyncContext * redis;
};
typedef void(*application_cb_t)(evhtp_request_t * req, struct app* app, evthr_t* thread, evhtp_connection_t * conn);
namespace evhtp {
	class EvhServer;
	
	
	namespace evhtphelp {
		void set_response_attr(evhtp_request_t * req, const char* key, const char* val, char kalloc = 1);
		void set_response_json(evhtp_request_t * req, string  &str, evhtp_res code = EVHTP_RES_OK);
		void set_response_printf(evhtp_request_t * req, string  str, evhtp_res code = EVHTP_RES_OK);
		void set_response_buf(evhtp_request_t * req, void const *data, std::size_t bytes, evhtp_res code = EVHTP_RES_OK);
	}
	class EvhServer
	{
	public:
		
		int start_libevhtp(EvhServer *server, void *pSSL);
		int stop_libevhtp();
		application_cb_t getAppCb() {return app_cb;}
		void setAppCb(application_cb_t _cb) { app_cb = _cb; }
	private:
		static void app_process_request(evhtp_request_t * req, void * arg);
		static void app_init_thread(evhtp_t * htp, evthr_t * thread, void * arg);

		evbase_t          * evbase = NULL;
		evhtp_t           * evhtp = NULL;
		struct app_parent * app_p = NULL;

		application_cb_t app_cb = NULL;
	};
	int start_libevhtp_main();
	int stop_libevhtp_main();
}//namespace evhtp
#endif //#if USE_LIBEVENT

#endif