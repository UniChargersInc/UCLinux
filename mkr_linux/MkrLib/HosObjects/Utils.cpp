#include "Utils.h"
#include "Definitions.h"


namespace HosEngine
{
	

std::unordered_map<std::string, StateRow*> Utils::DefaultStates;

			std::string Utils::GetDriverDefaultState(EmployeeRow *currentAuditDriver)
			{
				if (DefaultStates.find(currentAuditDriver->id()) != DefaultStates.end())
				{
					return DefaultStates[currentAuditDriver->id()]->Abbr;
				}
				if (currentAuditDriver != nullptr)
				{
					if (!currentAuditDriver->state().empty())
					{
						return currentAuditDriver->state();
					}
					if (!currentAuditDriver->homeaddress().empty())
					{
#if 0
						StateRow *sr = AuditGlobalInstance::getStates().Find([&] (void *st)
						{
//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
							return  currentAuditDriver->HomeAddress->Replace(" ", "")->find(std::string(",") + st::StateName + std::string(",")) > 0 || currentAuditDriver->HomeAddress->Replace(" ", "")->find(std::string(",") + st::Abbr + std::string(",")) > 0;
						});
						if (sr != nullptr)
						{
							DefaultStates[currentAuditDriver->id()] = sr;
							return sr->Abbr;
						}
#endif
					}
				}
				return "";
			}

			bool Utils::IsUSA(EmployeeRow *currentAuditDriver)
			{
				if (DefaultStates.find(currentAuditDriver->id()) != DefaultStates.end())
				{
					return DefaultStates[currentAuditDriver->id()]->Country == "US"?true:false;
				}
				if (currentAuditDriver != nullptr)
				{
#if 0
					if (!currentAuditDriver->HomeAddress.empty())
					{
						StateRow *sr = AuditGlobalInstance::getStates().Find([&] (void *st)
						{
//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
							return currentAuditDriver->HomeAddress->Replace(" ", "")->find(std::string(",") + st::StateName + std::string(",")) > 0 || currentAuditDriver->HomeAddress->Replace(" ", "")->find(std::string(",") + st::Abbr + std::string(",")) > 0;
						});
						if (sr != nullptr)
						{
							DefaultStates[currentAuditDriver->id()] = sr;
							return sr->Country == "US" ? true : false;
						}
					}
#endif
				}
				return false;
			}

std::unordered_map<std::string, DateTime> AuditGlobalInstance::LastThirtyMinuteOffDuty;
std::vector<StateRow*> AuditGlobalInstance::states;
			std::vector<StateRow*> AuditGlobalInstance::getStates() 
			{

				if (states.empty())
				{
					states = std::vector<StateRow*>();
					StateRow *sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Alberta";
					sr->ID = 3;
					sr->Abbr = "AB";

					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "British Columbia";
					sr->ID = 6;
					sr->Abbr = "BC";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Manitoba";
					sr->ID = 24;
					sr->Abbr = "MB";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "New Brunswick";
					sr->ID = 35;
					sr->Abbr = "NB";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Newfoundland";
					sr->ID = 36;
					sr->Abbr = "NL";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Nova Scotia";
					sr->ID = 41;
					sr->Abbr = "NS";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Northwest Territories";
					sr->ID = 40;
					sr->Abbr = "NT";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Nunavut";
					sr->ID = 42;
					sr->Abbr = "NU";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Ontario";
					sr->ID = 45;
					sr->Abbr = "ON";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "PEI";
					sr->ID = 47;
					sr->Abbr = "PE";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Quebec";
					sr->ID = 49;
					sr->Abbr = "QC";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Saskatchewan";
					sr->ID = 51;
					sr->Abbr = "SK";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "CA";
					sr->StateName = "Yukon";
					sr->ID = 55;
					sr->Abbr = "YT";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Alaska";
					sr->ID = 2;
					sr->Abbr = "AK";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Alabama";
					sr->ID = 1;
					sr->Abbr = "AL";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Arkansas";
					sr->ID = 5;
					sr->Abbr = "AR";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Arizona";
					sr->ID = 4;
					sr->Abbr = "AZ";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "California";
					sr->ID = 7;
					sr->Abbr = "CA";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Colorado";
					sr->ID = 8;
					sr->Abbr = "CO";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Connecticut";
					sr->ID = 9;
					sr->Abbr = "CT";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "District of Columbia";
					sr->ID = 11;
					sr->Abbr = "DC";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Delaware";
					sr->ID = 10;
					sr->Abbr = "DE";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Florida";
					sr->ID = 12;
					sr->Abbr = "FL";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Georgia";
					sr->ID = 13;
					sr->Abbr = "GA";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Hawaii";
					sr->ID = 15;
					sr->Abbr = "HI";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Iowa";
					sr->ID = 19;
					sr->Abbr = "IA";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Idaho";
					sr->ID = 16;
					sr->Abbr = "ID";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Illinois";
					sr->ID = 17;
					sr->Abbr = "IL";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Indiana";
					sr->ID = 18;
					sr->Abbr = "IN";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Kansas";
					sr->ID = 20;
					sr->Abbr = "KS";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Kentucky";
					sr->ID = 21;
					sr->Abbr = "KY";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Louisiana";
					sr->ID = 22;
					sr->Abbr = "LA";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Massachusetts";
					sr->ID = 26;
					sr->Abbr = "MA";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Maryland";
					sr->ID = 25;
					sr->Abbr = "MD";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Maine";
					sr->ID = 23;
					sr->Abbr = "ME";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Michigan";
					sr->ID = 27;
					sr->Abbr = "MI";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Minnesota";
					sr->ID = 64;
					sr->Abbr = "MN";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Missouri";
					sr->ID = 29;
					sr->Abbr = "MO";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Mississippi";
					sr->ID = 28;
					sr->Abbr = "MS";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Montana";
					sr->ID = 30;
					sr->Abbr = "MT";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "North Carolina";
					sr->ID = 38;
					sr->Abbr = "NC";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "North Dakota";
					sr->ID = 39;
					sr->Abbr = "ND";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Nebraska";
					sr->ID = 31;
					sr->Abbr = "NE";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "New Hampshire";
					sr->ID = 14;
					sr->Abbr = "NH";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "New Jersey";
					sr->ID = 33;
					sr->Abbr = "NJ";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "New Mexico";
					sr->ID = 34;
					sr->Abbr = "NM";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Nevada";
					sr->ID = 32;
					sr->Abbr = "NV";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "New York";
					sr->ID = 37;
					sr->Abbr = "NY";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Ohio";
					sr->ID = 43;
					sr->Abbr = "OH";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Oklahoma";
					sr->ID = 44;
					sr->Abbr = "OK";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Oregon";
					sr->ID = 46;
					sr->Abbr = "OR";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Pennsylvania";
					sr->ID = 48;
					sr->Abbr = "PA";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Rhode Island";
					sr->ID = 50;
					sr->Abbr = "RI";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "South Carolina";
					sr->ID = 52;
					sr->Abbr = "SC";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "South Dakota";
					sr->ID = 53;
					sr->Abbr = "SD";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Tennessee";
					sr->ID = 54;
					sr->Abbr = "TN";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Texas";
					sr->ID = 56;
					sr->Abbr = "TX";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Utah";
					sr->ID = 57;
					sr->Abbr = "UT";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Virginia";
					sr->ID = 59;
					sr->Abbr = "VA";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Vermont";
					sr->ID = 58;
					sr->Abbr = "VT";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Washington";
					sr->ID = 60;
					sr->Abbr = "WA";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Wisconsin";
					sr->ID = 62;
					sr->Abbr = "WI";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "West Virginia";
					sr->ID = 61;
					sr->Abbr = "WV";
					states.push_back(sr);

					sr = MKR_NEW StateRow();
					sr->Country = "US";
					sr->StateName = "Wyoming";
					sr->ID = 63;
					sr->Abbr = "WY";
					states.push_back(sr);
				}
				return states;
			}

}
