#ifndef MKR_BASEDB_H_INCLUDED
#define MKR_BASEDB_H_INCLUDED


#include "KompexSQLitePrerequisites.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "KompexSQLiteStreamRedirection.h"
#include "KompexSQLiteBlob.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <exception>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(ANDROID)
#include <unistd.h>
#endif
#include <errno.h>    // errno, ENOENT, EEXIST

//#include <pthread.h>
#include <thread>
#include <mutex>

//#include "MkrHeader.h"  // int64 problem here !! moved it here after Kompex definitions

//http://stackoverflow.com/questions/1680249/how-to-use-sqlite-in-a-multi-threaded-application
//http://www.sqlite.org/cvstrac/wiki?p=MultiThreading
/*
•Make sure you're compiling SQLite with -DTHREADSAFE=1. 
•Make sure that each thread opens the database file and keeps its own sqlite structure. 
•Make sure you handle the likely possibility that one or more threads collide when they access the db file at the same time: handle SQLITE_BUSY appropriately. 
•Make sure you enclose within transactions the commands that modify the database file, like INSERT, UPDATE, DELETE, and others. 

*/

#include <google/protobuf/stubs/stringprintf.h>
using namespace google::protobuf;  // to use StringPrintf  instead of string_format

using namespace std;
using namespace Kompex;
class BaseDB
{
	SQLiteDatabase *pDatabase;
	SQLiteStatement *pStmt;
	std::string fullPath;      //char *fileName;
	
public:
	
	mutex lockMutex; //pthread_mutex_t lockMutex;

	BaseDB(char *fname);

	virtual ~BaseDB();

	bool IsTableExist(SQLiteStatement *pStmt, std::string tableName);
	/*
	int	mkpath(std::string s, mode_t mode)
	{
		size_t pre = 0, pos;
		std::string dir;
		int mdret;

		if (s[s.size() - 1] != '/') {
			// force trailing / so we can handle everything in loop
			s += '/';
		}

		while ((pos = s.find_first_of('/', pre)) != std::string::npos) {
			dir = s.substr(0, pos++);
			pre = pos;
			if (dir.size() == 0) continue; // if leading / first time is 0 length
			if ((mdret = mkdir(dir.c_str(), mode)) && errno != EEXIST) {
				return mdret;
			}
		}
		return mdret;
	}
	*/
	std::string createPath(const char *fileName);

	SQLiteStatement* CreateConnection();
	
//	SQLiteStatement *GetStmt();
	SQLiteDatabase *GetDB();
	int GetNuberOfRowsUpdated();
	void CloseConn();

	void CreateTableA(SQLiteStatement *pStmt, std::string sql);
	int64_t GetLastInsertedRow();
	int64_t GetLastInsertedAutoIncrementID(std::string name);
	string  getStringValue(SQLiteStatement *p, string &sql);
};
#if 0
class A
{
	char *fileName;
public:

	A(char *fname) {
		int len = strlen(fname) + 1;
		this->fileName = new char[len];
		strcpy(this->fileName, fname);
		this->fileName[len - 1] = 0;
	}

	virtual ~A()
	{
		delete[] this->fileName;
	}
};
class  B :public A
{
public:
	B() : A("employee.db")
	{

	}
	~B()
	{

	}
};
#endif

#define CREATECONNECTION std::lock_guard<std::mutex> lock(lockMutex); \
SQLiteStatement *p = CreateConnection(); \
bool exception_flag=false; \
if (p == 0) \
goto __exit;

#define MY_CATCH_DB  catch (SQLiteException &ex) \
{ \
	string msg = ex.GetString(); \
	m_printf("Exc:%s\r\n", msg.c_str()); \
	exception_flag=true; \
}


#define CLOSECONN	MY_CATCH_DB \
__exit: \
CloseConn();


#endif
