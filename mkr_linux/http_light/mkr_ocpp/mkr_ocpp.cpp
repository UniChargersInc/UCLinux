
#include "CommonUtils/my_time.h"
#include <MkrLib.h>
#include <CommonUtils/my_utils.h>

#include "mkr_ocpp.hpp"
#include "sip_helpers.h"

#include <google/protobuf/stubs/stringprintf.h>
#include "db/CentralOcppDB.h"

#include "mqttc/ClsMqtt.h"

extern RequestQueue request_queue;

using namespace lgpl3::ocpp16;

static void redirect2webapp(Rtt_Ocpp_T& rtt,std::string& action, std::stringstream& ss1,int transactionId=0)
{
	
}

static bool parseOCPPRequest(OCPP_Request &req)
{
	rapidjson::Document doc;
	doc.Parse(req.req_str.c_str());

	//assert(doc.IsObject());

	int ind = 0;
	
	for (auto& v : doc.GetArray())
	{
		if (v.IsInt()) {
			req.callID = v.GetInt();
			//OCPP_PRINTF("req Id %d \r\n", req.callID);
			ind++;
		}
		else if (ind > 0 && v.IsString()) {
			if (ind == 1)
			{
				req.uniqueID = v.GetString();
				//OCPP_PRINTF("uniqueID:%s \r\n", req.uniqueID.c_str());
				ind++;
			}
			else if (ind == 2)
			{
				req.action = v.GetString();
				//OCPP_PRINTF("action:%s \r\n", req.action.c_str());
				ind++;

				break;   // just need an action
			}

		}
		/*
					else if (ind >= 3 && v.IsObject())
					{
						std::string g = v["chargePointVendor"].GetString();
						std::string g1 = v["chargePointModel"].GetString();
						std::string g2 = v["chargeBoxSerialNumber"].GetString();

						std::string imsi = v["imsi"].GetString();

						if (!g.empty())
						{

						}
					}*/

	}
	return true;
}

static void create_response(std::stringstream &ss, OCPP_Response &resp, const char *dbg_msg)
{
	std::string json = ss.str();
	json = google::protobuf::StringPrintf("[%d,\"%s\",%s]", resp.callID, resp.uniqueID.c_str(), json.c_str());
	//OCPP_PRINTF("%s:%s \r\n",dbg_msg, json.c_str());
	resp.resp_str.push_back(json);
}
bool createAuthorizeResponse(OCPP_Response &resp, AuthorizationStatus au_sts)
{
	AuthorizeResponse res;  //7.27. IdTagInfo
	std::string auSts = "Accepted";
	if (au_sts == AuthorizationStatus::AUTH_INVALID)
		auSts = "Invalid";
	res.idTagInfo.status = auSts;//IdTagInfo::StatusEnum::from_string(auSts);
	cereal::schema_data_time dt;
	dt.set_now();
	res.idTagInfo.expiryDate = dt;     //If expiryDate is not given, the status has no end date.
	res.idTagInfo.parentIdTag = "*";
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		res.serialize(archive);
	}

	/*std::string json = ss.str();
	json = google::protobuf::StringPrintf("[%d,\"%s\",%s]", resp.callID, resp.uniqueID.c_str(), json.c_str());
	OCPP_PRINTF("Auth payload:%s \r\n", json.c_str());
	resp.resp_str.push_back(json);*/
	create_response(ss, resp, "Auth response");
	return true;
}

bool createBootNotificationResponse(OCPP_Response &resp, RegistrationStatus r_sts)
{
	
	BootNotificationResponse res;

	std::string rStr = r_sts == RegistrationStatus::RS_ACCEPTED ? "Accepted" : RegistrationStatus::RS_REJECTED ? "Rejected" : "Pending";
	res.status = BootNotificationResponse::StatusEnum::from_string(rStr);
	res.interval = 60;
	cereal::schema_data_time dt;
	dt.set_now();
	res.currentTime = dt;
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		res.serialize(archive);
	}
	create_response(ss, resp, "BNResp");
	return true;
}

bool createStartTransactionResponse(OCPP_Response &resp, int transactionId)
{
	StartTransactionResponse res;

	res.transactionId = transactionId;
	res.idTagInfo.status= IdTagInfo::StatusEnum::from_string("Accepted");
	res.idTagInfo.parentIdTag = "12";

	cereal::schema_data_time dt;
	dt.set_now();
	res.idTagInfo.expiryDate = dt;

	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		res.serialize(archive);
	}

	create_response(ss, resp, "StartTransResp");
	return true;
}

bool createStopTransactionResponse(OCPP_Response &resp)
{

	StopTransactionResponse res;  // IdTagInfo is optinal here !!!
	
	cereal::schema_data_time dt;
	dt.set_now();
	/*auto &x = res.idTagInfo.value();   DO NOT DO THIS FOR OPTIONAL !!! */
	IdTagInfo x;
	x.expiryDate = dt;
	x.parentIdTag = "12";
	x.status = IdTagInfo::StatusEnum::from_string("Accepted");

	res.idTagInfo = x;
	
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		res.serialize(archive);
	}
	create_response(ss, resp, "StOPTransResp");
	return true;
}

bool createSetChargingProfileResponse(OCPP_Response &resp)
{
	SetChargingProfileResponse res;
	res.status = SetChargingProfileResponse::StatusEnum::from_string("Accepted");
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		res.serialize(archive);
	}

	create_response(ss, resp, "ChProfResp");
	return true;
}
bool createHeartbeatResponse(OCPP_Response &resp)
{
	HeartbeatResponse res;
	cereal::schema_data_time dt;
	dt.set_now();
	res.currentTime = dt;
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		res.serialize(archive);
	}
	create_response(ss, resp, "HB Resp");
	return true;
}
bool createDataTransferResponse(OCPP_Response &resp,std::string sts)
{
	DataTransferResponse res;
	
	res.status = DataTransferResponse::StatusEnum::from_string(sts);
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		res.serialize(archive);
	}
	create_response(ss, resp, "DT Resp");
	return true;
}
bool createEmptyResponse(OCPP_Response &resp)
{
	std::string json = google::protobuf::StringPrintf("[%d,\"%s\",{}]", resp.callID, resp.uniqueID.c_str());
	resp.resp_str.push_back(json);
	return true;
}
//***********************************************************************

int InsertOcppReqMsg2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, OCPP_DB_TYPE db_type, void *sts_req,bool update_hb_flag=false)
{
	rtt.ts = (int64_t)DateTime::UtcNow().getMillis();
	rtt.db_type = db_type;
	
	int rv1 = 0;
	CentralOcppDB* db=CentralOcppDB::Get();
	if (!update_hb_flag)
		rv1 = db->InsertOcppRequestMessage2DB(req, rtt, sts_req);
	else
		rv1=db->UpdateOcppHBRequestMessage2DB(req, rtt);
	return rv1;
}

//***********************************************************************
static void doSimpleExtract(std::string &req_str,std::stringstream &ss1)
{
#if OLD_IMPL_DO_SIMPLE_EXTRACT
	std::string sstr = extractSubStr(req_str, "{", "}");
	if (!sstr.empty())
	{
		ss1 << "{" << sstr << "}";
	}
	else {
		ss1 << "{}"; //==
	}
#else
	std::size_t found_first = req_str.find_first_of("{");
	std::size_t found_last = req_str.find_last_of("}");
	if (found_first != std::string::npos &&  found_last != std::string::npos)
	{
		ss1 << req_str.substr(found_first, found_last - found_first + 1);
	}
	else {
		ss1 << "{}"; //==
	}
#endif
}
bool processOCPPRequestAction(OCPP_Request &req, OCPP_Response &resp)
{
	if (!req.action.empty())
	{
		if (req.action == "Heartbeat")
		{
			std::stringstream ss1;
			doSimpleExtract(req.req_str, ss1);

			HeartbeatRequest check_request;
			{
				cereal::JSONInputArchive archive(ss1);
				check_request.serialize(archive);
			}
			//OCPP_PRINTF("HeartbeatRequest..send resp\r\n");
			Rtt_Ocpp_T rtt;
			InsertOcppReqMsg2DB(req,rtt, OCPP_DB_TYPE::DBT_None, NULL, true);
			resp.uniqueID = req.uniqueID;
			resp.callID = 3;
			bool rv = createHeartbeatResponse(resp);
			return rv;
		}
		else if (req.action == "BootNotification")
		{
			std::stringstream ss1;
			doSimpleExtract(req.req_str, ss1);
			
			BootNotificationRequest check_request;
			{
				cereal::JSONInputArchive archive(ss1);
				check_request.serialize(archive);
			}

			//OCPP_PRINTF("BootNotification-- %s\r\n", ss1.str().c_str());

			//OCPP_PRINTF("BootNotification chargePointVendor: %s \r\n", /*check_request.chargePointVendor.to_string().c_str()*/check_request.chargePointSerialNumber.value().to_string().c_str());

			Rtt_Ocpp_T rtt;
			rtt.ts = (int64_t)DateTime::UtcNow().getMillis();
			if(check_request.firmwareVersion.has_value())
				rtt.fwVer = check_request.firmwareVersion.value().to_string();
			rtt.ser_number = check_request.chargePointSerialNumber.value().to_string();   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if (check_request.iccid.has_value())
				rtt.sensors = check_request.iccid.value().to_string();
			if (check_request.imsi.has_value()) {
				std::string imsi =  check_request.imsi.value().to_string();
				rtt.sensors += ("|" + imsi);
			}
			int rv1 = 0;
			CentralOcppDB *cdb= CentralOcppDB::Get();
			if (cdb != NULL) {
				rv1 = cdb->InsertOcppMessage2DB(req, rtt,true, StatusNotificationType::SN_NONE);
			}
			
			redirect2webapp(rtt, req.action, ss1);

			resp.uniqueID = req.uniqueID;
			resp.callID = 3;
			RegistrationStatus r_sts = rv1 > 0 ? RegistrationStatus::RS_ACCEPTED : RegistrationStatus::RS_REJECTED;
			bool rv=createBootNotificationResponse(resp,r_sts);
		    return rv;
		}else  if (req.action == "Authorize")
		{
				std::stringstream ss1;
				doSimpleExtract(req.req_str, ss1);

				AuthorizeRequest check_request;
				{
					cereal::JSONInputArchive archive(ss1);
					check_request.serialize(archive);
				}
				//OCPP_PRINTF("Authorize chargePointVendor: %s \r\n", check_request.idTag.to_string().c_str());
				Rtt_Ocpp_T rtt;
				rtt.ts = (int64_t)DateTime::UtcNow().getMillis();
				int rv1 = CentralOcppDB::Get()->InsertOcppMessage2DB(req, rtt, false, StatusNotificationType::SN_NONE);

				resp.uniqueID = req.uniqueID;
				resp.callID = 3;
				AuthorizationStatus a_sts = rv1 > 0 ? AuthorizationStatus::AUTH_ACCEPTED : AuthorizationStatus::AUTH_INVALID;
				bool rv = createAuthorizeResponse(resp,a_sts);
				return rv;
		}
		else  if (req.action == "StartTransaction")
		{
			std::stringstream ss1;
			doSimpleExtract(req.req_str, ss1);

			StartTransactionRequest check_request;
			{
				cereal::JSONInputArchive archive(ss1);
				check_request.serialize(archive);
			}
			
			Rtt_Ocpp_T rtt;
			int transactionId=InsertOcppReqMsg2DB(req,rtt, OCPP_DB_TYPE::DBT_StartTransaction, &check_request);

			redirect2webapp(rtt, req.action, ss1, transactionId);

			resp.uniqueID = req.uniqueID;
			resp.callID = 3;
			OCPP_PRINTF("StartTransaction Resp connectorId:%d idTag:%s trId:%d\r\n", check_request.connectorId,  check_request.idTag.to_string().c_str(), transactionId);
			bool rv = createStartTransactionResponse(resp, transactionId);
			return rv;
		}
		else  if (req.action == "StopTransaction")
		{
			std::stringstream ss1;
			doSimpleExtract(req.req_str, ss1);
			//string json = ss1.str();
			StopTransactionRequest check_request;
			{
				cereal::JSONInputArchive archive(ss1);
				check_request.serialize(archive);
			}
			//OCPP_PRINTF("StopTransaction meter:%d idTag:%s\r\n", check_request.meterStop, check_request.idTag.value().to_string().c_str());
			Rtt_Ocpp_T rtt;
			InsertOcppReqMsg2DB(req,rtt, OCPP_DB_TYPE::DBT_StopTransaction, &check_request);
			redirect2webapp(rtt, req.action, ss1);
			resp.uniqueID = req.uniqueID;
			resp.callID = 3;
			bool rv = createStopTransactionResponse(resp);
			return rv;
		}
		else if (req.action == "StatusNotification")
		{
			std::stringstream ss1;
			doSimpleExtract(req.req_str, ss1);

			StatusNotificationRequest check_request;
			{
				cereal::JSONInputArchive archive(ss1);
				check_request.serialize(archive);
			}
			
			Rtt_Ocpp_T rtt;
			rtt.ts = (int64_t)DateTime::UtcNow().getMillis();

			StatusNotificationRequest* pSts = &check_request;

			rtt.errCode = pSts->errorCode.to_string();
			rtt.status = pSts->status.to_string();
			{
				
				uint64_t ts = 0;
				if (pSts->timestamp.has_value()) {
					ts = pSts->timestamp.value().value().counter() / 1000000;   // to convert to millis /1000000
				}
				else {
					/*Optional. The time for which the status is reported. If absent time of receipt of the message will be assumed.*/
					ts = DateTime::Now().getMillis();
				}
				string info = "";
				if (pSts->info.has_value())
					info = pSts->info.value().to_string();

				string vendorId = "";
				if (pSts->vendorId.has_value())
					vendorId = pSts->vendorId.value().to_string();

				string vendorErrorCode = "";
				if (pSts->vendorErrorCode.has_value())
					vendorErrorCode = pSts->vendorErrorCode.value().to_string();

				rtt.sensors = std::to_string(pSts->connectorId) + "," + std::to_string(ts) + "," + info + "," + vendorId, +"," + vendorErrorCode;

			}
			int rv1 = CentralOcppDB::Get()->InsertOcppMessage2DB(req, rtt, false, StatusNotificationType::SN_STATUS);

			redirect2webapp(rtt, req.action, ss1);

			resp.uniqueID = req.uniqueID;
			resp.callID = 3;
			bool rv = createEmptyResponse(resp);
			return rv;
		}
		else if (req.action == "FirmwareStatusNotification")
		{
			std::stringstream ss1;
			doSimpleExtract(req.req_str, ss1);
			FirmwareStatusNotificationRequest check_request;
			{
				cereal::JSONInputArchive archive(ss1);
				check_request.serialize(archive);
			}
		
			Rtt_Ocpp_T rtt;
			rtt.ts = (int64_t)DateTime::UtcNow().getMillis();
			rtt.status = check_request.status.to_string();

			int rv1 = CentralOcppDB::Get()->InsertOcppMessage2DB(req, rtt, false, StatusNotificationType::SN_FIRMWARE);

			resp.uniqueID = req.uniqueID;
			resp.callID = 3;
			bool rv = createEmptyResponse(resp);
			return rv;
		}
		else if (req.action == "MeterValues")
		{
			std::stringstream ss1;
			doSimpleExtract(req.req_str, ss1);
			MeterValuesRequest check_request;
			{
				cereal::JSONInputArchive archive(ss1);
				check_request.serialize(archive);
			}
			//OCPP_PRINTF("MeterValues connId:%d \r\n", check_request.connectorId);
			Rtt_Ocpp_T rtt;
			InsertOcppReqMsg2DB(req,rtt, OCPP_DB_TYPE::DBT_MeterValues, &check_request);

			redirect2webapp(rtt, req.action, ss1);

			resp.uniqueID = req.uniqueID;
			resp.callID = 3;
			bool rv = createEmptyResponse(resp);
			return rv;
		}
		else if (req.action == "DataTransfer")
		{
			std::stringstream ss1;
			doSimpleExtract(req.req_str, ss1);
			DataTransferRequest check_request;
			{
				cereal::JSONInputArchive archive(ss1);
				check_request.serialize(archive);
			}
			DataTransferRequest* pSts = &check_request;
			std::string messageId = "*", data = "*";
			if (pSts->messageId.has_value())
				messageId = pSts->messageId.value().to_string();
			if (pSts->data.has_value())
				data = pSts->data.value();

			//OCPP_PRINTF("DataTransfer msgId:%s data:%s \r\n", messageId.c_str(),data.c_str());
			Rtt_Ocpp_T rtt;
			rtt.ts = (int64_t)DateTime::UtcNow().getMillis();

			rtt.errCode = "No Err";
			rtt.status = "***";
			{
				rtt.sensors = "1," + std::to_string(rtt.ts) + "," + data + "," + messageId, +",*";
			}
			InsertOcppReqMsg2DB(req, rtt, OCPP_DB_TYPE::DBT_LOG_MSG, &check_request);
			
			resp.uniqueID = req.uniqueID;
			resp.callID = 3;
			bool rv = createDataTransferResponse(resp, "Accepted");
			return rv;
		}
	}
	return false;
}

string Global_GenerateTextUID()
{
	static bool random_initialized = false;
	if (!random_initialized) {
		/* initialize random seed: */
		srand(time(NULL));
	}
	int rnd=rand();   //Returns a pseudo-random integral number in the range between 0 and RAND_MAX.
	/*if (rnd > RAND_MAX)
		rnd = RAND_MAX;*/
	int64_t ts = DateTime::UtcNow().getMillis();
	ostringstream oss;
	oss << std::hex << rnd << ts;
	return oss.str();
	
}


bool processIncomingCPResponseMessage(OCPP_Request &req, OCPP_Response &resp)
{
	string action = request_queue.getActionByUID(req.uniqueID);
	if (action.empty())
		return false;
	OCPP_PRINTF("CP conf msg action:%s uniqueID:%s", action.c_str(), req.uniqueID.c_str());
	if (action == SetChargingProfileRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);

		SetChargingProfileResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		
		//OCPP_PRINTF("conf SetCProfile:%s \r\n", rr.status.to_string().c_str());
		if (rr.status.to_string() == "Accepted") {
			
		}

		return true;
	}else if (action == ResetRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);

		ResetResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		//OCPP_PRINTF("conf Reset:%s \r\n", rr.status.to_string().c_str());
		if (rr.status.to_string() == "Accepted") {

		}
		return true;
	}
	else if (action == ClearChargingProfileRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);

		ClearChargingProfileResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		//OCPP_PRINTF("conf ClearCProf:%s \r\n", rr.status.to_string().c_str());
		if (rr.status.to_string() == "Accepted") {  // or Unknown

		}
		return true;
	}
	else if (action == ReserveNowRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);
		ReserveNowResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		//OCPP_PRINTF("conf ReserveNow:%s \r\n", rr.status.to_string().c_str());  // ocpp-1.6  7.40. ReservationStatus
		if (rr.status.to_string() == "Accepted") {  // or Faulted,Occupied,Rejected,Unavailable
		}
		return true;
	}
	else if (action == CancelReservationRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);
		CancelReservationResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		//OCPP_PRINTF("conf CancelReserv:%s \r\n", rr.status.to_string().c_str());  // ocpp-1.6  7.40. ReservationStatus
		if (rr.status.to_string() == "Accepted") {  // or Rejected
		}
		return true;
	}
	else if (action == RemoteStartTransactionRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);
		RemoteStartTransactionResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		//OCPP_PRINTF("conf RemoteStartTr:%s \r\n", rr.status.to_string().c_str());  // ocpp-1.6  7.40. ReservationStatus
		if (rr.status.to_string() == "Accepted") {  // or Rejected  7.39. RemoteStartStopStatus
		}
		return true;
	}
	else if (action == RemoteStopTransactionRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);
		RemoteStopTransactionResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		//OCPP_PRINTF("conf RemoteStOPTr:%s \r\n", rr.status.to_string().c_str());  // ocpp-1.6  7.40. ReservationStatus
		if (rr.status.to_string() == "Accepted") {  // or Rejected  7.39. RemoteStartStopStatus
		}
		return true;
	}
	else if (action == GetConfigurationRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);
		GetConfigurationResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		if (rr.configurationKey.has_value())
		{
			auto& cv_arr = rr.configurationKey.value();
			for (int i=0;i<cv_arr.size();i++)
			{
				auto &cv = cv_arr[i];
				std::string val="",key= cv.key.to_string();
				if (cv.value.has_value())
				{
					val = cv.value.value().to_string();
				}
				OCPP_PRINTF("conf GetConf key:%s val:%s\r\n", key.c_str(),val.c_str());
			}
		}
		return true;
	}
	else if (action == GetLocalListVersionRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);
		GetLocalListVersionResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		OCPP_PRINTF("conf GetLocList val:%d\r\n",rr.listVersion);
		return true;
	}
	else if (action == DataTransferRequest::action())
	{
		std::stringstream ss1;
		doSimpleExtract(req.req_str, ss1);
		DataTransferResponse rr;
		{
			cereal::JSONInputArchive archive(ss1);
			rr.serialize(archive);
		}
		std::string data = "";
		std::string sts = rr.status.to_string();
		if (rr.data.has_value())
		{
			 data=rr.data.value();
		}
		//OCPP_PRINTF("DataTransfer Resp status:%s %s\r\n", sts.c_str(),data.c_str());
		if (sts == "Accepted")
		{
			std::vector<std::string> v1 = split(data, ',');
			if (v1.size() >= 4)
			{
				if (v1[0] == "11") {
					std::string sern = v1[1];
					int tz = Str2Int(v1[2]);
					int dls = Str2Int(v1[3]);
					int rv = CentralOcppDB::Get()->UpdateCSTimeZone(sern, tz, dls);
					if (rv > 0)
					{
						OCPP_PRINTF("DataTransfer TZ DB updated %s %d %d\r\n", sern.c_str(),tz,dls);
					}
				}
			}

		}
		return true;
	}
	
	return false;
}
//===============================================================
//[2,"19223201","BootNotification",{ "chargePointVendor": "VendorX", "chargePointModel" : "SingleSocketCharger" }]
bool processOCPPIncoming(OCPP_Request &req, OCPP_Response &resp)
{
	const char *pStr = req.req_str.c_str();

	if ((::strstr(pStr, "Heartbeat") == NULL)  && (::strstr(pStr, "MeterValues") == NULL))
	{
		OCPP_PRINTF("OCPP-in:%s", pStr);  // do not show Heartbeat in the log
	}
	bool rv = false;
		try {
			rv = parseOCPPRequest(req);
		}
		catch (/*websocketpp::exception const & e*/std::exception &e) {
			OCPP_EXC_PRINTF("OCPPRequest Exc:%s", e.what());
			return false;
		}
		try {
			if (req.callID == 2)
			{
				rv = processOCPPRequestAction(req, resp);
				if (req.action == "StartTransaction")
				{
					//create SetChargingProfile Request to Charge Point
					OCPP_Request req1;
					req1.remoteIP = req.remoteIP;
					req1.uniqueID = req.uniqueID;
					createSetChargingProfileRequest(req1);
					resp.resp_str.push_back(req1.req_str); // add this to the list
				}
				else if (req.action == "BootNotification")
				{
					// just for test
					//if (::strstr(pStr, "\"chargePointSerialNumber\":\"GRS-40110009ef\"") != NULL)
					//{
					//	OCPP_Request req1;
					//	req1.remoteIP = req.remoteIP;
					//	req1.uniqueID = req.uniqueID;
					//	createSendLocalListRequest(req1);
					//	resp.resp_str.push_back(req1.req_str); // add this to the list
					//	OCPP_PRINTF("OCPP-sendLLR:%s", req1.req_str.c_str());
					//}

					
				}
			}
			else if (req.callID == 3)
			{
				rv = processIncomingCPResponseMessage(req, resp);  // process response from Charge Point
			}
		}
		catch (/*websocketpp::exception const & e*/std::exception &e) {
			OCPP_EXC_PRINTF("processOCPPRequestAction Exc: %s", e.what());
			return false;
		}
	
	return true;
}
