#pragma once
#include "BaseDB.h"
#include "HosObjects/Definitions.h"
#include "HosObjects/TimeUtil.h"

//#include "mkr_ocpp/mkr_ocpp.hpp"


enum ActStatus {
	Sts_Open = 0,
	Sts_Dispatched = 1,
	Sts_Accepted = 2,
	Sts_Started = 3,
	Sts_Completed = 4,
	Sts_Rejected = 5
};

using namespace HosEngine;

class AdminCSDB:public BaseDB
{
private:
	static AdminCSDB *_instance;
	int x;
	
	
	void createTables();
	void CreateAdminChargeStationsTable(SQLiteStatement *pStmt);
		
	bool readChS(SQLiteStatement *p, PCSItem *act);
	
	int getChargeStations(string sql, PCSList *woL,int haveSentFlag);
	void SetParams(SQLiteStatement *pStmt, PCSItem  *wi,bool sent, bool update);
	void SetParams4Update(SQLiteStatement *pStmt, PCSItem  *wi);

	void saveCSI(SQLiteStatement *pStmt, PCSItem  *wi);
	int64_t insertCSI(SQLiteStatement *pStmt, PCSItem  *wi);

public:
	AdminCSDB() : BaseDB((char*)"uc_admin_cs.db")
	{
		createTables();
		x = 1;
	}


	//No, destructors are called automatically in the reverse order of construction. (Base classes last). 
	//Do not call base class destructors.
	//!! you do not have to declare the destructor virtual in the derived class
	~AdminCSDB()
	{
		x = 0;
	}

	static AdminCSDB *Get(){
		if (_instance == 0)
			_instance = new AdminCSDB();
		return _instance; 
	}
	static void Clear() {
		if (_instance != 0)
		{
			delete _instance;
		}
		_instance=0;
	}

	int GetAllAdminChargeStations(int rid,PCSList *list,int max_number);
	
		
	void SaveAdminChargeStations(PCSList *alist);
#if 1
	void UpdateAdminChargeStation(PCSItem *woi/*,bool insert*/);
	void UpdateAdminChargeStationStatus(PCSItem *woi/*,bool insert*/);

	int DeleteAdminChargeStation(int woid,int cid);
	bool GetAdminChargeStationByID(int woid, PCSItem *woi);

	int64_t InsertNewAdminChargeStationItem(PCSItem  *wi);
#endif
	
	void UpdateAdminChargeStationsStatus(PCSList *alist, int status, bool have_sent_flag);
	
	int GetLastRowIDInAdminDB();

	
};

