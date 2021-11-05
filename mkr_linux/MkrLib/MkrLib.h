#ifndef MKRLIB_H
#define MKRLIB_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <fcntl.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdarg.h>

#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "my_lib/my_lib.h"

typedef char* PSTR;

#ifdef WIN32
#define ssize_t int
#include <io.h>          // for open/write/read/close file
#endif

using namespace std;
void InitLog(const char* _log_path);
void write_to_log_file(void *data, int length);

int write_to_file(string &fname, string &payload);
int write_to_file(const char* fname,const std::stringstream &payload);
int write_to_file_dir_check(string &fname, string &payload);
int ridit();
unsigned char* read_bin_file(const char *full_path, int &length);
int getSizeOfFile(const char *full_path);
//char* read_html_file(char *full_path);
int read_html_file(char *full_path,string &out);
int prcd(std::string &n);
int read_bin_file_to_buf(const char *full_path, unsigned char *pBuf,int max_size=0,int offset=0);

int read_bin_file2buf(char *full_path, unsigned char** buf);
int read_html_file2buf(char *full_path, unsigned char** buf);



int delete_file(const char* path);
bool M_PathFileExists(char *path,bool remove_file_name=false);
bool SplitFilename2Dir(const std::string& full_path, std::string &dir, std::string &filename);
int getDirectoryListing(std::string &dir_path, std::vector<std::string> &v);
std::string decr_2(std::string& str_en);
int M_CreateFile(char *path);
bool M_CopyFile(
	char* lpExistingFileName,
	char* lpNewFileName,
	bool    bFailIfExists
);
bool M_WriteFile(
	 int       hFile,
	unsigned char*      lpBuffer,
	unsigned int       nNumberOfBytesToWrite,
	unsigned int*      lpNumberOfBytesWritten,
	void* lpOverlapped
);
void m_printf(const char *format, ...);

#define LOGE	m_printf

//=======Debug in console app=================
#define MKR_DBG_PRINTF  m_printf
//#define MKR_DBG_PRINTF printf

#endif
