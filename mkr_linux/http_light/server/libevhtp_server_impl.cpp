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


	//=======================application specific implementation=================
	inline static void _response_with_html(evhtp_request_t * req, string &str)
	{
		evhtp_headers_add_header(req->headers_out, evhtp_header_new(server_key, server_val, 0, 0));
		string ext = "html";
		string contentType = Http::Content::TypeFromFileName(/*Path*/ext);
		evhtp::evhtphelp::set_response_attr(req, content_type, contentType.c_str(), 0); // 0 tells do not allocate content_type string in a heap
		evhtp::evhtphelp::set_response_json(req, str);
	}
	inline static void _response_logout(evhtp_request_t * req, string &root)
	{
		// we did not find a session for this credentials -> logout
		string str = "";
		read_html_file((char*)(root + "/lout.ht").c_str(), str);
		google::protobuf::GlobalReplaceSubstring("#href#", LOGOUT_URL, &str);
		_response_with_html(req, str);
	}
	static bool _process_mkr_request(evhtp_request_t * req, string &Root, const char *fname)
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
	static bool process_mkr_request(evhtp_request_t * req, string &Root)
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
	void process_main_application_request(evhtp_request_t * req, struct app* app, evthr_t* thread, evhtp_connection_t * conn)
	{
		string Root = string(Mkr_RootDir);  //evhtphelp::getWebRoot(req, conn);
		
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
					else if ((pSi = evhtp_kv_find(params, "peer_id")) != 0)
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
		if (ext == "ajx" || (ext == "pdf" && strstr(Path.c_str(), "uc_docs") == 0))
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
		int rv = tile_server_process_request(req, Path, ext);
		if (rv == 1)
			return;
#endif

		if (Path == "/")
		{
			Path = Path + string(DefaultPage);
		}
		Path = Root + Path;
		//MKR_DBG_PRINTF("conn Path:%s\r\n", Path.c_str());
		bool rv1 = process_mkr_request(req, Root);
		if (rv1)
			return;

		int fd = -1;
		struct stat st;
#if WIN32
		if ((fd = _open(Path.c_str(), O_RDONLY | O_BINARY)) == -1)
#else
		if ((fd = open(Path.c_str(), O_RDONLY)) == -1)
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
		string contentType = Http::Content::TypeFromFileName(/*Path*/ext);
		//m_printf("process_request Path:%s content-type:(%s)",Path.c_str(), contentType.c_str());
		evhtp::evhtphelp::set_response_attr(req, content_type, contentType.c_str(), 0); // 0 tells do not allocate content_type string in a heap
		evhtp_send_reply(req, EVHTP_RES_OK);

		//m_printf("evhtp_send_reply(req, EVHTP_RES_OK);");
		goto __exit;


	__exit:
		{}
	}

#endif  //#if USE_LIBEVENT