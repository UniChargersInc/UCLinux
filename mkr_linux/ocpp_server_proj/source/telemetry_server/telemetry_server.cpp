#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <fstream>
#include <iostream>
#include <set>
#include <streambuf>
#include <string>
#include "interface.hpp"
#include "mkr_ocpp.hpp"
#include "CommonUtils/crc32.hpp"
#include <google/protobuf/stubs/stringprintf.h>
#include "udorx_server_config.h"
#include <thread>
#include <memory>
#ifndef WIN32
#include <pthread.h>
#endif
void m_printf(const char *format, ...);

typedef websocketpp::server<websocketpp::config::asio> server;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg);

class telemetry_server {
public:
    typedef websocketpp::connection_hdl connection_hdl;
    typedef websocketpp::server<websocketpp::config::asio> server;

	typedef server::message_ptr message_ptr;

	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

    telemetry_server() : m_count(0) {
        // set up access channels to only log interesting things
        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.set_access_channels(websocketpp::log::alevel::access_core);
        m_endpoint.set_access_channels(websocketpp::log::alevel::app);

        // Initialize the Asio transport policy
        m_endpoint.init_asio();
		m_endpoint.set_reuse_addr(true);  //=====================================
        // Bind the handlers we are using
        using websocketpp::lib::placeholders::_1;
		
        using websocketpp::lib::bind;
        m_endpoint.set_open_handler(bind(&telemetry_server::on_open,this,_1));
        m_endpoint.set_close_handler(bind(&telemetry_server::on_close,this,_1));
        m_endpoint.set_http_handler(bind(&telemetry_server::on_http,this,_1));

				
    }

    void run(std::string docroot, uint16_t port) {
        std::stringstream ss;
        ss << "Running telemetry server on port "<< port <<" using docroot=" << docroot;
		OCPP_PRINTF("%s\r\n",ss.str().c_str());
        
        m_docroot = docroot;
        
		//svk
		m_endpoint.set_message_handler(&on_message);
		//m_endpoint.set_message_handler(bind(&on_message, this,::_1,::_2));

		/* Sep 12 18:57:22 uc_jira systemd[1]: Started Mikerel Server.
			Sep 12 18:57:24 uc_jira http_light.out[670]: [2021-09-12 18:57:24] [connect] WebSocket Connection [::ffff:74.14.43.184]:55350 v13 "TinyWebsockets Client" / 10
			Sep 12 19:13:42 uc_jira http_light.out[670]: [2021-09-12 19:13:42] [info] Error getting remote endpoint: system:9 (Bad file descriptor)
			Sep 12 19:13:42 uc_jira http_light.out[670]: [2021-09-12 19:13:42] [fail] WebSocket Connection Unknown - "" - 0 websocketpp:26 Operation canceled
			Sep 12 19:13:42 uc_jira http_light.out[670]: corrupted double-linked list

		*/
		m_endpoint.reset();              // added recently https://github.com/zaphoyd/websocketpp/issues/753

        // listen on specified port
        m_endpoint.listen(port);
		
        // Start the server accept loop
        m_endpoint.start_accept();

        // Set the initial timer to start telemetry
        //set_timer();

        // Start the ASIO io_service run loop
        try {
            m_endpoint.run();
        } catch (websocketpp::exception const & e) {
			OCPP_PRINTF("run Exc:%s\r\n", e.what() );
        }
    }
#if 0	
    void set_timer() {
        m_timer = m_endpoint.set_timer(
            1000,
            websocketpp::lib::bind(
                &telemetry_server::on_timer,
                this,
                websocketpp::lib::placeholders::_1
            )
        );
    }

    void on_timer(websocketpp::lib::error_code const & ec) {

		if (ec) {
            // there was an error, stop telemetry
            m_endpoint.get_alog().write(websocketpp::log::alevel::app,
                    "Timer Error: "+ec.message());
            return;
        }
        
        std::stringstream val;
        val << "srv count is " << m_count++;
        
		//svk
		//m_endpoint.get_alog().write(websocketpp::log::alevel::app, val.str());

        // Broadcast count to all connections
        con_list::iterator it;
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            m_endpoint.send(*it,val.str(),websocketpp::frame::opcode::text);
        }
        
        // set timer for next telemetry check
        set_timer();

    }
#endif
    void on_http(connection_hdl hdl) {
#if 0
		// Upgrade our connection handle to a full connection_ptr
        server::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
    
        std::ifstream file;
        std::string filename = con->get_resource();
        std::string response;
    
        m_endpoint.get_alog().write(websocketpp::log::alevel::app,
            "http request1: "+filename);
    
        if (filename == "/") {
            filename = m_docroot+"index.html";
        } else {
            filename = m_docroot+filename.substr(1);
        }
        
        m_endpoint.get_alog().write(websocketpp::log::alevel::app,
            "http request2: "+filename);
    
        file.open(filename.c_str(), std::ios::in);
        if (!file) {
            // 404 error
            std::stringstream ss;
        
            ss << "<!doctype html><html><head>"
               << "<title>Error 404 (Resource not found)</title><body>"
               << "<h1>Error 404</h1>"
               << "<p>The requested URL " << filename << " was not found on this server.</p>"
               << "</body></head></html>";
        
            con->set_body(ss.str());
            con->set_status(websocketpp::http::status_code::not_found);
            return;
        }
    
        file.seekg(0, std::ios::end);
        response.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
    
        response.assign((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
        con->set_body(response);
        con->set_status(websocketpp::http::status_code::ok);
#endif
    }

    void on_open(connection_hdl hdl) {
		server::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
		std::string ip_addr = con->get_remote_endpoint();
		setChargeStationStatus(ip_addr, ChargeStationStatus::AVAILABLE);    //set Status field of CH_STATION table to 1 
        m_connections.insert(hdl);
		//printf("OCPP Server..Insert conn ip_addr:%s\r\n", ip_addr.c_str());
    }

    void on_close(connection_hdl hdl) {
		server::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
		std::string ip_addr = con->get_remote_endpoint();
		setChargeStationStatus(ip_addr, ChargeStationStatus::UNAVAILABLE);

        m_connections.erase(hdl);
    }
	con_list getConnections() { return m_connections; }
	server *getServer() { return &m_endpoint; }
private:
    //typedef std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;
    
    server m_endpoint;
    con_list m_connections;
   // server::timer_ptr m_timer;
    
    std::string m_docroot;
    
    // Telemetry data
    uint64_t m_count;
};

static std::shared_ptr<telemetry_server> t_server=nullptr;

static bool sendResponse2Client(websocketpp::connection_hdl hdl, std::string &str)
{
	if (t_server == nullptr)
		return false;
	bool rv = true;
	server *s = t_server->getServer();
	try {
		s->send(hdl, str, /*msg->get_opcode()*/websocketpp::frame::opcode::text);
	}
	catch (websocketpp::exception const & e) {
		OCPP_PRINTF("OCPP send_resp failed: %s \r\n", e.what());
		rv = false;
	}
	return rv;
}
static websocketpp::connection_hdl* get_ocpp_client_connection(std::string forIP)
{
	if (t_server == nullptr)
		return nullptr;
	server *s = t_server->getServer();
	for (const websocketpp::connection_hdl &hdl : /*m_connections*/t_server->getConnections())
	{
		server::connection_ptr con = s->get_con_from_hdl(hdl);
		std::string remoteIP = con->get_remote_endpoint();
		if (remoteIP == forIP)
		{
			//applying the address-of operator to the reference is the same as taking the address of the original object
			/*The general solution is to use std::addressof, as in:
				#include <type_traits>
				void foo(T & x)
				{
					T * p = std::addressof(x);
				}*/
			return (websocketpp::connection_hdl*)&hdl;
		}
	}
	return nullptr;
}

struct fw_download_header {
	unsigned int t_size=0;
	unsigned int crc=0;

};
#define header_size	sizeof(fw_download_header)
void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
	if (t_server == nullptr)
		return;
	auto opcode = msg->get_opcode();
	if (opcode == websocketpp::frame::opcode::BINARY)
	{
#ifdef WIN32
		const char* pStr = msg->get_payload().c_str();
		int tmp = *(int*)pStr;
		OCPP_PRINTF("FW got: %x \r\n", tmp);
		if ((tmp & 0xFF000000) == 0x55000000)  // byte 3 0x55 magic signature, byte2 - sequence number, byte1/0 WORD offset from the file
		{
			
			std::string fname = "";
			{
				char fpath[128];
				sprintf(fpath, "%s/firmware/%s", UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT, "Grizzly_E_Smart.bin");
				fname = std::string(fpath);
			}

			int size = getSizeOfFile(fname.c_str());
			if (size <= 0)
				return;

			int seq_num = ((tmp & 0x00ff0000) >> 16);
			int size_to_be_sent = 0;
			unsigned char *pBuf = NULL;
			fw_download_header hdr;
			if (seq_num == 0)
			{

				size_to_be_sent = size / 2 + header_size;
				pBuf = new unsigned char[size + header_size];  // first 4 bytes CRC32 value
				int fsize = read_bin_file_to_buf(fname.c_str(), pBuf + header_size);
				if (fsize != size) {
					if (pBuf)
						delete[] pBuf;
					return;
				}
				//unsigned int crc_out=0;

				hdr.t_size = size;   // the whole file size
				crc32((pBuf + header_size), size, &hdr.crc);

				memcpy(pBuf, /*&crc_out*/&hdr, header_size);
			}
			else if (seq_num == 1) {
				int offset = (tmp & 0xffff);
				size_to_be_sent = size - offset;
				pBuf = new unsigned char[size_to_be_sent + header_size];  // first 4 bytes CRC32 value
				int fsize = read_bin_file_to_buf(fname.c_str(), (pBuf + header_size), size_to_be_sent, offset);
				if (fsize != size_to_be_sent) {
					if (pBuf)
						delete[] pBuf;
					return;
				}


				hdr.t_size = size_to_be_sent;   // the remaining part of the file
				crc32((pBuf + header_size), size_to_be_sent, &hdr.crc);  // crc for the remaining part

				memcpy(pBuf, /*&crc_out*/&hdr, header_size);

				size_to_be_sent += header_size;  // make sure the header is included
			}

			server *s = t_server->getServer();
			try {
				s->send(hdl, pBuf,/*size+ header_size*/size_to_be_sent, websocketpp::frame::opcode::BINARY);
				OCPP_PRINTF("Resp FW sent : %d crc=%x seq_num:%d\r\n", /*size*/size_to_be_sent, hdr.crc, seq_num);
			}
			catch (websocketpp::exception const & e) {
				OCPP_PRINTF("FW send failed: %s \r\n", e.what());
			}
			if (pBuf)
				delete[] pBuf;
		}
#else
		const char* pStr = msg->get_payload().c_str();
		int tmp = *(int*)pStr;
		OCPP_PRINTF("FW got: %x \r\n", tmp);
		if ((tmp & 0xFF000000) == 0x55000000)  // byte 3 0x55 magic signature, byte2 - sequence number, byte1/0 WORD offset from the file
		{
//#ifdef MKR_LINUX			
//			std::string fname = UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT + std::string("grizzle.bin");
//#else
//			std::string fname = UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT+std::string("grizzle.bin");
//#endif
			std::string fname = "";
			{
				char fpath[128];
				sprintf(fpath, "%s/firmware/%s", UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT, "grizzle.bin");
				fname = std::string(fpath);
			}

			int size = getSizeOfFile(fname.c_str());
			if (size <= 0)
				return;
			
			int seq_num = ((tmp & 0x00ff0000) >> 16);
			int size_to_be_sent = 0;
			unsigned char *pBuf = NULL;
			fw_download_header hdr;
			if (seq_num == 0)
			{
				
				size_to_be_sent = size / 2 + header_size;
				pBuf = new unsigned char[size + header_size];  // first 4 bytes CRC32 value
				int fsize = read_bin_file_to_buf(fname.c_str(), pBuf + header_size);
				if (fsize != size) {
					if (pBuf)
						delete[] pBuf;
					return;
				}
				//unsigned int crc_out=0;
				
				hdr.t_size = size;   // the whole file size
				crc32((pBuf + header_size), size, &hdr.crc);

				memcpy(pBuf, /*&crc_out*/&hdr, header_size);
			}
			else if(seq_num==1) {
				int offset = (tmp & 0xffff);
				size_to_be_sent = size - offset;
				pBuf = new unsigned char[size_to_be_sent + header_size];  // first 4 bytes CRC32 value
				int fsize = read_bin_file_to_buf(fname.c_str(), (pBuf + header_size), size_to_be_sent,offset);
				if (fsize != size_to_be_sent) {
					if (pBuf)
						delete[] pBuf;
					return;
				}

				
				hdr.t_size = size_to_be_sent;   // the remaining part of the file
				crc32((pBuf + header_size), size_to_be_sent, &hdr.crc);  // crc for the remaining part

				memcpy(pBuf, /*&crc_out*/&hdr, header_size);

				size_to_be_sent += header_size;  // make sure the header is included
			}
			
			server *s = t_server->getServer();
			try {
				s->send(hdl, pBuf,/*size+ header_size*/size_to_be_sent, websocketpp::frame::opcode::BINARY);
				OCPP_PRINTF("Resp FW sent : %d crc=%x seq_num:%d\r\n", /*size*/size_to_be_sent, hdr.crc, seq_num);
			}
			catch (websocketpp::exception const & e) {
				OCPP_PRINTF("FW send failed: %s \r\n", e.what());
			}
			if (pBuf)
				delete[] pBuf;
		}
#endif		
	}
	else if(opcode == websocketpp::frame::opcode::text)
	{
		OCPP_Request req;
		req.req_str = msg->get_payload();
		
		server *s = t_server->getServer();
		server::connection_ptr con = s->get_con_from_hdl(hdl);
		//std::string host=con->get_host();
		req.remoteIP = con->get_remote_endpoint();
		//OCPP_PRINTF("msg from %s\r\n", req.remoteIP.c_str());   // ==============  [::ffff:99.238.89.71:58765]
		
		if (req.req_str.size() <= 2)
		{
			//OCPP_PRINTF("Empty msg: %s\r\n", req.req_str.c_str());
			return;
		}

		OCPP_Response resp;
		bool	rv = processOCPPIncoming(req, resp);
		if (rv)
		{
			for (std::string &s : resp.resp_str) {
				sendResponse2Client(hdl, s);
#ifdef WIN32
				::Sleep(100);
#endif
			}
		}
		else {

			//OCPP_PRINTF("Internal Err msg: %s\r\n", req.req_str.c_str());
			
			//[<MessageTypeId>, "<UniqueId>", "<errorCode>", "<errorDescription>", {<errorDetails>}]
			const char *errCode = "InternalError",
				*errDesc = "**";
			std::string json = "{}";
			std::string str = google::protobuf::StringPrintf("[%d,\"%s\",\"%s\",\"%s\",%s]", 4, req.uniqueID.c_str(), errCode, errDesc, json.c_str());
			sendResponse2Client(hdl, str);
			OCPP_PRINTF("Internal Err msg: %s\r\n", str.c_str());
		}
	}
}

//======================================================================
bool sendResponse2Client(std::string forIP, std::string &str)
{
	websocketpp::connection_hdl* hdl=get_ocpp_client_connection(forIP);
	if (hdl == nullptr) {
		OCPP_PRINTF("could not find conn for IP:%s", forIP.c_str());
		return false;
	}
	return sendResponse2Client(*hdl, str);
}

#if 0
static void signal_callback_handler(int signum) {

	if (signum > 0)
	{
		m_printf("Caught signal SIGPIPE %d\n", signum);
		signum = signum;
	}
}

void handle_sigint(int signal)
{
	OCPP_PRINTF("handle_sigint %d ..\r\n", signal);
	ocpp_light_main_run = 0;

	stop_ocpp_server();

}
/* Signal handler for SIGHUP - flag a config reload. */
void handle_sighup(int signal)
{
	//flag_reload = true;
	OCPP_PRINTF("sighup %d ..\r\n", signal);
}
/* Signal handler for SIGUSR1 - backup the db. */
void handle_sigusr1(int signal)
{
#ifdef WITH_PERSISTENCE
	flag_db_backup = true;
#endif
	OCPP_PRINTF("sigusr1 %d ..\r\n", signal);
}

/* Signal handler for SIGUSR2 - vacuum the db. */
void handle_sigusr2(int signal)
{
	//flag_tree_print = true;
	OCPP_PRINTF("sigusr2 %d ..\r\n", signal);
}
void handle_sigsegv(int signal)
{
	//flag_tree_print = true;
	OCPP_PRINTF("SIGSEGV %d ..\r\n", signal);
}
#endif

//static const string log_path = "/home/mkr/service/ocpp/ocpp_light.log";
static std::thread::native_handle_type ocpp_thread_native_handle;
int main_ocpp_server_start() {
	t_server = std::make_shared<telemetry_server>();
	if (t_server == nullptr)
		return -1;
//	InitLog(log_path);

   
	std::thread thrd = std::thread([]() {
		std::string docroot = "./";
		uint16_t port = WS_PORT;
		OCPP_PRINTF("*Initialize OCPP Server.");
		t_server->run(docroot, port);
	});
	ocpp_thread_native_handle=thrd.native_handle();
	//if (ocpp_listener_thread.joinable())
	thrd.detach();  //the thread to run in the background (daemon)
	
	return 0;
}

//======================================================================
//static int ocpp_light_main_run = 1;
void stop_ocpp_server()
{
	//OCPP_PRINTF("Stop  OCPP Server..\r\n");
	if (t_server != nullptr)
	{

		server *s = t_server->getServer();
		s->stop();
#ifndef WIN32
		pthread_cancel(ocpp_thread_native_handle);
#endif
		t_server.reset();
		OCPP_PRINTF("Stop  OCPP Server..DONE.");
	}
}