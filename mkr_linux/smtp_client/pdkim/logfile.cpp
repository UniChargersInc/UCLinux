#include "logfile.h"



static const char *log_path = "dkim.log";
int to_log_file(string str) {
	//printf(str.c_str());
	ofstream myfile;
	myfile.open(log_path, std::ios::out | ios::binary | std::ios::app);
	myfile << str;// << endl;
	//myfile.close();  //No need to close the file manually, as it does so upon destruction. 
	return 0;
}

void write_to_log_file(void *data, int length)
{
	string str((char*)data, length);
	to_log_file(str);

}
//===========================================================
//#define m_printf printf
void WriteLogFile(const char* szString)
{
	FILE* pFile = fopen("logFile.txt", "a");
	fprintf(pFile, "%s\n", szString);
	fclose(pFile);
}
void /*sk_log_to_file*/m_printf(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	char msg[2048];// = { 0 };
	vsprintf(msg, format, ap);  //The buffer should be large enough to contain the resulting string.

	WriteLogFile(msg);
	printf("::%s", msg);

	va_end(ap);
}
//==============================================================



int read_size_file() {
	streampos begin, end;
	ifstream myfile("example.bin", ios::binary);
	begin = myfile.tellg();
	myfile.seekg(0, ios::end);
	end = myfile.tellg();
	myfile.close();
	cout << "size is: " << (end - begin) << " bytes.\n";
	return 0;
}
int read_entire_log_file(char * memblock,int maxsize) {
	streampos size;
	

	ifstream file(log_path, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		if (size > maxsize)
			size = maxsize;
		//memblock = new char[size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		cout << "the entire file content is in memory";

		//delete[] memblock;
	}
	else cout << "Unable to open file";
	return size;
}

void delete_log_file()
{
	remove(log_path);
}


//======================Read Html file================================
char* read_html_file(char *full_path) {
	streampos size;
	char * memblock=0;
	//int maxsize;

	ifstream file(full_path, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		//if (size > maxsize)
		//	size = maxsize;
		memblock = new char[size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		//cout << "the entire file content is in memory";

		//delete[] memblock;
	}
	//else cout << "Unable to open file";
	return memblock;
}
unsigned char* read_bin_file(char *full_path, int &length)
{
	streampos size;
	unsigned char * memblock = 0;
	//int maxsize;

	ifstream file(full_path, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		//if (size > maxsize)
		//	size = maxsize;
		length = size;
		memblock = new unsigned char[size];
		file.seekg(0, ios::beg);
		file.read((char*)memblock, size);
		file.close();

	}

	return memblock;
}