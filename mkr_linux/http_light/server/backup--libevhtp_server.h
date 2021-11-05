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




using namespace std;

struct app_parent {
	evhtp_t  * evhtp;
	evbase_t * evbase;
//	char     * redis_host;
//	uint16_t   redis_port;
};

struct app {
	struct app_parent * parent;
	evbase_t          * evbase;
//	redisAsyncContext * redis;
};



#include "http_content_type.h"
using namespace Network;

//class EvhServer
//{
//public:
	void process_web_request(evhtp_request_t * req);
	void set_response_attr(evhtp_request_t * req, const char* key, const char* val, char kalloc = 1);
	void set_response_json(evhtp_request_t * req, string  &str);
	void set_response_printf(evhtp_request_t * req, string  str);
	void set_response_buf(evhtp_request_t * req, void const *data, std::size_t bytes);

	void process_p_web_request(evhtp_request_t * req); // using protobytes
//};
int start_libevhtp_main();
int stop_libevhtp_main();

#endif //#if USE_LIBEVENT

#endif