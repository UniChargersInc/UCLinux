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

extern const char* server_key ;
extern const char* server_val;
extern const char* welcome_msg;

extern const char* content_type;
extern const char* content_disp;
extern const char* content_len;

WebSession* GetWebSessionByID(WebSession *pws);

static evthr_t *get_request_thr(evhtp_request_t * request) 
{
	evhtp_connection_t * htpconn;
	evthr_t            * thread;

	htpconn = evhtp_request_get_connection(request);
	thread = htpconn->thread;

	return thread;
}

void set_response_attr(evhtp_request_t * req, const char* key, const char* val, char kalloc)
{
	// last two pars are very important!! 
	evhtp_headers_add_header(req->headers_out, evhtp_header_new(key, val, kalloc, 1));
}
void set_response_printf(evhtp_request_t * req, string str)
{   // we pass str as a value -> it should be copied !!!
	evbuffer_add_printf(req->buffer_out, str.c_str());
	evhtp_send_reply(req, EVHTP_RES_OK);

	//set_response_buf(req, str.data(), str.size());
}
void set_response_json(evhtp_request_t * req, string &str)
{  
	set_response_buf(req, str.data(), str.size());
}

static void FreeBuf_CB(void const *data, std::size_t datalen, void *extra)
{
	delete[] data;
}
void set_response_buf(evhtp_request_t * req, void const *data, std::size_t bytes)
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
	evhtp_send_reply(req, EVHTP_RES_OK);
}
inline static void _response_with_html(evhtp_request_t * req, string &str)
{
	evhtp_headers_add_header(req->headers_out, evhtp_header_new(server_key, server_val, 0, 0));
	string ext = "html";
	string contentType = Http::Content::TypeFromFileName(/*Path*/ext);
	set_response_attr(req, content_type, contentType.c_str(), 0); // 0 tells do not allocate content_type string in a heap
	set_response_json(req, str);
}
inline static void _response_logout(evhtp_request_t * req,string &root)
{
	// we did not find a session for this credentials -> logout
	string str = "";
	read_html_file((char*)(root +"/lout.ht").c_str(), str);
	google::protobuf::GlobalReplaceSubstring("#href#", LOGOUT_URL, &str);
	_response_with_html(req, str);
}
string getWebRoot(evhtp_request_t * req, evhtp_connection_t * conn)
{
	char                 tmp[32];  // to keep ip address
	struct sockaddr_in * sin;

	sin = (struct sockaddr_in *)conn->saddr;

	evutil_inet_ntop(AF_INET, &sin->sin_addr, tmp, sizeof(tmp));    //1897181194=0x7114B00A -> http://10.176.20.113:5555/
	string path = "";
																//16777343 = 0x100007F   ->127.0.0.1
	unsigned int ip_addr = 0;
#ifdef WIN32
	ip_addr = sin->sin_addr.S_un.S_addr;
#else
	ip_addr = sin->sin_addr.s_addr;
#endif
	
	{
		path = string(Mkr_RootDir) ;
	}
//	m_printf("process_request path:peer:%s (%s)\r\n", tmp, req->uri->path->full);
	return path;
}

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

static bool _process_mkr_request(evhtp_request_t * req, string &Root,const char *fname)
{
	bool rv = false;
		int req_method = evhtp_request_get_method(req);
		if (req_method == htp_method_GET)
		{
			evhtp_kvs_t * params = req->uri->query;
			if (params)
			{
				const char* pSi = evhtp_kv_find(params, "si");  // sessionid
																//const char* pU = evhtp_kv_find(params, "u");  // sessionid
				if (pSi/*&&pU*/)
				{
					string sessionid = string(pSi);
					int64_t si = Str2Int64(sessionid);
					WebSession* pWs = GetWebSessionByID((WebSession *)si);
					if (pWs != 0/*&&pWs->user==string(pU)*/)
					{
						std::string ll = fname;
						if (pWs->lang == "1")
							ll = "es_" + ll;
						string str = "";
						read_html_file((char*)(Root + "/" + ll).c_str(), str);
						google::protobuf::GlobalReplaceSubstring("#m_si#", sessionid, &str);
						_response_with_html(req, str);
						rv = true;
					}
					else
					{
						_response_logout(req, Root);
						rv = true;
					}
				}
				else
				{
					_response_logout(req, Root);
					rv = true;
				}
			}//params
			else
			{
				_response_logout(req, Root);
				rv = true;
			}
		}
		else
			rv = false;
		return rv;
}

bool process_mkr_request(evhtp_request_t * req,string &Root)
{
	const char *pStr = "mkr.html";
	if (strstr(req->uri->path->full, pStr) != 0)
	{
		return _process_mkr_request(req, Root, pStr);
	}
	pStr = "adm.html";
	if (strstr(req->uri->path->full, pStr) != 0)
	{
		return _process_mkr_request(req, Root, pStr);
	}
	return false;
}
int header_cb(evhtp_kv_t *kv, void *arg)
{
	if (!kv)
		return 0;
	m_printf("Key:%s Val:%s\r\n", kv->key,kv->val);
	return 1;
}
bool isStrContainsSpace(string &str)
{
	for (auto& ch : str)
	{
		if (ch=='%' || isspace(ch))
			return true;
	}
	return false;
}
void app_process_request(evhtp_request_t * req, void * arg) {
	
	//struct app_parent  * app_parent;
	struct app         * app;
	evthr_t            * thread;
	evhtp_connection_t * conn;
		

	conn = evhtp_request_get_connection(req);
	thread = conn->thread;
	app = (struct app *)evthr_get_aux(thread);
	
	string Root = getWebRoot(req, conn);
	//MKR_DBG_PRINTF("conn Root:%s\r\n", Root.c_str());
	if (Root.empty())
		return;

	
	string Path = string(req->uri->path->full);
	//if (isStrContainsSpace(Path))
	//{
	//	//m_printf("Got request with Space Path:%s \r\n", Path.c_str());
	//	return;
	//}
	
	//=====================test video=========================
#if 0
	m_printf("Got request Path:%s \r\n", Path.c_str());

	if (Path.find("video") != std::string::npos)
	{
		int req_method = evhtp_request_get_method(req);
		if (req_method == htp_method_GET)
		{
			m_printf("GET method \r\n");
			evhtp_kvs_t * params = req->uri->query;
			if (params)
			{
				const char* pSi = evhtp_kv_find(params, "name");
				if (pSi != 0)
				{
					m_printf("GET query name:%s\r\n", pSi);
					set_response_attr(req, content_type, "text/plain", 0); // 0 tells do not allocate content_type string in a heap
					std::string str = "my_name_0,1,1\n";
					set_response_json(req, str);
				}
				else if((pSi = evhtp_kv_find(params, "peer_id"))!=0) 
				{
					m_printf("GET query peer_id:%s\r\n", pSi);

					if (Path.find("wait") != std::string::npos)
					{
						//create a waiting socket
						set_response_attr(req, content_type, "text/plain", 0); // 0 tells do not allocate content_type string in a heap
						std::string str = "my_name_0,1,1\n";
						set_response_json(req, str);
					}
					else if (Path.find("sign_out") != std::string::npos)
					{
						// close connection
					}


				}
			}
		}
		else if (req_method == htp_method_POST)
		{
			m_printf("POST method \r\n");
		}
	}
#endif
	
//================================================================



	const char *content_type = "Content-Type";// evhtp_kv_find(req->headers_in, "Content-Type");
	/*if (content_type == NULL) {
		MKR_DBG_PRINTF("conn ContT:%s\r\n", content_type);
	}
	*/
	//MKR_DBG_PRINTF("conn Path:%s\r\n", Path.c_str());

	string ext = Http::Content::GetExtFromFileName(Path);
	if (ext == "ajx" || (ext=="pdf" && strstr(Path.c_str(),"uc_docs")==0))
	{
		/*evbuffer_add(req->buffer_out, "foobar", 6);
		evhtp_send_reply(req, EVHTP_RES_OK);*/
		process_web_request(req);
		return;
	}
	else if (ext == "req")
	{
		//MKR_DBG_PRINTF("process req..\r\n");
		process_p_web_request(req);
		return;
	}
	
#if MKR_USE_INTERNAL_MAP_TILES_SERVICE
	int rv=tile_server_process_request(req,Path, ext);
	if (rv == 1)
		return;
#endif

	if (Path == "/")
	{
		Path = Path+string(DefaultPage);
	}
	Path = Root + Path;
	bool rv1 = process_mkr_request(req, Root);
	if (rv1)
		return;

	int fd = -1;
	struct stat st;
#if WIN32
	if ((fd = _open(Path.c_str(), O_RDONLY | O_BINARY)) == -1)
#else
	if ((fd = open(Path.c_str(), O_RDONLY )) == -1)
#endif
	{
		//evbuffer_add_printf(req->buffer_out, welcome_msg);
		//m_printf("fatal_1 WM Path:%s ", Path.c_str());
		evhtp_send_reply(req, EVHTP_RES_NOTFOUND);
		return;
	}
	else
	{
		if (fstat(fd, &st) < 0)
		{
			/* Make sure the length still matches, now that we
			* opened the file :/ */
			//LOG_PRINT(LOG_DEBUG, "Root_page Length fstat Failed. Return Default Page.");
			//evbuffer_add_printf(req->buffer_out, welcome_msg);
			//m_printf("fatal_2 WM Path:%s ", Path.c_str());

			evhtp_send_reply(req, EVHTP_RES_NOTFOUND);
			return;
		}
		else
		{
			evbuffer_add_file(req->buffer_out, fd, 0, st.st_size);
		}
			
		
	}
	evhtp_headers_add_header(req->headers_out, evhtp_header_new(server_key, server_val, 0, 0));
	//https://developers.google.com/web/fundamentals/performance/optimizing-content-efficiency/http-caching
//	evhtp_headers_add_header(req->headers_out, evhtp_header_new("Cache-Control", "public, max-age=99936000", 0, 0)); //https://medium.com/pixelpoint/best-practices-for-cache-control-settings-for-your-website-ff262b38c5a2
	if (ext == "") ext = "html";
	string contentType= Http::Content::TypeFromFileName(/*Path*/ext);
	//m_printf("process_request Path:%s content-type:(%s)",Path.c_str(), contentType.c_str());
	set_response_attr(req, content_type, contentType.c_str(),0); // 0 tells do not allocate content_type string in a heap
	evhtp_send_reply(req, EVHTP_RES_OK);
	
	//m_printf("evhtp_send_reply(req, EVHTP_RES_OK);");
	goto __exit;


__exit:
	{}
}

void
app_init_thread(evhtp_t * htp, evthr_t * thread, void * arg) {
	struct app_parent * app_parent;
	struct app        * app;

	app_parent = (struct app_parent *)arg;
	app =(struct app        *) calloc(sizeof(struct app), 1);

	app->parent = app_parent;
	app->evbase = evthr_get_base(thread);

	evthr_set_aux(thread, app);
}

static evbase_t          * evbase=NULL;
static evhtp_t           * evhtp=NULL;
static struct app_parent * app_p=NULL;

int start_libevhtp_main() 
{
	
//--------------------------------------------------------
	/*SSL_load_error_strings();
	SSL_library_init();*/
	
//-------------------------------------------------------

#if MKR_USE_INTERNAL_MAP_TILES_SERVICE
	tile_server_init();
#endif

#if MKR_USE_SSL	
	evhtp_ssl_cfg_t  sslcfg;
	memset(&sslcfg, 0, sizeof(evhtp_ssl_cfg_t));
#endif
	evbase = event_base_new();
	evhtp = evhtp_new(evbase, NULL);
	app_p = (struct app_parent *)calloc(sizeof(struct app_parent), 1);

	app_p->evhtp = evhtp;
	app_p->evbase = evbase;

	//init ssl
#if MKR_USE_SSL	
	sslcfg.pemfile = mkr_mbedtls_x509_crt_cert_file;
	sslcfg.privfile = mkr_mbedtls_pk_cert_keyfile;
	sslcfg.scache_type = evhtp_ssl_scache_type_internal;
	sslcfg.scache_timeout = 5000;
	m_printf("Initialize SSL...\r\n");

	evhtp_ssl_init(evhtp, &sslcfg);
#endif
	evhtp_set_gencb(evhtp, app_process_request, NULL);
	evhtp_use_threads(evhtp, app_init_thread, 4, app_p);
		
	uint16_t port = 0;
#if MKR_USE_SSL
	port=HTTPS_LIGHT_SERVER_PORT;
#else
	port = HTTP_LIGHT_SERVER_PORT;
#endif
	int rv=evhtp_bind_socket(evhtp, HTTP_LIGHT_SERVER_IPADDR, port, 1024);
	if (rv < 0)
	{
		m_printf("\r\n**BIND Error %s %d errno=%d...\r\n", HTTP_LIGHT_SERVER_IPADDR,port,errno);
		/*if (errno == EACCES)
		{
		}*/
		//errno = 98  -> address already in use
		//https://www.liquidweb.com/kb/error-98address-already-in-use-make_sock-could-not-bind-to-address-0-0-0-080-on-ubuntu-solved/
		// Evaluate which process(es) may be using port 443 with the following: ss -plnt sport eq :443
	}
	else {
		m_printf("\r\n** GOTO Loop on server side ...");
		event_base_loop(evbase, 0);

	}

	return 0;
}
//https://stackoverflow.com/questions/29219314/how-to-break-out-libevents-dispatch-loop
int stop_libevhtp_main()
{
	if (evbase)
	{
		event_base_loopbreak(evbase);
		event_base_free(evbase);
		evbase = NULL;
	}
	if (evhtp)
	{
		evhtp_free(evhtp);
		evhtp = NULL;
	}
	if (app_p)
	{
		free(app_p);
		app_p = NULL;
	}
	return 1;
}

#endif  //#if USE_LIBEVENT