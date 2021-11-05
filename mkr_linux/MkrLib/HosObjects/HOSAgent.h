#ifndef HOSAgent_H
#define HOSAgent_H

#include <string>
#include "CommonUtils/my_time.h"
#include "Definitions.h"

namespace HosEngine
{
			class HOSAgent
			{

		//============30 minutes break==============================
			private:
				static bool _30in_break;
				static DateTime _30in_dt;
			public:
				static void set30MinBreak(bool v);
				static bool is30MinBreak();
				static TimeSpan get30MinRemaining();
				//============Personal break==============================
			private:
				static bool _personal_driving;
				static float _personal_use_odometer;
				static DateTime _personal_driving_dt;
			public:
				static void set_personal_driving(bool v, float odo = 0);
				static bool is_personal_driving();

				static TimeSpan get_personal_driving_dtRemaining();
				//-------------------------------------------------
				//============Emergency driving==============================
			private:
				static bool _emergency_driving;
				static DateTime _emegency_driving_dt;
			public:
				static void set_emergency_driving(bool v);
				static bool is_emergency_driving();

				static TimeSpan get_emergency_driving_dtRemaining();

		//=======================================================================================

			private:
				static int SelectedDriverStatus; //-1;
			public:
				static void setSelectedDriverStatus(int status);
				static int getSelectedDriverStatus();
				static int getSelectedDriverStatusIndexRow();
				//static int GetDriverStatusIndex(TimeLogRow *tlr);

				static void setSelectedDriverStatusByIndexRow(int indexRow);
				static int getDriverStatusByIndexRow(int indexRow);
				static int getIndexRowByDriverStatus(int status);
				//===========Jobs Status===================
			private:
				static int selectedIndexRow;
			public:
				static int getJobsStatusSelectedIndexRow();
				static void setJobsStatusSelectedIndexRow(int ind);



				// DutyStatus Transition Checks

				static bool isDutyStatusTransitioningFromOFFtoON(int currentDutyStatus, int requestedDutyStatus);

				static bool isDutyStatusTransitioningFromONtoOFF(int currentDutyStatus, int requestedDutyStatus);


				//===========================================================================

				static std::string BsmDevice_BsmHome_Program_Language;
				static DateTime BsmDevice_BsmHome_Program_ShiftStart;



				//**************************** DateTime Utils ***********************
				static DateTime getUtcDate();
				static long long getEpochTime(DateTime dt); // like getTime()  in Java
				static long long getEpochAtMidnight(DateTime currentDate);
				static std::string getDuration(DateTime transitionTimestamp, DateTime previousTransitionTimestamp);
				static int GetMinutesFromMidNight(DateTime dt);

				//================================================================================
				//================================================================================
			private:
				static HOSCYCLE GetCycleByIndex(int index);
			public:
				static int GetCycleIndexByCycle(HOSCYCLE cycle);

				static int NumOfCycles();
				static std::string GetHosCycleStrByIndex(int index);
				//==========================================================
			private:
				static HOSCYCLE selectedHosCycle;
			public:
				static std::string GetSelectedHosCycleStr();

				static int GetSelectedCycleIndex();

				static void SetSelectedCycleByIndex(int radioElementIndex);
				static HOSCYCLE GetHosCycleByIndex(int ind);


				static void SetSelectedHosCycle(HOSCYCLE cycle);

				static HOSCYCLE GetCurrentHosCycle();



				static string GetCurrentHosCycleCountry();
			};


	//======================================================================
			
}
#endif