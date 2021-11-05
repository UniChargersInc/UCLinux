#ifndef LOGFILE_H
#define LOGFILE_H
#include <iostream>
#include <fstream>
#include <sstream>

#include <fcntl.h>

#include <stdarg.h>

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

int to_log_file(string str);
int read_entire_log_file(char * memblock, int maxsize);
void delete_log_file();

void /*sk_log_to_file*/m_printf(const char *format, ...);
char* read_html_file(char *full_path);
unsigned char* read_bin_file(char *full_path,int &length);

#endif