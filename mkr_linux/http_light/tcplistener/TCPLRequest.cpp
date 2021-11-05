#include "udorx_server_config.h"
#include "TcpListener.h"
#include "db/FleetEmployeeDB.h"
#include "db/FleetAssetsDB.h"
#include "db/CentralOcppDB.h"
#include "db/HistoryDB.h"

#include "webservice/Reports.h"

int main_send_register_email(char *mailTo,char *subject,const Client &c);
int main_send_hos_combined_email(char *mailTo, char *report_file, const Client &c);

void download_file_from_server(string &fname,int file_type, int sockfd);

std::string mkr_createPathForMediaFile(DownloadFileTypeEnum file_type, const char *filename)
{
	char fpath[128] = { 0 };
	//int file_type  1 - vehicle image, 2 - inspection image, 3 - mobile worker image, 10 - map data
	switch (file_type)
	{
		case DownloadFileTypeEnum::MEDIA_FILE_VEHICLE_IMAGE:   // vehicle image
			sprintf(fpath, "%s%s", MEDIA_VEH, filename);
			break;
		case DownloadFileTypeEnum::MEDIA_FILE_INSPECTION_IMAGE:
		case DownloadFileTypeEnum::MEDIA_FILE_MOBILEWORKER_IMAGE:
			break;
		case DownloadFileTypeEnum::BIG_MAP_TILE_FILE_DOWNLOAD:
			sprintf(fpath, "%s/tiles/%s", UDORX_MAP_DATA_DOWNLOAD_ROOT, filename);
			break;
		case DownloadFileTypeEnum::BIG_ROUTE_TILE_FILE_DOWNLOAD:
			sprintf(fpath, "%s/routing/%s", UDORX_MAP_DATA_DOWNLOAD_ROOT, filename);
			break;
		case DownloadFileTypeEnum::BIG_GEOCODER_FILE_DOWNLOAD:
			sprintf(fpath, "%s/geocoder/%s", UDORX_MAP_DATA_DOWNLOAD_ROOT, filename);
			break;
		case DownloadFileTypeEnum::BIG_MEDIA_FILE_DOWNLOAD:
		{
			// based on file extention
			if (strstr(filename, ".jpg") != 0)
			{
				sprintf(fpath, "%s/pictures/%s", MEDIA_DIRECTORY, filename);
			}
			else if (strstr(filename, ".m4a") != 0)  // MPEG_4
			{
				sprintf(fpath, "%s/audio/%s", MEDIA_DIRECTORY, filename);
			}
			else if (strstr(filename, ".mp4") != 0)  // MPEG_4
			{
				sprintf(fpath, "%s/video/%s", MEDIA_DIRECTORY, filename);
			}
		}
		break;//switch (file_type)
		case	BIG_MEDIA_FILE_DOWNLOAD_FW_CPU:
		case	BIG_MEDIA_FILE_DOWNLOAD_FW_FRAPHICS:
		case BIG_MEDIA_FILE_DOWNLOAD_FW_ESP32:
		{
			sprintf(fpath, "%s/firmware/%s", UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT, filename);
		}
			break;
	}
	
	std::string res = std::string(fpath);
	return res;
}



#if TEST_TEST
int main_proto_conversion_test()
{
	/*
	PCSItem *wo = new PCSItem();
	wo->set_id(11 + 1);
	wo->set_name("My Name");
	wo->set_createdts((1456075016058 + 11 + 2) * 4000);

	std::string str;
	pbjson::pb2json(wo, str);
	printf("PB2Json result:\n%s\n", str.c_str());
	*/
	/* {"ID":0,"Name":"sdfsfds","Caller":"fdgdgfhgfj","Reference":"7a6347cc-77d7-a803-2c22-9f6561b1975f","Street":"1000 yonge st","City":"toronto","Country":"canada","Province":"on","PostalCode":"","LocationName":"1000_yonge_st_toronto_canada_","ActInstrs":"xsdfdsg","Instructions":"cvbcbvc","resourceID":1001,"StartTime":1476132020000,"EndTime":1476132020000,"serviceGroup":1,"status":0,"Lat":43.6756957,"Lon":-79.38918419999999}
	*/
	string str = "{ \"ID\":0,\"Name\" : \"sdfsfds\",\"Caller\" : \"fdgdgfhgfj\",\"Reference\" : \"7a6347cc-77d7-a803-2c22-9f6561b1975f\",\"Street\" : \"1000 yonge st\",\"City\" : \"toronto\",\"Country\" : \"canada\",\"Province\" : \"on\",\"PostalCode\" : \"\",\"LocationName\" : \"1000_yonge_st_toronto_canada_\",\"ActInstrs\" : \"xsdfdsg\",\"Instructions\" : \"cvbcbvc\",\"resourceID\" : 1001,\"StartTime\" : 1476132020000,\"EndTime\" : 1476132020000,\"serviceGroup\" : 1,\"status\" : 0,\"Lat\" : 43.6756957,\"Lon\" : -79.38918419999999 }";
	PCSItem new_request;
	std::string err;
	int ret = pbjson::json2pb(str, &new_request, err);
	printf("Json2PB result:\%d\n", ret);

	return 0;
}
bool  getWoiList(std::vector<PCSItem*> &v)
{

	for (int i = 0; i < 10; i++)
	{
		PCSItem *wo = new PCSItem();
		wo->set_id(i + 1);
		wo->set_name("My Name");
		wo->set_createdts(1456075016058 + (i + 2) * 4000);
		//v.push_back(wo);
	}

	return true;
}
#endif
//--------------------------------------------------------
static void sendResponse(/*struct bufferevent *bev*/int sockfd,string& resp)
{
	int len = resp.size();
	unsigned char buf[4];
	Convert::Int32ToBytes(len, buf, 0);
//	bufferevent_write(bev, buf, 4);  // send total length
//	bufferevent_write(bev, resp.c_str(), len);  // send payload
	int flags = 0;
#ifndef WIN32
	flags = MSG_NOSIGNAL;
#endif
	send(sockfd, (char*)buf, 4, flags);
	send(sockfd, resp.c_str(), len, flags);
}

static void process_login(CommPacket &commP, /*struct bufferevent *bev*/int sockfd)
{
	     ClsLoginMD lgn;
	lgn.ParseFromString(commP.payload());
	CLogin *L = lgn.mutable_lgn();
	MKR_DBG_PRINTF("process_login_0..\r\n");
	
	Client c;
	c.user = L->user();
	c.pass = L->pass();
	c.ResourceID = lgn.rid();
	PEmpl *emp = new PEmpl();
	emp->set_id(-1);

	L->set_user("NO");
	L->set_pass("NO");
	L->set_cid(-1);
	lgn.set_rid(-1);
	L->set_si(0);

	bool rv=FleetEmployeeDB::Get()->ValidateMobileClient(&c,emp);   // special case rid=-1 -> login using user/pass only
	if (!rv)  // could not validate 
	{
		
		L->set_res(1);  // failed validation
		m_printf("TCPL Login user=%s pass=%s.. Rid=%d validation failed\r\n", c.user.c_str(), c.pass.c_str(), c.ResourceID);
		lgn.set_allocated_driverprof(emp);
	}
	else
	{
		int login_sts = FleetEmployeeDB::Get()->GetLoginStsById(c.ResourceID);
		MKR_DBG_PRINTF("process_login_1.. sts=%d\r\n", login_sts);
		if (login_sts != 1)
		{
			L->set_cid(c.cid);
			lgn.set_drname(c.drivername);
			lgn.set_rid(c.ResourceID);
			L->set_si(0);
			L->set_res(2);   // we are OK
			lgn.set_allocated_driverprof(emp);

			/*PVehicleList *vehicles = FleetEmployeeDB::Get()->GetVehicleListFromDB(c.cid);
			lgn.set_allocated_vehicles(vehicles);*/

			FleetEmployeeDB::Get()->UpdateLoginStsById(c.ResourceID, 1);  // set Login status
		}
		else {
			
			L->set_cid(c.cid);
			lgn.set_drname(c.drivername);
			lgn.set_rid(c.ResourceID);
			L->set_si(0);

			L->set_res(/*-1*/2);  // we already logged in  -> login anyway !!!!

			lgn.set_allocated_driverprof(emp);

			/*PVehicleList *vehicles = FleetEmployeeDB::Get()->GetVehicleListFromDB(c.cid);
			lgn.set_allocated_vehicles(vehicles);*/

			//m_printf("TCPL Login user=%s pass=%s.. Rid=%d Already Logged In\r\n", c.user.c_str(), c.pass.c_str(), c.ResourceID);
		}
	}
	//if (emp != 0)  // should be freed when lgn goes out of scope
	//	delete emp;
	string payload = lgn.SerializeAsString();
	sendResponse(/*bev*/sockfd, payload);
	//IMPORTANT!!!
	// lgn should be disposed here !!! when it goes out of scope as well as emp, vehicles which are dynamically allocated !!!

	MKR_DBG_PRINTF("process_login_..DONE\r\n");
} 

static void process_register_from_mobile(CommPacket &commP, /*struct bufferevent *bev*/int sockfd)
{
	//PRegister rr;
	//rr.ParseFromString(commP.payload());
	//Client c;
	//c.user = rr.user();
	//c.pass = rr.pass();
	//c.fullname = rr.fname();
	//c.email = rr.email();
	//m_printf("TCPL Register user=%s pass=%s.. fname=%s\r\n", rr.user().c_str(), rr.pass().c_str(), rr.fname().c_str());
	//
	////1. Create client
	//
	//FleetEmployeeDB *db=FleetEmployeeDB::Get();
	//m_printf("1. Create client_1=\r\n");
	//int rv=db->InsertNewClient(c);
	//if (rv < 0)
	//{
	//	string resp = std::to_string(rv);   // Username or Email is already in use
	//	sendResponse(/*bev*/sockfd, resp);
	//	return;
	//}

	//// 2. Create Default AssetGroup for this ClientID -> will do it later in one procedure
	//m_printf("2. Create Default AssetGroup=\r\n");
	////3. For mobile worker we need to create mobile Resource 
	//PEmpl emp;
	//emp.set_id(-1);
	//emp.set_user(c.user);
	//emp.set_pass(c.pass);
	//emp.set_drname(c.fullname);
	//emp.set_email(c.email);
	//emp.set_haddr("");
	//emp.set_cid(c.cid);
	//// Canadian_70hr_7day = 6,		US_60hr_7day = 10,
	//emp.set_cycle(10);
	////if (emp.HomeAddress.ToLower().IndexOf("canada")>-1)
	//{
	//	emp.set_cycle(6);
	//}
	//emp.set_tz(-1);          // set time zone on the device  ======================
	//db->SaveEmployee(&emp,true);  //it should be InsertNewEmployee
	//int CID = emp.id();
	//if (CID > 0)
	//{
	//	//send email to inform client that he has been registered !!!
	//	rv=main_send_register_email((char*)c.email.c_str(),"Your UdorX credentials is here",c);
	//	if (rv == 1)
	//	{
	//		FleetAssetsDB::Get()->InsertNewAsset(&emp,ASSETTYPE::MOBILE_WORKERS);
	//		string payload = emp.SerializeAsString();
	//		m_printf("TCPL Register sending response\r\n");
	//		sendResponse(/*bev*/sockfd, payload);
	//	}
	//	else {
	//		db->RemoveClient(c.cid);
	//		db->RemoveEmployee(CID);
	//		string payload = "Failed.. Check you email address";
	//		sendResponse(sockfd, payload);
	//	}
	//}
	//else
	//{
	//	string resp = "**";
	//	sendResponse(/*bev*/sockfd, resp);
	//}
}

static void process_logout(CommPacket &commP, /*struct bufferevent *bev*/int sockfd)
{
	/*ClsLoginMD lgn;
	lgn.ParseFromString(commP.payload());
	
	int rid = lgn.rid();

	FleetEmployeeDB::Get()->UpdateLoginStsById(rid, 0);

	string resp = "OK";
	sendResponse(sockfd, resp);*/
	
	
}



static void process_special_case(CommPacket &commP, int sockfd)
{
}
static void process_hb(CommPacket &commP, /*struct bufferevent *bev*/int sockfd)
{
#if 1	
	RttHB mhb;
	mhb.ParseFromString(commP.payload());
	
	//FleetAssetsDB::Get()->InsertHB(commP.cid(),&mhb);
	int cid = commP.cid();
	int rid = mhb.rid();
	//m_printf("mob hb ts=%lld cid:%d rid:%d", mhb.ts(),cid,rid);
	if (mhb.devtype() == ASSETTYPE::MOBILE_WORKERS)
	{
		RttHBResp response;
		response.set_res(2);  // 1-NO, 2-YES
		response.set_rid(rid);
		PCSList *wolist = new PCSList();
		int rv = CentralOcppDB::Get()->GetAllChargeStations_UpdMob(cid, wolist);

		int wo_size = wolist->cs_items_size();
		if (wo_size > 0)
		{
			
		}
		//m_printf("process HB..wo_size=%d ts=%lld cid:%d rid:%d", wo_size,mhb.ts(), cid, rid);
		response.set_allocated_cs_list(wolist);
		string payload = response.SerializeAsString();

		sendResponse(sockfd, payload);
	}
	else
	{
		string payload = "OK";
		sendResponse(sockfd, payload);
	}
#endif	
}
static void process_fcm_data(CommPacket &commP, int sockfd)
{
	std::string msg=commP.payload();
	std::string msg1 = commP.user();
	int rid = commP.rid();

	FleetEmployeeDB::Get()->UpdateFCMLoginStsById(rid, msg, 1);

	{
		string payload = "OK";
		sendResponse(sockfd, payload);
	}

}
static void ReceiveInsReports(CommPacket &commP, int sockfd)
{
//	PIReports rpts;
//	rpts.ParseFromString(commP.payload());
//	int cid = commP.cid();
//	int count = rpts.reports().list_size();
//	//m_printf("ReceiveInsReports count=%d ... payload=%s\r\n",count, commP.payload().c_str());
//	if (count>0)
//	{
//		bool rv = FleetInspectionDB::Get()->SaveInspectionReports(rpts.mutable_reports());
//		if (!rv)
//		{
//			m_printf("Error wsDB.storeInsReports2DB ...\r\n");
//			goto __exit;
//		}
//	}
//	count = rpts.mfiles().size();
//	for (int i = 0; i < count; i++)
//	{
//		PIRptMFile *imf = rpts.mutable_mfiles(i);
//		if (imf)
//		{
//			string payload = imf->payload();
//			string fname = MEDIA_DVIR + imf->fname();
//			int rv = write_to_file(fname, payload);
//		}
//	}
//__exit:	
//	string resp = "OK";
//		sendResponse(sockfd, resp);
}

static void ReceiveTimeLogs(CommPacket &commP, int sockfd)
{
	/*string res = "OK";

	PTlrList tlrs;
	tlrs.ParseFromString(commP.payload());
	int rv = FleetTimeLogDB::Get()->StoreTimeLogs2DB(commP.cid(), &tlrs);
	if (rv != 1)
		res = "NOT";
	sendResponse(sockfd, res);*/
}
static void GetImagesFromServer(CommPacket &commP, int sockfd)
{
	//PVehicleReports rpts;
	//rpts.ParseFromString(commP.payload());
	//PVehicle *v = 0;
	//if (rpts.has_vehicles())
	//{
	//	int count1 = rpts.vehicles().vitem_size();
	//	v=rpts.mutable_vehicles()->mutable_vitem(0);;
	//}
	//if (v == 0)
	//	return;
	//int file_type = v->cid();
	//int number_of_files = v->flag();

	//int count = rpts.mfiles().size();

	//// m_printf("GetImagesFromServer..count=%d number_of_files=%d file_type=%d\r\n", count, number_of_files, file_type);
	//for (int i = 0; i < count; i++)
	//{
	//	PIRptMFile *imf = rpts.mutable_mfiles(i);
	//	if (imf)
	//	{
	//		string fname = mkr_createPathForMediaFile((DownloadFileTypeEnum) file_type, imf->fname().c_str());
	//		
	//		//m_printf("GetImagesFromServer..fname=%s\r\n", fname.c_str());
	//		int length = 0;
	//		unsigned char* pCh= read_bin_file(fname.c_str(), length);
	//		imf->set_payload(pCh, length);
	//	}
	//}
	//

	//commP.set_payload(rpts.SerializeAsString());

	//string payload = commP.SerializeAsString();

	//sendResponse(sockfd, payload);
	////m_printf("GetImagesFromServer..resp sent paySize=%d\r\n", payload.size());
}
static void ReceiveVehicleReports(CommPacket &commP, int sockfd)
{
	//PVehicleReports rpts;
	//rpts.ParseFromString(commP.payload());
	//int count = rpts.vehicles().vitem_size();
	//if (count > 0)
	//{
	//	string ridS = Int2Str(commP.rid());// 
	//	bool rv = FleetEmployeeDB::Get()->SaveVehicleList2DB(rpts.mutable_vehicles(), ridS);
	//	if (!rv)
	//	{
	//		m_printf("Error wsDB.storeVehs2DB ...\r\n");
	//		return;
	//	}
	//}
	//count = rpts.mfiles().size();
	//for (int i = 0; i < count; i++)
	//{
	//	PIRptMFile *imf = rpts.mutable_mfiles(i);
	//	if (imf)
	//	{
	//		string payload = imf->payload();
	//		string fname = MEDIA_VEH + imf->fname();  //C:\\inetpub\\www_mkr\\mkr_files\\common\\v_img
	//		int rv = write_to_file(fname, payload);
	//	}
	//}
	//string resp = "OK";
	//sendResponse(sockfd, resp);
}
static void GetChargeStationsFromDB_Mob( CommPacket &commP, int sockfd)
{
#if 1
	int rid = commP.rid();
	int cid = commP.cid();
	PCSList  list;
	int rv = CentralOcppDB::Get()->GetAllChargeStations(cid, &list, true);
		
	if (rv>0)
	{		
		
		string payload = list.SerializeAsString();
		//mqtt->Publish(Int2Str(commP.rid()), payload);
		m_printf("GetChargeStationsFromDB_Mob ...sending response size=%d\r\n",list.cs_items_size());
		sendResponse(sockfd, payload);
		
	}
	else
	{
		string resp = "NOT";
		sendResponse(sockfd, resp);
		m_printf("GetChargeStationsFromDB_Mob ...sent NOT\r\n");
	}
#endif
}

static void GetChargeStationsStatisticsFromDB_Mob(CommPacket &commP, int sockfd)
{

	string sern = commP.user();
	int cid = commP.cid();
	//m_printf("CSsStats cid=%d sern:%s", cid, sern.c_str());
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
	string resp = "*";
	if (size > 0)
	{
		resp = trdatalist.SerializeAsString();
		sendResponse(sockfd, resp);
		//m_printf("Resp->CSsStats cid=%d sern:%s size:%d", cid, sern.c_str(), size);
	}
	else {
		sendResponse(sockfd, resp);
		//m_printf("Resp->CSsStats failed");
	}
}

int SendOcppCommandRequest_Mob(CommPacket &commP, int sockfd) {
	string sern = commP.user();
	int cid = commP.cid();
	string pld = commP.payload();
	string par2 = "";
	vector<string> v = split(pld, '|');
	int cmd = -1, par1 = -1;
	int rv = -1;
	if (v.size() >= 3)
	{
		cmd = std::stoi(v[0]);
		par1 = std::stoi(v[1]);
		par2 = (v[2]);
		if (cmd == OCPP_COMMAND::Ocpp_SimpleCommand && par1 == OCPP_COMMAND::Ocpp_SimpleCommand_GetProfile)
		{
			m_printf("Ocpp Mob Get Profile cid:%d  sn:%s ", cid, sern.c_str());
			std::string json_data;
			int rv1 = CentralOcppDB::Get()->getCSProfileFromDBBySern(-1, sern, json_data);
			string resp1 = "*";
			if (rv1)
			{
				resp1= "OCPP" + std::to_string(cmd) + std::to_string(par1) + "|" + json_data;
				sendResponse(sockfd, resp1);
			}
			else
				// if something is wrong send a star
			{
				m_printf("No CsProfile cid:%d id:%d sn:%s ", cid, -1, sern.c_str());
				sendResponse(sockfd, resp1);
			}
			return 1;
		}
		else 
		{
			rv = createSendOcppCommandfromMob(sern, cid, cmd, par1, par2);
		}

	}
	string resp = "*";
	if (rv>0 && cmd > 0)
	{

		resp = "OCPP"+std::to_string(cmd)+ std::to_string(par1)+"|"+std::to_string(rv);
		sendResponse(sockfd, resp);
		//m_printf("Resp->CSsStats cid=%d sern:%s size:%d", cid, sern.c_str(), size);
	}
	else {
		sendResponse(sockfd, resp);
		//m_printf("Resp->CSsStats failed");
	}
	return 1;
}

static void UpdateWorkOrdersStatusList(CommPacket &commP, int sockfd)
{
#if 0
	PCSIStatusList lst;
	lst.ParseFromString(commP.payload());
	bool rv = WorkOrderDB::Get()->UpdateWorkOrdersStatusListInDB(commP.cid(), lst,true);  // update is coming from mobile
	string resp = "OK";
	sendResponse(sockfd, resp);
#endif
}

static void GetWorkOrderTripByTripID(CommPacket &commP, int sockfd)
{
#if 0	
	int cid = commP.cid();
	std::string tripID = commP.payload();
	PTrip trip;
	printf("get trip:%s\r\n", tripID.c_str());
	bool rv = WorkOrderDB::Get()->GetWOTripDB(cid, tripID,&trip);
	if (rv)
	{
		string payload = trip.SerializeAsString();
		sendResponse(sockfd, payload);
		printf("get trip:OK size:%d\r\n", payload.size());
	}
	else {
		string resp = "NOT";
		sendResponse(sockfd, resp);
		printf("get trip:%s\r\n", resp.c_str());
	}
#endif	
}



static void SendEmpProfileFromMobile(CommPacket &commP, int sockfd)
{
	PEmpl pemp;
	pemp.ParseFromString(commP.payload());
	FleetEmployeeDB::Get()->SaveEmployee(&pemp,true);
	string resp = "OK";
	sendResponse(sockfd, resp);
}

//----------------------------------------
static void ProcessEmailsFromMobile(CommPacket &commP, int sockfd)
{
	/*string s = commP.payload();
	m_printf("ProcessEmailsFromMobile ... payload=%s\r\n",  s.c_str());
	vector<string> v = split(s, '|');
	string resp = "NOT";
	if (v.size() >=4)
	{
		FleetEmployeeDB::Get()->InsertNewEmails(commP.cid(),commP.rid(), v);
		int rv = GenerateNewEmailsReport(commP.cid(), commP.rid(), v);
		if (rv)
		{
			m_printf("ProcessEmailsFromMobile ...GenerateNewEmailsReport--OK\r\n");
			resp = "OK";
		}
	}
	sendResponse(sockfd, resp);*/
}

//****************************************************************************************
static void GetAllHosEventsFromDB(CommPacket &commP, int sockfd)
{

	//int rid = commP.rid();
	//PTlrList tlr_list;
	//bool rv = FleetTimeLogDB::Get()->GetAllTimeLogs(tlr_list,rid, 0);
	//if (rv)
	//{
	//	CommPacket commP1;
	//	commP1.CopyFrom(commP);
	//	commP1.set_payload(tlr_list.SerializeAsString());
	//	string payload = commP1.SerializeAsString();
	//	sendResponse(sockfd, payload);
	//}
	//else
	//{
	//	string resp = "*";
	//	sendResponse(sockfd, resp);
	//}
}


//==================tile service================
int read_tile_from_db(int z, int x, int y, std::vector<char>& OutV);// { return 0; }
void ProcessTileDownload(CommPacket &commP, int sockfd)
{
	int z=-1, x=-1, y=-1;
	string req_str=commP.payload();
	//m_printf("ProcessTileDownload req_str=%s\r\n", req_str.c_str());
	vector<string> v = split(req_str, '_');
	//"%d_%d_%d", z, x, y);
	if (v.size() >= 3)
	{
		z = std::stoi(v[0]);
		x = std::stoi(v[1]);
		y = std::stoi(v[2]);
	}
	if (!(z >= 0 && x >= 0 && y >= 0))
	{
		string resp = "*";
		sendResponse(sockfd,resp);
		return;
	}
	
	std::vector<char> OutV;
#if	MKR_USE_INTERNAL_MAP_TILES_SERVICE
	int rv = read_tile_from_db(z, x, y, OutV);
#endif
	string payload = "*";
	if (OutV.size() > 0)
	{
		payload = string(OutV.begin(), OutV.end());
	}

	 
	//m_printf("ProcessTileDownload payload=%d\r\n", payload.size());
	sendResponse(sockfd, payload);
	
}

//========================================================================================
void process_tcp_listener_request(/*struct bufferevent *bev*/int sockfd,unsigned char *buf,int len)
{
	if (len>=4 && buf[0] == 0xff && buf[1] == 0xff && buf[2] == 0xff && buf[3] == 0xff)
	{
		if (len > 12)
		{
			char *p = (char*)(buf + 12);
			string s = string(p, len - 12);
			m_printf("Get special case len=%d  json=%s\r\n", len, s.c_str());
		}
		string resp = "OK";
		sendResponse(/*bev*/sockfd, resp);
		return;
	}
	if (len == 128 && buf[0] == 0xff && buf[1] == 0xfe && buf[2] == 0xfd && buf[3] == 0xfc)
	{/* case HttpRequestNumber::RequestDownloadFileFromServer:
		
		break;
	 */
		int file_type = buf[4];
			char *p = (char*)(buf + 8);
			string fname = string(p);
			m_printf("Get HttpRequestNumber::RequestDownloadFileFromServer: len=%d  fname=%s\r\n", len, fname.c_str());
			download_file_from_server(fname,file_type, sockfd);
		return;
	}

	string req_str = string((char*)buf, len);
	CommPacket commP;
	commP.ParseFromString(req_str);
	int req = commP.req();
	//m_printf("Get Tcp RequestNumber:%d  \r\n", req);

	switch (req)
	{
	case HttpRequestNumber::Login:
		process_login(commP, /*bev*/sockfd);
		break;
	case HttpRequestNumber::RegisterMD:
		process_register_from_mobile(commP, /*bev*/sockfd);  // register request which comes from mobile
		break;
	case HttpRequestNumber::Logout:
		process_logout(commP, /*bev*/sockfd);
		break;
	case HttpRequestNumber::HBReq:
		process_hb(commP, /*bev*/sockfd);
		break;
	case HttpRequestNumber::SendTimeLogs:
		ReceiveTimeLogs(commP, sockfd);
		break;
	case HttpRequestNumber::SendInspectionReports:
		ReceiveInsReports(commP, sockfd);
		break;
	case HttpRequestNumber::Send4VeicleInfo:
		ReceiveVehicleReports(commP, sockfd);
		break;
	case HttpRequestNumber::RequestGetImagesFromServer:
		GetImagesFromServer(commP, sockfd);
			break;
	
	case HttpRequestNumber::GetActivities:
		GetChargeStationsFromDB_Mob(commP,sockfd);
		break;
	case HttpRequestNumber::UpdateActivitesStatusList:
		UpdateWorkOrdersStatusList(commP, sockfd);
		break;

	case HttpRequestNumber::GetWorkOrderTrip:
		GetWorkOrderTripByTripID(commP, sockfd);
		break;
	case HttpRequestNumber::Emails:
		ProcessEmailsFromMobile(commP, sockfd);
		
		break;
	case HttpRequestNumber::SendEmpRow:
		SendEmpProfileFromMobile(commP, sockfd);
		break;

	case HttpRequestNumber::GetAllHosEvents:
		// retrieve all hos events from server to the device  (14 days)
		GetAllHosEventsFromDB(commP, sockfd);
		break;
	case HttpRequestNumber::GetChargeStationsStatistics:
		GetChargeStationsStatisticsFromDB_Mob(commP, sockfd);
		break;
	case HttpRequestNumber::SendOcppCommandRequest:
		SendOcppCommandRequest_Mob(commP, sockfd);
		break;
	case HttpRequestNumber::LoadMapTileData:
		ProcessTileDownload(commP, sockfd);
		break;

	case HttpRequestNumber::SendFCMData:
		process_fcm_data(commP, sockfd);
		break;

	}
}

#if 1
void TestGetAllHosEvents()
{
	/*PTlrList tlr_list;
	bool rv= FleetTimeLogDB::Get()->GetAllTimeLogs(tlr_list,1000, 0);
	if (rv)
	{
		string payload = tlr_list.SerializeAsString();
		if (payload != "")
		{

		}
		
	}*/
}
#endif


//=============================Download File==================================

#define BUF_SIZE	1530
long file_size(const char *filename)
{
	struct stat st;
	int rc = stat(filename, &st);
	return rc == 0 ? st.st_size : -1;
}
int transfer_file(int connfd, string fname)
{
	int rv = 1;
	int fsize = file_size(fname.c_str());
	m_printf("Transfer File: %s size=%d\r\n", fname.c_str(), fsize);
	if (fsize <= 0)
	{
		return -1;
	}

	/* Open the file that we wish to transfer */
	FILE *fp = fopen(fname.c_str(), "rb");
	if (fp == NULL)
	{
		m_printf("File open error %s \r\n", fname.c_str());
		return -1;
	}
	send(connfd, (const char*)&fsize, 4, 0);

	/* Read data from file and send it */
	for (;;)
	{
		/* First read file in chunks of BUF_SIZE bytes */
		unsigned char buff[BUF_SIZE] = { 0 };
		int nread = fread(buff, 1, BUF_SIZE, fp);
		//printf("Bytes read %d \n", nread);

		/* If read was success, send data. */
		if (nread > 0)
		{
			//printf("Sending \n");
			//printf(".");
			send(connfd, (const char*)buff, nread, 0);
		}

		/*
		* There is something tricky going on with read ..
		* Either there was error, or we reached end of file.
		*/
		if (nread < BUF_SIZE)
		{
			if (feof(fp))
				m_printf("End of file\r\n");
			if (ferror(fp))
			{
				m_printf("Error tranferring file\r\n");
				rv = -2;
			}
			break;
		}
	}
	fclose(fp);
	return rv;
}

static void send_response_status(int sockfd, int status)
{
	
	unsigned char buf[4];
	Convert::Int32ToBytes(status, buf, 0);
	//	bufferevent_write(bev, buf, 4);  // send total length
	//	bufferevent_write(bev, resp.c_str(), len);  // send payload
	int flags = 0;
#ifndef WIN32
	flags = MSG_NOSIGNAL;
#endif
	send(sockfd, (char*)buf, 4, flags);
	
}

void download_file_from_server(string &fname,int file_type, int sockfd)
{
	int len = fname.size();
	if (fname.empty())
	{
		send_response_status(sockfd, -1);
		return;
	}
	if(len>32 && fname[len-3]=='.')
	{
		int res = prcd(fname);
		send_response_status(sockfd, res);
		return;
	}

	send_response_status(sockfd, 33);  // start downloading process

	string fname1 = mkr_createPathForMediaFile((DownloadFileTypeEnum)file_type, fname.c_str());
	m_printf("download_file_from_server: %s\r\n", fname1.c_str());
	int rv=transfer_file(sockfd, fname1);
	if (rv == 1)
	{
		m_printf("Success tranferring file\r\n");
	}
}


//=====================test==================
/*ProcessEmailsFromMobile ... payload=skulykov@gmail.com|1588170725740|1588516325740|1,1,1,

ProcessEmailsFromMobile ...GenerateNewEmailsReport--GOTO AttFile:rpt_1001-1572836488771.pdf Email Sending to: skulykov@gmail.com
*/
void test_email()
{
	//string s = "skulykov@gmail.com|1588170725740|1588516325740|1,1,1,";
	//m_printf("ProcessEmailsFromMobile ... payload=%s\r\n", s.c_str());
	//vector<string> v = split(s, '|');
	//string resp = "NOT";
	//if (v.size() >= 4)
	//{
	//	//FleetEmployeeDB::Get()->InsertNewEmails(2601, 1001, v);
	//	int rv = GenerateNewEmailsReport(2601, 1001, v);
	//	if (rv)
	//	{
	//		m_printf("ProcessEmailsFromMobile ...GenerateNewEmailsReport--OK\r\n");
	//		resp = "OK";
	//	}
	//}
}
#if 0
void test_time()
{
	DateTime dt = DateTime::UtcNow();
	dt = dt.AddHours(-4); //we assume dst is already applied
	uint64_t ts = dt.getMillis();//1588856064658;  //Thu May 07 2020 12:54:24

	uint64_t t = /*this->millis*/ts / (60 * 1000); // number of minutes
	//uint64_t t0 = /*this->millis*/ts - t*60000;         // remaining milliseconds
	//if (t0 >= 30 * 1000)
	//	t += 1;

	dt = DateTime(t * 60 * 1000);
	
	std::string str = dt.to_string();
	
	//printf("ts=%lld  dt:%s\r\n",ts, str.c_str());

	return;
}
#endif
#if 1



//void test_tlr_db()
//{
//	/*PTlrList* tlr_list = FleetTimeLogDB::Get()->GetAllTimeLogs(1001, 0);
//	if (tlr_list != 0)
//	{
//		
//	}*/
//
//	int cid = 2601;
//	std::string tripID = "aa0a1fa6-1412-9f30-3e23-fb004d2a24ce";
//	PTrip trip;
//	printf("get trip:%s\r\n", tripID.c_str());
//	bool rv = WorkOrderDB::Get()->GetWOTripDB(cid, tripID, &trip);
//	if (rv)
//	{
//		string payload = trip.SerializeAsString();
//		//sendResponse(sockfd, payload);
//		printf("get trip:OK size:%d\r\n", payload.size());
//	}
//	else {
//		string resp = "NOT";
//		//sendResponse(sockfd, resp);
//		printf("get trip:%s\r\n", resp.c_str());
//	}
//
//
//}



#if 0
void test_tlr_db()
{
	//PTlr tlr;
	//PTlrList list;

	//DateTime dt = DateTime::UtcNow();
	//dt = dt.AddHours(-4); //we assume dst is already applied
	//DateTime date1= dt.Date();
	//{
	//	// create at midnight
	//	bool rvv1=  HosEvents_CreateAtMidNight(tlr,date1.getMillis());
	//	list.mutable_list()->AddAllocated(&tlr);
	//}

	//
	//bool rv = FleetTimeLogDB::Get()->GetLast(1001,tlr);
	//if (rv != 0)
	//{

	//}

	PCSList *wolist = new PCSList();
	int rv1 = WorkOrderDB::Get()->GetRealTimeUpdatedWorkOrdersByResourceId(-1, wolist);

	int wo_size = wolist->wo_items_size();
	if (wo_size > 0)
	{
		for (int i = 0; i < wo_size; i++)
		{
			PCSItem *pwoi = wolist->mutable_wo_items(i);
			if (pwoi)
			{
				printf("woid=%d name=%s\r\n",pwoi->id(),pwoi->name().c_str());
			}
		}

	}

	

}
#endif
#endif