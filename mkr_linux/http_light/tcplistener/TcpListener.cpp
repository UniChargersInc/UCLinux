#include "TcpListener.h"
#include "udorx_server_config.h"
#include <thread>  // c++11 thread

#ifdef WIN32
void usleep(int d);
#endif
static int IsRun = 1;

#define IN_BUFFER_SIZE	1024

static int process_connection(int sockfd)
{
	uint8_t buff[IN_BUFFER_SIZE];
	int bytesRead = recv(sockfd,(char*) buff, 4,0);
	int totalLength = Convert::ToInt32(buff, 0);
	//m_printf("TCPL process_connection..total=%d\r\n", totalLength);
	if (totalLength <= 0)
		return -1;
	unsigned char *inBuffer = new unsigned char[totalLength];
	bytesRead = 0;
	//	usleep(200000);
	do
	{
		try
		{
			int bytes = 0;
			if(totalLength >= IN_BUFFER_SIZE)                 //!!!!!!!!!!!!!!!
				bytes = recv(sockfd,(char*) buff, IN_BUFFER_SIZE,0);
			else
				bytes = recv(sockfd, (char*)buff, totalLength,0);
			//m_printf("socket->socket_read = %d ..\r\n", bytes);
			if (bytes <= 0)
			{
				//m_printf("Fatal ERROR bytes=-1 ..!!");  0-when peer drops connection
				delete inBuffer;
				goto __exit__;
			}
			memcpy(inBuffer + bytesRead, buff, bytes);
			bytesRead += bytes;
			totalLength -= bytes;
		}
		catch (/*IOException*/ /*std::bad_alloc &e*/const std::exception& e)
		{
			break;
		}
	} while (totalLength>0);
	//m_printf("TCP process_connection _read=%d bytes..\r\n", bytesRead);

	try {
		process_tcp_listener_request(/*bev*/sockfd, inBuffer, bytesRead);
	}
	catch(std::exception &e) // must be by reference !!! Otherwise e.what() does not show the derived class message !!!
	{
		m_printf("Exception in process_tcp_listener_request.. reason:%s\r\n",e.what());
	}
	catch (...)
	{
		m_printf("Exception in process_tcp_listener_request..\r\n");
	}
	
	delete[] inBuffer;

__exit__:

	
	my_closesocket(sockfd);
	return 1;
}
static int sockfd = 0;
static void *doIt(void *ctx)
{
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		m_printf("ERROR opening TCP listening socket..\r\n");
		IsRun = 2;
		return 0;
	}
	memset(&serv_addr, 0, sizeof(serv_addr));//bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(TCP_LISTENER_IPADDR);//INADDR_ANY;    //
	serv_addr.sin_port = htons(TCP_LISTENER_IPADDR_SERVER_PORT);

#ifndef WIN32
	//You may need to use both SO_REUSEADDR and SO_REUSEPORT          -> it failed to bind after app restart WHY??? 
	const int trueFlag = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0)
	{
		m_printf("TcpListener ERROR on SO_REUSEADDR  listening socket..errn=%d\r\n", (int)errno);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &trueFlag, sizeof(int)) < 0)
	{
		m_printf("TcpListener ERROR on SO_REUSEPORT  listening socket..errn=%d\r\n", (int)errno);
	}
#endif
	int rv = 0;
	if (rv = ::bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		m_printf("ERROR on binding TCP listening socket..rv=%d addr=%s port=%d\r\n",rv, TCP_LISTENER_IPADDR, TCP_LISTENER_IPADDR_SERVER_PORT);
		IsRun = 2;
		return 0;
	}
		
	listen(sockfd, 10);                 // 10 - number of pending connections before rejecting the new one
	
	m_printf("ENTERING TCPL for (; IsRun; )... \n");
	while ( IsRun==1 )
	{
		socklen_t clilen = sizeof(cli_addr);
		int newsockfd = accept(sockfd,	(struct sockaddr *) &cli_addr,	&clilen);
		if (newsockfd > 0)
		{
			//should I spawn a new thread here ??
			//process_connection(newsockfd);
			std::thread t([newsockfd]() {
				//std::cout << "thread function " << newsockfd << "\n";
				process_connection(newsockfd);
				});
			if (t.joinable())
				t.detach();

			//MKR_DBG_PRINTF("Accept TCPL  )... \n");
			usleep(1 * 1000);
		}
	}

	if(sockfd>0)
		my_closesocket(sockfd);
	IsRun = 2;
	return 0;
}

//=============================================================
//static pthread_t pthread;

static std::thread::native_handle_type tcp_thread_native_handle;
void start_tcp_listener()
{

		//if (pthread_create(&pthread, NULL, doIt, 0) != 0)
		//{
		//	m_printf("ERROR MQTT threadProc... \n");//LOGE("ERROR threadProc... \n");

		//										/*if (arg != 0)
		//										{
		//										delete arg;
		//										}*/
		//	

		//}
	
	std::thread tcp_listener_thread = std::thread([]() {
		doIt(0);
	});
	tcp_thread_native_handle = tcp_listener_thread.native_handle();
	if (tcp_listener_thread.joinable())
		tcp_listener_thread.detach();  //the thread to run in the background (daemon)
}
void stop_tcp_listener()
{
	IsRun = 0;
#ifndef WIN32
	pthread_cancel(tcp_thread_native_handle);
#endif
	if (sockfd > 0)
	{
		my_closesocket(sockfd);  // since it might be in a blocking call ( accept )
		sockfd = 0;
	}
	m_printf("stop_tcp_listener..Done.");
}

bool is_tcp_listener_stopped()
{
	return IsRun == 2;
}