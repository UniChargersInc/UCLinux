// W_VoIP_Sip_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <stdarg.h>


#include <string>

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#ifdef WIN32
#include <sys/timeb.h>
#else
#include<sys/time.h>
#endif
#include <stdint.h>

#define RUN_WS_TEST_ONLY	0

std::string currentTime_ToString()
{
#ifdef WIN32
	struct _timeb tt;
#ifdef _INC_TIME_INL
	_ftime_s(&t);
#else
	_ftime(&tt);
#endif
#else
	/*struct timeval tv;
	gettimeofday(&tv, nullptr);
	return ((uint64_t)tv.tv_sec) * 1000 + tv.tv_usec / 1000;*/
#endif
	/*uint64_t val = tt.time;
	char buf[32] = { 0 };
	int h = val / 3600;
	int m = (val - (h * 3600)) / 60;
	int s = val - (h * 3600) - (m * 60);
	sprintf(buf, "%d:%d:%d.%d", h, m, s, tt.millitm);
	return buf;*/

	time_t     now = time(0);
	struct tm  *tstruct = localtime(&now);
	char       buf[48] = { 0 };
	//strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tstruct);
	strftime(buf, sizeof(buf), "%H:%M:%S", tstruct);

	return buf;

}


//https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
static std::string getexepath()
{
	std::string path = "";
#if 1	
	//WCHAR result[MAX_PATH];
	//std::string str= std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
	//std::size_t found = str.find_last_of("/\\");
	//if (found>0&&found<str.size())
	//{
	//	path = str.substr(0, found);
	//	//filename= str.substr(found+1)
	//}
	//else {

	//}
#ifdef WIN32
	path= "C:\\my\\projects";
#else
#endif
	return path;

#else
	char cCurrentPath[MAX_PATH];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		return "";
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

	return std::string(cCurrentPath);
#endif
}


extern "C" int main_pjturn_srv(int *terminate);
#ifdef WIN32
static std::string log_path = "win_sip_server_log.txt";
#else
static std::string log_path = "linux_sip_server_log.txt";
#endif
static std::mutex g_i_mutex;
static bool initialized = false;

bool init_log_file()
{
	std::string path=getexepath();
	if (!path.empty())
	{
		log_path = path + "\\" + log_path;
	}
	return true;
}

static inline void to_log_file(std::string str) {


	std::ofstream myfile;
	myfile.open(log_path.c_str(), std::ios::out | std::ios::app);
	myfile << str;  // << std::endl;
	//myfile.close();  //No need to close the file manually, as it does so upon destruction. 

}
extern "C" void write_to_log_file(int level,const char *data, int length)
{
	if (!initialized)
	{
		initialized=init_log_file();
	}

	int   fd;
	FILE* pFile = fopen(log_path.c_str(), "a+b");
	std::string str=currentTime_ToString();
	std::string dataStr = length == 0 ? std::string(data) : std::string(data, length);
	str = str + "  " + dataStr;
	fprintf(pFile, "%s", /*data*/str.c_str());
	fclose(pFile);

	/*
	if ( 1)
	{
		fd = open(log_path, O_CREAT + O_WRONLY + O_APPEND, 0666);
		if (fd >= 0) {

			write(fd, data, length);
			close(fd);
		}
	}*/

}

void m_printf(const char *format, ...)
{
#if USE_PTHRED_MUTEX
	if (!initialized)
		return;
	pthread_mutex_lock(&lockMutex);
#else
	std::lock_guard<std::mutex> lock(g_i_mutex);
#endif
	
	va_list ap;
	va_start(ap, format);
	char msg[2048*2];// = { 0 };
	vsnprintf(msg, 2048*2, format, ap);  //The buffer should be large enough to contain the resulting string.

	write_to_log_file(0,msg, 0);//to_log_file(msg);
#ifdef WIN32	
	printf(msg);   // printf gives an error when msg contains '%' character !!!
#else
	printf(msg);
#endif
	va_end(ap);
#if USE_PTHRED_MUTEX
	pthread_mutex_unlock(&lockMutex);
#endif


}

extern "C" int main_run = -1;
static int terminate_sip_loop_thread = 0;
static int terminate_turn_loop_thread=0;
static int terminate_websockets_loop_thread = 0;

#ifdef WIN32
extern "C" void service_uninstall(void);
extern "C" void service_install(void);
extern "C" void service_run(void);
void stop_sip_listener();

static VOID CALLBACK  TimerFunction(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	if (main_run == 0)
	{
		stop_sip_listener();
		main_run == 2;
	}
}

void stop_sip_listener()
{
	terminate_sip_loop_thread = 0;
	terminate_turn_loop_thread = 0;
	terminate_websockets_loop_thread = 0;
}
#endif

int start_mkr_voip_server(int *terminate);
int main_ws_sip(int *terminate_websockets_loop_thread);

int main(int argc, char *argv[])
{
#if defined(WIN32) 
	if (argc == 2) {
		if (!strcmp(argv[1], "run")) {
			service_run();
			return 0;
		}
		else if (!strcmp(argv[1], "install")) {
			service_install();
			return 0;
		}
		else if (!strcmp(argv[1], "uninstall")) {
			service_uninstall();
			return 0;
		}
	}
#endif

	main_run = 1;
#ifdef WIN32
	HANDLE timer_handle_ = NULL;
	if (/*argc == 3*/1) // this program was started by Windows SC ( service controller)
	{
		// the main_run can be changed from SC when we stop the service !!!
		CreateTimerQueueTimer(&timer_handle_, NULL, TimerFunction, /*user_object_ptr*/&main_run, 1000, 1000, WT_EXECUTEDEFAULT);

	}
#endif
#if RUN_WS_TEST_ONLY!=1
	std::thread t1([&](/*int sockfd*/) {
		terminate_turn_loop_thread = 1;
		int rv = main_pjturn_srv(&terminate_turn_loop_thread);
	});
	t1.detach();
#endif
#ifndef WIN32
	// do not run websockets on windows test server since we have no SSL certificates on it !!!
	std::thread t2([&](/*int sockfd*/) {
		terminate_websockets_loop_thread = 1;
		main_ws_sip(&terminate_websockets_loop_thread);
		});
	t2.detach();
#endif
#if RUN_WS_TEST_ONLY==1
	//for test
	while (true)
	{
#ifdef WIN32
		Sleep(1900);
#else
		usleep(1000 * 1000);
#endif
	}
#else
	terminate_sip_loop_thread =1;
	start_mkr_voip_server(&terminate_sip_loop_thread);
#endif
#ifdef WIN32
	if (timer_handle_ != NULL && timer_handle_ != INVALID_HANDLE_VALUE)
	{
		DeleteTimerQueueTimer(NULL, timer_handle_, NULL);
		timer_handle_ = NULL;
	}
#endif

	return 1;
}

