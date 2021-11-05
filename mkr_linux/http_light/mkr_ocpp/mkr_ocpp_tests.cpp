
#include "CommonUtils/my_time.h"
#include <MkrLib.h>

#include "mkr_ocpp.hpp"
#include "sip_helpers.h"

#include <google/protobuf/stubs/stringprintf.h>
#include "db/CentralOcppDB.h"
#include "db/HistoryDB.h"

//#define OCPP_PRINTF m_printf
#define OCPP_PRINTF printf

using namespace lgpl3::ocpp16;


string Global_GenerateTextUID();


#ifdef WIN32
/*OCPP-req:<[2,"26500","StopTransaction",{"idTag":"4011000010","timestamp":"2021-02-14T21:42:54.001Z","transactionId":1,"meterStop":127,"reason":"Remote"}]>
req Id 2
uniqueID:26500
action:StopTransaction
StopTransaction meter:127 idTag:4011000010
processOCPPRequestAction Exc: Attempt to access value of a disengaged optional object
*/
void test_ocpp()
{
#if 0
	OCPP_Request req; 
	OCPP_Response resp;
	//req.req_str = "[2, \"15724\", \"StopTransaction\", { \"idTag\":\"4011000010\",\"timestamp\" : \"2021-02-16T10:56:48.001Z\",\"transactionId\" : 1,\"meterStop\" : 127,\"reason\" : \"Remote\",\"transactionData\" : [{\"timestamp\":\"2021-02-16T10:56:48.001Z\",\"sampledValue\" : [{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1230\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1231\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1232\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1233\"}]},{\"timestamp\":\"2021-02-16T10:56:48.001Z\",\"sampledValue\" : [{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1230\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1231\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1232\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1233\"}]},{\"timestamp\":\"2021-02-16T10:56:48.001Z\",\"sampledValue\" : [{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1230\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1231\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1232\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1233\"}]},{\"timestamp\":\"2021-02-16T10:56:48.001Z\",\"sampledValue\" : [{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1230\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1231\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1232\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1233\"}]}] }]";

	
	req.req_str = "[2, \"26962\", \"MeterValues\", { \"connectorId\":1,\"meterValue\" : [{\"timestamp\":\"2021-02-17T14:25:23.001Z\",\"sampledValue\" : [{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1230\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1231\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1232\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1233\"}]},{\"timestamp\":\"2021-02-17T14:25:23.001Z\",\"sampledValue\" : [{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1230\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1231\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1232\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1233\"}]},{\"timestamp\":\"2021-02-17T14:25:23.001Z\",\"sampledValue\" : [{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1230\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1231\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1232\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1233\"}]},{\"timestamp\":\"2021-02-17T14:25:23.001Z\",\"sampledValue\" : [{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1230\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1231\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1232\"},{\"measurand\":\"Power.Active.Import\",\"unit\" : \"Percent\",\"value\" : \"VV:1233\"}]}],\"transactionId\" : 1 }]";
	req.remoteIP = "[::ffff:192.168.0.20]:64298";
	bool rv = processOCPPRequest(req, resp);
	if (rv)
	{

	}
#else
	/*StopTransactionRequest tr_data;
	int transactionId = 4;
	int rv = CentralOcppDB::Get()->GetStopTransactionRequestDataFromDB(transactionId, tr_data);
	if (rv)
	{

	}*/
	string rnd=Global_GenerateTextUID();
	if (rnd!="")
	{

	}

	/*std::vector<lgpl3::ocpp16::MeterValuesRequest> tr_data;
	int transactionId = 1;
	int rv = CentralOcppDB::Get()->GetMeterValuesRequestDataFromDB(transactionId, tr_data);
	if (rv)
	{

	}*/
	//=======================================================
	int cid = 2970;
	string sern = "GRS-4011001d93";

	m_printf("CSsStats cid=%d sern:%s", cid, sern.c_str());
	std::map<int64_t, std::vector<CS_Statistics_T>> stat_map;

	int rv = HistoryDB::Get()->GetAllChargeStationsStatistics(cid, sern.c_str(), stat_map);
	udorx_pbf::TrDataList trdatalist;

	for (auto& x : stat_map)
	{
		udorx_pbf::TrData *trdata = trdatalist.add_tdlist();;
		int64_t id = x.first;
		int connId = (id >> 32) & 0xffffffff;
		int transId = (id & 0xffffffff);
		trdata->set_sern(sern);
		trdata->set_transid(transId);
		trdata->set_connid(connId);
		for (auto &xx : x.second)
		{
			udorx_pbf::MeterValues *mvv = trdata->add_mvalues();
			mvv->ParseFromString(xx.meterValues);
		}

	}
	int size = trdatalist.tdlist_size();
	if (size > 0)
	{
		string str = trdatalist.SerializeAsString();
		//set_response_buf(req, str.data(), str.size());
		m_printf("Resp->CSsStats cid=%d sern:%s size:%d", cid, sern.c_str(), size);
	}
	else {
		//set_response_printf(req, "*");
		m_printf("Resp->CSsStats failed");
	}


#endif
}

//int main_authorize_test()
//{
//	
//	BootNotificationRequest request;
//
//	request.chargeBoxSerialNumber = "SN-deadbeef";
//	request.chargePointModel = "Model-123";
//	request.chargePointVendor = "Vendor-999";
//	std::stringstream ss;
//	{
//		cereal::JSONOutputArchive archive(ss);
//		request.serialize(archive);
//	}
//
//	std::string json = ss.str();
//	std::cout << "json payload: " << json << std::endl;
//
//	std::string rjson = "[2," + std::string("\"121898234\",\"") + request.action() + "\"," + json + "]";
//
//
//
//	return 0;
//}


//create a thread to simulate ocpp requests

#if OCPP_CPNSOLE_SIMULATION
#include <windows.h>
#include <conio.h>
static std::thread ocpp_console_thread;
static bool terminate_console = false;
int main_ocpp_console_simulation_start() {
	
	ocpp_console_thread = std::thread([]() {
		while (!terminate_console)
		{
			if (_kbhit()) {
				int key = _getch();
				if (key == 'q')
				{
					::Sleep(4);
					exit(1);
					break;
				}
				else if (key == 'r')
				{
					OCPP_Request req;
					int rv= createResetRequest(req,"Soft");

					std::string serN = "SIM-4011000010";
					std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
					bool rv1= sendResponse2Client(ipAddr, req.req_str);
					if (rv1) {}
				}
				else if (key == 't')
				{
					std::string serN = "SIM-4011000010";
					std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
					OCPP_Request req;
					int rv=createRemoteStartTransactionRequest(req, "4011000010");
					bool rv1 = sendResponse2Client(ipAddr, req.req_str);
					if (rv1) {}
				}
				else if (key == 'h')
				{
					std::string serN = "SIM-4011000010";
					std::string ipAddr = CentralOcppDB::Get()->getIPAddrBySerNumber(serN);
					OCPP_Request req;
					int rv = createChangeConfigurationRequest(req, "AuthorizationCacheEnabled", "true");
					bool rv1 = sendResponse2Client(ipAddr, req.req_str);
					if (rv1) {}
				}
			}

		}
	});
	//if (ocpp_listener_thread.joinable())
	ocpp_console_thread.detach();  //the thread to run in the background (daemon)

	return 0;
}
void terminateConsoleSimThread() { terminate_console = true; }
#endif

#endif