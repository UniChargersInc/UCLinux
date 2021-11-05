#pragma once
#include "BaseDB.h"
#include "HosObjects/Definitions.h"
#include "HosObjects/TimeUtil.h"
#include "mkr_ocpp/mkr_ocpp.hpp"

#include <map>

#include "OcppDBTypes.h"

class HistoryDB:public BaseDB
{
private:
	static HistoryDB *_instance;
	int x;
	
	
	void createTables();
	
	void CreateChargeStationHistTable(SQLiteStatement *p);
	
	int updateHistOnStartTransaction(SQLiteStatement *p, OCPP_Request &req, Rtt_Ocpp_T &rtt,int hist_rowid);
	//-----------------------------------------------------------
	std::string getLastStatusNoficationFromHistTable(std::string sern);
	
	void CreateStartTransactionTable(SQLiteStatement *p);
	int insertStartTransactionReqMsg2DB(SQLiteStatement *p, int hist_rowid, void *ptr);
	void CreateStopTransactionTable(SQLiteStatement *p);
	void CreateMeterValuesTable(SQLiteStatement *p);
	int insertStopTransactionReqMsg2DB(SQLiteStatement *p, int hist_rowid, void *ptr);

	void setStopTransactionParams(SQLiteStatement *p, StopTrData_T &data);
	bool readStopTrData(SQLiteStatement *p, StopTrData_T &data);

	void setMeterValuesParams(SQLiteStatement *p, MeterValuesData_T &data,int g_ts);
	int insertMeterValuesReqMsg2DB(SQLiteStatement *p, int hist_rowid,const char* sern, void *ptr);
	bool readMeterVData(SQLiteStatement *p, MeterValuesData_T &data);
	int insertOcppHistory2DB(SQLiteStatement *p, OCPP_Request &req, Rtt_Ocpp_T &rtt, std::string& sern);
	
public:
	HistoryDB() : BaseDB("uc_history_ocpp.db")
	{
		createTables();
		x = 1;
	}


	//No, destructors are called automatically in the reverse order of construction. (Base classes last). 
	//Do not call base class destructors.
	//!! you do not have to declare the destructor virtual in the derived class
	~HistoryDB()
	{
		x = 0;
	}

	static HistoryDB *Get(){
		if (_instance == 0)
			_instance = new HistoryDB();
		return _instance; 
	}
	static void Clear() {
		if (_instance != 0)
		{
			delete _instance;
		}
		_instance=0;
	}



	int GetStopTransactionRequestDataFromDB(int transactionId, lgpl3::ocpp16::StopTransactionRequest &tr_data);

	int GetAllChargeStationsHistory(int cid, const char *sern, std::vector<CS_History_T> &histv);
	int ClearAllChargeStationsHistory(int cid, const char *sern);

	int GetAllChargeStationsStatistics(int cid, const char *sern, std::map<int64_t,std::vector<CS_Statistics_T>> &stat_map);
	
	int OnStartTransactionRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, void *ptr,std::string& sern);
	int OnStopTransactionRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, void *ptr, std::string& sern);
	int OnMeterValuesRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, void *ptr, std::string& sern);
	int InsertOcppHistory2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, std::string& sern);
};

