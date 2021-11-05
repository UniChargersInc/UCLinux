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



//#if !USE_LIBEVENT
#include "httplib.h"
#include <cstdio>

#include <thread>
#include <pthread.h>

#include "webservice/WebService.h"

#ifdef WIN32
//#include <ws2tcpip.h>  //inet_ntop
#else
#include <arpa/inet.h> //-> linux inet_ntop
#endif
#include <google/protobuf/stubs/strutil.h>  //GlobalReplaceSubstring

#include "db/CentralOcppDB.h"


using namespace httplib;
using namespace std;

std::string log(const Request& req, const Response& res);


void set_response_buf(Response &res, const char* buf, int len, int sts=200)
{
	res.body.assign(buf, len);
	res.status = sts;
}

void process_web_request(const Request& req, Response& res, const char* sern)
{
	int buf_len = 0;
	unsigned char *pBuf = 0;

	const char* pStr = req.path.c_str();
	if (strstr(pStr, "uc_smart.ino.bin") != 0)
	{
		std::string fname = "";
		{
			char fpath[128];
			sprintf(fpath, "%s/firmware/%s", UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT, "UC_SmartGrizzle.ino.esp32.bin");
			fname = std::string(fpath);
		}

		int size = getSizeOfFile(fname.c_str());
		if (size > 0)
		{
			buf_len = size;
			pBuf = new unsigned char[size];
			int fsize = read_bin_file_to_buf(fname.c_str(), pBuf);
			if (fsize == size) {

			}
			else {
				buf_len = 0;  // fatal case
			}
				
		}
			

	}

	if (buf_len > 0 && pBuf)
	{

		m_printf("Sending binary response len=%d\r\n", buf_len);
		
		//string contentDispV = "attachment; filename=\"" + req.path /*par->uid*/ + ".bin\"";
		res.set_header(content_type, "application/octet-stream");
		//res.set_header(content_disp, contentDispV.c_str());
		string strL = Int2Str(buf_len);
		//res.set_header(content_len, strL.c_str());
		set_response_buf(res, (const char*)pBuf, buf_len);
		//m_printf("Sending binary response len=%s\r\n", strL.c_str());
		//m_printf("%s", log(req, res).c_str());
		//CentralOcppDB::Get()->UpdateCSFWUpdateFromOcppMessage2DB(sern, "Stage2-Start");
	}
	else {
		m_printf("Could NOT send bin =%s\r\n", pStr);
		set_response_buf(res, "*", 1, 500); // could not generate a report
	}

	if (pBuf)
		delete[] pBuf;
}



//=============================================================================
inline bool handle_file_request(const Request& req, Response& res)
{
	
		std::string path = Mkr_RootDir + req.path;

		if (!path.empty() && path.back() == '/') {
			path += Mkr_No_SSL_Ref;
		}
		//printf(">NoSSL File  %s\r\n", path.c_str());
		if (detail::is_file(path)) {
			detail::read_file(path, res.body);
			
				res.set_header("Content-Type", "text/html");
				//std::string age_ctrl = "public, max-age=";
				//int age_time = 100 * 24 * 3600;    // 100 days
				//if (ext=="html" || ext == "css" || ext == "js")
				//{
				//	age_time = 0;// *3600;  // 1 hour
				//}
				//age_ctrl += std::to_string(age_time);
				//res.set_header("Cache-Control", age_ctrl.c_str());
			
			res.status = 200;
			return true;
		}
	

	return false;
}

inline bool handle_certbot_request(const Request& req, Response& res)
{

	std::string path = Mkr_RootDir + req.path;

	
	//printf(">CertBot File  %s\r\n", path.c_str());
	if (detail::is_file(path)) {
		detail::read_file(path, res.body);

		res.set_header("Content-Type", "text/html");
		//std::string age_ctrl = "public, max-age=";
		//int age_time = 100 * 24 * 3600;    // 100 days
		//if (ext=="html" || ext == "css" || ext == "js")
		//{
		//	age_time = 0;// *3600;  // 1 hour
		//}
		//age_ctrl += std::to_string(age_time);
		//res.set_header("Cache-Control", age_ctrl.c_str());

		res.status = 200;
		return true;
	}


	return false;
}


//void set_response_post(Response &res,std::string &str,int sts)
//{
//	res.body.assign(str.data(), str.size());
//	res.status = sts;
//}

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
			
		
	}
	return rv;
}

//bool process_get_request(const Request& req, Response& res)
//{
//	
//
//	string ext = detail::file_extension(req.path); //Http::Content::GetExtFromFileName(req.path);
//	m_printf("http reqpath:%s", req.path.c_str());
//	if (ext == "bin" )
//	{
//		process_web_request(req, res, 0);
//		return true;
//	}
//
//
//
//
//	/*bool rv1=process_mkr_request(req, res);
//	if (rv1)
//		return true;*/
//
//	if (handle_file_request(req, res)) {
//		return true;
//	}
//
//	return false;
//}
//bool process_post_request(const Request& req, Response& res)
//{
//	//string ext = detail::file_extension(req.path); //Http::Content::GetExtFromFileName(req.path);
//	//if (ext == "bin" )
//	//{
//	//	process_web_request(req,res,1);
//	//	return true;
//	//}
//
//	return false;
//}
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

static std::shared_ptr<httplib::Server> svr=nullptr;

bool execute_thread_function(int sock)
{
	//svr.read_and_close_socket(sock);
	return true;
}

static std::thread::native_handle_type mkr_no_ssl_server_handle;

static void __start_mkr_no_ssl_server(void *data)
{
	int *terminate = (int*)data;

	svr->set_pre_routing_handler([](const auto& req, auto& res){
		
		if (strstr(req.path.c_str(), ".well-known") != 0)
		{
			handle_certbot_request( req, res);
			return Server::HandlerResponse::Handled;
		}
		
		return Server::HandlerResponse::Unhandled;
		});

	svr->Get("/", [=](const auto& req, auto& res) {
		handle_file_request(req, res);
		});
	
	svr->Get("/uc_smart.ino.bin", [=](const auto& req, auto& res) {
		std::string sern = "";
		if (req.has_param("sern")) {
			sern = req.get_param_value("sern");
		}
		if (!sern.empty())
		{
			process_web_request(req, res, sern.c_str());
		}
		else {
			m_printf("FW binary req No SerN=%s\r\n");
		}
	});

	//svr.post("/", [=](const auto& req, auto& res) {
	//	//M_HTTP_PRINTF(">POST  %s\r\n", req.path.c_str());
	//	process_post_request(req, res);
	//	});
	

	svr->set_error_handler([](const auto& req, auto& res) {
		const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), fmt, res.status);
		res.set_content(buf, "text/html");
	});

	/*svr.set_logger([](const auto& req, const auto& res) {
		M_HTTP_PRINTF("%s", log(req, res).c_str());
	});*/
	//svr.set_base_dir(Mkr_RootDir);


	svr->listen(HTTP_LIGHT_SERVER_IPADDR, HTTP_LIGHT_SERVER_PORT/*, terminate*/);
	
}


int start_mkr_no_ssl_server(void *terminate) {
	svr = std::make_shared<httplib::Server>();
	std::thread thrd = std::thread(__start_mkr_no_ssl_server,terminate);
	mkr_no_ssl_server_handle = thrd.native_handle();
	//if (ocpp_listener_thread.joinable())
	thrd.detach();  //the thread to run in the background (daemon)
	return 0;
}

int stop_mkr_no_ssl_server(void)
{
	//printf("stop_mkr_no_ssl_server..\r\n");
	if (svr != nullptr)
	{
		svr->stop();
		usleep(100 * 1000);
		//printf("stop_mkr_no_ssl_server_1\r\n");
		pthread_cancel(mkr_no_ssl_server_handle);
		m_printf("stop_mkr_no_ssl_server_2\r\n");
		
		svr.reset();
		
	}
	return true;
}

// vim: et ts=4 sw=4 cin cino={1s ff=unix


//#endif  //#if !USE_LIBEVENT