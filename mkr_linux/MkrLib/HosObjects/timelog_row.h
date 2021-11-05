#pragma once

#include <string>
#include "CommonUtils/my_time.h"
#include "SharedObjects.h"

#include "proto/udorx_comm.pb.h"

using namespace std;
using namespace udorx_pbf;
#if 0
namespace HosEngine
{
	class TimeLogRow: public PTlr
	{
	public:
		LOGSTATUS Event() {/* LOGSTATUS::OffDuty*/ return (LOGSTATUS)this->event(); };
		DateTime LogTime() {                                                              // DateTime::MinValue;
			DateTime dt(this->logtime());
			return dt;
		}


		//bool Signed = false;
		//int TLID = -100;
		//TimeLogType Type = TimeLogType::Manual;
		////public DataRowIdentifier DataRowID { get { return DataRowIdentifier.TimeLogRow; } }
		//int Odo = 0;
		//int Logbookstopid = -1000; //30 min break -> make an assignement = AuditLogic.ThirtyMinutesOffDutyStart

		//						   //public string FuelProvider = string.Empty;
		//						   //public int FuelQuantity = 0;
		//						   //public string FuelUnit = string.Empty;

		//std::string EqNo = ""; //EquipmentNo = string.Empty;

		//float Lat = 0;
		//float Lon = 0;
		//std::string Comment = "";
		//std::string DriverId = "";
		//std::string State = ""; // Devin Added for audit
		//int BoxID = -1;

		//DateTime OLT = DateTime::MinValue; //OrigLogTime = DateTime.MinValue; // used when editing timelog record
		//int Editor = 0; // 1- TimeLog has been edited
		//float TZ = 0, DST = 0; //TimeZone(TZ) added recently -> ApplyDST(DTS) Daylight Saving Time
		//std::string Addr = "";
		//int arGeo = -1; // -1 - No Reverse Geocoding, 0- Reverse Geocoding ( address resolution) is in progress, 1 - DONE
		//std::string CoDriver = "";
		////public string QualifyRadiusRule="";  //"0" - false, "1" - true, "" - empty

	private:
		bool haveSend = false;
		bool isFromServer = false;

		/*DateTime NextTimestamp;
		int NextDutyStatusTypeIndex = 0;
		std::string Duration;*/
	public:
		//int QualifyRadiusRule;//Nullable<bool> QualifyRadiusRule = Nullable<bool>();

		void SetHaveSend(bool f);
		bool GetHaveSend();

		void SetisFromServer(bool f);
		bool GetisFromServer();




		TimeLogRow();

		TimeLogRow(string uid, int logStatus, int64_t logTime, int logbookstopid, int odometer, int driverId, bool HaveSent, bool IsFromServer, float latitude, float longitude, const std::string &vehId, const std::string &comment, const std::string &address, bool signed_Renamed, int type, int64_t origLogTime, int editor, const std::string &state = "");

		//this constructor is invoked from FleetTimeLogDB
		TimeLogRow(string uid, int logStatus, int64_t logTime, int logbookstopid, int odometer, int driverId, bool HaveSent, bool IsFromServer, float latitude, float longitude, const std::string &vehId);
		void init(string uid, int logStatus, int64_t logTime, int logbookstopid, int odometer, int driverId, bool HaveSent, bool IsFromServer, float latitude, float longitude, const std::string &vehId);
		void init1(const std::string &comment, const std::string &address, bool signed_Renamed, int type, int64_t origLogTime, int editor, const std::string &state);

		~TimeLogRow();

		//TimeLogRow *Clone();   just use operator =  => tlr1=tlr;

		//void Copy(TimeLogRow *dest);

		int CompareTo(void *obj);
		void updateEdit(int64_t dt, int editor, const std::string &s);
		void SetSigned(bool v);
		int getDutyStatusType();
		int getDutyStatusTypeIndex();
		bool isOnDuty();
		bool isDriving();

		DateTime getTimestamp();
		
		/*std::string getDuration();
		void setDuration(DateTime nextTimestamp, int nextDutyStatusTypeIndex);*/

		long long getTimeMilli();

		/*DateTime getNextTimestamp();
		int getNextDutyStatusTypeIndex();
*/
		std::string toString();

	};

}

#endif