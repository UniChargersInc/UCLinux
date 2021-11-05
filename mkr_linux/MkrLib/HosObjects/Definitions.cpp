#include "Definitions.h"
//#include "HOSAgent.h"
#include "TimeUtil.h"
namespace HosEngine
{
	
		/*	const DataRowIdentifier &RecapRow::getDataRowID() const
			{
				return DataRowIdentifier::RecapRow;
			}
			*/

			/*const DataRowIdentifier &DeferHourRow::getDataRowID() const
			{
				return DataRowIdentifier::DeferHourRow;
			}*/
#if 0
			RuleSelectionHistoryRow::RuleSelectionHistoryRow()
			{
				init(-1, HOSCYCLE::NONE, TimeUtil::DateTime_Now().getMillis(), "");
			}

			RuleSelectionHistoryRow::RuleSelectionHistoryRow(int id, HOSCYCLE rule, DateTime t, const std::string &cn)
			{
				init(id, rule, t, cn);
			}
			void RuleSelectionHistoryRow::init(int id, HOSCYCLE rule, DateTime t, const std::string &cn)
			{
				this->set_rshid(id);
				this->set_ruleid((int)rule);
				this->set_selecttime(t.getMillis()); // !!!!!!!!!!!!!!!!!=================
				this->set_country(cn);
			}

			

			const DataRowIdentifier &RuleSelectionHistoryRow::getDataRowID() const
			{
				return DataRowIdentifier::RuleSelectionHistoryRow;
			}
#endif
//			void TimeLogRow::SetHaveSend(bool f)
//			{
//				haveSend = f;
//			}
//
//			bool TimeLogRow::GetHaveSend()
//			{
//				return haveSend;
//			}
//
//			void TimeLogRow::SetisFromServer(bool f)
//			{
//				isFromServer = f;
//			}
//
//			bool TimeLogRow::GetisFromServer()
//			{
//				return isFromServer;
//			}
//
//			TimeLogRow::TimeLogRow()
//			{
//
//				this->LogTime = TimeUtil::DateTime_Now();
//				this->OLT = DateTime::UtcNow(); // must be kept the same since it has been created
//			}
//
//			TimeLogRow::TimeLogRow(int id, int logStatus, DateTime logTime, int logbookstopid, int odometer, const std::string &driverId, bool HaveSent, bool IsFromServer, double latitude, double longitude, int boxID, const std::string &equipId, const std::string &comment, const std::string &address, bool signed_Renamed, int type, DateTime origLogTime, int editor,const std::string &state )
//			{
//				this->TLID = id;
//				this->Event = (LOGSTATUS)logStatus;
//				this->LogTime = logTime;
//				this->Logbookstopid = logbookstopid;
//				this->Odo = odometer;
//				this->DriverId = driverId;
//				this->haveSend = HaveSent;
//				this->isFromServer = IsFromServer;
//				this->Lat = latitude;
//				this->Lon = longitude;
//				this->BoxID = boxID;
//				this->EqNo = equipId;
//				this->Comment = comment;
//				this->Addr = address;
//				this->Signed = signed_Renamed;
//				this->Type = static_cast<TimeLogType>(type);
//				this->OLT = origLogTime;
//				this->Editor = editor;
//				this->State = state; // Devin Added for audit
//			}
//
//			TimeLogRow::TimeLogRow(int id, int logStatus, DateTime logTime, int logbookstopid, int odometer, const std::string &driverId, bool HaveSent, bool IsFromServer, double latitude, double longitude, int boxID, const std::string &equipId)
//			{
//				this->TLID = id;
//				this->Event = static_cast<LOGSTATUS>(logStatus);
//				this->LogTime = logTime;
//				this->Logbookstopid = logbookstopid;
//				this->Odo = odometer;
//				this->DriverId = driverId;
//				this->haveSend = HaveSent;
//				this->isFromServer = IsFromServer;
//				this->Lat = latitude;
//				this->Lon = longitude;
//				this->BoxID = boxID;
//				this->EqNo = equipId;
//			}
//
//			TimeLogRow *TimeLogRow::Clone()
//			{
//				TimeLogRow *tl = MKR_NEW TimeLogRow();
//				Copy(tl);
//				return tl;
//			}
//
//			void TimeLogRow::Copy(TimeLogRow *dest)
//			{
//				dest->TLID = this->TLID;
//				dest->Event = this->Event;
//				dest->LogTime = this->LogTime;
//				dest->Logbookstopid = this->Logbookstopid;
//				dest->Odo = this->Odo;
//				dest->DriverId = this->DriverId;
//				dest->haveSend = this->haveSend;
//				dest->isFromServer = this->isFromServer;
//				dest->Lat = this->Lat;
//				dest->Lon = this->Lon;
//				dest->BoxID = this->BoxID;
//				dest->EqNo = this->EqNo;
//				dest->Signed = this->Signed;
//				dest->OLT = this->OLT;
//				dest->Editor = this->Editor;
//				dest->Comment = this->Comment;
//
//				dest->Addr = this->Addr;
//				dest->arGeo = this->arGeo;
//
//			}
//
//			int TimeLogRow::CompareTo(void *obj)
//			{
//				TimeLogRow *to = static_cast<TimeLogRow*>(obj);
//				int i = this->LogTime.CompareTo(to->LogTime);
//				if (i != 0)
//				{
//					return i;
//				}
//				return 1;/*this->TLID.compare(to->TLID)*/;
//			}
//
//			void TimeLogRow::updateEdit(DateTime dt, int editor, const std::string &s)
//			{
//				this->OLT = dt;
//				this->Editor = editor;
//				this->Comment = s;
//			}
//
//			void TimeLogRow::SetSigned(bool v)
//			{
//				this->Signed = v;
//			}
//
//			int TimeLogRow::getDutyStatusType()
//			{
//				return static_cast<int>(this->Event);
//			}
//
//			int TimeLogRow::getDutyStatusTypeIndex()
//			{
//				return (int)(this->Event) - (int)DutyStatus::Offduty;
//			}
//
//			bool TimeLogRow::isOnDuty()
//			{
//				return (static_cast<int>(this->Event) == static_cast<int>(DutyStatus::Onduty));
//			}
//
//			bool TimeLogRow::isDriving()
//			{
//				return (static_cast<int>(this->Event) == static_cast<int>(DutyStatus::Driving));
//			}
//
//			DateTime TimeLogRow::getTimestamp()
//			{
//				return this->LogTime;
//			}
//
//			std::string TimeLogRow::getDuration()
//			{
//				return Duration;
//			}
//
//			void TimeLogRow::setDuration(DateTime nextTimestamp, int nextDutyStatusTypeIndex)
//			{
//				this->NextTimestamp = nextTimestamp;
//				this->NextDutyStatusTypeIndex = nextDutyStatusTypeIndex;
//				this->Duration = HOSAgent::getDuration(this->NextTimestamp, this->LogTime);
//			}
//
//			long long TimeLogRow::getTimeMilli()
//			{
//				//I used TimeUtil.DateTime_Now().TimeOfDay.TotalMilliseconds (for current day),
//				return 0;// static_cast<long long>(TimeUtil::DateTime_Now().TimeOfDay::TotalMilliseconds); // (for current day),//Timestamp;
//			}
//
//			DateTime TimeLogRow::getNextTimestamp()
//			{
//				return NextTimestamp;
//			}
//
//			int TimeLogRow::getNextDutyStatusTypeIndex()
//			{
//				return NextDutyStatusTypeIndex;
//			}
//
//			std::string TimeLogRow::toString()
//			{
////C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
//				std::string str = "";// std::string::Format("{0}  {1}", this->LogTime.ToString(), this->Event.ToString());
//				return str;
//			}

			/*EmployeeRow::EmployeeRow()
			{
			}

			EmployeeRow::EmployeeRow(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &state )
			{
				this->ID = id;
				this->DriverName = driverName;
				this->UserName = username;
				this->Password = pw;
				this->License = license;
				this->AutoLogin = autoLogin;
				this->HomeAddress = homeAddress;
				this->IsSupervisor = isSupervisor;
				this->State = state;
			}

			EmployeeRow::EmployeeRow(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &signature, const std::string &domain, HOSCYCLE cycle, const std::string &state )
			{
				this->ID = id;
				this->DriverName = driverName;
				this->UserName = username;
				this->Password = pw;
				this->License = license;
				this->AutoLogin = autoLogin;
				this->HomeAddress = homeAddress;
				this->IsSupervisor = isSupervisor;
				this->Signature = signature;
				this->Domain = domain;
				this->Cycle = cycle;
				this->State = state;
			}

			void EmployeeRow::SetDomain(const std::string &s)
			{
				Domain = s;
			}

			void EmployeeRow::SetSignature(const std::string &v)
			{
				this->Signature = v;
			}

			void EmployeeRow::SetOrg(const std::string &name, const std::string &addr)
			{
				OrgAddr = addr;
				OrgName = name;
			}

			void EmployeeRow::SetCycle(HOSCYCLE v)
			{
				this->Cycle = (v);
			}

			void EmployeeRow::SetHOSExcept(int v)
			{
				this->HOSExcept = v;
			}

			int EmployeeRow::GetHOSExcept()
			{
				return this->HOSExcept;
			}

			void EmployeeRow::SetTimeZone(float v)
			{
				this->TZ = v;
			}

			float EmployeeRow::GetTimeZone()
			{
				return this->TZ;
			}

			void EmployeeRow::SetApplyDTS(int v)
			{
				this->ApplyDTS = v;
			}

			int EmployeeRow::GetApplyDTS()
			{
				return this->ApplyDTS;
			}

			void EmployeeRow::SetHaveSent(bool v)
			{
				this->haveSend = v;
			}
			
			std::string EmployeeRow::ToString()
			{
				return DriverName;
			}*/
			/*
			int EmployeeRow::GetHashCode()
			{
				return __super::GetHashCode();
			}
			*/

			StateRow::StateRow()
			{
			}

}
