#include "TcpListener.h"
#include "udorx_server_config.h"
#include <thread>  // c++11 thread

#include "CommonUtils/my_utils.h"

#define tcp_voip_listener_IPADDR "192.168.0.20"
#define tcp_voip_listener_IPADDR_SERVER_PORT	5060

#ifdef WIN32
void usleep(int d);
#endif
static int IsRun = 1;

#define IN_BUFFER_SIZE	1024

static void sendResponse(int sockfd, string& resp)
{
	int len = resp.size();
	
	int flags = 0;
#ifndef WIN32
	flags = MSG_NOSIGNAL;
#endif
	send(sockfd, resp.c_str(), len, flags);
}


int process_sip_message(vector<string> &v)
{
	int rv = 0;
	if (v.size() > 0)
	{
		for (auto &v1 : v)
		{
			printf("%s\n", v1.c_str());
		}
		vector<string> v1=split(v[0],' ');
		if (v1.size() > 0)
		{
			string cmdS = v1[0];
			if (cmdS == "REGISTER")
			{
				// or <401 Unauthorized> if authorization is required  !!!
				rv = 200; // OK
			}
		}
	}
	return rv;
}

static int process_connection(int sockfd)
{
	
	printf("TCP Voip process_connection..\r\n" );
	
	unsigned char buff[IN_BUFFER_SIZE];
	//	usleep(200000);
	for(;;)
	{
		try
		{
						
			int bytes = recv(sockfd,(char*) buff, IN_BUFFER_SIZE,0);
			if (bytes <= 0)
			{
				printf("VoIP ERROR bytes=-1 ..conn dropped\r\n"); // 0-when peer drops connection
				goto __exit__;
			}
			//printf("TCP VoIP process_connection _read=%d bytes..\r\n", bytes);
			string str((const char*)buff, bytes);
			
			vector<string> v=split(str,'\n');
			int rv = process_sip_message(v);
			char buff[100];
			if (rv == 200) {
				string resp = "200 OK\r\n";
				printf("sending : %s", resp.c_str());
				sendResponse(sockfd, resp);
			}
		}
		catch (/*IOException*/ /*std::bad_alloc &e*/const std::exception& e)
		{
			break;
		}
	}
	
	try {
		//process_tcp_voip_listener_request(/*bev*/sockfd, inBuffer, bytesRead);
	}
	catch(std::exception &e) // must be by reference !!! Otherwise e.what() does not show the derived class message !!!
	{
		printf("Exception in process_tcp_voip_listener_request.. reason:%s\r\n",e.what());
	}
	catch (...)
	{
		printf("Exception in process_tcp_voip_listener_request..\r\n");
	}
	
	

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
		printf("ERROR opening TCP Voip listening socket..\r\n");
		IsRun = 2;
		return 0;
	}
	memset(&serv_addr, 0, sizeof(serv_addr));//bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(tcp_voip_listener_IPADDR);//INADDR_ANY;    //
	serv_addr.sin_port = htons(tcp_voip_listener_IPADDR_SERVER_PORT);

#ifndef WIN32
	//You may need to use both SO_REUSEADDR and SO_REUSEPORT          -> it failed to bind after app restart WHY??? 
	const int trueFlag = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0)
	{
		printf("TcpListener ERROR on SO_REUSEADDR  listening socket..errn=%d\r\n", (int)errno);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &trueFlag, sizeof(int)) < 0)
	{
		printf("TcpListener ERROR on SO_REUSEPORT  listening socket..errn=%d\r\n", (int)errno);
	}
#endif
	int rv = 0;
	if (rv = ::bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR on binding TCP listening socket..rv=%d addr=%s port=%d\r\n",rv, tcp_voip_listener_IPADDR, tcp_voip_listener_IPADDR_SERVER_PORT);
		IsRun = 2;
		return 0;
	}
		
	listen(sockfd, 10);                 // 10 - number of pending connections before rejecting the new one
	
	printf("ENTERING TCP VOIP for (; IsRun; )... \n");
	while ( IsRun==1 )
	{
		socklen_t clilen = sizeof(cli_addr);
		int newsockfd = accept(sockfd,	(struct sockaddr *) &cli_addr,	&clilen);
		//should I spawn a new thread here ??
		//process_connection(newsockfd);
		std::thread t([newsockfd]() {
			//std::cout << "thread function " << newsockfd << "\n";
			process_connection(newsockfd); 
		});
		if (t.joinable())
			t.detach();
		
		MKR_DBG_PRINTF("Accept TCP Voip  )... \n");
		usleep(1 * 1000);
	}

	my_closesocket(sockfd);
	IsRun = 2;
	return 0;
}

//=============================================================
//static pthread_t pthread;
static std::thread tcp_voip_listener_thread;
void start_tcp_voip_listener()
{

		//if (pthread_create(&pthread, NULL, doIt, 0) != 0)
		//{
		//	printf("ERROR MQTT threadProc... \n");//LOGE("ERROR threadProc... \n");

		//										/*if (arg != 0)
		//										{
		//										delete arg;
		//										}*/
		//	

		//}
	
	tcp_voip_listener_thread = std::thread([]() {
		doIt(0);
	});
	if (tcp_voip_listener_thread.joinable())
		tcp_voip_listener_thread.detach();  //the thread to run in the background (daemon)
}
void stop_tcp_voip_listener()
{
	IsRun = 0;
	my_closesocket(sockfd);  // since it might be in a blocking call ( accept )
}

bool is_tcp_voip_listener_stopped()
{
	return IsRun == 2;
}
