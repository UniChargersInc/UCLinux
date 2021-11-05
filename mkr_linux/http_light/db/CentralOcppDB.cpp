#include "CommonUtils/my_utils.h"//#include "CommonUtils/my_utils.h"
#include "CentralOcppDB.h"
#include "HistoryDB.h"
#include "MkrLib.h"


void m_printf(const char *format, ...);
#define OCPPDB_PRINTF m_printf
using namespace lgpl3::ocpp16;
using namespace udorx_pbf;

CentralOcppDB *CentralOcppDB::_instance = 0;  // initialize static var member

void CentralOcppDB::createTables()
{
	CREATECONNECTION
	try {
		if (!IsTableExist(p, "CH_STATIONS"))
		{
			CreateChargeStationsTable(p);
		}
		
		if (!IsTableExist(p, "CSProfTbl"))
		{
			CreateCSProfileTable(p);
		}
		
	}
	CLOSECONN
}

void CentralOcppDB::CreateChargeStationsTable(SQLiteStatement *p)
{
	string sql = "";  //CSSerN - Charge Station SerialNumber 
	if (!IsTableExist(p, "CH_STATIONS"))
	{
		sql = "CREATE TABLE CH_STATIONS (CSSerN ntext PRIMARY KEY,Caller ntext,Param1 ntext,Reference ntext, Type integer,"
			"Address ntext,Instrs ntext,CertFlag integer,CreatedTS bigint,LastUpdtdTS bigint, Latitude real, Longitude real, ClientID INTEGER,"
			"Status integer,serviceGroup integer,ProfID integer,ObjB ntext,IPAddr ntext,FWVer ntext,TZ integer,DLS integer,UpdMob boolean)";  

		CreateTableA(p, sql);
		//OCPPDB_PRINTF("CH_STATIONS created..\r\n");
	}
	/*else {
		sql = "ALTER TABLE CH_STATIONS ADD COLUMN FWVer ntext NOT NULL DEFAULT '*'";
		try {
			p->SqlStatement(sql);
		}
		catch (SQLiteException &ex)
		{
			m_printf("get CH_STATIONS exception : ", ex.GetString().c_str());
		}
	}*/

	/*ALTER TABLE CH_STATIONS RENAME COLUMN ProfID TO ProfID;*/
	
}
//========================================================================

//------------------------------------------------------------------------
void CentralOcppDB::CreateCSProfileTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE CSProfTbl (ID INTEGER PRIMARY KEY, profName ntext,TS bigint,data ntext,ClientID integer)";
	CreateTableA(p, sql);

	/*ALTER TABLE CSProfTbl ADD COLUMN ClientID integer*/
}
int CentralOcppDB::getCSProfileID(SQLiteStatement *p)
{
	string sql = "SELECT id from csproftbl order by ID DESC limit 1";
	int rv = 0;
		
		p->Sql(sql);
		while (p->FetchRow())
		{
			rv=p->GetColumnInt(0);
			break;
		}
		// do not forget to clean-up
		p->FreeQuery();
	
		return rv;
}
int CentralOcppDB::insertOrUpdateCSProf2DB(int cid, int profile_id, std::string prof_name, std::string assign_to_sern, std::string &json_data)
{
	int rv = 0;
	CREATECONNECTION
	try
	{
		uint64_t ts = DateTime::Now().getMillis();

		if (profile_id > 0)
		{
			std::string sql = StringPrintf("UPDATE CSProfTbl SET ClientID=%d,profName='%s',TS=%lld,data='%s'  WHERE id=%d",cid, prof_name.c_str(),ts, json_data.c_str(),profile_id);
			p->SqlStatement(sql);
			rv = GetNuberOfRowsUpdated();
			UpdateCSStationsWithProfile(p, profile_id, cid, assign_to_sern);
		}
		else {
			rv=getCSProfileID(p);
			if (rv == 0 && profile_id == -1)
			{
				//empty table
				rv = 1;
			}
			if (rv > 0)
			{
				rv++;
				std::string sql = StringPrintf("INSERT INTO CSProfTbl (ID, profName,TS,data,ClientID) VALUES (%d,'%s',%lld,'%s',%d)",rv, prof_name.c_str(), ts, json_data.c_str(),cid);
				p->SqlStatement(sql);
				//rv = GetLastInsertedRow();  // return profile id ( rowid)
			}
			
		}
	}
	CLOSECONN
	return rv;
}
int CentralOcppDB::getCSProfile(SQLiteStatement *p,int id, std::string &sern, std::string &data)
{
	int rv = 0;
	
		if (!sern.empty())
		{
			string sql = StringPrintf("SELECT data FROM CSProfTbl where id=(select ProfID from CH_Stations where CSSerN='%s')", sern.c_str());
			p->Sql(sql);
			while (p->FetchRow())
			{
				data = p->GetColumnString(0);
				rv++;
			}
			// do not forget to clean-up
			p->FreeQuery();
		}
		return rv;
}
int CentralOcppDB::getCSProfileFromDBByIp(int id, /*std::string &ip_addr,*/OCPP_Request &req, std::string &data)
{
	int rv = 0;

	
	CREATECONNECTION
	try
	{
		std::string sern="";

		int cid= getSerNumberByIPAddr(p, req,sern);


		//OCPPDB_PRINTF("getCSProfileFromDBByIp sn:%s ip:%s \r\n", sern.c_str(), ip_addr.c_str());
		rv = getCSProfile(p, id, sern, data);
	}
	CLOSECONN

	return rv;
}
int CentralOcppDB::getCSProfileFromDBBySern(int id, std::string &sern, std::string &data)
{
	int rv = 0;
	CREATECONNECTION
	try
	{
		//OCPPDB_PRINTF("getCSProfileFromDBBySern sn:%s \r\n", sern.c_str());
		rv = getCSProfile(p, id, sern, data);
	}
	CLOSECONN
	return rv;
}
int CentralOcppDB::getCSProfileFromDBByProfName(int id, std::string &prof_name, std::string &data)
{
	int rv = 0;
	CREATECONNECTION
	try
	{
		string sql = StringPrintf("SELECT data FROM CSProfTbl where profName='%s'", prof_name.c_str());
		p->Sql(sql);
		while (p->FetchRow())
		{
			data = p->GetColumnString(0);
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
	return rv;
}

int CentralOcppDB::getCSProfilesFromDBByCID(int cid, std::string &data)
{
	int rv = 0;
	CREATECONNECTION
	try
	{
		//string sql = StringPrintf("SELECT profName FROM CSProfTbl where ClientID=%d", cid);
		//SELECT profName FROM CSProfTbl where rowid IN(SELECT ProfID FROM CH_stations where ClientID = 2601)
		string sql = StringPrintf("SELECT profName FROM CSProfTbl where id IN(SELECT ProfID FROM CH_stations where ClientID = %d)", cid);
		p->Sql(sql);
		while (p->FetchRow())
		{
			data += (p->GetColumnString(0)+",");
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
	return rv;
}
int CentralOcppDB::removeChargingProfilefromCS(int id, std::string &sern)
{
	int rv = 0;
	CREATECONNECTION
	try
	{
		string sql = StringPrintf("UPDATE CH_STATIONS SET ProfID=0,UpdMob=1  WHERE CSSerN='%s'", sern.c_str());
		p->SqlStatement(sql);
		rv = GetNuberOfRowsUpdated();
	}
	CLOSECONN
	return rv;
}
//------------------------------------------------------------------------
int CentralOcppDB::UpdateOcppHBRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt)
{
	int rv = 0;
	CREATECONNECTION
	try
	{
		std::string sern = "";
		int cid = getSerNumberByIPAddr(p, req,sern);
		if (!sern.empty())
		{
			string sql = StringPrintf("UPDATE CH_STATIONS SET LastUpdtdTS=%lld,Status=1,UpdMob=1  WHERE CSSerN='%s'", rtt.ts, sern.c_str()); // UpdMob=1 -> to update CSs list on mobile
			p->SqlStatement(sql);
			rv = GetNuberOfRowsUpdated();
			//OCPPDB_PRINTF("%s rv:%d \r\n", sql.c_str(), rv);
		}
	}
	CLOSECONN
	return rv;
	
}
int CentralOcppDB::UpdateCSTimeZone(std::string &sern, int tz,int dls)
{
	int rv = 0;
	CREATECONNECTION
	try
	{
		
		if (!sern.empty())
		{
			string sql = StringPrintf("UPDATE CH_STATIONS SET TZ=%d,DLS=%d,UpdMob=1  WHERE CSSerN='%s'", tz,dls, sern.c_str());
			p->SqlStatement(sql);
			rv = GetNuberOfRowsUpdated();
			//OCPPDB_PRINTF("%s rv:%d \r\n", sql.c_str(), rv);
		}
	}
	CLOSECONN
	return rv;

}
int CentralOcppDB::UpdateCSStationsWithProfile(SQLiteStatement *p, int profile_id, int cid, std::string sern)
{
	int rv = 0;
		if (!sern.empty())
		{
			string sql;
			if (sern == "*")
			{
				//for all stations
				sql = StringPrintf("UPDATE CH_STATIONS SET ProfID=%d,UpdMob=1  WHERE ClientID=%d", profile_id, cid);
			}
			else {
				sql = StringPrintf("UPDATE CH_STATIONS SET ProfID=%d,UpdMob=1  WHERE CSSerN='%s' and ClientID=%d", profile_id, sern.c_str(), cid);
			}
			p->SqlStatement(sql);
			rv = GetNuberOfRowsUpdated();
			//OCPPDB_PRINTF("%s rv:%d \r\n", sql.c_str(), rv);
		}
	
	
		return rv;

}
int CentralOcppDB::InsertOcppRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, void *ptr)
{
	int rv = 0;
	
			switch (rtt.db_type)
			{
			case OCPP_DB_TYPE::DBT_StartTransaction:
			{
				
				auto sern = preprocessOcppMessage2DB( req, rtt, false, StatusNotificationType::SN_NONE);
				if (!sern.empty())
				{
					rv = HistoryDB::Get()->OnStartTransactionRequestMessage2DB(req, rtt, ptr, sern);
				}
			}
				break;
			case OCPP_DB_TYPE::DBT_StopTransaction:
			{
				auto sern = preprocessOcppMessage2DB(req, rtt, false, StatusNotificationType::SN_NONE);
				if (!sern.empty())
				{
					rv = HistoryDB::Get()->OnStopTransactionRequestMessage2DB(req, rtt, ptr, sern);
				}
			}
				break;
			case OCPP_DB_TYPE::DBT_MeterValues:
			{
				auto sern = preprocessOcppMessage2DB(req, rtt, false, StatusNotificationType::SN_NONE);
				if (!sern.empty())
				{
					rv = HistoryDB::Get()->OnMeterValuesRequestMessage2DB(req, rtt, ptr, sern);
				}
			}
				break;
			case OCPP_DB_TYPE::DBT_LOG_MSG:
				//hist_rowid = insertOcppMessage2DB(p, req, rtt, false, StatusNotificationType::SN_NONE);
				auto sern = preprocessOcppMessage2DB(req, rtt, false, StatusNotificationType::SN_NONE);
				if (!sern.empty())
					rv = HistoryDB::Get()->InsertOcppHistory2DB(req, rtt, sern);
				
				break;
			}
		
	
	return rv;
}

int CentralOcppDB::getSerNumberByIPAddr(SQLiteStatement *p, /*std::string &ipaddr,*/OCPP_Request &req, std::string &sern)
{
	int cid = 0;
	//OCPPDB_PRINTF("CentralOcppDB::getSerNumberByIPAddr rv:%d \r\n", 0);
#if 0

	std::string sql = StringPrintf("Select CSSerN,ClientID from CH_STATIONS where IPAddr='%s'", ipaddr.c_str());  
	p->Sql(sql);
	while (p->FetchRow())
	{
		sern = p->GetColumnString(0);
		cid = p->GetColumnInt(1);
		break;
	}
	// do not forget to clean-up
	p->FreeQuery();
#else
	sern = "";
	if (!req.uniqueID.empty() && req.uniqueID.size() > 10)
	{
		sern = "GRS-" + req.uniqueID.substr(0, 10);
		std::string sql = StringPrintf("Select ClientID,CertFlag from CH_STATIONS where CSSerN='%s'", sern.c_str());
		p->Sql(sql);
		while (p->FetchRow())
		{
			cid = p->GetColumnInt(0);
			req.cert_flags = p->GetColumnInt(1);
			req.sern = sern;
			break;
		}
		// do not forget to clean-up
		p->FreeQuery();


	}
	else {
		OCPPDB_PRINTF("FATAL! getSerNumberByIPAddr uid:%s  ip:%s \r\n", req.uniqueID.c_str(), req.remoteIP.c_str());
	}
#endif
	//OCPPDB_PRINTF("CentralOcppDB::getSerNumberByIPAddr rv:%d \r\n", cid);
	return cid;
}

std::string CentralOcppDB::getIPAddrBySerNumber(std::string &serN, bool fw_update_request)
{
	std::string rv = "";
	CREATECONNECTION
	try
	{
		std::string sql = StringPrintf("Select IPAddr from CH_STATIONS where CSSerN='%s'", serN.c_str());
		p->Sql(sql);
		while (p->FetchRow())
		{
			rv = p->GetColumnString(0);
			break;
		}
		// do not forget to clean-up
		p->FreeQuery();

		if (fw_update_request && !serN.empty())
		{
			uint64_t ts = (uint64_t)DateTime::UtcNow().getMillis();
			updateCSFWUpdateFromOcppMessage2DB(p, serN.c_str(), "Started", ts);
		}

	}
	
	CLOSECONN
	return rv;
}
std::string CentralOcppDB::preprocessOcppMessage2DB( OCPP_Request &req, Rtt_Ocpp_T &rtt, bool requireIPaddrUpdate, StatusNotificationType sts_type)
{
	int rv = 0;
	std::string sern = "";
	CREATECONNECTION
		try
	{
		if (requireIPaddrUpdate)
		{
			// rtt contains Charge Station ser_number  ( e.g. BootNotification)
			rv = updateIPAddrFromOcppMessage2DB(p, req, rtt);//should return 1 if Charge Station has already been registered !!!
			if (rv > 0)
				sern = rtt.ser_number;

			{
				std::string sern1 = "";
				int cid = getSerNumberByIPAddr(p, req, sern1);
				if (rtt.ser_number != sern1) {
					OCPPDB_PRINTF("DB sernMismatch:%s %s", sern1.c_str(), rtt.ser_number.c_str());
				}
				rtt.cid = cid;
			}

		}
		else {

			int cid = getSerNumberByIPAddr(p, req, sern);
			rtt.ser_number = sern;
			rtt.cid = cid;
			if (sts_type == StatusNotificationType::SN_STATUS)
			{

				rv = updateCSObjBFromOcppMessage2DB(p, req, rtt);
			}
			else if (sts_type == StatusNotificationType::SN_FIRMWARE) {

				/*OCPP-in:[2,"401100084f6dfa045d","FirmwareStatusNotification",{"status":"Installed"}]
				FW Cmd to Ref sern:GRS-401100086a sts:Installed ts:1622662551615 rv=1          Error!!!!!!      ???????            */
				rv = updateCSFWUpdateFromOcppMessage2DB(p, sern.c_str(), rtt.status.c_str(), rtt.ts);
			}
		}
	}
	CLOSECONN
	return sern;
}


int CentralOcppDB::InsertOcppMessage2DB(OCPP_Request &req,Rtt_Ocpp_T &rtt, bool requireIPaddrUpdate, StatusNotificationType sts_type)
{
	int rv = 0;
	auto sern = preprocessOcppMessage2DB( req, rtt, requireIPaddrUpdate, sts_type);
	if (!sern.empty())
		rv=HistoryDB::Get()->InsertOcppHistory2DB(req, rtt, sern);
	return rv;
}
int CentralOcppDB::updateIPAddrFromOcppMessage2DB(SQLiteStatement *p,OCPP_Request &req, Rtt_Ocpp_T &rtt)
{
	// rtt.fwVer is comming with BootNotification  !!!   -> added Status=1  !!!!
#if 0
	string sql = "";   
	if(rtt.fwVer.empty())
		sql=StringPrintf("UPDATE CH_STATIONS SET IPAddr='%s',LastUpdtdTS=%lld,Status=1  WHERE CSSerN='%s'", req.remoteIP.c_str(),rtt.ts, rtt.ser_number.c_str());
	else
		sql = StringPrintf("UPDATE CH_STATIONS SET IPAddr='%s',FWVer='%s',LastUpdtdTS=%lld,Status=1  WHERE CSSerN='%s'", req.remoteIP.c_str(),rtt.fwVer.c_str(), rtt.ts, rtt.ser_number.c_str());
	p->SqlStatement(sql);
#else
	int flags = 0;
	std::vector<std::string> vv = split(rtt.sensors, '|');
	if (vv.size() >= 2)
	{
		flags = Str2Int ( vv[1] );
	}
	std::string flagStr = flags & 4 ? "CertFlag = CertFlag | 4" : "CertFlag = CertFlag & ~4";
	string sql = StringPrintf("UPDATE CH_STATIONS SET IPAddr='%s',LastUpdtdTS=%lld,Status=1,%s", req.remoteIP.c_str(), rtt.ts,flagStr.c_str());
	if (!rtt.fwVer.empty())
		sql = StringPrintf("%s ,FWVer='%s'  WHERE CSSerN='%s'", sql.c_str(), rtt.fwVer.c_str(), rtt.ser_number.c_str());

	OCPPDB_PRINTF("BootN IP DB update sql:%s ", sql.c_str());

	p->SqlStatement(sql);
#endif
	//UPDATE ch_stations SET CertFlag = CertFlag | 4 WHERE CSSerN='GRS-40110009ef'
	//UPDATE ch_stations SET CertFlag = CertFlag & ~4 WHERE CSSerN='GRS-40110009ef'
	
	return GetNuberOfRowsUpdated();
}
int CentralOcppDB::updateCSObjBFromOcppMessage2DB(SQLiteStatement *p, OCPP_Request &req, Rtt_Ocpp_T &rtt)
{
	string objb = req.action + "," + rtt.status + "," + rtt.errCode + "," + rtt.sensors;  // store last charge station status
	string sql = StringPrintf("UPDATE CH_STATIONS SET ObjB='%s',LastUpdtdTS=%lld,UpdMob=1  WHERE CSSerN='%s'", objb.c_str(), rtt.ts, rtt.ser_number.c_str());
	p->SqlStatement(sql);
	return GetNuberOfRowsUpdated();
}
int CentralOcppDB::updateCSFWUpdateFromOcppMessage2DB(SQLiteStatement *p, const char* sern, const char* status, uint64_t ts)
{
	string objb = std::string(status) + "," + std::to_string(ts);  // store last charge station FW Update status in Reference field
	string sql = StringPrintf("UPDATE CH_STATIONS SET Reference='%s'  WHERE CSSerN='%s'", objb.c_str(), sern);
	p->SqlStatement(sql);
	int rv= GetNuberOfRowsUpdated();
	OCPPDB_PRINTF("FW Cmd to Ref sern:%s sts:%s ts:%lld rv=%d", sern,status,ts,rv);
	return rv;
}
//int CentralOcppDB::UpdateCSFWUpdateFromOcppMessage2DB(const char* sern, const char* status)
//{
//	int rv = 0;
//	CREATECONNECTION
//	try
//	{
//		uint64_t ts = (uint64_t)DateTime::UtcNow().getMillis();
//		rv = updateCSFWUpdateFromOcppMessage2DB(p, sern, status, ts);
//	}
//	CLOSECONN
//	return rv;
//}
//*************************************************************************************************************************
//CSSerN ntext, Caller ntext, Param1 ntext, Reference ntext, Type integer, Address ntext,Instrs ntext,CertFlag integer,CreatedTS bigint,LastUpdtdTS bigint, Latitude real, Longitude real, ClientID INTEGER,
//Status integer,serviceGroup integer,ProfID integer,ObjB ntext,IPAddr ntext,FWVer ntext
bool  CentralOcppDB::readChS(SQLiteStatement *p, PCSItem *act)
{
	//PCSItem *act = new PCSItem();
	act->set_id(p->GetColumnInt64(0));
	act->set_serialn( p->GetColumnString(1) );  //CSSerN
	act->set_caller(p->GetColumnString(2));
	act->set_param1 ( p->GetColumnString(3) );   //Param1
	act->set_reference ( p->GetColumnString(4) );
	act->set_type ( p->GetColumnInt(5) );
	act->set_address ( p->GetColumnString(6) );
	
	act->set_instrs ( p->GetColumnString(7) );
	act->set_certflag ( p->GetColumnInt(8) );

	act->set_createdts(p->GetColumnInt64(9));
	act->set_lastupdtdts(p->GetColumnInt64(10));

	act->set_lat ( p->GetColumnDouble(11));
	act->set_lon ( p->GetColumnDouble(12) );
	act->set_clientid ( p->GetColumnInt(13) );
	act->set_status ( p->GetColumnInt(14));
	act->set_servicegroup ( p->GetColumnInt(15) );
	act->set_profid(p->GetColumnInt(16));												//ProfID integer
	
   /* int len=p->GetColumnBytes(17);
    const void *buf = p->GetColumnBlob(17);
    string s="";
    if(len>0&&buf!=0)
        s=string((const char*)buf,len);
    act->set_objb(s);*/

	string s = p->GetColumnString(17);
	act->set_objb(s);
    
	act->set_ipaddr(p->GetColumnString(18));
	act->set_fwver(p->GetColumnString(19));
	act->set_tz(p->GetColumnInt(20));
	act->set_dls(p->GetColumnInt(21));
	return true;// act;
}
int CentralOcppDB::readChS1(SQLiteStatement *p, PCSList *list)
{
	PCSItem *pWoi = new PCSItem();
	readChS(p, pWoi);
	list->mutable_cs_items()->AddAllocated(pWoi);
	//v.push_back(pWoi->id());
	int id1 = ridit();
	if (id1 == -1) id1 = 0;
	int id = !id1?pWoi->id():id1;
	return id;
}

int CentralOcppDB::getChargeStations(string sql, PCSList *list, bool with_status_update)
{

	int rv = 0;
	//std::vector<int> v;
	CREATECONNECTION
	try {
		if (with_status_update)
		{
			int64_t ts = (int64_t)DateTime::UtcNow().getMillis();
			ts = ts - (2 * 60 * 1000);  // if update did not come within last 3 minutes set status to zero
			string sql = StringPrintf("UPDATE CH_STATIONS SET status=0,UpdMob=1 WHERE LastUpdtdTS < %lld and status > 0", ts);
			p->SqlStatement(sql);
			rv = GetNuberOfRowsUpdated();
			//OCPPDB_PRINTF("getChargeStations: ts:%lld rv:%d \r\n", ts, rv);
		}

		p->Sql(sql);
		while (p->FetchRow())
		{
			readChS1(p,list);
			
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();


	}
	CLOSECONN
	return rv;
}
int CentralOcppDB::GetAllChargeStations(int cid,PCSList *list, bool with_status_update)
{
	//cid = 0;
	//OCPPDB_PRINTF("GetAllChargeStations: cid:%d rv:%d \r\n", cid);
	string sql = " SELECT ROWID,* FROM CH_STATIONS ";
	if(cid>0)
		sql += StringPrintf(" WHERE ClientID = %d ", cid);   ////ORDER BY StartTime DESC; ";
	return getChargeStations(sql,list, with_status_update);
}
int CentralOcppDB::GetAllChargeStations_UpdMob(int cid, PCSList *list)
{
	int rv = 0;
	//std::vector<int> v;
	CREATECONNECTION
	try {
		string sql = StringPrintf( "SELECT ROWID,* FROM CH_STATIONS WHERE UpdMob=1 and ClientID=%d",cid);
		p->Sql(sql);
		string idStr = "";
		while (p->FetchRow())
		{
			int id=readChS1(p, list);
			rv++;
			string s = std::to_string(id);
			if (!idStr.empty()) {
				idStr += ("," + s);
			}
			else {
				idStr += s;
			}
		}
		// do not forget to clean-up
		p->FreeQuery();

		//2. Clean UpdMob flag
		if (rv > 0 && !idStr.empty()) {
			sql = StringPrintf("UPDATE CH_STATIONS SET UpdMob=0 WHERE rowid IN(%s) and ClientID=%d",idStr.c_str(), cid);
			p->SqlStatement(sql);
			rv = GetNuberOfRowsUpdated();
		}
	}
	CLOSECONN
	return rv;
}

//*************************************************************************************************************************
void CentralOcppDB::SetParams(SQLiteStatement *p, PCSItem  *wi, int flags, bool sent,bool update)
{
//CSSerN ntext, Caller ntext, Param1 ntext, Reference ntext, Type integer, Address ntext,Instrs ntext,CertFlag integer,CreatedTS bigint,LastUpdtdTS bigint, Latitude real, Longitude real, ClientID INTEGER,
//Status integer,serviceGroup integer,ProfID integer,ObjB Blob,IPAddr ntext	
	p->BindString(1, wi->serialn());
	p->BindString(2, wi->caller());
	p->BindString(3, wi->param1());
	p->BindString(4, wi->reference());
	p->BindInt(5, wi->type());
	p->BindString(6, wi->address());
	p->BindString(7, wi->instrs());
	p->BindInt(8, /*wi->certflag()*/flags);

	p->BindInt64(9, wi->createdts());
	p->BindInt64(10, wi->lastupdtdts());
	p->BindDouble(11, wi->lat());
	p->BindDouble(12, wi->lon());
	
	p->BindInt(13, wi->clientid());
	p->BindInt(14, wi->status());
	p->BindInt(15, wi->servicegroup());
	p->BindInt(16, wi->profid());

	
	string s=wi->objb();
    //p->BindBlob(17,s.data(),s.length());
	p->BindString(17, s);

	p->BindString(18, wi->ipaddr());
	//p->BindString(19, wi->fwver());
	if(update)
		p->BindInt64(19, wi->id()); // must be a last one

}

int64_t CentralOcppDB::insertCSI(SQLiteStatement *p, PCSItem  *wi, int flags)
{
//CSSerN ntext, Caller ntext, Param1 ntext, Reference ntext, Type integer, Address ntext,Instrs ntext,CertFlag integer,CreatedTS bigint,LastUpdtdTS bigint, Latitude real, Longitude real, ClientID INTEGER,
//Status integer,serviceGroup integer,ProfID integer,ObjB Blob,IPAddr ntext	
	string sql = " INSERT INTO CH_STATIONS (CSSerN,Caller,Param1,Reference,Type,Address,Instrs,CertFlag,CreatedTS,LastUpdtdTS,Latitude,Longitude,ClientID,Status,serviceGroup,ProfID,ObjB,IPAddr)" +
		string(" VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	p->Sql(sql);
	SetParams(p, wi, flags, false,false);
	p->ExecuteAndFree();
	return GetLastInsertedRow();
}

#if 0
void CentralOcppDB::saveCSI(SQLiteStatement *p, PCSItem  *wi,int cud)
{
	
	string sql = "UPDATE CH_STATIONS SET CSSerN=?,Caller=?,Param1=?,Reference=?,Type=?,Address=?,Instrs=?,CertFlag=?,CreatedTS=?,LastUpdtdTS=?," +
		string(" Latitude=?,Longitude=?,ClientID=?,Status=?,serviceGroup=?,ObjB=?,IPAddr=?  WHERE ROWID = ?");
	p->Sql(sql);
	SetParams(p, wi,true,true);
	p->Execute();
	int tmp = GetNuberOfRowsUpdated();
	p->FreeQuery();
	if (tmp == 0)
	{
		insertCSI(p, wi);
	}

}

void CentralOcppDB::SaveChargeStations(PCSList *alist)
{
	CREATECONNECTION
	try {
		int len = alist->cs_items_size();
		if (len > 0)
		{
			for (int i = 0; i < len; i++)
			{
				PCSItem *pwi = alist->mutable_cs_items(i);
				if (pwi != 0)saveCSI(p, pwi);
			}
		}
	}
	CLOSECONN
}
#endif
int CentralOcppDB::UpdateChargeStationStatus(const char* field_name, std::string &field_value, int status)
{
	int rv = 0;
	CREATECONNECTION
	try {
		
				//string sql = StringPrintf("UPDATE CH_STATIONS SET Status=%d  WHERE IPAddr='%s'",status, field_name, field_value.c_str());
				string sql = StringPrintf("UPDATE CH_STATIONS SET Status=%d  WHERE %s='%s'", status, field_name, field_value.c_str());
				p->SqlStatement(sql);
				rv= GetNuberOfRowsUpdated();
	}
	CLOSECONN
	return rv;
}

#if 1
bool CentralOcppDB::GetChargeStationByID(int csid, PCSItem *woi)
{
	CREATECONNECTION
		try {
		string sql = StringPrintf("SELECT ROWID,* FROM CH_STATIONS  WHERE ROWID=%d", csid);
		p->Sql(sql);
		while (p->FetchRow())
		{
			 readChS(p, woi);
			 break;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
		return !exception_flag;
}
int CentralOcppDB::DeleteChargeStation(string sern, int cid)
{
	int rv = 0;
	CREATECONNECTION
		try {
		/*string sql = StringPrintf("DELETE FROM CH_STATIONS  WHERE CSSerN='%s'",sern.c_str());
		p->SqlStatement(sql);*/

		string sql = StringPrintf("UPDATE CH_STATIONS SET ClientID=0  WHERE CSSerN='%s'", sern.c_str());
		p->SqlStatement(sql);
		rv = GetNuberOfRowsUpdated();
	}
	CLOSECONN
		return rv;
}
int64_t CentralOcppDB::InsertNewChargeStationItem(PCSItem  *wi)
{
	int64_t rv = 0;
	CREATECONNECTION
	try {
		//1. Try to update it first
		std::string sern = wi->serialn();
		int cid = wi->clientid();

		int flags = 0;
		//1. Read CertFlag because it contains Public use flag
		string sql = StringPrintf("SELECT CertFlag FROM CH_STATIONS WHERE CSSerN='%s'",sern.c_str());
		p->Sql(sql);
		while (p->FetchRow())
		{
			flags = p->GetColumnInt(0);
		}
		// do not forget to clean-up
		p->FreeQuery();

		flags = (flags & ~3);
		flags |= (wi->certflag() & 3);

		sql = StringPrintf("UPDATE CH_STATIONS SET ClientID=%d,CertFlag=%d  WHERE ClientID=0 and CSSerN='%s'",cid,/*wi->certflag()*/flags, sern.c_str());
		p->SqlStatement(sql);
		rv = GetNuberOfRowsUpdated();
		if (rv == 0)
		{
			rv = insertCSI(p, wi,flags);
		}
	}
	CLOSECONN
	OCPPDB_PRINTF("InsertNewChargeStationItem..rv=%d\r\n",rv);
	return rv;
}
#endif

int CentralOcppDB::getLastRowIDInDB(SQLiteStatement *p, std::string tbl_name)
{
	int rv = 0;
	string sql = StringPrintf("SELECT ROWID FROM %s ORDER BY ROWID DESC LIMIT 1", tbl_name.c_str());
	p->Sql(sql);
	while (p->FetchRow())
	{
		rv = p->GetColumnInt(0);
	}
	// do not forget to clean-up
	p->FreeQuery();
	return rv;
}

int CentralOcppDB::GetLastRowIDInDB()
{
	int rv= -1;
	CREATECONNECTION
		try {
		rv = getLastRowIDInDB(p, "CH_STATIONS");
	}
	CLOSECONN
	return rv;
}


