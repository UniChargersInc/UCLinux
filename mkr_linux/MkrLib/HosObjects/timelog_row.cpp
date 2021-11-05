#include "timelog_row.h"

#include "HOSAgent.h"
#include "TimeUtil.h"

namespace HosEngine
{
	void TimeLogRow::SetHaveSend(bool f)
	{
		haveSend = f;
	}

	bool TimeLogRow::GetHaveSend()
	{
		return haveSend;
	}

	void TimeLogRow::SetisFromServer(bool f)
	{
		isFromServer = f;
	}

	bool TimeLogRow::GetisFromServer()
	{
		return isFromServer;
	}
	TimeLogRow::~TimeLogRow()
	{
		haveSend = false;
	}
	TimeLogRow::TimeLogRow()
	{
		//printf("TimeLogRow::TimeLogRow()\r\n");
		int64_t ts=TimeUtil::DateTime_Now().getMillis(),ts1= DateTime::UtcNow().getMillis();
		//printf("TimeLogRow::TimeLogRow()__1\r\n");
		string emptyStr = "";
		init(emptyStr, 101, ts, 0, 0, 0, false, false, 0, 0, emptyStr);
		//printf("TimeLogRow::TimeLogRow()__2\r\n");
		init1(emptyStr, emptyStr, false, 0, ts1, 0, emptyStr);
		//printf("TimeLogRow::TimeLogRow()__3\r\n");
	}

	TimeLogRow::TimeLogRow(string uid, int logStatus, int64_t logTime, int logbookstopid, int odometer, int driverId, bool HaveSent, bool IsFromServer, float latitude, float longitude, const std::string &equipId, const std::string &comment, const std::string &address, bool signed_Renamed, int type, int64_t origLogTime, int editor, const std::string &state)
	{
		init(uid, logStatus, logTime, logbookstopid, odometer, driverId, HaveSent, IsFromServer, latitude, longitude, equipId);
		init1(comment, address, signed_Renamed, type, origLogTime, editor, state);
		
	}

	TimeLogRow::TimeLogRow(string uid, int logStatus, int64_t logTime, int logbookstopid, int odometer, int driverId, bool HaveSent, bool IsFromServer, float latitude, float longitude,  const std::string &equipId)
	{
		init(uid, logStatus, logTime, logbookstopid, odometer, driverId, HaveSent, IsFromServer, latitude, longitude, equipId);
	}
	void TimeLogRow::init(string uid, int logStatus, int64_t logTime, int logbookstopid, int odometer, int driverId, bool HaveSent, bool IsFromServer, float latitude, float longitude, const std::string &equipId)
	{
		this->set_uid(uid);
		this->set_event((int)logStatus);
		this->set_logtime(logTime);
		this->set_lbsid(logbookstopid);
		this->set_odo(odometer);
		this->set_rid ( driverId);
		this->haveSend = HaveSent;
		this->isFromServer = IsFromServer;
		this->set_lat(latitude);
		this->set_lon(longitude);
		this->set_vid( equipId);
		//this->set_eqno(equipId);
	}
	void TimeLogRow::init1(const std::string &comment, const std::string &address, bool signed_Renamed, int type, int64_t origLogTime, int editor, const std::string &state)
	{
		this->set_comment(comment);
		this->set_addr(address);
		this->set_signed_(signed_Renamed);
		this->set_type((int)type);// = static_cast<TimeLogType>(type);
		this->set_olt(origLogTime);
		this->set_editor(editor);
		this->set_state(state); // Devin Added for audit
	}

	/*TimeLogRow *TimeLogRow::Clone()
	{
		TimeLogRow *tl = MKR_NEW TimeLogRow();
		Copy(tl);
		return tl;
	}*/

	/*void TimeLogRow::Copy(TimeLogRow *dest)
	{
		dest->TLID = this->TLID;
		dest->Event = this->Event;
		dest->LogTime = this->LogTime;
		dest->Logbookstopid = this->Logbookstopid;
		dest->Odo = this->Odo;
		dest->DriverId = this->DriverId;
		dest->haveSend = this->haveSend;
		dest->isFromServer = this->isFromServer;
		dest->Lat = this->Lat;
		dest->Lon = this->Lon;
		dest->BoxID = this->BoxID;
		dest->EqNo = this->EqNo;
		dest->Signed = this->Signed;
		dest->OLT = this->OLT;
		dest->Editor = this->Editor;
		dest->Comment = this->Comment;

		dest->Addr = this->Addr;
		dest->arGeo = this->arGeo;

	}*/

	int TimeLogRow::CompareTo(void *obj)
	{
		TimeLogRow *to = static_cast<TimeLogRow*>(obj);
		
		int64_t t = this->logtime(), t1 = to->logtime();

		if (t == t1) return 0;
		if (t > t1) return 1;
		else
			return -1;
	}

	void TimeLogRow::updateEdit(int64_t dt, int editor, const std::string &s)
	{
		this->set_olt( dt);
		this->set_editor(editor);
		this->set_comment(s);
	}

	void TimeLogRow::SetSigned(bool v)
	{
		this->set_signed_( v);
	}

	int TimeLogRow::getDutyStatusType()
	{
		return this->event();
	}

	int TimeLogRow::getDutyStatusTypeIndex()
	{
		return (int)(this->event()) - (int)DutyStatus::Offduty;
	}

	bool TimeLogRow::isOnDuty()
	{
		return (static_cast<int>(this->event()) == static_cast<int>(DutyStatus::Onduty));
	}

	bool TimeLogRow::isDriving()
	{
		return (static_cast<int>(this->event()) == static_cast<int>(DutyStatus::Driving));
	}

	DateTime TimeLogRow::getTimestamp()
	{
		//return this->LogTime;
		DateTime dt(this->logtime());
		return dt;
	}

	/*std::string TimeLogRow::getDuration()
	{
		return Duration;
	}

	void TimeLogRow::setDuration(DateTime nextTimestamp, int nextDutyStatusTypeIndex)
	{
		this->NextTimestamp = nextTimestamp;
		this->NextDutyStatusTypeIndex = nextDutyStatusTypeIndex;
		this->Duration = HOSAgent::getDuration(this->NextTimestamp, this->LogTime);
	}
*/
	long long TimeLogRow::getTimeMilli()
	{
		//I used TimeUtil.DateTime_Now().TimeOfDay.TotalMilliseconds (for current day),
		return 0;// static_cast<long long>(TimeUtil::DateTime_Now().TimeOfDay::TotalMilliseconds); // (for current day),//Timestamp;
	}

	/*DateTime TimeLogRow::getNextTimestamp()
	{
		return NextTimestamp;
	}

	int TimeLogRow::getNextDutyStatusTypeIndex()
	{
		return NextDutyStatusTypeIndex;
	}*/

	std::string TimeLogRow::toString()
	{
		//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
		std::string str = "";// std::string::Format("{0}  {1}", this->LogTime.ToString(), this->Event.ToString());
		return str;
	}
}
