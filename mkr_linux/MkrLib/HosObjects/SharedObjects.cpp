#include "SharedObjects.h"
#include "CommonUtils/my_time.h"


namespace HosEngine
{
		

			std::string Timelog::ToString()
			{
				//return std::string::Format("{0}, {1}, {2}, {3},{4}", Logtime, duty, Location, stop, odometer);
				char buf[128] = { 0 };
				sprintf(buf, "%s, %d, %d, %d, %d ", Logtime.ToString().c_str(), duty, Location, stop, odometer);
				return std::string(buf);
			}

			Timelog *Timelog::Clone()
			{
				Timelog *tl = new Timelog();
				tl->Logtime = this->Logtime;
				tl->Location = this->Location;
				tl->duty = this->duty;
				tl->stop = this->stop;
				tl->odometer = this->odometer;
				tl->_clone_flag = true;
				return tl;
			}

			int Timelog::CompareTo(void *obj)
			{
				Timelog *to = (Timelog*)obj;
				return Logtime.CompareTo(to->Logtime);
			}
			bool Timelog::operator>(Timelog &tl)  // requires for sort operations
			{
				return this->Logtime > tl.Logtime;
			}
			bool Timelog::operator<(Timelog &tl)
			{
				return this->Logtime < tl.Logtime;
			}

			//-------------------------------------
			AuditTime::AuditTime()
			{
			}
			std::string AuditTime::ToString()
			{
				return "";// std::string::Format("rule={0}, used={1} available={2}", rule, used, available);
			}
	
	
}
