#include "BaseDB.h"
#include "udorx_server_config.h"

BaseDB::BaseDB(char *fname) {
	/*int len = strlen(fname) + 1;
	this->fileName = new char[len];
	strncpy(this->fileName, fname,len-1);
	this->fileName[len - 1] = 0;*/

	fullPath=createPath(fname);
	//if (pthread_mutex_init(&lockMutex, NULL) != 0)
	//{
	//	//printf("\n mutex init failed\n");
	//	//return 1;
	//}

}
 BaseDB::~BaseDB()
{
	/*delete[] this->fileName;
	this->fileName = 0;*/
}

 bool BaseDB::IsTableExist(SQLiteStatement *pStmt, std::string tableName)
 {

	 pStmt->Sql("select * from sqlite_master where type='table' and name=?");
	 pStmt->BindString(1, tableName);		// bind tableName
	 int rv = 0;
	 while (pStmt->FetchRow())
	 {
		 rv = pStmt->GetDataCount();
	 }
	 // pStmt->GetColumnInt(0);//pStmt->GetColumnCount();
	 //const char *res = pStmt->GetColumnName(0);
	 pStmt->FreeQuery();
	 return rv > 0;
 }

#if MKR_LINUX 
 std::string BaseDB::createPath(const char *fileName)
 {
	 std::string res = "";
	 
	 if (!fileName || !(strlen(fileName) > 0))
	 return res;
	 char dir[128 * 2];
	 memset(dir, 0, sizeof(dir));
	 strcpy(dir, DB_ROOT_DIR);
	 int rv = -1;
	 struct stat st = { 0 };
	 if (stat(dir, &st) == -1) {
		 rv = mkdir(dir, 0777);
	 }

	 strcat(dir, "db/");
	 st = { 0 };
	 if (stat(dir, &st) == -1) {
		 rv = mkdir(dir, 0777);
	 }

	 //strcat(dir, "/");

	 strcat(dir, fileName);
	 res = std::string(dir);

	 //int x=	mkpath(rv, 0777);


	 return res;
 }

#elif defined(WIN32)
 std::string BaseDB::createPath(const char* fileName)
 {
	 char dir[128 * 2];
	 memset(dir, 0, sizeof(dir));
	 strcpy(dir, DB_ROOT_DIR);
	 strcat(dir, fileName);
	 std::string res = std::string(dir);

	 //int x=	mkpath(rv, 0777);
	 return res;
 }
#endif
 SQLiteStatement* BaseDB::CreateConnection()
 {
	 pStmt = 0;
	 //std::string fullPath = createPath();
	 try
	 {
		 if (fullPath.length() > 0)
		 {
			 pDatabase = new SQLiteDatabase(fullPath.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
			 pStmt = new SQLiteStatement(pDatabase);
		 }
	 }
	 catch (SQLiteException &ex)
	 {
		 pDatabase = 0; pStmt = 0;
	 }
	 return pStmt;
 }

// SQLiteStatement *BaseDB::GetStmt() { return pStmt; }
 SQLiteDatabase *BaseDB::GetDB() { return pDatabase; }
 int BaseDB::GetNuberOfRowsUpdated()
 {
	 //get the number of rows updated
	 int tmp = pDatabase->GetDatabaseChanges();
	 return tmp;
 }
 void BaseDB::CloseConn()
 {
	 if (pStmt != 0)
	 {
		 delete pStmt;
	 }
	 if (pDatabase != 0)
	 {
		 try {
			 pDatabase->Close();
		 }
		 catch (SQLiteException &ex)
		 {
			 delete pDatabase;
			 pDatabase = 0;
		 }
		 if(pDatabase!=0)delete pDatabase;
	 }
	 pStmt = 0;
	 pDatabase = 0;
	
 }

 void BaseDB::CreateTableA(SQLiteStatement *pStmt, std::string sql)
 {
	 pStmt->SqlStatement(sql);

 }

 //https://stackoverflow.com/questions/2127138/how-to-retrieve-the-last-autoincremented-id-from-a-sqlite-table
 /*
 One other option is to look at the system table sqlite_sequence. 
 Your sqlite database will have that table automatically if you created any table with autoincrement primary key. 
 This table is for sqlite to keep track of the autoincrement field so that it won't repeat the primary key even after 
 you delete some rows or after some insert failed (read more about this here http://www.sqlite.org/autoinc.html).
 */

int64_t BaseDB::GetLastInsertedAutoIncrementID(std::string table_name)
 {
	
	string sql = " select seq from sqlite_sequence where name = '";
	sql = sql + table_name + "'";
	pStmt->Sql(sql);
	int64_t id = -1;
	while (pStmt->FetchRow())
	{
		id = pStmt->GetColumnInt64(0);
	}
	pStmt->FreeQuery();

	return id;
 }


 int64_t BaseDB::GetLastInsertedRow()
 {
	 int64_t id = -1;
#if 0	
	 string sql = " SELECT last_insert_rowid();";    //immediately after your insert.  !!!
	 pStmt->Sql(sql);
	 while (pStmt->FetchRow())
	 {
		 id = pStmt->GetColumnInt64(0);
	 }
	 pStmt->FreeQuery();
#else
	 id= pDatabase->GetLastInsertRowId();
#endif
	 return id;
 }
 string  BaseDB::getStringValue(SQLiteStatement *p, string &sql)
 {
	 string rv = "";
	 p->Sql(sql);

	 while (p->FetchRow())
	 {
		 rv = p->GetColumnString(0);
	 }
	 // do not forget to clean-up
	 p->FreeQuery();
	 return rv;
 }