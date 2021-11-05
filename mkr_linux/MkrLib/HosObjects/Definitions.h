#ifndef DEFINITIONS_H
#define DEFINITIONS_H


#include <string>
#include "CommonUtils/my_time.h"
#include "SharedObjects.h"


#include "timelog_row.h"
#include "proto/udorx_comm.pb.h"
#include "proto/udorx_ws_comm.pb.h"
namespace HosEngine
{


			class HosAlertRow
			{
			public:
				DateTime dt;
				std::string ondutyruleviolated = "", drivingruleviolated = ""; // rule violated
				AlertTypes alertType = static_cast<AlertTypes>(0);
				int drivingAvailable = 0; // driving time available
				int ondutyAvailable = 0; // On Duty time available

				bool haveSent = false;
			};
#if 0
			class RecapRow:public PRecap
			{
			public:
				bool haveSend = false;
			
				/*DateTime date;
				int today = 0;
				int cycleTotal = 0;
				int cycleAvailable = 0;
				std::string cycle = "";*/
			public:
				const DataRowIdentifier &getDataRowID() const;
				
			};
#endif
			class DeferHourRow
			{
			public:
				bool haveSend = false;
				DateTime day1 = DateTime::MinValue;
				DateTime day2 = DateTime::MinValue;
				TimeSpan defers = TimeSpan(0);

				//const DataRowIdentifier &getDataRowID() const;
			};
			class RuleSelectionRow
			{
			public:
				int RuleType = 0;
				int RuleId = 0;
			};
#if 0
			class RuleSelectionHistoryRow:public PRuleSH
			{
			public:
				/*int RSHID = -1;
				HOSCYCLE Ruleid = HOSCYCLE::NONE;
				DateTime selectTime = DateTime::MinValue;
				std::string country = "";
				RuleExceptions ruleException = RuleExceptions::None;*/

				bool haveSend = false;

				RuleSelectionHistoryRow();
				RuleSelectionHistoryRow(int id, HOSCYCLE rule, DateTime t, const std::string &cn);
				void init(int id, HOSCYCLE rule, DateTime t, const std::string &cn);

				
				const DataRowIdentifier &getDataRowID() const;

			};
#endif
			//class TimeLogRow
			//{
			//public:
			//	LOGSTATUS Event = LOGSTATUS::OffDuty;
			//	DateTime LogTime = DateTime::MinValue;
			//	bool Signed = false;
			//	int TLID = -100;
			//	TimeLogType Type = TimeLogType::Manual;
			//	//public DataRowIdentifier DataRowID { get { return DataRowIdentifier.TimeLogRow; } }
			//	int Odo = 0;
			//	int Logbookstopid = -1000; //30 min break -> make an assignement = AuditLogic.ThirtyMinutesOffDutyStart

			//	//public string FuelProvider = string.Empty;
			//	//public int FuelQuantity = 0;
			//	//public string FuelUnit = string.Empty;

			//	std::string EqNo = ""; //EquipmentNo = string.Empty;

			//	float Lat = 0;
			//	float Lon = 0;
			//	std::string Comment = "";
			//	std::string DriverId = "";
			//	std::string State = ""; // Devin Added for audit
			//	int BoxID = -1;

			//	DateTime OLT = DateTime::MinValue; //OrigLogTime = DateTime.MinValue; // used when editing timelog record
			//	int Editor = 0; // 1- TimeLog has been edited
			//	float TZ = 0, DST = 0; //TimeZone(TZ) added recently -> ApplyDST(DTS) Daylight Saving Time
			//	std::string Addr = "";
			//	int arGeo = -1; // -1 - No Reverse Geocoding, 0- Reverse Geocoding ( address resolution) is in progress, 1 - DONE
			//	std::string CoDriver = "";
			//	//public string QualifyRadiusRule="";  //"0" - false, "1" - true, "" - empty

			//private:
			//	bool haveSend = false;
			//	bool isFromServer = false;

			//	DateTime NextTimestamp;
			//	int NextDutyStatusTypeIndex = 0;
			//	std::string Duration;
			//public:
			//	int QualifyRadiusRule;//Nullable<bool> QualifyRadiusRule = Nullable<bool>();

			//	void SetHaveSend(bool f);
			//	bool GetHaveSend();

			//	void SetisFromServer(bool f);
			//	bool GetisFromServer();




			//	TimeLogRow();

			//	TimeLogRow(int id, int logStatus, DateTime logTime, int logbookstopid, int odometer, const std::string &driverId, bool HaveSent, bool IsFromServer, double latitude, double longitude, int boxID, const std::string &equipId, const std::string &comment, const std::string &address, bool signed_Renamed, int type, DateTime origLogTime, int editor, const std::string &state = "");

			//	//this constructor is invoked from FleetTimeLogDB
			//	TimeLogRow(int id, int logStatus, DateTime logTime, int logbookstopid, int odometer, const std::string &driverId, bool HaveSent, bool IsFromServer, double latitude, double longitude, int boxID, const std::string &equipId);


			//	TimeLogRow *Clone();

			//	void Copy(TimeLogRow *dest);

			//	int CompareTo(void *obj);
			//	void updateEdit(DateTime dt, int editor, const std::string &s);
			//	void SetSigned(bool v);
			//	int getDutyStatusType();
			//	int getDutyStatusTypeIndex();
			//	bool isOnDuty();
			//	bool isDriving();

			//	DateTime getTimestamp();
			//	std::string getDuration();


			//	void setDuration(DateTime nextTimestamp, int nextDutyStatusTypeIndex);

			//	long long getTimeMilli();
			//	DateTime getNextTimestamp();
			//	int getNextDutyStatusTypeIndex();

			//	std::string toString();

			//};

			

			//class EmployeeRow
			//{
			//public:
			//	std::string ID = "";
			//	std::string DriverName = "";
			//	std::string UserName = "";
			//	std::string Password = "";
			//	std::string License = "";
			//	std::string Country = "";
			//	bool AutoLogin = false;
			//	std::string Signature = "";
			//	std::string Domain = "";
			//	HOSCYCLE Cycle = HOSCYCLE::Canadian_70hr_7day;
			//
			//	int HOSExcept = 0, ApplyDTS = 0; //applyDTS - apply Day Time Savings
			//	float TZ = 0; //TimeZone
			//	//internal LocationInfoRow Home = new LocationInfoRow();
			//	// DataNode Members
			//public:
			//	bool haveSend = true;
			//	bool isFromServer = false;

			//	//public virtual DataRowIdentifier DataRowID { get { return DataRowIdentifier.EmployeeRow; } }
			//	//Add by Iman
			//	std::string HomeAddress = "";
			//	bool IsSupervisor = false;
			//	std::string State = "";
			//	std::string OrgName = "", OrgAddr = "";
			//	std::string Email = "";
			//	EmployeeRow();

			//	EmployeeRow(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &state = "");

			//	EmployeeRow(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &signature, const std::string &domain, HOSCYCLE cycle, const std::string &state = "");
			//	void SetDomain(const std::string &s);
			//	void SetSignature(const std::string &v);
			//	void SetOrg(const std::string &name, const std::string &addr);
			//	void SetCycle(HOSCYCLE v);
			//	void SetHOSExcept(int v);
			//	int GetHOSExcept();

			//	void SetTimeZone(float v);
			//	float GetTimeZone();

			//	void SetApplyDTS(int v);
			//	int GetApplyDTS();

			//	void SetHaveSent(bool v);
			//	std::string ToString() ;

			//	//virtual int GetHashCode() override;
			//};

			class StateRow
			{
			public:
				int ID = 0;
			
				StateRow();
				std::string StateName;
				
				std::string Abbr;
				
				std::string Country;
				
			};

		//================M_Definitions===============


}

#endif
