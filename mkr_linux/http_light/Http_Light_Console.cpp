/*
**BIND Error 199.243.91.4 80 errno=98...
https://handsonnetworkprogramming.com/articles/bind-error-98-eaddrinuse-10048-wsaeaddrinuse-address-already-in-use/#:~:text=If%20bind()%20causes%20error,start%20it%20a%20second%20time.

If the error returned by bind() is 98 (Linux) or 10048 (Windows), then it means the local port you are trying to bind to is already in use.

2. YOUR PROGRAM IS ALREADY RUNNING
This is more common than you think. Is your program already running? If it is, then of course the port is already in use!
This also isn't always as easy to determine as you might like. In the server world, it's very common for programs to run in the background or as daemons. 
In any case, if you get a bind() error or address already in use error, be sure to check that your program isn't already running.

3. TCP LINGER / TIME-WAIT
I've saved the most complicated scenario for last. Suppose your server program is running, and then you terminate it (or it crashes). When you restart it, you get the address in use error (EADDRINUSE or WSAEADDRINUSE). However, if you wait a couple minutes, you are able to start your program without error. What is going on here?

I won't go into too much depth here (read my book for that), but basically when a socket is closed it can go into a TIME-WAIT state on the end that initiated the close. This isn't a problem for a client program, since it doesn't use bind() anyway (typically). However, this TIME-WAIT state can be a problem for server applications. It means the operating system is still keeping track of the closed socket, even after your entire program is closed. This is enough to cause the "Address already in use" error if you restart your program.

Now, there is an easy way to prevent this error. You can use the SO_REUSEADDR socket option before calling bind(). It looks like this:

int yes = 1;
if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR,
						(void*)&yes, sizeof(yes)) < 0) {
	fprintf(stderr, "setsockopt() failed. Error: %d\n", GETSOCKETERRNO());
}
This option will allow bind() to succeed even if a few TIME-WAIT connections are still around. There is very little downside to using the SO_REUSEADDR, so I suggest most server applications use it.

In any case, check out Chapter 13 of my book for more information.

SUMMARY
If bind() causes error 98 (EADDRINUSE) or 10048 (WSAEADDRINUSE), it means you are trying to bind to a local port that is already in use. This is usually caused by one of three reasons:

Another program is already using that port.
Your program is already running and you've attempted to start it a second time.
Your program was running recently and some connections are still lingering around in the half-dead TIME-WAIT state.
Cause 1 can be fixed by terminating the program using your port, or having your program use a different port.

Cause 2 can be fixed by only running one instance of your program at a time.

Cause 3 can be fixed by waiting a few minutes or using the SO_REUSEADDR socket option.


*/

/*
[2021-08-26 19:58:51] [info] asio listen error: system:98 (Address already in use)

*/


#include <memory>
#include <cstdint>
#include <iostream>
#include <thread>
#include <mutex>

#include "mqttc/ClsMqtt.h"
#include "tcplistener/TcpListener.h"

#include <MkrLib.h>
#include "pdf_report.h"
#include "event.h"
#include "udorx_server_config.h"
#include "libevhtp_server.h"

//#include <openssl/ssl.h>
//class SSLInit {
//public:
//	SSLInit() {
//		SSL_load_error_strings();
//		SSL_library_init();
//	}
//};
//
//static SSLInit sslinit_;

using namespace std;


//#if !USE_LIBEVENT
int stop_mkr_no_ssl_server(void);
int start_mkr_no_ssl_server(void *terminate);
//#endif
bool is_tcp_listener_stopped();

void stop_ocpp_server();
int main_ocpp_server_start();
int main_sig_setup();
//int stop_libevhtp_main();
int main_proto_conversion_test();

volatile int http_light_main_run = 0;
void stop_http_light_main_run()
{
	http_light_main_run = 0;
	stop_tcp_listener();
	stop_ocpp_server();
	stop_mkr_no_ssl_server();
	ClsMqtt::Get()->stopMqtt();
	evhtp::stop_libevhtp_main();

	usleep(1000 * 1000);
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
	MKR_DBG_PRINTF("handle_sigint %d ..\r\n", signal);
	http_light_main_run = 0;
#if !USE_LIBEVENT	
	stop_mkr_server();
#endif
}
/* Signal handler for SIGHUP - flag a config reload. */
void handle_sighup(int signal)
{
	//flag_reload = true;
	MKR_DBG_PRINTF("sighup %d ..\r\n", signal);
}
/* Signal handler for SIGUSR1 - backup the db. */
void handle_sigusr1(int signal)
{
#ifdef WITH_PERSISTENCE
	flag_db_backup = true;
#endif
	MKR_DBG_PRINTF("sigusr1 %d ..\r\n", signal);
}

/* Signal handler for SIGUSR2 - vacuum the db. */
void handle_sigusr2(int signal)
{
	//flag_tree_print = true;
	MKR_DBG_PRINTF("sigusr2 %d ..\r\n", signal);
}
void handle_sigsegv(int signal)
{
	//flag_tree_print = true;
	MKR_DBG_PRINTF("SIGSEGV %d ..\r\n", signal);
}
#endif
static const char* log_path = "/home/mkr/service/logs/http_light.log";
static const char* log_path_1 = "/home/mkr/service/logs/http_int.log";
static inline void to_log_file_1(const char* msg) {
	ofstream myfile;
	myfile.open(log_path_1, std::ios::out | std::ios::app);
	myfile << msg << endl;
	//myfile.close();  //No need to close the file manually, as it does so upon destruction. 
}
static mutex log_mutex;
void event_log_func(int severity, const char *msg)
{
	// printf("%s\r\n", msg);
	std::lock_guard<std::mutex> lock(log_mutex);
	to_log_file_1(msg);
}
void test_sigv();

#if 0
static void getCSFWVersion()
{
	//char fpath[256];
	//sprintf(fpath, "%s/firmware/%s", UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT, "UC_SmartGrizzle.md5");

	//int read_html_file(char *full_path, string &str);
	if (true)
	{
		string str = "",res="***";
		/*int rv = read_html_file((char*)fpath, str);
		m_printf("getCSFWVersion..rv:%d size:%d str:%s ", rv, str.size(), str.c_str());*/
		if (/*rv && str.size() > 36*/true)
		{
			str = "t////9v////J////v////77////6////7v///9z////J////3P///+7////d////3f///7/////d////FwAAAL7///8gAAAA+v///93////J////yf///7////+/////vv///77////6////+v///7f///+3////2////9v////K////yv///+7////u////";
			res = decr_2(str);
			m_printf("getCSFWVersion_11 res:%s ", res.c_str());
			str = res.substr(8, 4);
			m_printf("getCSFWVersion..res-> str:%s ", str.c_str());
			//set_response_printf(req, str);

			return;
		}
	}

	//set_response_printf(req, "*");
}
#endif


int main()
{
//	getCSFWVersion();
//	return 1;
#if 1
	/*if (!make_fake_invokation_to_build_successfully())
		return 0;*/
	event_set_log_callback(event_log_func);
	
#ifndef WIN32
	main_sig_setup();

#endif

	/*const char *p = "\r\n**Initialize TCPListener ...\r\n";
	char *p1=mkr_my_strdup(p);
	if (!p1)
		return 0;*/
	InitLog(log_path);
	/*m_printf(p1);
	delete p1;*/

	
	MKR_DBG_PRINTF("**Initialize TCPListener.");
	start_tcp_listener();

	m_printf("**Initialize Mqtt Client on server side.");
	ClsMqtt::Get()->doInitMqtt(0, 0);

	main_ocpp_server_start();

	http_light_main_run = 1;

	MKR_DBG_PRINTF("start_http_main.");
#if MKR_USE_SSL	
	start_mkr_no_ssl_server((void*)&http_light_main_run);   // 
#endif
//	test_sigv();
	
	evhtp::start_libevhtp_main();

	/*for (;;)
	{
		if (http_light_main_run == 0)
			break;
		usleep(100 * 1000);
	}*/

	MKR_DBG_PRINTF("-mkr_server terminated.");
	stop_tcp_listener();
	stop_ocpp_server();
	stop_mkr_no_ssl_server();
	ClsMqtt::Get()->stopMqtt();

	int timeout = 0;
	while (1)
	{
		bool rv = is_tcp_listener_stopped();
		if (rv)
			break;
		usleep(100 * 1000);
		timeout++;
		if (timeout > 40)   // wait 4 seconds
			break;
	}
	m_printf("**Http_Light EXIT app.");
	
	return 0;
#endif
}

