#include "udorx_server_config.h"


const char* server_key = "Server";
const char* server_val = "Mikerel";
const char* welcome_msg = "<html>\n<body>\n<h1>\n***</h1>\n</body>\n</html>\n";

const char* content_type = "Content-Type";
const char* content_disp = "Content-Disposition";
const char* content_len = "Content-Length";
const char * content_enc = "Content-Encoding";
const char *access_control = "Access-Control-Allow-Origin";
const char *cache_control = "Cache-Control";



//#if !USE_LIBEVENT
//#include "httplib.h"
#include "libevhtp_server.h"
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


//using namespace httplib;
using namespace std;

static void process_fw_request(evhtp_request_t * req)
{
#if 0
	evhtp_kvs_t * params = req->uri->query;  // for GET request !!!
	std::string sern = "";
	if (params)
	{
		const char* pC = evhtp_kv_find(params, "sern");
		if (pC)
			sern = std::string(pC);
	}
#endif
	int buf_len = 0;
	unsigned char *pBuf = 0;
	
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
	
	if (buf_len > 0 && pBuf)
	{

		m_printf("Sending binary response len=%d\r\n", buf_len);
		
		//string contentDispV = "attachment; filename=\"" + req.path /*par->uid*/ + ".bin\"";
		
		evhtp::evhtphelp::set_response_attr(req, content_type, "application/octet-stream", 0); // 0 tells do not allocate content_type string in a heap
		//res.set_header(content_disp, contentDispV.c_str());
		string strL = Int2Str(buf_len);
		//res.set_header(content_len, strL.c_str());
		evhtp::evhtphelp::set_response_buf(req, (const char*)pBuf, buf_len);
		//m_printf("Sending binary response len=%s\r\n", strL.c_str());
		//m_printf("%s", log(req, res).c_str());
		//CentralOcppDB::Get()->UpdateCSFWUpdateFromOcppMessage2DB(sern, "Stage2-Start");
	}
	else {
		m_printf("Could NOT send FW bin \r\n");
		evhtp::evhtphelp::set_response_printf(req, "*", 500); // could not generate a report
	}

	if (pBuf)
		delete[] pBuf;
}



//=============================================================================
inline static bool is_file(const std::string &path) {
	struct stat st;
	return stat(path.c_str(), &st) >= 0 && S_ISREG(st.st_mode);
}
//inline static bool handle_certbot_request(evhtp_request_t * req, std::string& path)
//{
//	//printf(">CertBot File  %s\r\n", path.c_str());
//	if (is_file(path)) {
//		//detail::read_file(path, res.body);
//		//printf(">CertBot Reading File  %s\r\n", path.c_str());
//		int fd = -1;
//		struct stat st;
//		if ((fd = open(path.c_str(), O_RDONLY)) == -1)
//		{
//			//evbuffer_add_printf(req->buffer_out, welcome_msg);
//			//m_printf("fatal_1 WM Path:%s ", Path.c_str());
//			evhtp_send_reply(req, EVHTP_RES_NOTFOUND);
//			//printf(">CertBot Reading File_ERR1\r\n");
//			return false;
//		}
//		if (fstat(fd, &st) < 0)
//		{
//			evhtp_send_reply(req, EVHTP_RES_NOTFOUND);
//			return false;
//		}
//		else
//		{
//			evbuffer_add_file(req->buffer_out, fd, 0, st.st_size);
//		}
//		evhtp::evhtphelp::set_response_attr(req, content_type, "text/html", 0); // 0 tells do not allocate content_type string in a heap
//		evhtp_send_reply(req, EVHTP_RES_OK);
//		
//		return true;
//	}
//
//
//	return false;
//}

static void process_no_ssl_application_request(evhtp_request_t * req, struct app* app, evthr_t* thread, evhtp_connection_t * conn)
{
	string Root = string(Mkr_RootDir); 
	//MKR_DBG_PRINTF("NoSSLconn Root:%s\r\n", Root.c_str());
	if (Root.empty())
		return;

	string Path = string(req->uri->path->full);
	
	const char *content_type = "Content-Type";// evhtp_kv_find(req->headers_in, "Content-Type");
	string ext = Http::Content::GetExtFromFileName(Path);
	
	/*if (strstr(Path.c_str(), ".well-known") != 0)
	{
		std::string path = Root + Path;
		handle_certbot_request( req, path);
		return ;
	}*/
	//=============================================================
	if (strstr(Path.c_str(), "uc_smart.ino.bin") != 0)
	{
		process_fw_request(req);
		return;
	}
	//*********************************************************************
	
	if (Path == "/")
	{
		Path = Path + string(Mkr_No_SSL_Ref);
		Path = Root + Path;
	}
	else {
		//Path = Root + Path;     // -> do not process anything else
	}
	
	//MKR_DBG_PRINTF("NoSSLconn Path:%s\r\n", Path.c_str());

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

static std::shared_ptr<evhtp::EvhServer> ev_no_ssl_server = nullptr;
static std::thread::native_handle_type no_ssl_thread_native_handle;
int start_mkr_no_ssl_server(void *terminate) {

	 std::thread th  = std::thread([]() {
		 m_printf("Enter_mkr_no_ssl_server thread.");
		 ev_no_ssl_server = std::make_shared<evhtp::EvhServer>();
		 ev_no_ssl_server->setAppCb(process_no_ssl_application_request);
		 ev_no_ssl_server->start_libevhtp(ev_no_ssl_server.get(), NULL);
		 m_printf("Exit_mkr_no_ssl_server thread.");
	 });
	if (th.joinable())
		th.detach();  //the thread to run in the background (daemon)
	no_ssl_thread_native_handle = th.native_handle();

	
	return 0;
}

int stop_mkr_no_ssl_server(void)
{
	
#ifndef WIN32
	pthread_cancel(no_ssl_thread_native_handle);
#endif
	if (ev_no_ssl_server != nullptr)
	{
		ev_no_ssl_server->stop_libevhtp();
		ev_no_ssl_server.reset();
	}
	m_printf("stop_mkr_no_ssl_server..DONE.");
	return 0;
}

// vim: et ts=4 sw=4 cin cino={1s ff=unix


//#endif  //#if !USE_LIBEVENT