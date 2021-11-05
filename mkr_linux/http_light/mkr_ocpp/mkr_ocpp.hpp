#pragma once

#include <fstream>
#include <iostream>
#include <set>
#include <streambuf>
#include <string>
#include <vector>

#include "cereal/no_pretty_json.hpp"
#include <sstream>
#include "protocol16/authorize.hpp"
#include "protocol16/authorize_response.hpp"
#include "protocol16/boot_notification.hpp"
#include "protocol16/boot_notification_response.hpp"

#include "protocol16/start_transaction.hpp"
#include "protocol16/start_transaction_response.hpp"

#include "protocol16/stop_transaction.hpp"
#include "protocol16/stop_transaction_response.hpp"

#include "protocol16/set_charging_profile.hpp"
#include "protocol16/set_charging_profile_response.hpp"

#include "protocol16/heartbeat.hpp"
#include "protocol16/heartbeat_response.hpp"

#include "protocol16/status_notification.hpp"
#include "protocol16/status_notification_response.hpp"

#include "protocol16/meter_values.hpp"
#include "protocol16/meter_values_response.hpp"

#include "protocol16/reset.hpp"
#include "protocol16/reset_response.hpp"

#include "protocol16/update_firmware.hpp"
#include "protocol16/update_firmware_response.hpp"

#include "protocol16/clear_charging_profile.hpp"
#include "protocol16/clear_charging_profile_response.hpp"

#include "protocol16/reserve_now.hpp"
#include "protocol16/reserve_now_response.hpp"

#include "protocol16/cancel_reservation.hpp"
#include "protocol16/cancel_reservation_response.hpp"

#include "protocol16/remote_start_transaction.hpp"
#include "protocol16/remote_start_transaction_response.hpp"

#include "protocol16/remote_stop_transaction.hpp"
#include "protocol16/remote_stop_transaction_response.hpp"

#include "protocol16/get_configuration.hpp"
#include "protocol16/get_configuration_response.hpp"

#include "protocol16/change_configuration.hpp"
#include "protocol16/change_configuration_response.hpp"

#include "protocol16/get_local_list_version.hpp"
#include "protocol16/get_local_list_version_response.hpp"

#include "protocol16/change_availability.hpp"
#include "protocol16/change_availability_response.hpp"

#include "protocol16/firmware_status_notification.hpp"
#include "protocol16/firmware_status_notification_response.hpp"

#include "protocol16/send_local_list.hpp"
#include "protocol16/send_local_list_response.hpp"

#include "protocol16/data_transfer.hpp"
#include "protocol16/data_transfer_response.hpp"

enum OCPP_DB_TYPE {
	DBT_None,
	DBT_Authorize,
	DBT_StsNotification,
	DBT_StartTransaction,
	DBT_StopTransaction,
	DBT_MeterValues,
	DBT_LOG_MSG
};

struct OCPP_Request
{
	int callID=0;
	std::string uniqueID="";
	std::string action="";
	std::string req_str="";
	std::string remoteIP="";
	
	std::string sern = "";    // this info is returned on BootNotification in CentralOcppDB::getSerNumberByIPAddr
	int cert_flags = -1;        // need to determine if this station is public or home use only  -> bit 2
};

struct OCPP_Response
{
	int callID;
	std::string uniqueID;
	std::vector<std::string> resp_str;  // in case we need to send several messages  // StartTransaction response to Charge station , then SetChargingProfile Request from central station
};

typedef struct {
	int cid = 0; // client id
	std::string ser_number="";
	int64_t ts ;  // should be in UTC !!!
	
	std::string status="*";
	std::string errCode="*";
	std::string fwVer = "";
	std::string sensors="*";
	OCPP_DB_TYPE db_type= OCPP_DB_TYPE::DBT_None;
} Rtt_Ocpp_T;

enum ChargeStationStatus
{
	UNAVAILABLE=0,
	AVAILABLE,
	CHARGING
};


enum RegistrationStatus
{
	RS_ACCEPTED,
	RS_PENDING,
	RS_REJECTED,

};
//7.2. AuthorizationStatus
enum AuthorizationStatus
{
	AUTH_ACCEPTED,
	AUTH_BLOCKED,
	AUTH_EXPIRED,
	AUTH_INVALID
};


enum OCPP_COMMAND
{
	OCPP_CMD_GetStatistics = 100,
	OCPP_CMD_SetChargeLimit = 101,
	OCPP_CMD_SendRemoteTransaction = 102,
	Ocpp_SimpleCommand = 103,
	OCPP_CMD_SetSampleInterval = 104,

	Ocpp_SimpleCommand_SaveProfile = 200,
	Ocpp_SimpleCommand_ClearProfile = 201,
	Ocpp_SimpleCommand_NewProfile = 202,
	Ocpp_SimpleCommand_GetProfile = 203
};

class RequestQueue
{
public:
	void addToQueue(OCPP_Request &v)
	{
		_elements.push_back(v);
	}
	string getActionByUID(string &uid)
	{
		string rv = "";
		for (std::vector<OCPP_Request>::iterator it = _elements.begin(); it != _elements.end(); ++it)
		{
			if (it->uniqueID == uid)
			{
				rv = it->action;
				_elements.erase(it);
				break;
			}
		}
		return rv;
	}
private:
	vector<OCPP_Request> _elements;
};

#define OCPP_CPNSOLE_SIMULATION	1 
#define OCPP_PRINTF m_printf
#define OCPP_EXC_PRINTF m_printf

//#define OCPP_PRINTF printf

bool processOCPPIncoming(OCPP_Request &req, OCPP_Response &resp);
bool setChargeStationStatus(std::string& ip_addr, ChargeStationStatus sts);


bool sendResponse2Client(std::string forIP, std::string &str);

//central station originated requests
int createSetChargingProfileRequest(OCPP_Request &req);
int createClearChargingProfileRequest(OCPP_Request &req);
int createGetConfigurationRequest(OCPP_Request &req);
int createRemoteStopTransactionRequest(OCPP_Request &req);
int createRemoteStartTransactionRequest(OCPP_Request &req, string idTag);
int createCancelReservationRequest(OCPP_Request &req, int reservId);
int createReserveNowRequest(OCPP_Request &req, string idTag,int connId,int reservId);
int createUpdateFirmwareRequest(OCPP_Request &req);
int createResetRequest(OCPP_Request &req,const char* type);
int createChangeConfigurationRequest(OCPP_Request &req,const char* key,const char *val);
int createChangeAvailabilityRequest(OCPP_Request &req, int connId, bool flag);
int createSendLocalListRequest(OCPP_Request &req);

int createDataTransferRequest(OCPP_Request &req, const char* messageID, const char *data);
//from Web
int createUpdateFWRequestfromWeb(std::string serN,int cmd);
int createConfigReqFromWeb(std::string serN, std::string forField);
int createGetLocalListVersionReqFromWeb(std::string serN);

int createRemoteStartTransactionfromWeb(std::string serN, std::string idTag);
int createRemoteStopTransactionfromWeb(std::string serN);
int createReserveNowfromWeb(std::string serN, std::string idTag);
int createCancelReservationfromWeb(std::string serN);
int createChangeAvailabilityfromWeb(std::string serN,int connId,bool flag);
int createResetfromWeb(std::string serN, bool flag);
//int createChangeConfigurationIntervalfromWeb(std::string serN, const char *key, int val);
int createChangeConfigurationIntervalfromWeb(std::string serN, const char *key, const char* val);
int createGetCompositeSchedulefromWeb(std::string serN);

int createTestChargingProfilefromWeb(std::string serN, int cmd);

int createUpdateTZfromWeb(std::string serN, int tz,int dls);

//*****************************************************************
int createSendOcppCommandfromMob(std::string &serN,int cid,int cmd,int par1, std::string &par2);