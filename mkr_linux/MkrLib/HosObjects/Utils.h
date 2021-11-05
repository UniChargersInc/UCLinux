#ifndef MKR_UTILS_H
#define MKR_UTILS_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Definitions.h"

namespace HosEngine
{
	
			class Utils
			{

				//Right now in employee or timelog table, there is not state value. Temporarily solution, using defaultStates to store employee default state 
			public:
				static std::unordered_map<std::string, StateRow*> DefaultStates;


				//Right now in employee or timelog table, there is not state value. Temporarily solution, using defaultStates to store employee default state 
				static std::string GetDriverDefaultState(EmployeeRow *currentAuditDriver);

				static bool IsUSA(EmployeeRow *currentAuditDriver);

			};


			class AuditGlobalInstance
			{
			public:
				static std::unordered_map<std::string, DateTime> LastThirtyMinuteOffDuty;
			private:
				static std::vector<StateRow*> states;

			public:
				static std::vector<StateRow*> getStates() ;

			};


}

#endif