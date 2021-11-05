#include "HOSAgent.h"

#include "TimeUtil.h"
#include "CommonUtils/my_utils.h"


namespace HosEngine
{
	int getHosCyclesSizeOfArray();
	//================================HOS Rules=================================

	string Rule2Str(int rule)
	{
		return "Rule:" + Int2Str(rule);
	}

	string RuleT2Str(int ruletype)
	{
		if (ruletype == (int)RuleType::Day)
			return "Day";
		else if (ruletype == (int)RuleType::Shift)
			return "Shift";
		else
			return "";
	}




	bool IsUSRule(RULEID rule)
	{
		int v = (int)rule;
		if (v != 19 && v != 6 && v != 7 && v != 18)
			return true;
		else
			return false;
	}

	bool IsCARule(RULEID rule)
	{
		int v = (int)rule;

		if (v != 19 && (v == 6 || v == 7 || v == 18))
			return true;
		else
			return false;
	}

	HOSCycleStruct	_cycles_[] = {
		{ "Canada/70h/7d" , 6 },
		{ "Canada/120h/14d" , 7 },
		{ "US_60h/7d" , 10 },
		{ "US_70h/8d" , 11 },
		{ "Canada_Oil_Field" , 18 },

		{ "NONE" , 19 },

		{ "US_Oil_Field" , 20 },

		//MKR_NEW rules for Kiewit  -> 

		{ "US_Texas" , 30 },

		{ "US_California" , 31 }, //intra state rules

								  //{"US_NewYork,32,

		{ "US_Florida_707" , 32 },

		{ "US_Florida_808" , 33 },

		{ "US_Alaskan_707" , 34 },

		{ "US_Alaskan_808" , 35 },




		{ "CA_AlbertaRule" , 36 },

		{ "US_NewJersey_707" , 38 },

		{ "US_NewJersey_808" , 39 },

		{ "US_Georgia_707" , 40 },

		{ "US_Georgia_808" , 41 },

		{ "US_Alabama_607" , 42 },

		{ "US_Alabama_708" , 43 },

		{ "US_Arizona_607" , 44 },

		{ "US_Arizona_708" , 45 },

		{ "US_Connecticut_607" , 46 },

		{ "US_Connecticut_708" , 47 },

		{ "US_Delaware_607" , 48 },

		{ "US_Delaware_708" , 49 },

		{ "US_Idaho_607" , 50 },

		{ "US_Idaho_708" , 51 },

		{ "US_Indiana_607" , 52 },

		{ "US_Indiana_708" , 53 },



		{ "US_Iowa_707" , 54 },

		{ "US_Iowa_808" , 55 },

		{ "US_Kansas_607" , 56 },

		{ "US_Kansas_708" , 57 },

		{ "US_Louisiana_607" , 58 },

		{ "US_Louisiana_708" , 59 },

		{ "US_Maryland_707" , 60 },

		{ "US_Maryland_808" , 61 },

		{ "US_Michigan_607" , 62 },

		{ "US_Michigan_708" , 63 },

		{ "US_Mississippi_607" , 64 },

		{ "US_Mississippi_708" , 65 },

		{ "US_Missouri_607" , 66 },

		{ "US_Missouri_708" , 67 },

		{ "US_Montana_607" , 68 },

		{ "US_Montana_708" , 69 },

		{ "US_Nebraska_707" , 70 },

		{ "US_Nebraska_808" , 71 },

		{ "US_Nevada_707" , 72 },

		{ "US_NewMexico_607" , 73 },

		{ "US_NewMexico_708" , 74 },

		{ "US_NorthCarolina_707" , 75 },

		{ "US_NorthCarolina_808" , 76 },

		{ "US_NorthDakota_707" , 77 },

		{ "US_Ohio_607" , 78 },

		{ "US_Ohio_708" , 79 },

		{ "US_Oklahoma_607" , 80 },

		{ "US_Oklahoma_708" , 81 },

		{ "US_Oregon_707" , 82 },

		{ "US_Oregon_808" , 83 },

		{ "US_Pennsylvania_607" , 84 },

		{ "US_Pennsylvania_708" , 85 },

		{ "US_South_Dakota_707" , 86 },

		{ "US_South_Dakota_808" , 87 },

		{ "US_South_Dakota_607" , 88 },

		{ "US_South_Dakota_708" , 89 },

		{ "US_Tennessee_607" , 90 },

		{ "US_Tennessee_708" , 91 },

		{ "US_Vermont_607" , 92 },

		{ "US_Vermont_708" , 93 },

		{ "US_Virginia_607" , 94 },

		{ "US_Virginia_708" , 95 },

		{ "US_Wisconsin_707" , 96 },

		{ "US_Wisconsin_808" , 97 },

		{ "US_Wyoming_607" , 98 },

		{ "US_Wyoming_708" , 99 }

	};




	string Cycle2Str(/*HOSCYCLE*/int cycle)
	{
		string rv = "";
		for (HOSCycleStruct c : _cycles_)
		{
			if (c.val == (int)cycle)
			{
				rv = string(c.pStr);
				break;
			}
		}
		return rv;
	}
	
#if 0
	std::string getCycleStrByIndex(int cycleInd)

	{

		std::string rv = "";

		int size = getHosCyclesSizeOfArray();

		if (cycleInd<size)

			rv = string(_cycles_[cycleInd].pStr);



		return rv;

	}

	HOSCYCLE getHosCycleByIndex(int cycleInd)

	{

		int rv = 0;

		int size = getHosCyclesSizeOfArray();

		if (cycleInd<size)

			rv = _cycles_[cycleInd].val;

		return (HOSCYCLE)rv;

	}



	std::string GetHosCycleList()

	{

		std::string rv = "";

		for (HOSCycleStruct c : _cycles_)

		{

			std::string rv1 = string(c.pStr) + "," + Int2Str(c.val) + "|";

			rv.append(rv1);



		}




		return rv;

	}
#endif
	int getHosCyclesSizeOfArray()

	{

		return (sizeof _cycles_ / sizeof _cycles_[0]);

	}
//==========================================================================================



bool HOSAgent::_30in_break = false;
DateTime HOSAgent::_30in_dt = DateTime::MinValue;

			void HOSAgent::set30MinBreak(bool v)
			{
				_30in_break = v;
				if (v)
				{
					_30in_dt = TimeUtil::DateTime_Now().AddMinutes(30);
				}
			}

			bool HOSAgent::is30MinBreak()
			{
				return _30in_break;
			}

			TimeSpan HOSAgent::get30MinRemaining()
			{
				DateTime dt1 = _30in_dt;
				DateTime dt2= TimeUtil::DateTime_Now();
				TimeSpan ts = dt1 - dt2;
				return ts;
			}

bool HOSAgent::_personal_driving = false;
float HOSAgent::_personal_use_odometer = 0;
DateTime HOSAgent::_personal_driving_dt;
DateTime HOSAgent::_emegency_driving_dt;

			void HOSAgent::set_personal_driving(bool v, float odo )
			{
				_personal_driving = v;
				if (v)
				{
					_personal_driving_dt = TimeUtil::DateTime_Now(); //.AddMinutes (30);
				}
				_personal_use_odometer = odo;
			}

			bool HOSAgent::is_personal_driving()
			{
				return _personal_driving;
			}

			TimeSpan HOSAgent::get_personal_driving_dtRemaining()
			{
				
				DateTime dt1 = _personal_driving_dt;
				DateTime dt2 = TimeUtil::DateTime_Now();
				TimeSpan ts = dt1 - dt2;
				return ts;
			}

bool HOSAgent::_emergency_driving = false;

			void HOSAgent::set_emergency_driving(bool v)
			{
				_emergency_driving = v;
				if (v)
				{
					_emegency_driving_dt = TimeUtil::DateTime_Now(); //.AddMinutes (30);
				}
			}

			bool HOSAgent::is_emergency_driving()
			{
				return _emergency_driving;
			}

			TimeSpan HOSAgent::get_emergency_driving_dtRemaining()
			{
				DateTime dt1 = _emegency_driving_dt;
				DateTime dt2 = TimeUtil::DateTime_Now();
				TimeSpan ts = dt1 - dt2;
				
				return ts;
			}

int HOSAgent::SelectedDriverStatus =(int) DutyStatus::Offduty;

			void HOSAgent::setSelectedDriverStatus(int status)
			{
				SelectedDriverStatus = status;
			}

			int HOSAgent::getSelectedDriverStatus()
			{
				return SelectedDriverStatus;
			}

			int HOSAgent::getSelectedDriverStatusIndexRow()
			{
				int ind = SelectedDriverStatus - static_cast<int>(DutyStatus::Offduty);
				return ind;
			}

			int HOSAgent::GetDriverStatusIndex(TimeLogRow *tlr)
			{
				int ind = static_cast<int>(tlr->Event()) - static_cast<int>(DutyStatus::Offduty);
				return ind;
			}

			void HOSAgent::setSelectedDriverStatusByIndexRow(int indexRow)
			{
				SelectedDriverStatus = indexRow + static_cast<int>(DutyStatus::Offduty);
			}

			int HOSAgent::getDriverStatusByIndexRow(int indexRow)
			{
				return (indexRow + static_cast<int>(DutyStatus::Offduty));
			}

			int HOSAgent::getIndexRowByDriverStatus(int status)
			{
				return (status - static_cast<int>(DutyStatus::Offduty));
			}

int HOSAgent::selectedIndexRow = 0;

			int HOSAgent::getJobsStatusSelectedIndexRow()
			{
				return selectedIndexRow;
			}

			void HOSAgent::setJobsStatusSelectedIndexRow(int ind)
			{
				selectedIndexRow = ind;
			}

			bool HOSAgent::isDutyStatusTransitioningFromOFFtoON(int currentDutyStatus, int requestedDutyStatus)
			{
				return ((currentDutyStatus == static_cast<int>(DutyStatus::Offduty) || currentDutyStatus == static_cast<int>(DutyStatus::Sleeping)) && (requestedDutyStatus == static_cast<int>(DutyStatus::Onduty) || requestedDutyStatus == static_cast<int>(DutyStatus::Driving)));
			}

			bool HOSAgent::isDutyStatusTransitioningFromONtoOFF(int currentDutyStatus, int requestedDutyStatus)
			{
				return ((requestedDutyStatus == static_cast<int>(DutyStatus::Offduty) || requestedDutyStatus == static_cast<int>(DutyStatus::Sleeping)) && (currentDutyStatus == static_cast<int>(DutyStatus::Onduty) || currentDutyStatus == static_cast<int>(DutyStatus::Driving)));
			}

			DateTime HOSAgent::getUtcDate()
			{
				return TimeUtil::DateTime_Now(); //DateTime.UtcNow;
			}

			long long HOSAgent::getEpochTime(DateTime dt)
			{
				//DateTime nd = DateTime(2010, 1, 1); //DateTime (1970, 0, 0);
				return 0;// static_cast<long long>((dt - nd).TotalMilliseconds);
			}

			long long HOSAgent::getEpochAtMidnight(DateTime currentDate)
			{
				//DateTime dt = DateTime(currentDate.Year,currentDate.Month,currentDate.Day);
				return 0;// getEpochTime(dt);
			}

			std::string HOSAgent::getDuration(DateTime transitionTimestamp, DateTime previousTransitionTimestamp)
			{
				//return getDuration(transitionTimestamp.getTime(), previousTransitionTimestamp.getTime(), true); 
				TimeSpan duration = transitionTimestamp - previousTransitionTimestamp;
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return "";// duration.ToString(R"(hh\:mm)");

			}

			int HOSAgent::GetMinutesFromMidNight(DateTime dt)
			{
				DateTime dt0 = dt.Date();//DateTime(dt.Year,dt.Month,dt.Day);
				TimeSpan ts = dt - dt0;
				return ts.TotalMinutes();
			}


			//===========================================================================================
			//===========================================================================================
#if 0 //TODO
			HOSCYCLE HOSAgent::GetCycleByIndex(int index)
			{
				//HOSCYCLE c = HOSCYCLE.Canadian_120hr_14day;
				HOSCYCLE cycle = static_cast<HOSCYCLE>((Enum::GetValues(HOSCYCLE::typeid))->GetValue(index));
				return cycle;
			}

			int HOSAgent::GetCycleIndexByCycle(HOSCYCLE cycle)
			{
				Type *type = cycle.GetType();
				//Type type1 = typeof(HOSCYCLE);
				Array *ar = Enum::GetValues(type);
				int index = Array::IndexOf(ar, cycle);
				return index;
			}

			int HOSAgent::NumOfCycles()
			{
				return Enum::GetNames(HOSCYCLE::typeid)->Length;
			}

			std::string HOSAgent::GetHosCycleStrByIndex(int index)
			{
				HOSCYCLE c = GetCycleByIndex(index);
				//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return c.ToString();
			}

			HOSCYCLE HOSAgent::selectedHosCycle = HOSCYCLE::NONE;

			std::string HOSAgent::GetSelectedHosCycleStr()
			{
				//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return selectedHosCycle.ToString();
			}

			int HOSAgent::GetSelectedCycleIndex()
			{
				return GetCycleIndexByCycle(selectedHosCycle);
			}

			void HOSAgent::SetSelectedCycleByIndex(int radioElementIndex)
			{
				selectedHosCycle = GetCycleByIndex(radioElementIndex);
			}

			HOSCYCLE HOSAgent::GetHosCycleByIndex(int ind)
			{
				return GetCycleByIndex(ind);
			}

			void HOSAgent::SetSelectedHosCycle(HOSCYCLE cycle)
			{
				selectedHosCycle = cycle;

			}

			HOSCYCLE HOSAgent::GetCurrentHosCycle()
			{
				return static_cast<HOSCYCLE>(selectedHosCycle);
			}

			std::string HOSAgent::GetCurrentHosCycleCountry()
			{
				std::string rv = "";
				//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				std::string str = GetCurrentHosCycle().ToString();

				if (str.find("CA_") != string::npos || str.find("Canadian") != string::npos)
				{
					rv = "CA";
				}
				else
				{
					if (str.find("US_") != string::npos)
					{
						rv = "US";
					}
				}
				return rv;
			}
#endif		

}
