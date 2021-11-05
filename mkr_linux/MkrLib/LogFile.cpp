
#include "MkrLib.h"

#define USE_PTHRED_MUTEX	0

#if USE_PTHRED_MUTEX
#include <pthread.h>
#else
#include <thread>
#include <mutex>
#endif


using namespace std;
//static const string logs_path = "/home/mkr/service/logs/";
//static const string log_file_name = "http_light.log";
#if MKR_LINUX
static  string log_path;// = "/home/mkr/service/logs/http_light.log";
#elif WIN32
static  string log_path;// = "C:\\my\\linux\\logs\\http_light.log";
#endif
#if USE_PTHRED_MUTEX
static bool initialized = false;
static pthread_mutex_t lockMutex;
#else
static mutex g_i_mutex;  
#endif


void InitLog(const char* _log_path)
{
	log_path = string(_log_path);
#if USE_PTHRED_MUTEX
	if (!initialized)
	{
		if (pthread_mutex_init(&lockMutex, NULL) != 0)
		{
			//printf("\n mutex init failed\n");
			return ;
		}
	}
	
	initialized = true;
#endif	
}


static inline void to_log_file(string str) {
	
	
	ofstream myfile;
	myfile.open(log_path.c_str(), std::ios::out | std::ios::app);
	myfile << str << endl;
	//myfile.close();  //No need to close the file manually, as it does so upon destruction. 
	
}

void write_to_log_file(void *data, int length)
{
	//int   fd;
	//
	//if (/*log_initialized ==*/ 1)
	//{
	//	fd = open(log_path, O_CREAT + O_WRONLY + O_APPEND, 0666);
	//	if (fd >= 0) {

	//		write(fd, data, length);
	//		close(fd);
	//	}
	//}

}

//===========================================================
#if 0
//#define m_printf printf
void WriteLogFile(const char* szString)
{
	FILE* pFile = fopen("logFile.txt", "a");
	fprintf(pFile, "%s\n", szString);
	fclose(pFile);
}
#endif
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
	char msg[2048];// = { 0 };
	vsnprintf(msg,2048, format, ap);  //The buffer should be large enough to contain the resulting string.

	to_log_file(msg);
	//printf(msg);         // for debug only !!! remove it
#ifdef WIN32	
	printf(msg);   // printf gives an error when mag contains '%' character !!!
#endif
	va_end(ap);
#if USE_PTHRED_MUTEX
	pthread_mutex_unlock(&lockMutex);
#endif
}
