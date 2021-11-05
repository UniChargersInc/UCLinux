#include "Dbg.h"


//--------------------------------------------------------
#if 0
void sk_log(int level, const char *source,int line, const char *format, ...)
{

	va_list ap;

	va_start(ap, format);
	char msg[1024];// = { 0 };
	vsprintf(msg, format, ap);  //The buffer should be large enough to contain the resulting string.

#if ANDROID      
								//LOGI(str);
	LOGI("%s line=%d %s", source, line, msg);
#else
								/*char str[256];
								sprintf(str, "%s line=%d %s", source, line, msg);
								OutputDebugString(str);*/

	printf("%s line:%d %s\r\n",source,line, msg);

#endif
	va_end(ap);


}
#endif
//Log file impl
void sk_log(int level, const char *tag, const char *format, ...)
{
#if 1
	va_list ap;

	va_start(ap, format);
	char msg[1024];// = { 0 };
	vsprintf(msg, format, ap);  //The buffer should be large enough to contain the resulting string.

#if ANDROID      
	//LOGI(str);
	LOGI("%s  %s", tag, msg);
#else
	/*char str[256];
	sprintf(str, "%s line=%d %s", source, line, msg);
	OutputDebugString(str);*/

	printf( "%s\r\n", msg);

#endif
	va_end(ap);
#endif

}
//--------------------------------------------------------
void error(const char *msg)
{
	fprintf(stderr, msg);
}

void m_printf(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char msg[1024] ;  //={0} compiler must insert memset (msg,0,1024)
	vsprintf(msg, format, ap);
#if SK_BUILD_FOR_WINDOWS
	char szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugString(szBuff);
#elif ANDROID 
	LOGI(" %s",  msg);
#elif SK_BUILD_FOR_IPHONE
	fprintf(stderr, " %s", msg);
#endif

	va_end(ap);
}


void printDump(unsigned char* buf, int len){

	int ind = 0;
	while (len>0){
		m_printf("%02X,", buf[ind]);
		ind++;
		if (!(ind % 40))
			m_printf("\r\n");
		len--;
	}
	m_printf("\r\n");
}

//==================Log File================================
