
#include "CommonUtils/my_time.h"
#include <MkrLib.h>

#include "mkr_ocpp.hpp"
#include "sip_helpers.h"

#include <google/protobuf/stubs/stringprintf.h>
#include "db/CentralOcppDB.h"

#include "CommonUtils/my_utils.h"
#include "httplib.h"

#define OCPP_PRINTF m_printf
//#define OCPP_PRINTF 


using namespace lgpl3::ocpp16;


string Global_GenerateTextUID();


RequestQueue request_queue;

static void create_central_request(OCPP_Request &req, std::stringstream &ss)
{
	req.callID = 2;
	req.uniqueID = Global_GenerateTextUID();
	//req.action = "SetChargingProfile";
	req.req_str = google::protobuf::StringPrintf("[%d,\"%s\",\"%s\",%s]", req.callID, req.uniqueID.c_str(), req.action.c_str(), ss.str().c_str());

	request_queue.addToQueue(req);
}

//=============================Charging Profile==================================================

void fillChargingSchedule(ChargingSchedule& cS, schema_date_time *startDate)
{
	cS.chargingRateUnit = ChargingSchedule::ChargingRateUnitEnum::from_string("W");

	cS.duration = 222;  //optional<int> duration;
	cS.minChargingRate = 22.2; // optional<double> minChargingRate;
	if (startDate != nullptr) cS.startSchedule = *startDate; // optional<schema_date_time> startSchedule;
	int start = 0;
	for (int i = 0; i < 1; i++)
	{
		ChargingSchedulePeriodEntry cspe;
		cspe.numberPhases = 1;
		cspe.limit = 32.0;
		start += (33 * i);
		cspe.startPeriod = start;
		cS.chargingSchedulePeriod.push_back(cspe);
	}

}
void fillSetChargingProfileRequest(SetChargingProfileRequest &rr)
{
	rr.connectorId = -1;
	CsChargingProfiles &cpf = rr.csChargingProfiles;
	cpf.chargingProfileId = -1;
	cpf.chargingProfileKind = CsChargingProfiles::ChargingProfileKindEnum::from_string("Absolute");
	cpf.chargingProfilePurpose = CsChargingProfiles::ChargingProfilePurposeEnum::from_string("ChargePointMaxProfile");
	
	cpf.recurrencyKind = CsChargingProfiles::RecurrencyKindEnum::from_string("Daily");

	cpf.stackLevel=22;  // invalid profile
	cpf.transactionId=-3;  // optional

	//optional<schema_date_time> validFrom;
	//optional<schema_date_time> validTo;
	cereal::schema_data_time dt;
	dt.set_now();
	cpf.validFrom = dt;

	auto millis = dt.value().counter()/1000000;
	int nDays = 4;
	millis += (3600 * 24 * nDays);

	cereal::schema_data_time dt1;
	dt1.set_millis(millis);
	cpf.validTo = dt1;

	fillChargingSchedule(cpf.chargingSchedule,&dt);
}

int createSetChargingProfileRequest(OCPP_Request &req)
{
	SetChargingProfileRequest rr;
	//std::string prof_name = "Profile_1";
	//std::string ip_addr = req.remoteIP;
	std::string data;
	int rv = CentralOcppDB::Get()->getCSProfileFromDBByIp(-1, req, data);
	std::stringstream ss;
	if (rv && !data.empty())
	{
		ss << data;
		OCPP_PRINTF("set charging prof..uid:%s-- %s", req.uniqueID.c_str(),data.c_str());
	}
	else {
		// consider it as a default profile
		fillSetChargingProfileRequest(rr);
		{
			cereal::JSONOutputArchive archive(ss);
			rr.serialize(archive);
		}
		OCPP_PRINTF("set default charging prof..");
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
//=========================================================
int createClearChargingProfileRequest(OCPP_Request &req)
{
	ClearChargingProfileRequest rr;
	//rr.id = 1;            //optional the ID of charging profile
	//rr.connectorId = 1;  // optional

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
int createResetRequest(OCPP_Request &req, const char* type)
{
	ResetRequest rr;
	rr.type = type;
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}

//---------------------------------------------------------
int createSendLocalListRequest(OCPP_Request &req)
{
	SendLocalListRequest rr;
	
	// fake it for a test
	rr.listVersion = 11;
	rr.updateType = SendLocalListRequest::UpdateTypeEnum::from_string("Full");
	schema_array<LocalAuthorizationListEntry, 65536, 0> lal1;
	
	for (int i = 0; i < 10; i++)
	{
		LocalAuthorizationListEntry lal;
		lal.idTag = "40110009ef";
		IdTagInfo ti;
		ti.status = IdTagInfo::StatusEnum::from_string("Accepted");
		lal.idTagInfo = ti;


		lal1.push_back(lal);
	}
	rr.localAuthorizationList = lal1;

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
//=========================================================
int createUpdateFirmwareRequest(OCPP_Request &req)
{
	UpdateFirmwareRequest rr;
	rr.location = "*";
	rr.retries = 3;  //optional
	cereal::schema_data_time dt;
	dt.set_now();
	rr.retrieveDate = dt;
	rr.retryInterval = 120;

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}

//*****************************************************************************************
//int createRemoteStopTransactionRequest(OCPP_Request &req);
//int createRemoteStartTransactionRequest(OCPP_Request &req, string idTag);
int createCancelReservationRequest(OCPP_Request &req);
int createReserveNowRequest(OCPP_Request &req, string idTag);
int createReserveNowfromWeb(std::string serN, std::string idTag)
{
	OCPP_Request req;
	int reservId = 3,connId=1;
	int rv = createReserveNowRequest(req, idTag, connId, reservId);
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
int createCancelReservationfromWeb(std::string serN)
{
	OCPP_Request req;
	int reservId = 3;
	int rv = createCancelReservationRequest(req, reservId);
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
int createChangeAvailabilityfromWeb(std::string serN,int connId, bool flag)
{
	OCPP_Request req;
	int reservId = 3;
	int rv = createChangeAvailabilityRequest(req,connId,flag);
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
int createResetfromWeb(std::string serN,bool flag)
{
	OCPP_Request req;
	const char* type = flag ? "Soft" : "Hard";
	int rv = createResetRequest(req,type);
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}

int createChangeConfigurationIntervalfromWeb(std::string serN, const char *key, /*int*/const char* val)
{
	OCPP_Request req;
	//const char *key = "HeartBeatInterval";
	//std::string valStr = std::to_string(val);
	int rv = createChangeConfigurationRequest(req, key, /*valStr.c_str()*/val);
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
int createGetCompositeSchedulefromWeb(std::string serN)
{
	OCPP_Request req;
	
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	req.req_str = "[2,\"1225545-54321\",\"GetCompositeSchedule\",{\"connectorId\":1,\"duration\":3600}]";
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
//*************************************************************************

int createSendOcppCommandfromMob(std::string &serN,int cid, int cmd, int par1, std::string& par2)
{
	OCPP_Request req;
	int rv = -1;
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	req.callID = 2;
	req.uniqueID = Global_GenerateTextUID();
	if (cmd == OCPP_COMMAND::OCPP_CMD_SetChargeLimit)
	{
		//req.req_str = "[2,\"1225545-54321\",\"GetCompositeSchedule\",{\"connectorId\":1,\"duration\":3600}]";
		std::string json = google::protobuf::StringPrintf("{\"connectorId\": 1,\"csChargingProfiles\": {\"chargingProfileId\": 20,\"stackLevel\": 1,\"chargingProfilePurpose\": \"TxProfile\",\"chargingProfileKind\": \"Relative\",\"chargingSchedule\": {\"chargingRateUnit\": \"A\",\"chargingSchedulePeriod\": [{\"startPeriod\": 0,\"limit\": %d}]}}}",par1);
		req.action = "SetChargingProfile";
		req.req_str = google::protobuf::StringPrintf("[%d,\"%s\",\"%s\",%s]", req.callID, req.uniqueID.c_str(), req.action.c_str(), json.c_str());
		bool rv1 = sendResponse2Client(ipAddr, req.req_str);
		rv= (rv1 ? 1 : 0);
	}
	if (cmd == OCPP_COMMAND::OCPP_CMD_SetSampleInterval)
	{
		std::string valStr = std::to_string(par1);
		rv = createChangeConfigurationIntervalfromWeb(serN, "MeterValueSampleInterval", valStr.c_str());
	}
	else if (cmd == OCPP_COMMAND::OCPP_CMD_SendRemoteTransaction)
	{
		if (par1 == 0)
		{
			// RemoteStopTransaction
			rv=createRemoteStopTransactionfromWeb(serN);
		}
		else if (par1 == 1)
		{
			// RemoteStartTransaction
			std::vector<std::string> v = split(serN, '*');
			if (v.size() >= 2) {
				string sern = v[0];
				std::string idTag=v[1];
				m_printf("Ocpp Mob RemStartTransaction cid:%d sn:%s tag:%s", cid,  sern.c_str(), idTag.c_str());
				rv = createRemoteStartTransactionfromWeb(sern, idTag);
				
			}
			else {
				OCPP_Request req; prcd(par2);std::string data = std::to_string(0x41);
				int rv = createDataTransferRequest(req, "13", data.c_str());
				std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
				// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
				bool rv1 = sendResponse2Client(ipAddr, req.req_str);
			}
		}
	}
	else if (cmd == OCPP_COMMAND::Ocpp_SimpleCommand)
	{
		if (par1 == OCPP_COMMAND::Ocpp_SimpleCommand_SaveProfile)
		{
			std::string json1 = httplib::detail::decode_url(par2, true);
			std::vector<std::string> v = split(serN, ',');
			if (v.size() >= 3)
			{
				string sern = v[0];
				string profName = v[2];
				int profile_id = - 1;
				profile_id = std::stoi(v[1]);
				m_printf("Ocpp Mob Profile cid:%d id:%d pn:%s sn:%s json:%s", cid, profile_id, profName.c_str(), sern.c_str(), json1.c_str());
				rv = CentralOcppDB::Get()->insertOrUpdateCSProf2DB(cid, profile_id, profName, sern, json1);
				if (rv)
				{

				}
			}
			else {
				m_printf("Ocpp Mob Profile Err %d %s", cid,serN.c_str() );
			}
		}
		else if (par1 == OCPP_COMMAND::Ocpp_SimpleCommand_ClearProfile)
		{
			std::string json1 = httplib::detail::decode_url(par2, true);
			std::vector<std::string> v = split(serN, ',');
			string sern = v[0];
			string profName = v[2];
			int profile_id = -1;
			profile_id = std::stoi(v[1]);
			m_printf("Ocpp Mob Remove Profile cid:%d id:%d pn:%s sn:%s ", cid, profile_id, profName.c_str(), sern.c_str());
			rv = CentralOcppDB::Get()->removeChargingProfilefromCS(profile_id, sern);
		}
		else if (par1 == OCPP_COMMAND::Ocpp_SimpleCommand_NewProfile)
		{
			string sern = "*";
			string profName = "*",json1="?";
			int profile_id = -1;
			m_printf("Ocpp Mob New Profile cid:%d id:%d pn:%s sn:%s ", cid, profile_id, profName.c_str(), sern.c_str());
			rv = CentralOcppDB::Get()->insertOrUpdateCSProf2DB(cid, profile_id, profName, sern, json1);  // this returns profile ID
		}
		
	}
	OCPP_PRINTF("Send Ocpp from Mob req IP:%s serN:%s cmd%d par1:%d", ipAddr.c_str(),serN.c_str(),cmd,par1);
	
	return rv;
}
//-------------------------------------------------------------------------
int createTestChargingProfilefromWeb(std::string serN, int cmd)
{
	OCPP_Request req;
	
	std::string data;
	std::string prof_name = "ChargingProfile_" + std::to_string(cmd - 30);
	CentralOcppDB* db=CentralOcppDB::Get();
	int rv = db->getCSProfileFromDBByProfName(0, prof_name, data);
	std::string ipAddr = db->getIPAddrBySerNumber(serN);
	
	std::stringstream ss;
	ss << data.c_str();
	req.action = "SetChargingProfile";
	create_central_request(req, ss);

	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	OCPP_PRINTF("createTestChargingProfilefromWeb req IP:%s profN:%s pld:%s", ipAddr.c_str(), prof_name.c_str(), req.req_str.c_str());
	return (rv1 ? 1 : 0);

}

//--------------------------------------------------------------------------
int createRemoteStartTransactionfromWeb(std::string serN,std::string idTag)
{
	OCPP_Request req;
	int rv = createRemoteStartTransactionRequest(req, idTag);
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
int createRemoteStopTransactionfromWeb(std::string serN)
{
	OCPP_Request req;
	int rv = createRemoteStopTransactionRequest(req);
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}

//---------------------------TZ-----------------------------------------------
int createDataTransferRequest(OCPP_Request &req,const char* messageID,const char *data)
{
	DataTransferRequest rr;
	rr.vendorId = "UC";
	rr.messageId = std::string(messageID);
	
	rr.data = std::string(data);

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
int createUpdateTZfromWeb(std::string serN, int tz, int dls)
{
	OCPP_Request req;
	std::string data = std::to_string(tz) + "," + std::to_string(dls);
	int rv = createDataTransferRequest(req, "11", data.c_str());
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);  // update info in database if cmd>11
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
//----------------------------------------------------------------------------
int createUpdateFWRequestfromWeb(std::string serN,int cmd)
{
	OCPP_Request req;
	int rv=createUpdateFirmwareRequest(req);
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN,cmd==12);  // update info in database if cmd>11
	// OCPP_PRINTF("UpdFW req IP:%s serN:%s", ipAddr.c_str(),serN.c_str());
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1?1:0);
}
int createConfigReqFromWeb(std::string serN,std::string forField)
{
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	std::stringstream ss;
	if(!forField.empty())
		ss << "{\"key\":[\"HeartBeatInterval\"]}";
	else
		ss << "{}";
	OCPP_Request req;
	req.action = "GetConfiguration";
	create_central_request(req, ss);
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
int createGetLocalListVersionReqFromWeb(std::string serN)
{
	std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
	std::stringstream ss;
	OCPP_Request req;
	req.action = "GetLocalListVersion";
	ss << "{}";
	create_central_request(req, ss);
	bool rv1 = sendResponse2Client(ipAddr, req.req_str);
	return (rv1 ? 1 : 0);
}
bool setChargeStationStatus(std::string& ip_addr, ChargeStationStatus sts)
{
	int rv=CentralOcppDB::Get()->UpdateChargeStationStatus("IPAddr",ip_addr, (int) sts);
	return (rv > 0);
}

//===========================Reservation===================
int createReserveNowRequest(OCPP_Request &req,string idTag, int connId, int reservId)
{
	ReserveNowRequest rr;
	rr.connectorId = connId;// 1; // required
	rr.reservationId = reservId;// 3;  //required
	cereal::schema_data_time dt;
	dt.set_now();
	rr.expiryDate = dt;   // required
	rr.idTag = idTag;
	rr.parentIdTag = "ParentID";  // optional

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
int createCancelReservationRequest(OCPP_Request &req, int reservId)
{
	CancelReservationRequest rr;
	rr.reservationId = reservId;// 3;  //required
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
int createChangeAvailabilityRequest(OCPP_Request &req,int connId,bool flag)
{
	ChangeAvailabilityRequest rr;
	rr.connectorId = connId;

	std::string tS = flag?"Operative":"Inoperative";
	rr.type = ChangeAvailabilityRequest::TypeEnum::from_string(tS);

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}


int createRemoteStartTransactionRequest(OCPP_Request &req, string idTag)
{
	RemoteStartTransactionRequest rr;
	rr.connectorId = 3;  //optional
	rr.idTag = idTag;   /*Required. The identifier that Charge Point must use to start a transaction.*/
	//rr.chargingProfile=  //optional  chargingProfile  page 23 3.12.1. Charging profile purposes
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
int createRemoteStopTransactionRequest(OCPP_Request &req)
{
	RemoteStopTransactionRequest rr;
	rr.transactionId = 3;  //Required. The identifier of the transaction which	Charge Point is	requested to stop.
	
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
int createGetConfigurationRequest(OCPP_Request &req)
{
	GetConfigurationRequest rr;
	schema_array<schema_string<50>, 65536, 0 > key_arr;
	std::string key = "AuthorizationCacheEnabled";
	key_arr.push_back(key);
	rr.key = key_arr;
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
int createChangeConfigurationRequest(OCPP_Request &req, const char* key, const char *val)
{
	ChangeConfigurationRequest rr;
	rr.key = key;
	rr.value = val;
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		rr.serialize(archive);
	}
	req.action = rr.action();
	create_central_request(req, ss);
	return 0;
}
