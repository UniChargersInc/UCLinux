#ifndef SHAREDOBJECTS_H
#define SHAREDOBJECTS_H

#include <string>
#include <vector>
#include "CommonUtils/my_time.h"

//C# TO C++ CONVERTER NOTE: Forward class declarations:


namespace HosEngine
{
	class AuditTime;
	class RecapRowAudit;

	enum class HOSCYCLE
	{
		Canadian_70hr_7day = 6,
		Canadian_120hr_14day = 7,

		US_60hr_7day = 10,
		US_70hr_8day = 11,
		Canadian_Oil_Field = 18,

		NONE = 19,

		US_Oil_Field = 20,
		//new rules for Kiewit  -> 
		US_Texas = 30,
		US_California = 31, //intra state rules
							//US_NewYork=32,
		US_Florida_707 = 32,
		US_Florida_808 = 33,
		US_Alaskan_707 = 34,
		US_Alaskan_808 = 35,

		CA_AlbertaRule = 36,
		US_NewJersey_707 = 38,
		US_NewJersey_808 = 39,
		US_Georgia_707 = 40,
		US_Georgia_808 = 41,
		US_Alabama_607 = 42,
		US_Alabama_708 = 43,
		US_Arizona_607 = 44,
		US_Arizona_708 = 45,
		US_Connecticut_607 = 46,
		US_Connecticut_708 = 47,
		US_Delaware_607 = 48,
		US_Delaware_708 = 49,
		US_Idaho_607 = 50,
		US_Idaho_708 = 51,
		US_Indiana_607 = 52,
		US_Indiana_708 = 53,
		US_Iowa_707 = 54,
		US_Iowa_808 = 55,
		US_Kansas_607 = 56,
		US_Kansas_708 = 57,
		US_Louisiana_607 = 58,
		US_Louisiana_708 = 59,
		US_Maryland_707 = 60,
		US_Maryland_808 = 61,
		US_Michigan_607 = 62,
		US_Michigan_708 = 63,
		US_Mississippi_607 = 64,
		US_Mississippi_708 = 65,
		US_Missouri_607 = 66,
		US_Missouri_708 = 67,
		US_Montana_607 = 68,
		US_Montana_708 = 69,
		US_Nebraska_707 = 70,
		US_Nebraska_808 = 71,
		US_Nevada_707 = 72,
		US_NewMexico_607 = 73,
		US_NewMexico_708 = 74,
		US_NorthCarolina_707 = 75,
		US_NorthCarolina_808 = 76,
		US_NorthDakota_707 = 77,
		US_Ohio_607 = 78,
		US_Ohio_708 = 79,
		US_Oklahoma_607 = 80,
		US_Oklahoma_708 = 81,
		US_Oregon_707 = 82,
		US_Oregon_808 = 83,
		US_Pennsylvania_607 = 84,
		US_Pennsylvania_708 = 85,
		US_South_Dakota_707 = 86,
		US_South_Dakota_808 = 87,
		US_South_Dakota_607 = 88,
		US_South_Dakota_708 = 89,
		US_Tennessee_607 = 90,
		US_Tennessee_708 = 91,
		US_Vermont_607 = 92,
		US_Vermont_708 = 93,
		US_Virginia_607 = 94,
		US_Virginia_708 = 95,
		US_Wisconsin_707 = 96,
		US_Wisconsin_808 = 97,
		US_Wyoming_607 = 98,
		US_Wyoming_708 = 99,
	};
	enum class DataRowIdentifier
	{
		ActivationPacket = 1,
		SubscriptionPacket = 2,
		EmailSettingPacket = 3,
		SendDataPacket = 4,
		InstanceEmailPacket = 5,
		RestoreDataPacket = 7,
		ReturnMessagePacket = 8,
		GetExpireDatePacket = 9,
		GetCompanyInfoPacket = 10,
		ReturnMessagePacketEx = 11,
		EmailSettingPacketEx = 12,
		ReturnMessagePacketEx2 = 13,
		InstanceEmailPacketEx = 14,
		ActivationPacketEx = 15,
		TripInfoRowEx = 84,
		TeamDriverRow = 85,
		EmployeeRowEx = 86,
		TimeDiscrepancyRow = 87,
		CustomInspectionCategory = 88,
		InspectionCategory = 89,
		InspectionRow = 90,
		InspectionItem = 91,
		CustomInspectionItem = 92,
		CompanyInfoRowEx = 93,
		InspectorSignatureRow = 94,
		EmailSettings = 95,
		CompanyInfoRow = 96,
		TimeLogHistoryRow = 97,
		CustomLogBookStop = 98,
		CustomLocationInfoRow = 99,
		DetailLogRow = 100,
		EmployeeRow = 101,
		LocationInfoRow = 102,
		LogLocationRow = 103,
		RecapRow = 104,
		RuleHourOverAllRow = 105,
		RuleHoursRow = 106,
		RuleHourSelectionRow = 107,
		StateRow = 108,
		StateNeighborRow = 109,
		TimeAvailableRow = 110,
		TimeLogRow = 111,
		TripInfoRow = 112,
		RecapDescRow = 113,
		SignatureRow = 114,
		LogBookStop = 115,
		ForcastRuleHourRow = 116,
		RuleSelectionHistoryRow = 117,
		DisplaySetting = 118,
		DeferHourRow = 119,
		ExpireDate = 120,
		NewDriversPacket = 121,
		DashBoardPacket = 122,
		SyncLogSheet = 123,

		UpdateVersion = 124,
		UpdateVersionFile = 125,

		GetReverseGeoCoding = 126,

		ConfigurationSetting = 127,
		SyncLogSheetDaysPacket = 65,
		InspectionRowExt = 66,
		//05.12
		SendDataPacketNew = 67,

		RequestUserInfo = 200, //Devin
		RequestMissingData = 201, //Devin

		AntSyncData = 211, //Added for TDSB project; To sync user/inspection&category with the backend server

						   //Devin 08.13
		AntInspectionData = 214, //Added for TDSB project; To send inspeciton forms back
		RequestForVehicleInfo = 215,
		RequestForInspectionReports = 216,


		ShiftStartTime = 255
	};

	enum class TimeLogType
	{
		Manual, //when the data entry is generated by user
		Auto, //when the data entry is generated by program
		Add, //special for recap data entry
		Modified //mark this as a modified record, to be used on central server
	};
	enum class RuleExceptions
	{
		None = 0,
		USA_24_hour_cycle_reset = 1, //24-hour cycle reset $395.1
		USA_Oilfield_waiting_time = 2, //Oilfield waiting time $395.1
		USA_100_air_mile_radius = 4, //"Short-haul operations, 100 air< span style="font-style:italic;color:rgb(136,136,136)"> mile radius driver $395.1
		USA_Transportation_construction_Materialsandequipment = 8, //" has the meaning assigned by Title 49, Code of Federal Regulations, §395.2.
		USA_150_air_mile_radius = 16, //"Short-haul operations, 100 air< span style="font-style:italic;color:rgb(136,136,136)"> mile radius driver $395.1
	};

	enum class LOGSTATUS
	{
		OffDuty = 101,
		Sleeping = 102,
		Driving = 103,
		OnDuty = 104,

		AssignDriverToVehicle = 200,
		/*DeleteStatus*/		DeleteThisTimeLog = 201,
		RequestForVehicleDescription = 202,


	};
	enum class RULEID
	{
		CA_Driving = 0,
		CA_OnDuty = 1,
		CA_REST = 2,
		CA_16Hour = 3,
		CA_OffDuty = 4,
		CA_70Hour = 5,
		CA_Service_707 = 6,
		CA_Service_12014 = 7,
		US_Driving = 8,
		US_OnDuty = 9,
		US_Service_607 = 10,
		US_Service_708 = 11,
		CA_Service_607 = 12,
		CA_Service_708 = 13,
		CA_Service = 14,
		US_ChangeCycle = 15,
		CA_ChangeCycle = 16,
		CA_Combine = 17,
		CA_OilField = 18,
		NONE = 19,
		US_OilField = 20,
		Thirty_Minutes_Break = 21,

		//New rule for Kiewit
		US_Texas = 30,
		US_California = 31,
		//US_NewYork = 32,
		US_Florida_707 = 32,
		US_Florida_808 = 33,
		US_Alaskan_707 = 34,
		US_Alaskan_808 = 35,
		CA_AlbertaRule = 36,
		Ten_Minutes_Break = 37,
		US_NewJersey_707 = 38,
		US_NewJersey_808 = 39,
		US_Georgia_707 = 40,
		US_Georgia_808 = 41,
		//US_Gergia = 34
		US_Alabama_607 = 42,
		US_Alabama_708 = 43,

		US_Arizona_607 = 44,
		US_Arizona_708 = 45,

		US_Connecticut_607 = 46,
		US_Connecticut_708 = 47,

		US_Delaware_607 = 48,
		US_Delaware_708 = 49,

		US_Idaho_607 = 50,
		US_Idaho_708 = 51,

		US_Indiana_607 = 52,
		US_Indiana_708 = 53,

		US_Iowa_707 = 54,
		US_Iowa_808 = 55,

		US_Kansas_607 = 56,
		US_Kansas_708 = 57,

		US_Louisiana_607 = 58,
		US_Louisiana_708 = 59,

		US_Maryland_707 = 60,
		US_Maryland_808 = 61,

		US_Michigan_607 = 62,
		US_Michigan_708 = 63,

		US_Mississippi_607 = 64,
		US_Mississippi_708 = 65,

		US_Missouri_607 = 66,
		US_Missouri_708 = 67,

		US_Montana_607 = 68,
		US_Montana_708 = 69,

		US_Nebraska_707 = 70,
		US_Nebraska_808 = 71,

		US_Nevada_707 = 72,

		US_NewMexico_607 = 73,
		US_NewMexico_708 = 74,

		US_NorthCarolina_707 = 75,
		US_NorthCarolina_808 = 76,

		US_NorthDakota_707 = 77,

		US_Ohio_607 = 78,
		US_Ohio_708 = 79,

		US_Oklahoma_607 = 80,
		US_Oklahoma_708 = 81,

		US_Oregon_707 = 82,
		US_Oregon_808 = 83,

		US_Pennsylvania_607 = 84,
		US_Pennsylvania_708 = 85,

		US_South_Dakota_707 = 86,
		US_South_Dakota_808 = 87,

		US_South_Dakota_607 = 88,
		US_South_Dakota_708 = 89,

		US_Tennessee_607 = 90,
		US_Tennessee_708 = 91,

		US_Vermont_607 = 92,
		US_Vermont_708 = 93,

		US_Virginia_607 = 94,
		US_Virginia_708 = 95,

		US_Wisconsin_707 = 96,
		US_Wisconsin_808 = 97,


		US_Wyoming_607 = 98,
		US_Wyoming_708 = 99,
	};

	enum class AlertTypes
	{
		HoursViolatingAlert = 1,
		UnsignInDrivingAlert = 2,
		PreTripInspectionAlert = 3, // driving without pretrip inspection after shift started
		DrivingWithMajorDefect = 4,
		PostTripInspectionLogout = 5
	};

	enum class DutyStatus
			{
				Onduty = 104,
				Offduty = 101,
				Driving = 103,
				Sleeping = 102
			};
	enum class RuleType
	{
		Day,
		Shift
	};

			class Cycle
			{
			public:
				RULEID rule;
				DateTime selectTime;
				RuleExceptions ruleException = RuleExceptions::None;
			};

			enum class StopAction
			{
				Standby = 2000,
				DeferDay1 = 1000,
				DeferDay2 = 1001
			};
			enum class AuditJurisdiction
			{
				US = 1,
				Canada = 2,
				CA_North = 3,
				US_North = 4,
				UnKnown
			};
			class Timelog //: public IComparable
			{
				bool _clone_flag = false;
			public:
				DateTime Logtime;
				AuditJurisdiction Location = AuditJurisdiction::UnKnown;
				DutyStatus duty = DutyStatus::Offduty;
				double Latitude = 0;
				double Longitude = 0;
				int QualifyRadiusRule=0;//Nullable<bool> QualifyRadiusRule = Nullable<bool>();
				int stop = 0;
				int odometer = 0;
				std::string ToString() ;
				Timelog *Clone();
				bool isClone() { return _clone_flag; }
				bool operator>(Timelog &tl);  // requires for sort operations
				bool operator<(Timelog &tl);
				int CompareTo(void *obj);
			};

			class AuditRequest
			{
			public:
				std::vector<Cycle*> CycleList;
				std::vector<Timelog*> timelog;
				~AuditRequest()
				{
					for (Cycle *c : CycleList)
					{
						if (c != 0)
							delete c;
					}
					CycleList.clear();
					
					for (Timelog *t : timelog)
					{
						if (t != 0)
						{
							delete t;
							t = 0;
						}
					}
					timelog.clear();
					
				}
			};

			class RecapRowAudit
			{
			public:
				DateTime Day;
				TimeSpan DayUsed = TimeSpan(0);
				TimeSpan CycleTotal = TimeSpan(0);
				TimeSpan CycleAvail = TimeSpan(0);
//C# TO C++ CONVERTER NOTE: The variable Cycle was renamed since it is named the same as a user-defined type:
				std::string Cycle_Renamed = "-";
			};
			class AuditTime
			{
			public:
				std::string DispName = "";
				std::string RuleDescription = "";
				std::string ViolationDescription = "";
				RULEID rule;
				TimeSpan used;
				TimeSpan available;
				RuleType ruletype = RuleType::Day;
				TimeSpan ondutyavailable = TimeSpan::MaxValue;

				int maxdriving = 0;
				double shiftOndty = 0;
				bool notNeedCal = false;

				AuditTime();
				std::string ToString();
			};
			class AuditResultQuick
			{
			public:
				std::vector<AuditTime*> AuditTimes;
				//C# TO C++ CONVERTER NOTE: The variable AuditTime was renamed since it is named the same as a user-defined type:
				DateTime AuditTime_Renamed = DateTime::MinValue;
				RecapRowAudit *AuditRecap = 0;// new RecapRowAudit();

				~AuditResultQuick()
				{
					for (AuditTime *aq : AuditTimes)
					{
						if (aq != 0)
							delete aq;
					}
					AuditTimes.clear();
					if (AuditRecap != 0)
						delete AuditRecap;
				}
			};
						
			
			class AuditResult
			{
			public:
				std::vector<AuditTime*> AuditTimes;
				std::vector<RecapRowAudit*> recap;

				~AuditResult()
				{
					for (AuditTime *o : AuditTimes)
					{
						if (o != 0)
							delete o;
					}
					AuditTimes.clear();

					for (RecapRowAudit *t : recap)
					{
						if (t != 0)
							delete t;
					}
					recap.clear();
				}

			};

			typedef struct {
				char *pStr;
				int val;
			}HOSCycleStruct;

//Helper function declaration 
			std::string Cycle2Str(/*HOSCYCLE*/int c);
			std::string GetHosCycleList();

} //namespace HosEngine





#endif