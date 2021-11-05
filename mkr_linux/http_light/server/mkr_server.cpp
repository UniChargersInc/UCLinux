#include "udorx_server_config.h"


const char* server_key = "Server";
const char* server_val = "Mikerel";
const char* welcome_msg = "<html>\n<body>\n<h1>\n***</h1>\n</body>\n</html>\n";//"<html>\n<body>\n<h1>\nWelcome To Udorx!</h1>\n</body>\n</html>\n";

const char* content_type = "Content-Type";
const char* content_disp = "Content-Disposition";
const char* content_len = "Content-Length";
const char * content_enc = "Content-Encoding";
const char *access_control = "Access-Control-Allow-Origin";
const char *cache_control = "Cache-Control";



#if !USE_LIBEVENT
#include "httplib.h"
#include <cstdio>


#include "webservice/WebService.h"

#ifdef WIN32
//#include <ws2tcpip.h>  //inet_ntop
#else
#include <arpa/inet.h> //-> linux inet_ntop
#endif
#include <google/protobuf/stubs/strutil.h>  //GlobalReplaceSubstring






#define SERVER_CERT_FILE mkr_mbedtls_x509_crt_cert_file
#define SERVER_PRIVATE_KEY_FILE mkr_mbedtls_pk_cert_keyfile

using namespace httplib;
using namespace std;


#if MKR_USE_INTERNAL_MAP_TILES_SERVICE
int tile_server_init(void);
int tile_server_process_request(const Request&, Response&, std::string &req_ext);
#endif

WebSession* GetWebSessionByID(WebSession *pws);
void process_p_web_request(const Request& req, Response& res);
void process_web_request(const Request& req, Response& resint,int req_method);



//=============================================================================
inline bool handle_file_request(const Request& req, Response& res)
{
	
		std::string path = Mkr_RootDir + req.path;

		if (!path.empty() && path.back() == '/') {
			path += "index.html";
		}
		//M_HTTP_PRINTF(">File  %s\r\n", path.c_str());
		if (detail::is_file(path)) {
			detail::read_file(path, res.body);
			std::string ext = detail::file_extension(path);
			auto type = detail::content_type_by_ext(ext);
			if (type) {
				res.set_header("Content-Type", type);
				std::string age_ctrl = "public, max-age=";
				int age_time = 100 * 24 * 3600;    // 100 days
				if (ext=="html" || ext == "css" || ext == "js")
				{
					age_time = 0;// *3600;  // 1 hour
				}
				age_ctrl += std::to_string(age_time);
				res.set_header("Cache-Control", age_ctrl.c_str());
			}
			res.status = 200;
			return true;
		}
	

	return false;
}


void set_response_buf(Response &res, const char* buf,int len,int sts)
{
	res.body.assign(buf, len);
	res.status = sts;
}

void set_response_post(Response &res,std::string &str,int sts)
{
	res.body.assign(str.data(), str.size());
	res.status = sts;
}

static void _response_with_html(const Request& req, Response& res,string &outStr)
{
	//evhtp_headers_add_header(req->headers_out, evhtp_header_new(server_key, server_val, 0, 0));
	
	res.set_content(outStr, "text/html");
	res.status = 200;
}
static void _response_logout(const Request& req, Response& res)
{
	// we did not find a session for this credentials -> logout
	string str = "";
	read_html_file((char*)(Mkr_RootDir + string("/logout.html")).c_str(), str);
	google::protobuf::GlobalReplaceSubstring("#href#", LOGOUT_URL, &str);
	_response_with_html(req, res, str);
}

bool process_mkr_request(const Request& req, Response& res)
{
	bool rv = false;
	string Root = req.path;
	if (strstr(req.path.c_str(), "mkr.html") != 0)
	{
			
			if (req.params.size()>0)
			{
				string sessionid = req.get_param("si");
				
				if (!sessionid.empty())
				{
					int64_t si = Str2Int64(sessionid);
					WebSession* pWs = GetWebSessionByID((WebSession *)si);
					if (pWs != 0/*&&pWs->user==string(pU)*/)
					{
						string ll = "mkr.html";
						if (pWs->lang == "1")
							ll = "es_" + ll;
						string str = "";
						read_html_file((char*)(Mkr_RootDir +string("/")+ ll).c_str(), str);
						google::protobuf::GlobalReplaceSubstring("#m_si#", sessionid, &str);
						_response_with_html(req,res, str);
						rv = true;
					}
					else
					{
						_response_logout(req, res);
						rv = true;
					}
				}
				else
				{
					_response_logout(req, res);
					rv = true;
				}
			}//params
			else
			{
				_response_logout(req, res);
				rv = true;
			}
		
	}
	return rv;
}

bool process_get_request(const Request& req, Response& res)
{
	

	string ext = detail::file_extension(req.path); //Http::Content::GetExtFromFileName(req.path);
	if (ext == "ajx" || ext == "pdf")
	{
		process_web_request(req, res, 0);
		return true;
	}

#if MKR_USE_INTERNAL_MAP_TILES_SERVICE
	int rv = tile_server_process_request(req, res, ext);
	if (rv == 1)
		return true;
#endif


	bool rv1=process_mkr_request(req, res);
	if (rv1)
		return true;

	if (handle_file_request(req, res)) {
		return true;
	}

	return false;
}
bool process_post_request(const Request& req, Response& res)
{
	string ext = detail::file_extension(req.path); //Http::Content::GetExtFromFileName(req.path);
	if (ext == "ajx" || ext == "pdf")
	{
		process_web_request(req,res,1);
		return true;
	}

	else if (ext == "req")
	{
		process_p_web_request(req,res);  // post web request !!!
		return true;
	}
	return false;
}
//=============================================================================

//std::string dump_headers(const MultiMap& headers)
//{
//	std::string s;
//	char buf[BUFSIZ];
//
//	for (auto it = headers.begin(); it != headers.end(); ++it) {
//		const auto& x = *it;
//		snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
//		s += buf;
//	}
//
//	return s;
//}

//std::string log(const Request& req, const Response& res)
//{
//	std::string s;
//	char buf[BUFSIZ];
//
//	s += "================================\n";
//
//	snprintf(buf, sizeof(buf), "%s %s", req.method.c_str(), req.path.c_str());
//	s += buf;
//
//	std::string query;
//	for (auto it = req.params.begin(); it != req.params.end(); ++it) {
//		const auto& x = *it;
//		snprintf(buf, sizeof(buf), "%c%s=%s",
//			(it == req.params.begin()) ? '?' : '&', x.first.c_str(), x.second.c_str());
//		query += buf;
//	}
//	snprintf(buf, sizeof(buf), "%s\n", query.c_str());
//	s += buf;
//
//	s += dump_headers(req.headers);
//
//	s += "--------------------------------\n";
//
//	snprintf(buf, sizeof(buf), "%d\n", res.status);
//	s += buf;
//	s += dump_headers(res.headers);
//
//	if (!res.body.empty()) {
//		s += res.body;
//	}
//
//	s += "\n";
//
//	return s;
//}

static httplib::HttpServer *pServer = NULL;

bool execute_thread_function(int sock)
{
	if(pServer)
		pServer->read_and_close_socket(sock);
	return true;
}

int start_mkr_server(int *terminate)
{
#if MKR_USE_INTERNAL_MAP_TILES_SERVICE
	tile_server_init();
#endif
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	SSLHttpServer svr(SERVER_CERT_FILE, SERVER_PRIVATE_KEY_FILE);
#else
	HttpServer svr;
#endif
	pServer = &svr;
	svr.get("/", [=](const auto& req, auto& res) {
		//M_HTTP_PRINTF(">GET  %s\r\n", req.path.c_str());
		process_get_request(req, res);
	});

	svr.post("/", [=](const auto& req, auto& res) {
		//M_HTTP_PRINTF(">POST  %s\r\n", req.path.c_str());
		process_post_request(req, res);
		});
	

	svr.set_error_handler([](const auto& req, auto& res) {
		const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), fmt, res.status);
		res.set_content(buf, "text/html");
	});

	/*svr.set_logger([](const auto& req, const auto& res) {
		M_HTTP_PRINTF("%s", log(req, res).c_str());
	});*/
	//svr.set_base_dir(Mkr_RootDir);
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	svr.listen(HTTP_LIGHT_SERVER_IPADDR, HTTPS_LIGHT_SERVER_PORT, terminate);
#else
	svr.listen(HTTP_LIGHT_SERVER_IPADDR, HTTP_LIGHT_SERVER_PORT, terminate);
#endif
	return 0;
}

int stop_mkr_server(void)
{
	if (pServer)
	{
		pServer->stop();
	}
	return true;
}

// vim: et ts=4 sw=4 cin cino={1s ff=unix


#endif  //#if !USE_LIBEVENT