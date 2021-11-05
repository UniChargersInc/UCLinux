#ifndef MKR_TIMELOGDB_H_INCLUDED
#define MKR_TIMELOGDB_H_INCLUDED

#include <string>
#include <vector>
#include <stdexcept>

#include "BaseDB.h"
#include "HosObjects/Definitions.h"
#include "HosObjects/TimeUtil.h"

#include "CommonUtils/my_utils.h"

using namespace HosEngine;
using namespace Kompex;
using namespace udorx_pbf;
class FleetTimeLogDB:public BaseDB
{
			private:
				static FleetTimeLogDB *_instance;
				
				
				void CreateTables();
				
			public:
				FleetTimeLogDB() : BaseDB("uc_fleet_timelog.db")
				{
					CreateTables();
				}
				static FleetTimeLogDB *Get();

			private:
				void CreateTimeLogTable(SQLiteStatement *pStmt);

				//LogStatus<>201  -> do not show deleted events
			public:
				bool GetAllTimeLogs(PTlrList& list,int DriverID,int64_t ts=0);
				MyDailyLogsList* GetDailyLogs(int DriverID,int64_t start,int64_t end);
				
				bool GetLastBeforeDate(int DriverID, int64_t date, PTlr &tlr);
				void DeleteLastRecord();
			private:
				bool getTTList(PTlrList& list,const std::string &sql, int _status, bool _all);
				//======================================================

			public:
				bool GetAllUnSent(PTlrList& list);

				void SetHaveSentDeleteOlder();

		//==========================Request for Vehicle info after scan===================
				bool GetAllUnSent4Vehicle(PTlrList& list);

				void SetHaveSentDeleteOlder4Vehicle();
		//================================================================================

				bool UpdateSignedStatus(vector<TimeLogRow *> tlr_list);

			private:
				void SetParams(SQLiteStatement *pStmt, /*TimeLogRow*/PTlr *timLogRow, bool sent = false);
				bool getLast(string sql, PTlr &tlr);
				int saveTimeLog(SQLiteStatement *p, /*TimeLogRow*/PTlr *timLogRow);
			public:
				
				int StoreTimeLogs2DB(int cid, PTlrList *tlrl);
				int SaveTimeLog(PTlr &timLogRow, bool insert_this_row=false);
				bool UpdateDriverStatus(PTlr *tlr);

				void UpdateAddressInTimeLog(DateTime dt, const std::string &address);
				#if defined(NOT_ONLY_ONE_LOG)

				void SaveTimeLogUpdateFromServer(TimeLogRow *timLogRow);
				#else

				void SaveTimeLogsUpdateFromServer(std::vector<TimeLogRow*> &list);
				#endif
				void DeleteAll();

				void DeleteRow(int id);


				bool GetLast(int driverId, PTlr &tlr);

				TimeLogRow *GetLastRecord();
				//-------------------additional--------------------------
				#if defined(NOT_USE)
				TimeLogRow *GetLast(const std::string &driverId, LOGSTATUS *logStatus);
				TimeLogRow *GetLastBeforeDate(const std::string &driverId, DateTime before);

				std::vector<TimeLogRow*> GetAllForDate(DateTime date, const std::string &driverId = "");
				#endif

#pragma region RecapDB
private:
	void CreateRecapTable(SQLiteStatement *pStmt);
public:
	void Recap_SetHaveSentDeleteOlder();
private:
	PRecapList* getAllRecap(const std::string &driverId, const std::string &sql);
public:
	PRecapList* GetAllRecapByDriver(const std::string &driverId);
	PRecapList* GetAllRecapByDriverForCurrentDay(const std::string &driverId);
private:
	void SetParams(SQLiteStatement *pStmt, PRecap *rr, const std::string &driverId);
public:
	void AddRecap(PRecap *rr, const std::string driverId);
	void CleanUpRecapTable();
private:
	void _doRecap(PRecap *rr, const std::string &driverId, const std::string &sql);
public:
	void UpdateRecap(PRecap *rr, const std::string driverId);
#pragma endregion
#pragma region class DeferHoursRepository
private:
	void CreateDeferHoursTable(SQLiteStatement *pStmt);
public:
	void DeleteDeferHour(DeferHourRow *dhr, const std::string &driverId);
	DeferHourRow *FindDeferHourByDate(DateTime date, const std::string &driverId);
	void DeferHours_SetHaveSentDeleteOlder();
	void AddDeferHour(DeferHourRow *dhr, const std::string &driverId);
#pragma endregion

#pragma region RuleSelectionHistory
private:
	void CreateHOSRulesTable(SQLiteStatement *pStmt);
	void setParams(SQLiteStatement *pStmt, PRuleSH *rsr, const std::string &driverId, bool haveSent);

public:
	void AddRuleSelectionHistory(PRuleSH *rsr, const std::string &driverId);
	// we use selectTime as Unique Identifier
	void UpdateRuleSelectionHistoryFromServer(std::vector<PRuleSH*> &rlist, const std::string &driverId);

private:
	PRuleSH *GetLastBeforeSelectTime(SQLiteStatement *pStmt, const std::string &driverId, DateTime selectTime);
	PRuleSH *readRuleRow(SQLiteStatement *pStmt);


	bool _getRuleSelectionHistory(const std::string &sql, PRuleSHList* rlist);

	//"DriverId ntext, ruleid int, selectTime datetime, country ntext, HaveSent BOOLEAN); ";
public:
	PRuleSHList* GetRuleSelectionHistory(const std::string &driverId);
	PRuleSHList* GetUnsentRuleSelectionHistory(const std::string &driverId);
	void RulesHos_SetHaveSentDeleteOlder(const std::string &drId);
	
	PRuleSH* GetLastRuleByDriverCountry(string driverId, string country);
#pragma endregion
#pragma region AlertDB
	private:
		void CreateAlertsTable(SQLiteStatement *pStmt);
			public:
				void Alerts_SetHaveSentDeleteOlder();
			private:
				bool getAllAlerts(std::vector<HosAlertRow*> &alerts, SQLiteStatement *p);
			public:
				std::vector<HosAlertRow*> GetAllAlertsByDriver(const std::string &driverId);
				bool isAlertExist(DateTime dt_shift, int al_type, const std::string &dr_rule, const std::string &onduty_rule, const std::string &driverId);
				void AddAlert(HosAlertRow *rr, const std::string &driverId);
#pragma endregion

};

#endif
