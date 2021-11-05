#include "PWebService.h"
#include "Reports.h"
#include "db/FleetEmployeeDB.h"
#include "db/FleetAssetsDB.h"
#include "db/CentralOcppDB.h"
#include "db/HistoryDB.h"
//#include "db/AdminCSDB.h"


#include <pthread.h>

#include "udorx_server_config.h"

#if !USE_LIBEVENT
#include "server/httplib.h"
using namespace httplib;
#else
#include "server/libevhtp_server.h"

#endif
#include "httplib.h"
using namespace evhtp::evhtphelp;

int main_send_register_email(char *mailTo, char *subject,const Client &c);

extern const char* content_type ;
extern const char* content_disp ;
extern const char* content_len ;

vector<WebSession*> web_sessions;

WebSession* GetWebSessionByID(WebSession *pws);
WebSession* GetWebSession(string user, int cid, int &index);



void process_p_login(evhtp_request_t * req, CommPacket &commP)
{
	ClsLogin lgn;
	lgn.ParseFromString(commP.payload());
	CLogin *L = lgn.mutable_lgn();
	m_printf("PWeb Login user=%s pass=%s.. \r\n", L->user().c_str(), L->pass().c_str());
	Client c;
	c.user = L->user();
	c.pass = L->pass();
	c.ResourceID = -1;

	bool rv = FleetEmployeeDB::Get()->ValidateWebClient(&c);

	if (rv)
	{
		WebSession *ws = new WebSession();  // create session for this user
		ws->cid = c.cid;
		ws->user = c.user;
		ws->pass = c.pass;
		ws->lang = "en";
		ws->sessionid = (int64_t)ws;
		web_sessions.push_back(ws);

		L->set_user(c.user);
		
		string url = string(UDORX_URL_AFTER_LOGIN) + "?si=" + Int642Str(ws->sessionid);
		L->set_pass(url);//g;

		L->set_cid(c.cid);
		L->set_si(ws->sessionid);
		//window.location.href = "mkr.html?si=" + gO.si + "&u=" + u + "&rn=" + g_rand();//URL+"req=0&save=33"+"&rn="+g_rand(); 
		L->set_res(2);
	}
	//else if (c.user == "ucadmin" && c.pass=="ucpassword")
	//{
	//	WebSession *ws = new WebSession();
	//	ws->cid = c.cid;
	//	ws->user = c.user;
	//	ws->pass = c.pass;
	//	ws->lang = "en";
	//	ws->sessionid = (int64_t)ws;
	//	web_sessions.push_back(ws);

	//	L->set_user(c.user);

	//	string url = string(UDORX_ADMIN_URL_AFTER_LOGIN) + "?si=" + Int642Str(ws->sessionid);
	//	L->set_pass(url);//g;

	//	L->set_cid(c.cid);
	//	L->set_si(ws->sessionid);
	//	//window.location.href = "mkr.html?si=" + gO.si + "&u=" + u + "&rn=" + g_rand();//URL+"req=0&save=33"+"&rn="+g_rand(); 
	//	L->set_res(2);
	//}
	else
	{
		L->set_user("NO");
		L->set_pass("NO");
		L->set_cid(-1);
		L->set_res(1);
	}
	/*string json;
	pbjson::pb2json(&lgn, json);
	set_response_json(req,json);*/

	string str = lgn.SerializeAsString();
	set_response_buf(req, str.data(), str.size());
}
void process_web_page_onload(evhtp_request_t * req, CommPacket &commP)
{
	string si_str = commP.user();
	int64_t si_long = Str2Int64(si_str);
	WebSession *si = GetWebSessionByID((WebSession *)si_long);
	if (si == 0)
	{
		set_response_printf(req, "*"); //context_Response_Write(context, "??");
		return;
	}

	CLogin L;
	L.set_res(2);
	L.set_cid(si->cid);
	L.set_user(si->user);
	L.set_pass("***");
	L.set_lang("en");
	L.set_si(si_long);

	string str = L.SerializeAsString();
	set_response_buf(req, str.data(), str.size());
}

void process_web_page_onUnload(evhtp_request_t * req, CommPacket &commP)
{
	string user = commP.user();
	int cid = commP.cid();
	m_printf("Leaving Mkr page ->");
	int index = -1;
	WebSession *si = GetWebSession(user, cid, index);
	if (si != 0)
	{
		bool rv = stdH::removeItemAtIndex(web_sessions, index);
		if (rv)
			m_printf("Leave Mkr page...OK\r\n");
	}
	set_response_printf(req, "LL");
}


void process_web_page_template(evhtp_request_t * req, CommPacket &commP)
{
	int index = -1;
	WebSession *si = GetWebSession(commP.user(), commP.cid(), index);

	if (si == 0 || si->lang == "")
	{
		set_response_printf(req, "??");
		return;
	}
	string ll = "content.html";

	if (si->lang == "1")
		ll = "es_" + ll;

	string s = "";
	read_html_file((char*)(string(WEB_TEMPLETES) + ll).c_str(), s);

	CLogin L;
	L.set_res(2);
	L.set_cid(si->cid);
	L.set_user(si->user);
	L.set_pass(s);  // pass content in pass field
	L.set_lang("en");
	L.set_si(1);

	string str = L.SerializeAsString();
	set_response_buf(req, str.data(), str.size());
}
void getAssetsOnLogin(evhtp_request_t * req, CommPacket &commP)
{
	string user = commP.user();
	int cid = commP.cid();

	//check out the session info !!! TODO 
	int index = -1;
	WebSession *ws = GetWebSession(user, cid, index);

	if (ws != 0)
	{
		ClsLogin lgn; //TODO
		CLogin *L = lgn.mutable_lgn();
		L->set_user(user/*si.user*/);
		L->set_pass("**"/*"{\"data\":123 }"*/);
		L->set_lang("en");
		L->set_cid(cid);
		L->set_si((int64_t)ws);
		L->set_res(2);  // 2 = true
		
		
		//FleetAssetsDB *db = FleetAssetsDB::Get();
			   
		//db->GetAssetsFromDB(cid, lgn.mutable_assets());
		/*db->GetAssetGroupList(cid, lgn.mutable_a_groups());
		db->GetPoisFromDB(cid, lgn.mutable_pois());
		db->GetPoiGroupList(cid, lgn.mutable_p_groups());*/

		RepeatedPtrField<AssetData> *alist= lgn.mutable_assets();

		PCSList list;
		int rv= CentralOcppDB::Get()->GetAllChargeStations(cid, &list,true);

		int size = list.cs_items_size();
		//OCPP_PRINTF("getAssetsOnLogin size:%d\r\n",size);
		for (int i = 0; i < size; i++)
		{
			AssetData *a = new AssetData();
			const PCSItem &it = list.cs_items(i);
			a->set_cid(cid);
			a->set_id(it.id());
			a->set_grid(0);
			a->set_username(it.caller());
			a->set_type(it.type());      //charger type 
			a->set_did(it.serialn());   // serial number is here
			a->set_desc(it.param1());   // charger type description ( string)
			a->set_lat(it.lat());
			a->set_lon(it.lon());
			a->set_address(it.address());
			a->set_spd(it.status());
			a->set_cog(1);
			a->set_sensors("");
			a->set_ts(it.createdts());
			a->set_its(it.lastupdtdts());

			alist->AddAllocated(a);
		}


		string str = lgn.SerializeAsString(); 
		set_response_buf(req, str.data(), str.size());
		//OCPP_PRINTF("getAssetsOnLogin Response -> size:%d\r\n", size);
	}
	else
	{
		set_response_printf(req, "*");
	}
}
void processUploadPhotoWS(evhtp_request_t * req, CommPacket &commP,int reqn)
{
	string upl_file_name = commP.user();
	int balength = commP.rid();
	string fname = string(reqn==26? WEB_QRCODE_IMG_DIR:WEB_RES_IMG_DIR) + upl_file_name;
	string pld = commP.payload();
	int rv = write_to_file(fname, pld);
	if (rv == balength)
		set_response_printf(req, "OK");
	else {
		string str = "Err:" + std::to_string(rv) + "  FN:" + fname + " reqn=" + std::to_string(reqn)+" baL:"+std::to_string(balength)+" pL:"+ std::to_string(pld.size());
		set_response_printf(req, str);
	}

}
void processAddMobileWorkerWS(evhtp_request_t * req, CommPacket &commP)
{
	//PEmpl emp;
	//emp.ParseFromString(commP.payload());

	//int groupID = -1;
	//try {
	//	groupID = std::stoi(commP.user());
	//}
	//catch (...) {}

	//if (groupID < 0)
	//{
	//	commP.set_rid(-2);
	//}
	//else
	//{
	//	emp.set_haddr("");
	//	// Canadian_70hr_7day = 6,		US_60hr_7day = 10,
	//	emp.set_cycle(10);
	//	//if (emp.HomeAddress.ToLower().IndexOf("canada")>-1)
	//	{
	//		emp.set_cycle(6);
	//	}


	//	FleetEmployeeDB::Get()->SaveEmployee(&emp,false);  //it should be InsertNewEmployee
	//	int eID = emp.id();
	//	if (eID > 0)
	//	{
	//		AssetData *ad = FleetAssetsDB::Get()->InsertAssetIntoGroup(&emp, groupID);
	//		string str1 = ad->SerializeAsString();
	//		commP.set_payload(str1);
	//		commP.set_rid(eID);
	//		if (ad != 0)
	//			delete ad;
	//	}
	//	else
	//	{
	//		commP.set_rid(-3);//set_response_printf(req, "-3");
	//	}
	//}

	//string str = commP.SerializeAsString();
	//set_response_buf(req, str.data(), str.size());

}
static void processRegisterWSForCS(evhtp_request_t * req, CommPacket &commP)
{
	PRegister rg;
	rg.ParseFromString(commP.payload());

	Client c;
	c.email = rg.email();

	c.fullname = rg.fname();
	c.user = rg.user();
	c.pass = rg.pass();

}
void processRegisterWS(evhtp_request_t * req, CommPacket &commP)
{
	PRegister rg;
	rg.ParseFromString(commP.payload());

	Client c;
	c.email = rg.email();

	c.fullname = rg.fname();
	c.user = rg.user();
	c.pass = rg.pass();
	m_printf("WS Register req user:%s pass:%s cid=%d\r\n", c.user.c_str(), c.pass.c_str(), -1);
	FleetEmployeeDB *edb = FleetEmployeeDB::Get();
	//1. Create client
	int rv = edb->InsertNewClient(c);
	if (rv < 0)
	{
		string resp = " is already in use";   // Username or Email is already in use
		if (rv == -1)
			resp = "Username" + resp;
		else
			if (rv == -2)
				resp = "Email" + resp;
		set_response_printf(req, resp);
		return;
	}
	m_printf("WS Register_1 req user:%s pass:%s cid=%d\r\n", c.user.c_str(), c.pass.c_str(), 0);
	// 2. Create Default AssetGroup for this ClientID -> will do it later in one procedure

	//3. For mobile worker we need to create mobile Resource 
	PEmpl emp;
	emp.set_id(-1);
	emp.set_user(c.user);
	emp.set_pass(c.pass);
	emp.set_drname(c.fullname);
	emp.set_email(c.email);
	emp.set_haddr("");
	emp.set_cid(c.cid);
	// Canadian_70hr_7day = 6,		US_60hr_7day = 10,
	emp.set_cycle(10);
	//if (emp.HomeAddress.ToLower().IndexOf("canada")>-1)
	{
		emp.set_cycle(6);
	}
	emp.set_tz(-1);          // set time zone on the device  ======================
	edb->SaveEmployee(&emp,false);  //it should be InsertNewEmployee
	int CID = emp.id();
	m_printf("WS Register_2 req user:%s pass:%s cid=%d\r\n", c.user.c_str(), c.pass.c_str(), CID);
	if (CID > 0) 
	{
#if 1
		//send email to inform client that he has been registered !!!
		m_printf("WS Register sending response user:%s pass:%s cid=%d\r\n", c.user.c_str(), c.pass.c_str(), c.cid);
		rv = main_send_register_email((char*)c.email.c_str(), "Your UC credentials are ready", c);
		if (rv == 1)
		{
			/*FleetAssetsDB::Get()->InsertNewAsset(&emp, ASSETTYPE::MOBILE_WORKERS);
			set_response_printf(req, "Your credentials have been emailed to you");*/

			m_printf("WS Register_1 sending response user:%s pass:%s cid=%d\r\n", c.user.c_str(), c.pass.c_str(), c.cid);
			string str = "OK:" + std::to_string(CID);
			set_response_printf(req, str);
		}
		else {
			edb->RemoveClient(c.cid);
			edb->RemoveEmployee(CID);
			set_response_printf(req, "Failed.. Check you email address");
		}
#else
		m_printf("WS Register sending response user:%s pass:%s cid=%d\r\n", c.user.c_str(), c.pass.c_str(), c.cid);
		string str = "OK:" + std::to_string(CID);
		set_response_printf(req, str);
#endif
	}
	else
	{
		set_response_printf(req, "Failed to create this username");
	}
}
static void ClientMessageFromContactUs(evhtp_request_t * req, CommPacket &commP)
{
	string resp = "*";
	string str = commP.user();
	vector<string> v = split(str, '|');
	if (v.size() >= 2)
	{
		string email = v[0];
		string name = v[1];
		string msg = commP.payload();

		m_printf("Custom email:%s name:%s msg:%s\r\n", email.c_str(), name.c_str(), msg.c_str());
		resp = "OK";
	}
	/*

	client.LUTimeStamp = DateTime.UtcNow;
	int rv = wsDB.insertClientMessageToDB(ref client);


	string emsg = String.Format("Client Massage Name:{0} Email:{1} Msg:{2} ", client.Name, client.Email, client.Message);
	string email = "skulykov@gmail.com";
	string eres = Emailer.SendEmailMessage(email, "info@mikerel.com", "Your client msg", emsg);
	MyLog.Log(String.Format("Register Result {0}  EmailRes:{1}", emsg, eres), LogFileDebugLevel.WebPage);
	if (eres.Contains("ERROR"))
	{
	context_Response_Write(context, "Could not send to " + email);
	return;
	}
	*/

	set_response_printf(req, resp);
}


void processHB(evhtp_request_t * req, CommPacket &commP)
{
#if 0
	int cid = commP.cid();
	//string user = getValueByKey("user", v);
	string tsStr = commP.payload();
	int64_t ts = Str2Int64(tsStr);

	RttHBList hbList;
	//hb.alarms = AlarmRes.checkAlarmForUser(cid.ToString(), user);


	PCSList *wolist = new PCSList();
	int rv1 = AdminCSDB::Get()->GetRealTimeUpdatedWorkOrdersByResourceId(-1, wolist);

	int wo_size = wolist->wo_items_size();
	if (wo_size > 0)
	{
		
	}

	hbList.set_allocated_wo_list(wolist);

	int rv = FleetAssetsDB::Get()->listenForIncomingRTTEvents(cid, ts, &hbList);
	if (rv > 0)
	{
		string str = hbList.SerializeAsString();
		set_response_buf(req, str.data(), str.size());
		//m_printf("hb rsp %d", str.size());
	}
	else {
		set_response_printf(req, "*");
		//m_printf("hb rsp none ts=%lld",ts);
	}
#endif	
}
void processChargeStationsHB(evhtp_request_t * req, CommPacket &commP)
{

	int cid = commP.cid();
	//string user = getValueByKey("user", v);
	string ChSs = commP.payload();
	vector<string> v = split(ChSs, '|');

	PCSList list;
	int rv1 = CentralOcppDB::Get()->GetAllChargeStations(cid, &list,true);
	string str = "{\"ChSs\":[";
	int size = list.cs_items_size();
	for (int i = 0; i < size; i++)
	{
		const PCSItem &it = list.cs_items(i);

		//int rv = CentralOcppDB::Get()->UpdateChargeStationStatus("IPAddr", ip_addr, (int)sts);


		string sern=it.serialn();
		int conn_status = it.status();
		std::string cs_sts = it.objb();
		std::string ref = it.reference(),fwv= it.fwver();
		//m_printf("CS HB ref%s", ref.c_str());
		str+=StringPrintf("{\"sern\":\"%s\",\"objb\":\"%s\",\"ref\":\"%s\",\"fwver\":\"%s\",\"sts\":%d}%s", sern.c_str(),cs_sts.c_str(),ref.c_str(),fwv.c_str(),conn_status,i<(size-1)?",":" ");
	}
	str += "]}";
	if (true)
	{
//string str ="1,1";
		set_response_buf(req, str.data(), str.size());
		
	}
	else {
		set_response_printf(req, "*");
		
	}
	
}
static void getChargeStationsHistory(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	string sern = commP.user();
	std::vector<CS_History_T> v;
	int rv = HistoryDB::Get()->GetAllChargeStationsHistory(cid, sern.c_str(), v);
	//m_printf("AllChargeStationsHistory: sern:%s rv=%d\r\n", sern.c_str(),rv);
	if (rv > 0 && v.size()>0)
	{
		string str = "{\"CsHist\":[";
		int size = v.size();
		for (int i = 0; i < size; i++)
		{
			CS_History_T &h = v.at(i);
			str += StringPrintf("{\"o_act\":\"%s\",\"cs_sts\":\"%s\",\"ts\":%lld,\"err_c\":\"%s\",\"ip_a\":\"%s\",\"rtt\":\"%s\"}%s", h.Action.c_str(), h.CSSts.c_str(),h.ts,h.ErrCode.c_str(),h.IPAddr.c_str(), h.RTValues.c_str(), i < (size - 1) ? "," : " ");
		}
		str += "]}";
		set_response_buf(req, str.data(), str.size());
	}
	else {
		set_response_printf(req, "*");

	}
}
static void clearChargeStationsHistory(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	string sern = commP.user();
	
	int rv = HistoryDB::Get()->ClearAllChargeStationsHistory(cid, sern.c_str());
	
	if (rv > 0)
	{
		set_response_printf(req, "OK");
	}
	else {
		set_response_printf(req, "*");

	}
}

static void getChargeStationsStatistics(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	string sern = commP.user();

	m_printf("CSsStats cid=%d sern:%s", cid,sern.c_str());
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
		set_response_buf(req, str.data(), str.size());
		m_printf("Resp->CSsStats cid=%d sern:%s size:%d", cid, sern.c_str(),size);
	}
	else {
		set_response_printf(req, "*");
		m_printf("Resp->CSsStats failed");
	}
}

static void _send_list(evhtp_request_t * req, google::protobuf::Message *list,bool do_not_delete=false)
{
	if (list != 0)
	{
		string str = list->SerializeAsString();
		set_response_buf(req, str.data(), str.size());
		if(!do_not_delete)
			delete list;
	}
	else {
		set_response_printf(req, "*");
	}
}
static void GetHosEvents(evhtp_request_t * req, CommPacket &commP)
{
	/*int rid = commP.rid();
	MyDailyLogsList* list = FleetTimeLogDB::Get()->GetDailyLogs(rid, 0, 0);
	_send_list(req, list);*/
}
static void getCSFWVersion(evhtp_request_t * req, CommPacket &commP)
{
	char fpath[256];
	sprintf(fpath, "%s/firmware/%s", UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT, "UC_SmartGrizzle.md5");
	
	int read_html_file(char *full_path, string &str);
	if (true)
	{
		string str = "";
		int rv=read_html_file((char*)fpath, str);
		//m_printf("getCSFWVersion..rv:%d size:%d str:%s ",rv,str.size(), str.c_str());
		if (rv && str.size() >= 36)
		{
			//str = "t////9v////J////v////77////6////7v///9z////J////3P///+7////d////3f///7/////d////FwAAAL7///8gAAAA+v///93////J////yf///7////+/////vv///77////6////+v///7f///+3////2////9v////K////yv///+7////u////";
			//std::string res = decr_2(str);
			//m_printf("getCSFWVersion_11 res:%s ", res.c_str());
			str = str.substr(8, 4);
			//m_printf("getCSFWVersion..res-> str:%s ", str.c_str());
			set_response_printf(req, str);

			return;
		}
	}

	set_response_printf(req, "*");
}
static void readAllResources(evhtp_request_t * req, CommPacket &commP)
{
	PEmplList * list = FleetEmployeeDB::Get()->GetEmployeesFromDB(commP.cid());
	_send_list(req, list);
}
static void readInspectionCategories(evhtp_request_t * req, CommPacket &commP)
{
	/*int cid = commP.cid();
	string vid = commP.payload();
	PICatList* list = FleetInspectionDB::Get()->GetAllCategories(vid);
	_send_list(req, list);*/
}
static void readVehicles(evhtp_request_t * req, CommPacket &commP)
{
	/*int cid = commP.cid();
	PVehicleList * list = FleetEmployeeDB::Get()->GetVehicleListFromDB(cid);
	_send_list(req, list);*/
}
static void readInsReports(evhtp_request_t * req, CommPacket &commP)
{
	//int cid = commP.cid();
	//string vid = commP.payload();
	//int rid = commP.rid();
	//PIRptList* list = FleetInspectionDB::Get()->GetAllInspReportsByVID(vid, 0, 0); //from=0 to=0
	//_send_list(req, list);
}
static void readHosReports(evhtp_request_t * req, CommPacket &commP)
{
	//int cid = commP.cid();
	//string timeStamp = commP.payload();
	//int64_t ts = Str2Int64(timeStamp);
	//int rid = commP.rid();
	////m_printf("enter HOSR id:%d ts:%s", rid, timeStamp.c_str());
	//PTlrList tlr_list;
	//bool rv= FleetTimeLogDB::Get()->GetAllTimeLogs(tlr_list,rid, ts);
	//_send_list(req, &tlr_list,true);
	////m_printf("HOSR exit..");
}
static void GetChargeStationsFromCentral(evhtp_request_t * req, CommPacket &commP, int max_number)
{
	int cid = commP.cid();
	PCSList  list;
	int rv = CentralOcppDB::Get()->GetAllChargeStations(cid, &list, true);
	//_send_list(req, &list);
	//m_printf("GetChargeStationsFromCentral: cid:%d rv:%d size:%d\r\n", cid,rv,list.cs_items_size());
	string str = list.SerializeAsString();
	set_response_buf(req, str.data(), str.size());

}
static void GetAdminChargeStations(evhtp_request_t * req, CommPacket &commP,int max_number)
{
	//int cid = commP.cid();
	//PCSList  list;
	//int rv = AdminCSDB::Get()->GetAllAdminChargeStations(-1, &list,max_number); // get all chargestations
	////_send_list(req, &list);
	//string str = list.SerializeAsString();
	//set_response_buf(req, str.data(), str.size());

}
void InsertChargeStation(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();

	PCSItem woi;
	woi.ParseFromString(commP.payload());
	string resp = "*";
	woi.set_clientid(cid);                                                      // IMPORTANT
	int64_t rv = CentralOcppDB::Get()->InsertNewChargeStationItem(&woi);
	
	if (rv > 0)
		resp = "OK:" + Int642Str(rv);

	set_response_printf(req, resp);
}
static void UpdateChargeStation(evhtp_request_t * req, CommPacket &commP)
{
	/*string response = "*";
	int cid = commP.cid();

	PCSItem woi;
	woi.ParseFromString(commP.payload());

	CentralOcppDB::Get()->UpdateChargeStation(&woi);
	response = "OK:" + Int2Str(woi.id());

	set_response_printf(req, response);*/
}
static void updateTZ(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	int rid = commP.rid();
	string sern = commP.user();
	
	PConf conf;
	conf.ParseFromString(commP.payload());

	int tz = conf.utcoff();
	int dls = conf.dls();

	string resp = "*";
	int rv = 1;//CentralOcppDB::Get()->UpdateCSTimeZone(sern, tz, dls);   // we update database when we got response from the station  !!!!
	if (rv > 0)
		resp = "OK:" + Int642Str(rv);
	set_response_printf(req, resp);

	createUpdateTZfromWeb(sern, tz, dls);

}
static void UpdateChargeStationStatus(evhtp_request_t * req, CommPacket &commP)
{
	/*string response = "*";
	int cid = commP.cid();

	PCSItem woi;
	woi.ParseFromString(commP.payload());

	CentralOcppDB::Get()->UpdateChargeStationStatus(&woi);
	response = "OK:" + Int2Str(woi.id());

	set_response_printf(req, response);*/
}
static void GetLastRowIDInDB(evhtp_request_t * req, CommPacket &commP)
{
	string response = "*";
	int cid = commP.cid();
	int woid= CentralOcppDB::Get()->GetLastRowIDInDB();
	response = "OK:" + Int2Str(woid);
	set_response_printf(req, response);
}
static void DeleteChargeStation(evhtp_request_t * req, CommPacket &commP)
{
	string response = "*";
	int cid = commP.cid();

	string sern = commP.user();
	//int aid = Str2Int(aID);

	int id = CentralOcppDB::Get()->DeleteChargeStation(sern, cid);
	if (id != -1)
		response = "OK:" + Int2Str(id);

	set_response_printf(req, response);
}

//------------------------------------------------------------------------

static void SaveWOTrip(evhtp_request_t * req, CommPacket &commP)
{
#if 0	
	string response = "Fatal error";
	int cid = commP.cid();
	int rid = commP.rid();
	string U = commP.user();

	vector<string> v = split(U, '|');
	if (v.size() >= 1)
	{
		string tripID = v[0];
		string routeWOs = v[1];

		/*PRouteObj obj;
		obj.ParseFromString(commP.payload());*/

		const string &routeStr = commP.payload();
		bool rv=AdminCSDB::Get()->SaveWOTrip2DB(cid,rid, tripID,routeWOs, routeStr);
		if (rv) 
		{
		   // update TripID in all waorkorders in this list
			v = split(routeWOs, ',');
			std::vector<int> lst;
			if (v.size() >= 1)
			{
				for (auto &s1 : v)
				{
					try {
						int woid = std::stoi(s1);
						lst.push_back(woid);
					}
					catch (...) {}
				}
			}
			if (lst.size() >= 1)
			{
				rv = AdminCSDB::Get()->UpdateWorkOrdersWithTripIDInDB(cid, tripID, lst);
				if (rv)
					response = "OK:" + tripID;
			}
		}
		else
			response = "Route already exists";
	}
	else {

	}
	set_response_printf(req, response);
#endif
}

static void DeleteWOTrip(evhtp_request_t * req, CommPacket &commP)
{
#if 0
	string response = "No";
	int cid = commP.cid();
	string routeID = commP.user();

		bool rv = AdminCSDB::Get()->DeleteWOTripDB(cid, routeID);
		if (rv)
			response = "OK:" + routeID;
		set_response_printf(req, response);
#endif
}



static void GetWOTrips(evhtp_request_t * req, CommPacket &commP)
{
#if 0	
	int cid = commP.cid();
	int rid = commP.rid();
	
	PTripList list;
	bool rv = AdminCSDB::Get()->GetWOTripsDB(cid,list);
	if (rv) {
		string str = list.SerializeAsString();
		set_response_buf(req, str.data(), str.size());
	}
	else {
		string response = "*";
		set_response_printf(req, response);
	}
#endif
}


//========================================================================
bool CartofUi_BaseMapView_Geocoder(const char *addrToFind, std::string *resultStr);
bool RequestGetAddressOriginatedFromAddresesList(uint64_t nameid, uint64_t isinid, std::string *resultStr);
static void ProcessGeoAddressSearch(evhtp_request_t * req, CommPacket &commP)
{

	int flag = commP.rid();
	string addr_to_find = commP.user();
	//m_printf("Geo Req :%s flag=%d", addr_to_find.c_str(),flag);
	std::string resultStr = "";
	bool rv = false;
#if 0
	if (flag == 1)
	{
		rv = CartofUi_BaseMapView_Geocoder(addr_to_find.c_str(), &resultStr);
	}
	else if (flag == 0)
	{
		vector<string> v = split(addr_to_find, ',');
		if (v.size() >= 2)
		{
			uint64_t nameid = 0, isinid = 0;
			try {
				nameid = std::stol(v[0]);
				isinid = std::stol(v[1]);
			}
			catch (std::exception &ex)
			{
				m_printf("GeoCoder AS Ex:%s", ex.what());
				rv = false;
			}

			if (nameid != 0 && isinid != 0)
			{
				rv = RequestGetAddressOriginatedFromAddresesList(nameid, isinid, &resultStr);

			}
		}
	}
	if (rv)
	{
		set_response_buf(req, resultStr.c_str(), resultStr.size());
	}
	else
#endif
	{
		set_response_printf(req, "*");
	}
}


bool getRevGeoAddressOffline(float lat, float lon, std::string &resultStr);
static void ProcessRevGeocoderSearch(evhtp_request_t * req, CommPacket &commP)
{
	float lat = 0, lon = 0;
	std::string str = commP.user();
	vector<string> v = split(str, ',');
	bool rv = false;
	if (v.size() >= 2)
	{
		rv = true;
		try {
			lat = std::stof(v[0]);
			lon = std::stof(v[1]);

		}
		catch (std::exception &ex)
		{
			m_printf("RevGeo AS Ex:%s", ex.what());
			rv = false;
		}
	}
	if (rv)
	{
		/*std::string resultStr = "";
		rv = getRevGeoAddressOffline(lat, lon, resultStr);
		if (rv && resultStr.size() > 0)
		{
			set_response_buf(req, resultStr.c_str(), resultStr.size());
			return;
		}*/
	}
	// if something is wrong send a star
	{
		string response = "*";
		set_response_printf(req, response);
	}
}

//*************************************************************************
#include "../mkr_ocpp/mkr_ocpp.hpp"

static void doOcppCommand(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	int cmd = commP.rid();
	std::string csSerNumber = commP.user();
	m_printf("do ocpp cmd :%d sn:%s", cmd, csSerNumber.c_str());
	string response = "OK";
	if (cmd >=11&&cmd<=13)
	{
		int rv = createUpdateFWRequestfromWeb(csSerNumber,cmd); //--
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}else if (cmd == 14)
	{
		std::string forField = "";
		int rv = createConfigReqFromWeb(csSerNumber, forField);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 37)  // Configuration_1
	{
		int rv = createChangeConfigurationIntervalfromWeb(csSerNumber, "LocalPreAuthorize", "false"); // 661 -> test value
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 15)
	{
		int rv=createGetLocalListVersionReqFromWeb(csSerNumber);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 16)
	{
		string idTag = "DummyTag";
		int rv = createRemoteStartTransactionfromWeb(csSerNumber,idTag);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 17)
	{
		int rv = createRemoteStopTransactionfromWeb(csSerNumber);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 18)
	{
		string idTag = "DummyTag";
		int rv = createReserveNowfromWeb(csSerNumber, idTag);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 19)
	{
		int rv = createCancelReservationfromWeb(csSerNumber);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 20 || cmd==21)
	{
		bool flag = cmd == 20 ? true : false;
		int connId = 1;
		int rv = createChangeAvailabilityfromWeb(csSerNumber, connId, flag);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 22)
	{
		bool flag =  false;
		
		int rv = createResetfromWeb(csSerNumber, flag);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd==30 || cmd==31 || cmd == 32 || cmd==33 || cmd==34)
	{
		

		int rv = createTestChargingProfilefromWeb(csSerNumber, cmd);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 35)
	{

	    std::string ValS= std::to_string(661);
		int rv = createChangeConfigurationIntervalfromWeb(csSerNumber,"HeartBeatInterval", ValS.c_str()); // 661 -> test value
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	else if (cmd == 36)
	{
		//GetCompositeSchedule
		int rv = createGetCompositeSchedulefromWeb(csSerNumber);
		if (rv)
		{
			set_response_printf(req, response);
			return;
		}
	}
	// if something is wrong send a star
	{
		response = "*";
		set_response_printf(req, response);
	}
}
static void doCsProfile(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	int profile_id = commP.rid();
	std::string s = commP.user();
	std::string profName = s,sern="*";
	std::vector v = split(s, '|');
	if (v.size() >= 2)
	{
		profName = v[1];
		sern = v[0];
	}
	std::string json_data= commP.payload();
	std::string json1= httplib::detail::decode_url(json_data,true);
	m_printf("doCsProfile cid:%d id:%d pn:%s sn:%s json:%s",cid, profile_id, profName.c_str(),sern.c_str(),json1.c_str());
	int rv = CentralOcppDB::Get()->insertOrUpdateCSProf2DB(cid,profile_id, profName,sern, json1);
	if (rv)
	{
		string str = "OK:" + std::to_string(rv);
		set_response_printf(req, str);
	}else
	// if something is wrong send a star
	{
		string response = "*";
		set_response_printf(req, response);
	}
}

static void getCsProfile(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	int profile_id = commP.rid();
	std::string sern = commP.user();
	
	//m_printf("getCsProfile cid:%d id:%d sn:%s ", cid, profile_id, sern.c_str());
	std::string json_data;
	int rv = CentralOcppDB::Get()->getCSProfileFromDBBySern(profile_id, sern, json_data);
	if (rv)
	{
		set_response_printf(req, json_data);
	}
	else
		// if something is wrong send a star
	{
		m_printf("No CsProfile cid:%d id:%d sn:%s ", cid, profile_id, sern.c_str());
		set_response_printf(req, "*");
	}
}
static void getCsProfileByName(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	int profile_id = commP.rid();
	std::string profile_name = commP.user();

	//m_printf("getCsProfile cid:%d id:%d sn:%s ", cid, profile_id, sern.c_str());
	std::string json_data;
	int rv = CentralOcppDB::Get()->getCSProfileFromDBByProfName(profile_id, profile_name, json_data);
	if (rv)
	{
		set_response_printf(req, json_data);
	}
	else
		// if something is wrong send a star
	{
		m_printf("No CsProfile cid:%d profName:%s ", cid, profile_id, profile_name.c_str());
		set_response_printf(req, "*");
	}
}
static void getCsProfiles(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	std::string list_data;
	int rv = CentralOcppDB::Get()->getCSProfilesFromDBByCID(cid, list_data);
	if (rv)
	{
		set_response_printf(req, list_data);
	}
	else
		// if something is wrong send a star
	{
		set_response_printf(req, "*");
	}
}
static void removeChargingProfilefromCS(evhtp_request_t * req, CommPacket &commP)
{
	int cid = commP.cid();
	int profile_id = commP.rid();
	std::string sern = commP.user();

	int rv = CentralOcppDB::Get()->removeChargingProfilefromCS(profile_id, sern);
	if (rv)
	{
		set_response_printf(req, "OK");
	}
	else
		// if something is wrong send a star
	{
		
		set_response_printf(req, "*");
	}
}
//*************************************************************************
static void getRttHBList(evhtp_request_t * req, CommPacket &commP)
{

	//int cid = commP.cid();
	//int rid = commP.rid();

	//ASSETTYPE  getAssetTypeOnDidType(std::string did_type);
	//ASSETTYPE  atype=getAssetTypeOnDidType(commP.user());

	//std::vector<std::string> v = split(commP.payload(),'|');
	//if (v.size() == 2)
	//{
	//	string tsStr0 = v[0];
	//	int64_t ts0 = Str2Int64(tsStr0);

	//	string tsStr = v[1];
	//	int64_t ts1 = Str2Int64(tsStr);

	//	RttHBList hb_list;

	//	int rv = FleetAssetsDB::Get()->getRttHBList(cid, rid, ts0, ts1, hb_list,atype);
	//	if (rv > 0)
	//	{
	//		string str = hb_list.SerializeAsString();
	//		set_response_buf(req, str.data(), str.size());
	//		return;
	//	}
	//}
	//{
	//	string resp = string("*");
	//	set_response_printf(req, resp);
	//}

}
static void getRttTrips4Rpl(evhtp_request_t * req, CommPacket &commP)
{
	//int cid = commP.cid();
	//int rid = commP.rid();

	//string didStr = commP.payload();
	//if (!didStr.empty())
	//{
	//	int64_t ts0=0,  ts1 = 0;
	//	RttTrips rtt_trips;
	//	int rv = FleetAssetsDB::Get()->getRttTrips4Rpl(cid, didStr, ts0, ts1, rtt_trips);
	//	if (rv)
	//	{
	//		string str = rtt_trips.SerializeAsString();
	//		set_response_buf(req, str.data(), str.size());
	//		return;
	//	}
	//}
	//{
	//	string resp = string("*");
	//	set_response_printf(req, resp);
	//}
}
void processRemoveMobileWorkerWS(evhtp_request_t * req, CommPacket &commP)
{
	/*int rid = commP.rid();
	FleetEmployeeDB::Get()->RemoveEmployee(rid);

	bool rv = FleetAssetsDB::Get()->removeAssetByMobileID(rid);
	string str = "OK";
	if (!rv)
		str = "NO";
	set_response_printf(req, str);*/
}

void processRemoveVehicleWS(evhtp_request_t * req, CommPacket &commP)
{
	/*string uid = commP.user();
	bool rv = FleetEmployeeDB::Get()->RemoveVehicle(uid);
	string str = "OK";
	if (!rv)
		str = "NO";
	set_response_printf(req, str);*/
}


static void processAddUpdateVehicle(evhtp_request_t * req, CommPacket &commP)
{
	/*PVehicle veh;
	veh.ParseFromString(commP.payload());

	std::string did = "*";
	bool rv = FleetEmployeeDB::Get()->SaveVehicle2DB(&veh, did);

	string str = "OK";
	if (!rv)
		str = "NO";
	set_response_printf(req, str);*/
}

static void processAddUpdatePoi(evhtp_request_t * req, CommPacket &commP)
{
	//PoiData poi;
	//poi.ParseFromString(commP.payload());
	//int cid = commP.cid();
	//int grId = commP.rid();   // we send grId here
	//int rv = FleetAssetsDB::Get()->SavePoiToDB(cid, grId, &poi);
	//string str = "OK";
	//if (!rv)
	//	str = "NO";
	//set_response_printf(req, str);
}

//=========================================================================
bool createRoute(float sLat, float sLon, float destLat, float destLon, std::string& resPtr);

static void ProcessRouteRequest(evhtp_request_t * req, CommPacket &commP)
{
	float lat = 0, lon = 0, lat1 = 0, lon1 = 0;
	std::string str = commP.user();
	//m_printf("RouteReq :%s", str.c_str());
	vector<string> v = split(str, ',');
	bool rv = false;
	if (v.size() >= 4)
	{
		rv = true;
		try {
			lat = std::stof(v[0]);
			lon = std::stof(v[1]);

			lat1 = std::stof(v[2]);
			lon1 = std::stof(v[3]);
		}
		catch (std::exception &ex)
		{
			m_printf("RouteReq AS Ex:%s", ex.what());
			rv = false;
		}
		//m_printf("RReq lat=%f lon=%f lat=%f lon=%f \r\n", lat, lon, lat1, lon1);
	}
	if (rv)
	{
		/*std::string resultStr = "";
		rv = createRoute(lat, lon, lat1, lon1, resultStr);
		if (rv) {
			set_response_buf(req, resultStr.c_str(), resultStr.size());
			return;
		}*/
	}

	{

		set_response_printf(req, "*");
	}

}


bool createOptRoute(std::vector<float> &vIn, std::string& resPtr);

static void ProcessOptRouteRequest(evhtp_request_t * req, CommPacket &commP) 
{
	//float sLat, float sLon, float destLat, float destLon,
	//float lat = 0, lon = 0, lat1 = 0, lon1 = 0;
	std::string str = commP.user();
	//m_printf("RouteReq :%s", str.c_str());
	vector<string> v = split(str, ',');
	bool rv = false;
	std::vector<float> vIn;
	if (v.size() >= 4)
	{
		rv = true;
		for (int i = 0; i < v.size(); i += 2)
		{
			try {
				float lat = std::stof(v[i + 0]);
				float lon = std::stof(v[i + 1]);
				vIn.push_back(lat);
				vIn.push_back(lon);

			}
			catch (std::exception &ex)
			{
				m_printf("RouteReq AS Ex:%s", ex.what());
				rv = false;
				break;
			}
		}
		//m_printf("RReq lat=%f lon=%f lat=%f lon=%f \r\n", lat, lon, lat1, lon1);
	}
	if (rv&&vIn.size() >= 4)
	{
		/*std::string resultStr = "";
		rv = createOptRoute(vIn, resultStr);
		if (rv) {
			set_response_buf(req, resultStr.c_str(), resultStr.size());
			return;
		}*/
	}

	{
		set_response_printf(req, "*");
	}

}




void process_p_web_request(evhtp_request_t * req)
{
	int req_method = evhtp_request_get_method(req);
	
	if (req_method == htp_method_POST)
	{
		evbuf_t *in_buf = req->buffer_in;
		int len = evbuffer_get_length(in_buf);
#if !MKR_LINUX
		unsigned char *buf = new unsigned char[len];  //a variable-length array (VLA) is not supported in WIN32
		if (!buf)
			return;
#else
		unsigned char buf[len];   //buf is a variable-length array (VLA).
#endif
		//req->GetContent(buf, len, false);
		evbuffer_copyout(in_buf, buf, len);

		CommPacket commP;
		commP.ParseFromArray(buf, len);
#if !MKR_LINUX
		delete buf;
#endif
		int rSize = 0,reqn= commP.req();
		
		switch (reqn)
		{
		case 1:
			process_p_login(req, commP);
			break;
		case 2:
			process_web_page_onload(req, commP);
			break;
		case 3:
			process_web_page_template(req, commP);
			break;
		case 5:
			getAssetsOnLogin(req, commP);
			break;
		case 8:
			process_web_page_onUnload(req, commP);
			break;
		case 10:
			processHB(req, commP);
			break;
		case 11:
			processRegisterWS(req, commP);
			break;
		case 12:
			processRegisterWSForCS(req, commP);
			break;
		case 13:
			processAddMobileWorkerWS(req, commP);
			break;
		case 14:    // upload photo for mob worker
		case 26:
			processUploadPhotoWS(req, commP,reqn);
			break;
		case 15:
			getRttHBList(req, commP);
			break;
		case 16:
			processAddUpdateVehicle(req,  commP);
			break;
		case 17:
			processRemoveMobileWorkerWS(req,  commP);
			break;
		case 18:
			processRemoveVehicleWS(req,  commP);
			break;
		case 19:
			processAddUpdatePoi(req,  commP);
			break;
		case 20:
			GetHosEvents(req, commP);
			break;
		case 21:
			readAllResources(req, commP);
			break;
		case 22:
			readInspectionCategories(req, commP);
			break;
		case 23:
			readVehicles(req, commP);
			break;
		case 24:
			readInsReports(req, commP);
			break;
		case 25:
			readHosReports(req, commP);
			break;
		case 30:
			ClientMessageFromContactUs(req, commP);
			break;
		case 40:
			GetChargeStationsFromCentral(req, commP, 2);
			break;
		case 41:
			updateTZ(req, commP);
			break;
		//case 49:
		//	GetAdminChargeStations(req, commP,2);
		//	break;
		//case 50:
		//	GetAdminChargeStations(req, commP,125);   // get last 125 records
		//	break;
		case 51:
			InsertChargeStation(req, commP);
			break;
		case 52:
			UpdateChargeStation(req, commP);
			break;
		case 56:
			DeleteChargeStation(req, commP);
			break;
		case 57:
			UpdateChargeStationStatus(req, commP);
			break;
		case 58:
			GetLastRowIDInDB(req, commP);
			break;
		case 60:
			SaveWOTrip(req, commP);
			break;
		case 61:
			GetWOTrips(req, commP);
			break;
		case 62:
			DeleteWOTrip(req, commP);
			break;

		case 63:
			doCsProfile(req, commP);
			break;
		case 64:
			getCsProfile(req, commP);
			break;
		case 65:
			doOcppCommand(req, commP);
			break;
		case 66:
			processChargeStationsHB(req, commP);
			break;
		case 67:
			getChargeStationsHistory(req, commP);
			break;
		case 68:
			clearChargeStationsHistory(req, commP);
			break;
		case 69:
			removeChargingProfilefromCS(req, commP);
			break;
		case 70:
			getChargeStationsStatistics(req, commP);
			break;
		case 71:
			getCsProfiles(req, commP);
			break;
		case 72:
			getCsProfileByName(req, commP);
			break;
		case 73:
			getCSFWVersion(req,commP);
			break;
		case 80:
			getRttTrips4Rpl(req, commP);
			break;
		case 100:
			ProcessGeoAddressSearch(req,  commP);
			break;
		case 101:
			ProcessRevGeocoderSearch(req,  commP);
			break;
		case 102:
			ProcessRouteRequest(req,  commP);
			break;
		case 103:
			ProcessOptRouteRequest(req,  commP);
			break;


		}
	}

}



//========================================================================
void processGrizzleMsgFromMobile(int len, void *data)
{
	CommPacket cp;
	cp.ParseFromArray(data, len);

	//printf("GrizzleMsgFromMobile: %d %d %s %s\r\n", cp.cid(), cp.rid(), cp.user().c_str(), cp.payload().c_str());
}
