
#include "sip_lib_stream.hpp"
#include <fcntl.h>  // non blocing socket

/*
 * Implementation
 */
namespace detail {


	bool socket_readln(SipStream* strm, char* buf, int bufsiz)
	{
		// TODO: buffering for better performance
		size_t i = 0;

		for (;;) {
			char byte;
			auto n = strm->read(&byte, 1);

			if (n == -1)  // svk added here to capture error
			{
				int err = errno;
				throw Error("strm.read=-1 err=%d",err);
			}
			if (n == 0)
			{
				// When a stream socket peer has performed an orderly shutdown, the
				//return value will be 0 (the traditional "end-of-file" return).
				throw Error("strm.read= shutdown");
			}

			if (n < 1) {
				if (i == 0) {
					return false;
				}
				else {
					break;
				}
			}

			buf[i++] = byte;

			if (byte == '\n') {
				break;
			}
		}

		buf[i] = '\0';
		return true;
	}

	int close_socket(socket_t sock)
	{
#ifdef _MSC_VER
		return closesocket(sock);
#else
		return close(sock);
#endif
	}

	int shutdown_socket(socket_t sock)
	{
#ifdef _MSC_VER
		return shutdown(sock, SD_BOTH);
#else
		return shutdown(sock, SHUT_RDWR);
#endif
	}




	bool is_file(const std::string& s)
	{
		struct stat st;
		return stat(s.c_str(), &st) >= 0 && S_ISREG(st.st_mode);
	}

	bool is_dir(const std::string& s)
	{
		struct stat st;
		return stat(s.c_str(), &st) >= 0 && S_ISDIR(st.st_mode);
	}

	void read_file(const std::string& path, std::string& out)
	{
		std::ifstream fs(path, std::ios_base::binary);
		fs.seekg(0, std::ios_base::end);
		auto size = fs.tellg();
		fs.seekg(0);
		out.resize(static_cast<size_t>(size));
		fs.read(&out[0], size);
	}

	std::string file_extension(const std::string& path)
	{
		std::smatch m;
		auto pat = std::regex("\\.([a-zA-Z0-9]+)$");
		if (std::regex_search(path, m, pat)) {
			return m[1].str();
		}
		return std::string();
	}

	// const char* content_type(const std::string& path)
	const char* content_type_by_ext(const std::string& ext)
	{

		return nullptr;
	}

	const char* status_message(int status)
	{
		switch (status) {
		case 200: return "OK";
		case 400: return "Bad Request";
		case 404: return "Not Found";
		default:
		case 500: return "Internal UdpSipServer Error";
		}
	}

	const char* get_header_value(const MultiMap& map, const char* key, const char* def)
	{
		auto it = map.find(key);
		if (it != map.end()) {
			return it->second.c_str();
		}
		return def;
	}

	int get_header_value_int(const MultiMap& map, const char* key, int def)
	{
		auto it = map.find(key);
		if (it != map.end()) {
			return std::stoi(it->second);
		}
		return def;
	}


	void set_dst_address_for_response(SipStream* strm, struct sockaddr_in *pAddr, int nBytesToSend)
	{
		SOCKADDR *pDestAddr = NULL;
		if (pAddr == NULL)
		{
			pDestAddr = strm->getClientAddr();
		}
		else {
			pDestAddr = (SOCKADDR *)pAddr;
		}
		strm->setDestAddr(pDestAddr, sizeof(struct sockaddr_in));

		if (pDestAddr) {
			struct sockaddr_in *pAddr1 = (struct sockaddr_in *)pDestAddr;
			char *ip = inet_ntoa(pAddr1->sin_addr);
			MKR_PRINTF(">>Res>> to %s:%d length=%d\r\n", ip, ntohs(pAddr1->sin_port), nBytesToSend);
		}
	}

	void write_response(SipStream* strm, struct sockaddr_in *pAddr, std::stringstream& ss)
	{
		const std::string &s = ss.str();
		set_dst_address_for_response(strm, pAddr, s.size());
		//M_HTTP_PRINTF("****Send Response****\r\n");
#if DEBUG_SHOW_FULL_OUT_MSG
		MKR_PRINTF("%s", s.c_str());
		MKR_PRINTF("****End Response****\r\n");
#endif
		strm->write(s.c_str(), s.size());

	}

	
	bool is_hex(char c, int& v)
	{
		if (0x20 <= c && isdigit(c)) {
			v = c - '0';
			return true;
		}
		else if ('A' <= c && c <= 'F') {
			v = c - 'A' + 10;
			return true;
		}
		else if ('a' <= c && c <= 'f') {
			v = c - 'a' + 10;
			return true;
		}
		return false;
	}

	int from_hex_to_i(const std::string& s, int i, int cnt, int& val)
	{
		val = 0;
		for (; s[i] && cnt; i++, cnt--) {
			int v = 0;
			if (is_hex(s[i], v)) {
				val = val * 16 + v;
			}
			else {
				break;
			}
		}
		return --i;
	}

	size_t to_utf8(int code, char* buff)
	{
		if (code < 0x0080) {
			buff[0] = (code & 0x7F);
			return 1;
		}
		else if (code < 0x0800) {
			buff[0] = (0xC0 | ((code >> 6) & 0x1F));
			buff[1] = (0x80 | (code & 0x3F));
			return 2;
		}
		else if (code < 0xD800) {
			buff[0] = (0xE0 | ((code >> 12) & 0xF));
			buff[1] = (0x80 | ((code >> 6) & 0x3F));
			buff[2] = (0x80 | (code & 0x3F));
			return 3;
		}
		else if (code < 0xE000) { // D800 - DFFF is invalid...
			return 0;
		}
		else if (code < 0x10000) {
			buff[0] = (0xE0 | ((code >> 12) & 0xF));
			buff[1] = (0x80 | ((code >> 6) & 0x3F));
			buff[2] = (0x80 | (code & 0x3F));
			return 3;
		}
		else if (code < 0x110000) {
			buff[0] = (0xF0 | ((code >> 18) & 0x7));
			buff[1] = (0x80 | ((code >> 12) & 0x3F));
			buff[2] = (0x80 | ((code >> 6) & 0x3F));
			buff[3] = (0x80 | (code & 0x3F));
			return 4;
		}

		// NOTREACHED
		return 0;
	}

	std::string decode_url(const std::string& s)
	{
		std::string result;

		for (int i = 0; s[i]; i++) {
			if (s[i] == '%') {
				i++;
				assert(s[i]);

				if (s[i] == '%') {
					result += s[i];
				}
				else if (s[i] == 'u') {
					// Unicode
					i++;
					assert(s[i]);

					int val = 0;
					i = from_hex_to_i(s, i, 4, val);

					char buff[4];
					size_t len = to_utf8(val, buff);

					if (len > 0) {
						result.append(buff, len);
					}
				}
				else {
					// HEX
					int val = 0;
					i = from_hex_to_i(s, i, 2, val);
					result += val;
				}
			}
			else if (s[i] == '+') {
				result += ' ';
			}
			else {
				result += s[i];
			}
		}

		return result;
	}

	
#ifdef _MSC_VER
	static WSInit wsinit_;
#endif

} // namespace detail

//======================================================================
std::string Request::get_param(const char* key) const
{
	/*auto it = params.find(key);
	if (it != params.end())
	{
		return it->second;
	}*/
	return std::string();
}

// Response implementation
bool Response::has_header(const char* key) const
{
	return headers.find(key) != headers.end();
}

std::string Response::get_header_value(const char* key) const
{
	return detail::get_header_value(headers, key, "");
}

void Response::set_header(const char* key, const char* val)
{
	headers.insert(std::make_pair(key, val));
}


#if USE_UDP_SIP_TRANSPORT

// Socket stream implementation
 UdpSocketStream::UdpSocketStream(socket_t sock): sock_(sock)
{
}

 UdpSocketStream::~UdpSocketStream()
{
}

 int UdpSocketStream::read(char* ptr, size_t size)
{
	int rv = 0;
   // return recv(sock_, ptr, size, 0);
	if (currentIndex >= bytesInBuffer)
		return -1;
	if ((currentIndex + size) < bytesInBuffer)
	{
		memcpy(ptr, pIn + currentIndex, size);
		rv = size;
		currentIndex += size;
	}
	else {
		size = bytesInBuffer - currentIndex;
		if (size > 0) {
			memcpy(ptr, pIn + currentIndex, size);
			rv = size;
			currentIndex += size;
		}

	}
	return rv;
}

 int UdpSocketStream::write(const char* ptr, size_t size)
{
	int res = sendto(sock_,	ptr, size, 0,(SOCKADDR*) &DestAddr, DestAddrLen);
#ifdef WIN32
	if (res == SOCKET_ERROR) {
		MKR_PRINTF("sendto failed with error: %d\n", WSAGetLastError());
	}
#else
	if (res== EMSGSIZE || res != size)
	{
		MKR_PRINTF("linux sendto failed  %d : %d\n", size, res);
	}
#endif
	
	//return send(sock_, ptr, size, 0);
	return res;
}

 int UdpSocketStream::write(const char* ptr)
{
    return write(ptr, strlen(ptr));
}

// HTTP server implementation
 UdpSipServer::UdpSipServer()
    : svr_sock_(-1)
{
#ifndef _MSC_VER
    signal(SIGPIPE, SIG_IGN);
#endif
}

 UdpSipServer::~UdpSipServer()
{
}

 void UdpSipServer::get(const char* pattern, Handler handler)
{
    //get_handlers_.push_back(std::make_pair(std::regex(pattern), handler));
	get_handler_ = handler;
}


 void UdpSipServer::set_error_handler(Handler handler)
{
    error_handler_ = handler;
}

 void UdpSipServer::set_logger(Logger logger)
{
  //  logger_ = logger;
}


//===============================================================
#define MAXLINE 2048*2
 bool UdpSipServer::listen(const char* host, int port,int *terminate, int socket_flags)
{
    //svr_sock_ = detail::create_server_socket(host, port, socket_flags);
	int sockfd;
	char buffer[MAXLINE];
	
	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	// Bind the socket with the server address 
	if (::bind(sockfd, (const struct sockaddr *)&servaddr,
		sizeof(servaddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}


	while (*terminate == 1)
	{
		int len = sizeof(cliaddr) , n;
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0 , (SOCKADDR *)&cliaddr, (socklen_t*)&len);
		//MKR_PRINTF("recvfrom.. %d \r\n",n);
#ifdef WIN32	
		if (n == SOCKET_ERROR) {
			MKR_PRINTF("recvfrom failed err:%d\n", WSAGetLastError());
			continue;
		}
		else if (n == 2)
		{
			// keep alive message
			continue;
		}
#else
		if (n == -1) {
			MKR_PRINTF("recvfrom failed err:%d\n", -1);
			continue;
		}
#endif

		UdpSocketStream strm(sockfd);
		strm.setInBuffer(buffer, n);
		strm.setClientAddr((SOCKADDR *)&cliaddr, len);
		//MKR_PRINTF("process_request n=%d\r\n",n);
		Request req;
		Response res;
		
		try {
			process_request((SipStream*)&strm, req, res);
		}
		catch (std::exception &ex)
		{
			MKR_PRINTF("udp session EXC..%s\r\n", ex.what());
			//break;
		}
		

	}

	MKR_PRINTF("The server shutdown()..\r\n");

    return true;
}

//=============test sip request================
 bool UdpSipServer::test_sip_request()
{
	//std::string invite = //"SIP/2.0 200 OK\r\n"
	//					  "INVITE sip:bob@biloxi.example.com SIP/2.0\r\n"
	//					  "Via: SIP/2.0/ TCP client.biloxi.example.com:5060;branch=z9hG4bKnashds7\r\n"
 //                        "From: Bob <sip:bob@biloxi.example.com>;tag=8321234356\r\n"
 //                        "To: Alice <sip:alice@atlanta.example.com>;tag=9fxced76sl\r\n"
 //                        "Call-ID: 3848276298220188511@atlanta.example.com\r\n"
 //                        "CSeq: 1 BYE\r\n"
	//	                 "Content-Type: application/sdp\r\n"
 //                        "Content-Length: 151\r\n"
	//	                "\r\n"
	//	        		"v=0\r\n"
	//	                "o=alice 2890844526 2890844526 IN IP4 client.atlanta.example.com\r\n"
	//					"s=-\r\n"
	//					"c=IN IP4 192.0.2.101\r\n"
	//					"t=0 0\r\n"
	//					"m=audio 49172 RTP/AVP 0\r\n"
	//	                "a=rtpmap:0 PCMU/8000\r\n"
	//	;

	std::string invite = "SIP/2.0 200 OK\r\n"
		"Via: SIP/2.0/ TCP client.biloxi.example.com:5060;branch=z9hG4bKnashds7\r\n"
		"Via: SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74b43\r\n"
		"From: Bob <sip:bob@biloxi.example.com>;tag=8321234356\r\n"
		"To: Alice <sip:alice@atlanta.example.com>;tag=9fxced76sl\r\n"
		"Call-ID: 3848276298220188511@atlanta.example.com\r\n"
		"CSeq: 1 BYE\r\n"
		"Content-Length: 0\r\n"
		;
	
	UdpSocketStream strm(-1);
	strm.setInBuffer(invite.c_str(), invite.size());
	//strm.setClientAddr((SOCKADDR *)&cliaddr, len);
	Request req;
	Response res;
	process_request((SipStream*)&strm,req,res);

	return true;
}

 void UdpSipServer::stop()
{
    detail::shutdown_socket(svr_sock_);
    detail::close_socket(svr_sock_);
    svr_sock_ = -1;
}

 bool UdpSipServer::read_request_line(SipStream* strm, Request& req)
{
    const auto BUFSIZ_REQUESTLINE = 2048;
    char buf[BUFSIZ_REQUESTLINE];
    if (!detail::socket_readln(strm, buf, BUFSIZ_REQUESTLINE)) {
        return false;
    }
	//std::string s1 = std::string(buf);
	/*std::string key;
	std::string val;
	detail::split(&s1[0], &s1[s1.size()], ' ', [&](const char* b, const char* e) {
		if (key.empty()) {
			key.assign(b, e);
		}
		else {
			val.assign(b, e);
		}
		});*/

	req.method = std::string(buf);
	//req.path = "";
	return true;
    
}
 bool UdpSipServer::read_headers(SipStream* strm, Request& req, std::vector<std::string> *headers)
{
	
		static std::regex re("(.+?): (.+?)\r\n");

		const auto BUFSIZ_HEADER = 2048;
		char buf[BUFSIZ_HEADER];
		int numberRead = 0;
		for (;;) {
			if (!detail::socket_readln(strm, buf, BUFSIZ_HEADER)) {
				goto __exit;
			}
			if (!strcmp(buf, "\r\n")) {
				break;
			}
			std::cmatch m;
			//MKR_PRINTF("%s", buf);             // debug header
			if (headers)
				headers->push_back(buf);
			if (std::regex_match(buf, m, re)) {
				auto key = std::string(m[1]);
				auto val = std::string(m[2]);
#if		!USE_SHORT_NAMES
				if (key == "Via") 
#else
				if (key == "v")
#endif
				{
					req.sr.via.push_back(val);
					numberRead++;
				}
				else {
					//req.headers.insert(std::make_pair(key, val));
					req.sr.processKeyVal(key, val);
					numberRead++;
				}
			}
		}
__exit:
		return numberRead>0;
	
}
void UdpSipServer::process_request(SipStream* strm, Request& req, Response& res)
{  
	bool rv, rv1;

	std::vector<std::string> *headers = NULL;
	rv = read_request_line(strm, req);
#if DEBUG_SHOW_FULL_IN_MSG
	std::vector<std::string> headers1;
	headers = &headers1;
#endif
	rv1 = read_headers(strm, req,headers);
	if (!rv1) {
        // TODO:
		int length;
		const char *pIn = strm->getInBuffer(length);
		MKR_PRINTF("KA.. len=%d\r\n",length);
        return;
    }

	
	int clen = detail::read_content(strm, req, false);
        if (clen<0) {
			MKR_PRINTF("process_request__E2..\r\n");
            // TODO:
            return;
        }
       
#if DEBUG_SHOW_FULL_IN_MSG
		MKR_PRINTF(">>>>Req-line1: %s", req.method.c_str());

		int size = headers->size();
		for (int i = 0; i < size; i++)
		{
			MKR_PRINTF("%s", headers->at(i).c_str());
		}

		//Fill data into SDP part of msg
		std::string cont_type = req.sr.cont_type;
		if (clen > 0 && cont_type == "application/sdp" && !req.body.empty())
		{
			//req.sr.processSdp(req.body);
			MKR_PRINTF("%s", req.body.c_str());
		}
		MKR_PRINTF(">>>>Request END>>>>\r\n");
#endif

		if (get_handler_)
			get_handler_(strm, req, res);

		assert(res.status != -1);

		if (400 <= res.status && error_handler_) {
			error_handler_(strm, req, res);
		}
}

 void UdpSipServer::sendResponse(SipStream* strm, struct sockaddr_in *pDestAddr, std::stringstream& ss)
{
	detail::write_response(strm, pDestAddr, ss);
}

#endif
#if USE_TCP_SIP_TRANSPORT
	// Socket stream implementation
	TcpSocketStream::TcpSocketStream(socket_t sock) : sock_(sock)
	{
	}

	TcpSocketStream::~TcpSocketStream()
	{
	}

	int TcpSocketStream::read(char* ptr, size_t size)
	{
		 int rv= recv(sock_, ptr, size, 0);
		 if(rv>0)
			bytesInBuffer += rv;
		 return rv;
	}

	int TcpSocketStream::write(const char* ptr, size_t size)
	{
		int res = ::send(sock_, ptr, size,0);
#ifdef WIN32
		if (res == SOCKET_ERROR) {
			MKR_PRINTF("sendto failed with error: %d\n", WSAGetLastError());
		}
#else
		if (res == EMSGSIZE || res != size)
		{
			MKR_PRINTF("linux sendto failed  %d : %d\n", size, res);
		}
#endif

		//return send(sock_, ptr, size, 0);
		return res;
	}


	// HTTP server implementation
	TcpSipServer::TcpSipServer()
		: svr_sock_(-1)
	{
#ifndef _MSC_VER
		signal(SIGPIPE, SIG_IGN);
#endif
	}

	TcpSipServer::~TcpSipServer()
	{
	}

	void TcpSipServer::get(const char* pattern, Handler handler)
	{
		//get_handlers_.push_back(std::make_pair(std::regex(pattern), handler));
		get_handler_ = handler;
	}


	void TcpSipServer::set_error_handler(Handler handler)
	{
		error_handler_ = handler;
	}

	void TcpSipServer::set_logger(Logger logger)
	{
		//  logger_ = logger;
	}

	//===============================================================
//#define MAXLINE 2048*2
	bool TcpSipServer::listen(const char* host, int port, int *terminate, int socket_flags)
	{
		
		struct sockaddr_in servaddr;

		// Creating socket file descriptor 
		if ((this->svr_sock_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket creation failed");
			exit(EXIT_FAILURE);
		}

		memset(&servaddr, 0, sizeof(servaddr));
		
		// Filling server information 
		servaddr.sin_family = AF_INET; // IPv4 
		servaddr.sin_addr.s_addr = INADDR_ANY;
		servaddr.sin_port = htons(port);
#ifndef WIN32
		int reuse = 1;
		if (::setsockopt(this->svr_sock_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
			MKR_PRINTF("setsockopt(SO_REUSEADDR) failed");
		if (::setsockopt(this->svr_sock_, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
			MKR_PRINTF("setsockopt(SO_REUSEPORT) failed");

		// where socketfd is the socket you want to make non-blocking
		//int status = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);

		//if (status == -1) {
		//	MKR_PRINTF("NonBlock calling fcntl..");
		//	// handle the error.  By the way, I've never seen fcntl fail in this way
		//}
#endif
		// Bind the socket with the server address 
		if (::bind(this->svr_sock_, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}

		// Now server is ready to listen and verification 
		if ((::listen(this->svr_sock_, 5)) != 0) {
			MKR_PRINTF("Listen failed...\n");
			exit(EXIT_FAILURE);
		}
		else
			MKR_PRINTF("Server listening..\n");
		

		while (*terminate == 1)
		{
			
			// Accept the data packet from client and verification 
			struct sockaddr_in cliaddr;
			memset(&cliaddr, 0, sizeof(cliaddr));
			int len = sizeof(cliaddr);
			int connfd = ::accept(this->svr_sock_, (sockaddr*)&cliaddr,(socklen_t*) &len);
			if (connfd < 0) {
				//int err = errno;
				//if (err == EAGAIN || err == EWOULDBLOCK)
				//{
				//	//MKR_PRINTF("=");
				//	usleep(20 * 1000);
				//}
				//else 
				{
					MKR_PRINTF("server acccept failed...\n");
					//exit(0);
					continue;
				}
			}
			else 
			{
				//MKR_PRINTF("server acccept the client...\n");

				TcpSocketStream *streamAsParam = new TcpSocketStream(connfd);
				streamAsParam->setClientAddr((SOCKADDR *)&cliaddr, sizeof(struct sockaddr_in));
				streamAsParam->setServerPtr(this);
				std::thread t2([](void *ptrToStream) {

					/*TcpSocketStream strm(connfd);

					strm.setClientAddr((SOCKADDR *)&cliaddr, sizeof(struct sockaddr_in));*/
					//MKR_PRINTF("process_request n=%d\r\n",n);

					TcpSocketStream *strm = (TcpSocketStream *)ptrToStream;
					if (strm==NULL)
						return;
					TcpSipServer *_this = (TcpSipServer *)strm->getServerPtr();
					_this->insert2ActConn(strm);
					for (;;)
					{
						Request req;
						Response res;
						strm->setInBuffer(NULL, 0);
						try {
							_this->process_request((SipStream*)strm, req, res);
						}
						catch (std::exception &ex)
						{
							MKR_PRINTF("request session EXC..%s\r\n", ex.what());
							break;
						}

					}
					_this->removeActConn(strm);
					detail::close_socket(strm->getSock());
					delete strm;

				}, streamAsParam);
				t2.detach();

			}
		}//while (*terminate == 1)

		detail::close_socket(this->svr_sock_);
		MKR_PRINTF("The server shutdown()..\r\n");

		return true;
	}

	//=============test sip request================
	bool TcpSipServer::test_sip_request()
	{
		//std::string invite = //"SIP/2.0 200 OK\r\n"
		//					  "INVITE sip:bob@biloxi.example.com SIP/2.0\r\n"
		//					  "Via: SIP/2.0/ TCP client.biloxi.example.com:5060;branch=z9hG4bKnashds7\r\n"
	 //                        "From: Bob <sip:bob@biloxi.example.com>;tag=8321234356\r\n"
	 //                        "To: Alice <sip:alice@atlanta.example.com>;tag=9fxced76sl\r\n"
	 //                        "Call-ID: 3848276298220188511@atlanta.example.com\r\n"
	 //                        "CSeq: 1 BYE\r\n"
		//	                 "Content-Type: application/sdp\r\n"
	 //                        "Content-Length: 151\r\n"
		//	                "\r\n"
		//	        		"v=0\r\n"
		//	                "o=alice 2890844526 2890844526 IN IP4 client.atlanta.example.com\r\n"
		//					"s=-\r\n"
		//					"c=IN IP4 192.0.2.101\r\n"
		//					"t=0 0\r\n"
		//					"m=audio 49172 RTP/AVP 0\r\n"
		//	                "a=rtpmap:0 PCMU/8000\r\n"
		//	;

		std::string invite = "SIP/2.0 200 OK\r\n"
			"Via: SIP/2.0/ TCP client.biloxi.example.com:5060;branch=z9hG4bKnashds7\r\n"
			"Via: SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74b43\r\n"
			"From: Bob <sip:bob@biloxi.example.com>;tag=8321234356\r\n"
			"To: Alice <sip:alice@atlanta.example.com>;tag=9fxced76sl\r\n"
			"Call-ID: 3848276298220188511@atlanta.example.com\r\n"
			"CSeq: 1 BYE\r\n"
			"Content-Length: 0\r\n"
			;

		TcpSocketStream strm(-1);
		strm.setInBuffer(invite.c_str(), invite.size());
		//strm.setClientAddr((SOCKADDR *)&cliaddr, len);
		Request req;
		Response res;
		process_request((SipStream*)&strm, req, res);

		return true;
	}

	void TcpSipServer::stop()
	{
		detail::shutdown_socket(svr_sock_);
		detail::close_socket(svr_sock_);
		svr_sock_ = -1;
	}

	bool TcpSipServer::read_request_line(SipStream* strm, Request& req)
	{
		const auto BUFSIZ_REQUESTLINE = 2048;
		char buf[BUFSIZ_REQUESTLINE];
		if (!detail::socket_readln(strm, buf, BUFSIZ_REQUESTLINE)) {
			return false;
		}
	
		req.method = std::string(buf);
		//req.path = "";
		return true;

	}
	bool TcpSipServer::read_headers(SipStream* strm, Request& req, std::vector<std::string> *headers)
	{

		/*static*/ std::regex re("(.+?): (.+?)\r\n");

		const auto BUFSIZ_HEADER = 2048;
		char buf[BUFSIZ_HEADER];
		int numberRead = 0;
		for (;;) {
			if (!detail::socket_readln(strm, buf, BUFSIZ_HEADER)) {
				goto __exit;
			}
			if (!strcmp(buf, "\r\n")) {
				break;
			}
			std::cmatch m;
			//MKR_PRINTF("%s", buf);             // debug header
			if (headers)
				headers->push_back(buf);
			if (std::regex_match(buf, m, re)) {
				auto key = std::string(m[1]);
				auto val = std::string(m[2]);
#if		!USE_SHORT_NAMES
				if (key == "Via")
#else
				if (key == "v")
#endif
				{
					req.sr.via.push_back(val);
					numberRead++;
				}
				else {
					//req.headers.insert(std::make_pair(key, val));
					req.sr.processKeyVal(key, val);
					numberRead++;
				}
			}
		}
	__exit:
		return numberRead > 0;

	}
	void TcpSipServer::process_request(SipStream* strm, Request& req, Response& res)
	{
		bool rv, rv1;

		std::vector<std::string> *headers = NULL;
		rv = read_request_line(strm, req);
		if (req.method == "\r\n" || req.method.size() == 2)
		{
			MKR_PRINTF("KA_0.. len=%02x %02x\r\n", (char)req.method[0], (char)req.method[1]);
			return;
		}
#if DEBUG_SHOW_FULL_IN_MSG
		std::vector<std::string> headers1;
		headers = &headers1;
#endif
		rv1 = read_headers(strm, req, headers);
		if (!rv1) {
			// TODO:
			int length;
			const char *pIn = strm->getInBuffer(length);
			MKR_PRINTF("KA.. len=%d\r\n", length);
			return;
		}


		int clen = detail::read_content(strm, req, false);
		if (clen < 0) {
			MKR_PRINTF("process_request__E2..\r\n");
			// TODO:
			return;
		}

#if DEBUG_SHOW_FULL_IN_MSG
		MKR_PRINTF(">>>>Req-line1: %s", req.method.c_str());

		int size = headers->size();
		for (int i = 0; i < size; i++)
		{
			MKR_PRINTF("%s", headers->at(i).c_str());
		}

		//Fill data into SDP part of msg
		std::string cont_type = req.sr.cont_type;
		if (clen > 0 && cont_type == "application/sdp" && !req.body.empty())
		{
			//req.sr.processSdp(req.body);
			MKR_PRINTF("%s", req.body.c_str());
		}
		MKR_PRINTF(">>>>Request END>>>>\r\n");
#endif

		if (get_handler_)
			get_handler_(strm, req, res);

		assert(res.status != -1);

		if (400 <= res.status && error_handler_) {
			error_handler_(strm, req, res);
		}
	}

	void TcpSipServer::sendResponse(SipStream* strm, struct sockaddr_in *pAddr, std::stringstream& ss)
	{
		//detail::write_response(strm, pDestAddr, ss);
		SOCKADDR *pDestAddr = NULL;
		std::string s = ss.str();
		int nBytesToSend = s.size(),sent=0;
		if (pAddr == NULL)
		{
			std::lock_guard<std::mutex> lock(send_mutex);
			sent=strm->write(s.c_str(), nBytesToSend);
			pDestAddr=strm->getClientAddr();
			
		}
		else 
		{
			std::lock_guard<std::mutex> lock(send_mutex); // to synchronize access to conn_list
			//find SipStream with a given pAddr
			for (ActiveConnection &x : conn_list)
			{
				SipStream* wsi1 = (SipStream*)x.wsi;
				if (wsi1)
				{
					struct sockaddr_in *pAddr1 = (struct sockaddr_in *) wsi1->getClientAddr();
					if (compareAddresses(pAddr1, pAddr))
					{
						int sockfd = wsi1->getSock();
						MKR_PRINTF("sendResponse_0: %d  wsi sock=%d\r\n", x.data_to_send.size(), sockfd);
						sent=wsi1->write(s.c_str(), nBytesToSend);
						pDestAddr =(SOCKADDR *) pAddr1;
						break;
					}
				}
			}
		}

		if (pDestAddr) {
			struct sockaddr_in *pAddr1 = (struct sockaddr_in *)pDestAddr;
			char *ip = inet_ntoa(pAddr1->sin_addr);
			MKR_PRINTF(">>Res>> to %s:%d length=%d\r\n", ip, ntohs(pAddr1->sin_port), nBytesToSend);
			if (sent != nBytesToSend)
				MKR_PRINTF("strm->write_0 error: %d  %d\r\n", sent, nBytesToSend);
		}
	}

#endif  //USE_TCP_SIP_TRANSPORT