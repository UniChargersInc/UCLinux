#ifndef _CPPSIPLIB_UDPLIB_H_
#define _CPPSIPLIB_UDPLIB_H_

#include "sip_server_config.h"
#define USE_SHORT_NAMES	1


//svk

void m_printf(const char *format, ...);
#define MKR_PRINTF m_printf

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#ifndef SO_SYNCHRONOUS_NONALERT
#define SO_SYNCHRONOUS_NONALERT 0x20
#endif
#ifndef SO_OPENTYPE
#define SO_OPENTYPE 0x7008
#endif
#if (_MSC_VER < 1900)
#define snprintf _snprintf_s
#endif

#define S_ISREG(m)  (((m)&S_IFREG)==S_IFREG)
#define S_ISDIR(m)  (((m)&S_IFDIR)==S_IFDIR)

#include <fcntl.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#undef min
#undef max

typedef SOCKET socket_t;
#else
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>

typedef int socket_t;
typedef struct sockaddr SOCKADDR;
#endif

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <assert.h>

#include <sstream>
#include "sip_helpers.h"
#include <thread>
#include <mutex>

#include <stdexcept>  // to throw exception
#include <stdarg.h>


typedef void* void_p_t;
struct ActiveConnection
{
	void  *wsi;
	std::string data_to_send;
	ActiveConnection(void *wsi) { this->wsi = wsi; data_to_send = ""; }
	~ActiveConnection()
	{
		wsi = NULL;
		data_to_send = "";
	}
};

struct Error : std::exception
{
	char text[1000];
#ifndef WIN32
	Error(char const* fmt, ...) __attribute__((format(printf, 2, 3))) {
#else
	Error(char const* fmt, ...)  {
#endif
		va_list ap;
		va_start(ap, fmt);
		vsnprintf(text, sizeof text, fmt, ap);
		va_end(ap);
}

	char const* what() const throw() { return text; }
};



#define USE_THREAD_POOL 0
#if USE_THREAD_POOL
#include "MyThreadPool.h"
#define NUM_THREADS 4
#endif

#include <thread>



typedef std::map<std::string, std::string>      Map;
typedef std::multimap<std::string, std::string> MultiMap;
typedef std::smatch                             Match;


#define DEBUG_SHOW_FULL_IN_MSG	0
#define DEBUG_SHOW_FULL_OUT_MSG	0

struct SipRequest
{
	std::vector<std::string> via;
	/* via = "", */  /*The Via header field indicates the path taken by the request so far and helps in routing the responses back along the same path. If the SIP INVITE passed through multiple SIP proxies, there will be multiple VIA headers.*/
	std::string	from = "",
		to = "",
		contact = "",
		cseq = "",
		cid = "",
		expireStr = "",
		cont_type = "",
		cont_len = "",
		allow = "",
		supported = "",
		userAgent = "",
		maxForwards = "";

	std::string  pEarlyMedia = "",
		prefferedIdentity = "";

	//SDP data
	/*std::vector<std::string> sdp_a;
	std::vector<std::string> sdp_b;
	std::string  sdp_v = "",
		sdp_c = "",
		sdp_m = "",
		sdp_o = "",
		sdp_s = "",
		sdp_t = "";*/
#if !USE_SHORT_NAMES
	void processKeyVal(std::string& key, std::string& val)
	{
		if (key == "From")
			this->from = val;
		else if (key == "To")
			this->to = val;
		else if (key == "Contact")
			this->contact = val;
		else if (key == "CSeq")
			cseq = val;
		else if (key == "Call-ID")
			cid = val;
		else if (key == "Expires")
			expireStr = val;
		else if (key == "Content-Type")
			cont_type = val;
		else if (key == "Content-Length")
			cont_len = val;
		else if (key == "Max-Forwards")
			maxForwards = val;
		else if (key == "Allow")
			allow = val;
		else if (key == "Supported")
			supported = val;
		else if (key == "User-Agent")
			userAgent = val;
		else if (key == "P-Early-Media")
			pEarlyMedia = val;
		else if (key == "P-Preferred-Identity")
			prefferedIdentity = val;
	}
#else
	void processKeyVal(std::string& key, std::string& val)
	{
		if (key == "f")
			this->from = val;
		else if (key == "t")
			this->to = val;
		else if (key == "m")
			this->contact = val;
		else if (key == "CSeq")
			cseq = val;
		else if (key == "i")
			cid = val;
		else if (key == "Expires")
			expireStr = val;
		else if (key == "c")
			cont_type = val;
		else if (key == "l")
			cont_len = val;
		else if (key == "Max-Forwards")
			maxForwards = val;
		/*else if (key == "Allow")
			allow = val;*/
		else if (key == "k")
			supported = val;
		/*else if (key == "User-Agent")
			userAgent = val;*/
		else if (key == "P-Early-Media")
			pEarlyMedia = val;
		else if (key == "P-Preferred-Identity")
			prefferedIdentity = val;
	}
#endif
	/*void processSdp(std::string &req_body)
	{
		std::vector<std::string> vV = split(req_body, '\n');
		if (vV.size() > 0)
		{
			for (std::string &s1 : vV)
			{
				std::string key;
				std::string val;
				sip_helpers::split_m_1(&s1[0], &s1[s1.size()], '=', [&](const char* b, const char* e) {
					if (key.empty()) {
						key.assign(b, e);
					}
					else {
						val.assign(b, e);
					}
					});
				removeCharFromStr(val, '\r');
				if (key == "v")
					sdp_v = val;
				else if (key == "o")
					sdp_o = val;
				else if (key == "s")
					sdp_s = val;
				else if (key == "c")
					sdp_c = val;
				else if (key == "t")
					sdp_t = val;
				else if (key == "m")
					sdp_m = val;
				else if (key == "a")
					sdp_a.push_back(val);
				else if (key == "b")
					sdp_b.push_back(val);
			}
		}
	}*/
};


struct Request {
	std::string method;
	//std::string path;
   // MultiMap    headers;

	std::string body;

	SipRequest sr;

	
	std::string get_param(const char* key) const;
};

struct Response {
	int         status;
	std::string statusStr;   //200 OK
	MultiMap    headers;
	std::string body;

	bool has_header(const char* key) const;
	std::string get_header_value(const char* key) const;
	void set_header(const char* key, const char* val);

	
	Response() : status(-1) {}
};

class SipStream {
public:
	virtual ~SipStream() {}
	virtual int read(char* ptr, size_t size) = 0;
	virtual int write(const char* ptr, size_t size1) = 0;
	virtual int getSock() = 0;
	virtual SOCKADDR *getClientAddr() = 0;
	virtual const char *getInBuffer(int &length) = 0;

	virtual void setClientAddr(SOCKADDR * pClientAddr_, int ClientAddrLen_) = 0;
	virtual void setDestAddr(SOCKADDR * pDestAddr_, int DestAddrLen_) = 0;

	virtual void* getUserData() = 0;
	virtual void setUserData(void* d) = 0;
};

typedef std::function<void(SipStream* strm, const Request&, Response&)> Handler;
typedef std::function<void(const Request&, const Response&)> Logger;

/*
 * Implementation
 */
namespace detail {


	inline bool socket_readln(SipStream* strm, char* buf, int bufsiz);

	template <typename ...Args>
	inline void socket_printf(SipStream& strm, const char* fmt, const Args& ...args)
	{
		char buf[BUFSIZ];
		auto n = snprintf(buf, BUFSIZ, fmt, args...);
		if (n > 0) {
			if (n >= BUFSIZ) {
				// TODO: buffer size is not large enough...
			}
			else {
				strm.write(buf, n);
			}
		}
	}

	inline int close_socket(socket_t sock);

	inline int shutdown_socket(socket_t sock);

	inline bool is_file(const std::string& s);

	inline bool is_dir(const std::string& s);

	inline void read_file(const std::string& path, std::string& out);

	inline std::string file_extension(const std::string& path);

	//inline const char* content_type(const std::string& path)
	inline const char* content_type_by_ext(const std::string& ext);
	inline const char* status_message(int status);

	inline const char* get_header_value(const MultiMap& map, const char* key, const char* def);

	inline int get_header_value_int(const MultiMap& map, const char* key, int def);

	template <typename T>
	int read_content(SipStream* strm, T& x, bool allow_no_content_length)
	{
		int len = x.sr.cont_len.empty() ? 0 : std::stoi(x.sr.cont_len);
		if (len) {
			x.body.assign(len, 0);
			auto r = 0;
			while (r < len) {
				auto r_incr = strm->read(&x.body[r], len - r);
				if (r_incr <= 0) {
					return -1;
				}
				r += r_incr;
			}
		}
		else if (allow_no_content_length) {
			for (;;) {
				char byte;
				auto n = strm->read(&byte, 1);
				if (n < 1) {
					if (x.body.size() == 0) {
						return 0; // no body
					}
					else {
						break;
					}
				}
				x.body += byte;
			}
		}
		return len;
	}

	inline void set_dst_address_for_response(SipStream* strm, struct sockaddr_in *pAddr, int nBytesToSend);
	inline void write_response(SipStream* strm, struct sockaddr_in *pAddr, std::stringstream& ss);

	inline bool is_hex(char c, int& v);
	inline int from_hex_to_i(const std::string& s, int i, int cnt, int& val);
	inline size_t to_utf8(int code, char* buff);
	inline std::string decode_url(const std::string& s);

#ifdef _MSC_VER
	class WSInit {
	public:
		WSInit() {
			WSADATA wsaData;
			WSAStartup(0x0002, &wsaData);
		}

		~WSInit() {
			WSACleanup();
		}
	};


#endif

} // namespace detail

#define MAXLINE 2048*2

#if USE_UDP_SIP_TRANSPORT

class UdpSocketStream : public SipStream {
public:
    UdpSocketStream(socket_t sock);
    virtual ~UdpSocketStream();

    int read(char* ptr, size_t size);
    virtual int write(const char* ptr, size_t size);
    virtual int write(const char* ptr);

	SOCKADDR *getClientAddr() { return pClientAddr; }
	const char *getInBuffer(int &length)
	{
		length = bytesInBuffer;
		return pIn;
	}

	void setInBuffer(const char *pIn_, int bytes) { this->pIn = pIn_; bytesInBuffer = bytes; currentIndex = 0;}
	void setClientAddr(SOCKADDR * pClientAddr_, int ClientAddrLen_) { this->pClientAddr = pClientAddr_; ClientAddrLen = ClientAddrLen_; }

	void setDestAddr(SOCKADDR * pDestAddr_, int DestAddrLen_)
	{
		memcpy(&DestAddr, pDestAddr_, DestAddrLen_);
		DestAddrLen = DestAddrLen_;
	}

	void* getUserData() { return udata; }
	void setUserData(void* d) { udata = d; }

	int getSock() {
		return sock_;
	}
private:
    socket_t sock_;
	const char *pIn;
	int bytesInBuffer;
	int currentIndex = 0;

	SOCKADDR * pClientAddr;
	int ClientAddrLen; 

	sockaddr_in  DestAddr;
	int DestAddrLen;

	void *udata = 0;
};


class UdpSipServer {
public:
   

    UdpSipServer();
    virtual ~UdpSipServer();

    void get(const char* pattern, Handler handler);
    
    void set_error_handler(Handler handler);
    void set_logger(Logger logger);
	Handler     getHandler() { return get_handler_; }
	Handler     getErrHandler() { return error_handler_; }

    virtual bool listen(const char* host, int port, int *terminate, int socket_flags = 0);
	bool test_sip_request();
    void stop();

	
	virtual void sendResponse(SipStream* strm, struct sockaddr_in *pDestAddr, std::stringstream&);

    void process_request(SipStream* strm, Request& req, Response& res);
	//virtual void post_process_request(SipStream& strm, Request& req,	Response& res);

private:

   
    bool read_request_line(SipStream* strm, Request& req);
	bool read_headers(SipStream* strm, Request& req,std::vector<std::string> *headers);

    socket_t    svr_sock_;
	Handler     get_handler_;
	Handler     error_handler_;
    //Logger      logger_;
};


//===============================================================
#endif
#if USE_TCP_SIP_TRANSPORT

	class TcpSocketStream : public SipStream {
	public:
		TcpSocketStream(socket_t sock);
		virtual ~TcpSocketStream();

		int read(char* ptr, size_t size);
		virtual int write(const char* ptr, size_t size);
		int getSock() { return sock_; }

		SOCKADDR *getClientAddr() { return &ClientAddr; }
		const char *getInBuffer(int &length)
		{
			length = bytesInBuffer;
			return NULL;// pIn;
		}

		void setInBuffer(const char *pIn_, int bytes) { /*this->pIn = pIn_;*/ bytesInBuffer = bytes; currentIndex = 0; }
		void setClientAddr(SOCKADDR * pClientAddr_, int ClientAddrLen_) 
		{ 
			memcpy(&ClientAddr, pClientAddr_, ClientAddrLen_);//this->pClientAddr = pClientAddr_; 
			ClientAddrLen = ClientAddrLen_; 
		}

		void setDestAddr(SOCKADDR * pDestAddr_, int DestAddrLen_)
		{
			memcpy(&DestAddr, pDestAddr_, DestAddrLen_);
			DestAddrLen = DestAddrLen_;
		}

		void* getUserData() { return udata; }
		void setUserData(void* d) { udata = d; }

		void* getServerPtr() { return serverPtr; }
		void setServerPtr(void* d) { serverPtr = d; }
		

	private:
		socket_t sock_;
		//const char *pIn=NULL;
		int bytesInBuffer=0;
		int currentIndex = 0;

		SOCKADDR  ClientAddr;
		int ClientAddrLen;

		sockaddr_in  DestAddr;
		int DestAddrLen;
		void *serverPtr = 0;
		void *udata = 0;
	};


	class TcpSipServer {
	public:


		TcpSipServer();
		virtual ~TcpSipServer();

		void get(const char* pattern, Handler handler);

		void set_error_handler(Handler handler);
		void set_logger(Logger logger);
		Handler     getHandler() { return get_handler_; }
		Handler     getErrHandler() { return error_handler_; }

		virtual bool listen(const char* host, int port, int *terminate, int socket_flags = 0);
		bool test_sip_request();
		void stop();


		virtual void sendResponse(SipStream* strm, struct sockaddr_in *pDestAddr, std::stringstream&);

		void process_request(SipStream* strm, Request& req, Response& res);
		//virtual void post_process_request(SipStream& strm, Request& req,	Response& res);


		// maintain active connections
		std::vector<ActiveConnection> conn_list;   // global for all threads  !!!
		bool removeActConn(void *wsi)
		{
			std::lock_guard<std::mutex> lock(send_mutex); // to synchronize access to conn_list
			auto it1 = std::find_if(conn_list.begin(), conn_list.end(), [wsi](const ActiveConnection& c) {
				return (c.wsi == wsi);
			});
			if (it1 != conn_list.end()) {
				conn_list.erase(it1);
				return true;
			}
			return false;
		}
		int insert2ActConn(void *wsi)
		{
			std::lock_guard<std::mutex> lock(send_mutex); // to synchronize access to conn_list
			ActiveConnection conn(wsi);
			conn_list.push_back(conn);
			return conn_list.size();
		}
	private:
		std::mutex send_mutex;

		bool read_request_line(SipStream* strm, Request& req);
		bool read_headers(SipStream* strm, Request& req, std::vector<std::string> *headers);

		socket_t    svr_sock_;
		Handler     get_handler_;
		Handler     error_handler_;
		
	};




	//===============================================================


#endif  //USE_UDP_SIP_TRANSPORT

#endif

// vim: et ts=4 sw=4 cin cino={1s ff=unix
