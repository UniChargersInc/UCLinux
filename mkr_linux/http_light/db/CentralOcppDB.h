#pragma once
#include "BaseDB.h"
#include "HosObjects/Definitions.h"
#include "HosObjects/TimeUtil.h"
#include "mkr_ocpp/mkr_ocpp.hpp"

#include <map>
#include "OcppDBTypes.h"


class CentralOcppDB:public BaseDB
{
private:
	static CentralOcppDB *_instance;
	int x;
	
	
	void createTables();
	void CreateChargeStationsTable(SQLiteStatement *pStmt);
		
	bool readChS(SQLiteStatement *p, PCSItem *act);
	int readChS1(SQLiteStatement *p, PCSList *list);
	
	int getChargeStations(string sql, PCSList *woL, bool with_status_update);
	void SetParams(SQLiteStatement *pStmt, PCSItem  *wi, int flags, bool sent, bool update);
	
	int64_t insertCSI(SQLiteStatement *pStmt, PCSItem  *wi,int flags);

	int getSerNumberByIPAddr(SQLiteStatement *p, /*std::string &ipaddr,*/OCPP_Request &req,std::string &sern);
	int updateIPAddrFromOcppMessage2DB(SQLiteStatement *p, OCPP_Request &req, Rtt_Ocpp_T &rtt);
	int updateCSObjBFromOcppMessage2DB(SQLiteStatement *p, OCPP_Request &req, Rtt_Ocpp_T &rtt);
	int updateCSFWUpdateFromOcppMessage2DB(SQLiteStatement *p,const char* sern, const char* status, uint64_t ts);

	int getLastRowIDInDB(SQLiteStatement *p, std::string tbl_name);
	
	std::string preprocessOcppMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, bool requireIPaddrUpdate, StatusNotificationType sts_type);
	
	//-----------------------------------------------------------
	
	void CreateCSProfileTable(SQLiteStatement *p);
	int getCSProfileID(SQLiteStatement *p);
	
	int UpdateCSStationsWithProfile(SQLiteStatement *p,int profile_id,int cid, std::string sern);
	int getCSProfile(SQLiteStatement *p,int id, std::string &sern, std::string &data);
public:
	CentralOcppDB() : BaseDB("uc_central_ocpp.db")
	{
		createTables();
		x = 1;
	}


	//No, destructors are called automatically in the reverse order of construction. (Base classes last). 
	//Do not call base class destructors.
	//!! you do not have to declare the destructor virtual in the derived class
	~CentralOcppDB()
	{
		x = 0;
	}

	static CentralOcppDB *Get(){
		if (_instance == 0)
			_instance = new CentralOcppDB();
		return _instance; 
	}
	static void Clear() {
		if (_instance != 0)
		{
			delete _instance;
		}
		_instance=0;
	}

	int GetAllChargeStations(int rid,PCSList *list,bool with_status_update=false);
	int GetAllChargeStations_UpdMob(int cid, PCSList *list);
	
#if 0		
	void SaveChargeStations(PCSList *alist);
#endif
#if 1
	int DeleteChargeStation(string sern,int cid);
	bool GetChargeStationByID(int woid, PCSItem *woi);
	int64_t InsertNewChargeStationItem(PCSItem  *wi);
#endif
	int UpdateChargeStationStatus(const char* field_name, std::string &field_value, int status);
	
	int GetLastRowIDInDB();

	int InsertOcppMessage2DB(OCPP_Request &req,Rtt_Ocpp_T &rtt, bool requireIPaddrUpdate, StatusNotificationType sts_type);
	

	int InsertOcppRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, void *ptr);
	int UpdateOcppHBRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt);

	int UpdateCSTimeZone(std::string &sern, int tz, int dls);
	

#if 0	
	int GetMeterValuesRequestDataFromDB(int transactionId, std::vector<lgpl3::ocpp16::MeterValuesRequest> &tr_data);
#endif
	std::string getIPAddrBySerNumber(std::string &serN,bool fw_update_request=false);

	int insertOrUpdateCSProf2DB(int cid,int profile_id,std::string prof_name,std::string assign_to_sern, std::string &data);
	int getCSProfileFromDBByIp(int id,/* std::string &ip_addr,*/OCPP_Request &req, std::string &data);
	
	int getCSProfileFromDBBySern(int id, std::string &ip_addr, std::string &data);
	int getCSProfileFromDBByProfName(int id, std::string &prof_name, std::string &data);
	int getCSProfilesFromDBByCID(int cid, std::string &data);

	int removeChargingProfilefromCS(int id, std::string &sern);

	

};

