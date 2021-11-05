#ifndef MKR_DBG_H_INCLUDED
#define MKR_DBG_H_INCLUDED



#if WIN32
#include <Windows.h>
#endif
#include <iostream>
#include <sstream>


//---------------------------------------
#if ANDROID
#include <jni.h>
#include <android/log.h>
#endif

#define strncpy_safe(dest, src, size) { strncpy ((dest), (src), (size)); (dest)[(size)-1] = '\0'; }

#define SK_MESSAGE_DEBUG      1
#define SK_MESSAGE_INFO       2
#define SK_MESSAGE_WARNING    3
#define SK_MESSAGE_ERROR      4
#define SK_MESSAGE_FATAL      5

#if defined (ANDROID) || defined (IPHONE) || defined(GTK)
//#define __FILE_NAME__      (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
//#define __FILE_NAME__  "filename"
#define __FILE_NAME__      (strrchr(__FILE__, '/') ? (char*)(strrchr(__FILE__, '/') + 1) :(char*) __FILE__)
#else
#define __FILE_NAME__      __FILE__
#endif

#define SK_DEBUG   SK_MESSAGE_DEBUG,__FILE_NAME__,__LINE__
#define SK_INFO    SK_MESSAGE_INFO,__FILE_NAME__,__LINE__
#define SK_WARNING SK_MESSAGE_WARNING,__FILE_NAME__,__LINE__
#define SK_ERROR   SK_MESSAGE_ERROR,__FILE_NAME__,__LINE__
#define SK_FATAL   SK_MESSAGE_FATAL,__FILE_NAME__,__LINE__

void sk_log(int level, const char *source, const char *format, ...);
//void sk_log(int level, const char *source,int line, const char *format, ...);
void sk_log_to_file(const char *format, ...);

#define flcnav_log_print
#if ANDROID
#define  LOG_TAG    "mkr_ud"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOG_INFO LOGI
#define LOG_ERROR	LOGE
#else
#define  LOG_TAG    "mkr_sim"
#define  LOGI(...)   sk_log_to_file(__VA_ARGS__)  //sk_log(1,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)   sk_log_to_file(__VA_ARGS__)  //sk_log(2,LOG_TAG,__VA_ARGS__)
#define LOG_INFO LOGI
#define LOG_ERROR	LOGE
#endif



void sk_check_allocated_with_source_line
(const char *source, int line, const void *allocated);

#define sk_check_allocated(p) \
            sk_check_allocated_with_source_line(__FILE__,__LINE__,p)


//--------------------------------------

void m_printf(const char* szFormat, ...);
void printDump(unsigned char* buf, int len);
void error(const char *msg);

void write_to_log_file(void *data, int length);
void sk_log_to_file(const char *format, ...);
#endif