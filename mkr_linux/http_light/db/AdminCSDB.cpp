#include "CommonUtils/my_utils.h"//#include "CommonUtils/my_utils.h"
#include "AdminCSDB.h"
#include "MkrLib.h"
//#include "mkr_ocpp/mkr_ocpp.hpp"

#define OCPPDB_PRINTF 

AdminCSDB *AdminCSDB::_instance = 0;  // initialize static var member

void AdminCSDB::createTables()
{
	CREATECONNECTION
	try {
		//if (!IsTableExist(p, "CH_STATIONS"))
		{
			CreateAdminChargeStationsTable(p);
			
		}
		
	}
	CLOSECONN
}

void AdminCSDB::CreateAdminChargeStationsTable(SQLiteStatement *p)
{
	string sql = "";  //CSSerN - Charge Station SerialNumber 
	if (!IsTableExist(p, "CH_STATIONS"))
	{
		sql = "CREATE TABLE CH_STATIONS (CSSerN ntext,Caller ntext,Param1 ntext,Reference ntext, Type integer,"
			"Address ntext,Instrs ntext,CertFlag integer,CreatedTS bigint,LastUpdtdTS bigint, Latitude real, Longitude real, ClientID INTEGER,"
			"Status integer,serviceGroup integer,editFlag integer,ObjB Blob,IPAddr ntext)";  // TripID -> IPAddr  , CHName ntext -> type int

		CreateTableA(p, sql);
		OCPPDB_PRINTF("CH_STATIONS created..\r\n");
	}
	/*else {
		sql = "ALTER TABLE CH_STATIONS ADD COLUMN TripID ntext NOT NULL DEFAULT '*'";
		try {
			p->SqlStatement(sql);
		}
		catch (SQLiteException &ex)
		{
			m_printf("get CH_STATIONS exception : ", ex.GetString().c_str());
		}
	}*/
	
}


//*************************************************************************************************************************
//CSSerN ntext, Caller ntext, Param1 ntext, Reference ntext, Type integer, Address ntext,Instrs ntext,CertFlag integer,CreatedTS bigint,LastUpdtdTS bigint, Latitude real, Longitude real, ClientID INTEGER,
//Status integer,serviceGroup integer,editFlag integer,ObjB Blob,IPAddr ntext
bool  AdminCSDB::readChS(SQLiteStatement *p, PCSItem *act)
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
	//act->set_editflag(/*p->GetColumnInt(16)*/0);												//editFlag integer
	
    int len=p->GetColumnBytes(17);
    const void *buf = p->GetColumnBlob(17);
    string s="";
    if(len>0&&buf!=0)
        s=string((const char*)buf,len);
    act->set_objb(s);
    
	act->set_ipaddr(p->GetColumnString(18));

	return true;// act;
}


int AdminCSDB::getChargeStations(string sql, PCSList *list, int haveSentFlag)
{
	int rv = 0;
	std::vector<int> v;
	CREATECONNECTION
	try {
		p->Sql(sql);
		while (p->FetchRow())
		{
			PCSItem *pWoi = new PCSItem();
			readChS(p,pWoi);
			list->mutable_cs_items()->AddAllocated(pWoi);
			v.push_back(pWoi->id());
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();


	}
	CLOSECONN
	return rv;
}
int AdminCSDB::GetAllAdminChargeStations(int rid,PCSList *list, int max_number)
{
	//string sql = " SELECT ROWID,* FROM CH_STATIONS ";
	//if(rid>0)
	//	sql += StringPrintf(" WHERE ClientID = %d ", rid);   ////ORDER BY StartTime DESC; ";
	//sql += "ORDER BY ROWID DESC LIMIT 100";

	string sql = StringPrintf("select * from (select rowid,* from ch_stations order by rowid desc limit %d) order by rowid asc",max_number);

	return getChargeStations(sql,list,-1);
}


//*************************************************************************************************************************
void AdminCSDB::SetParams(SQLiteStatement *p, PCSItem  *wi,bool sent,bool update)
{
//CSSerN ntext, Caller ntext, Param1 ntext, Reference ntext, Type integer, Address ntext,Instrs ntext,CertFlag integer,CreatedTS bigint,LastUpdtdTS bigint, Latitude real, Longitude real, ClientID INTEGER,
//Status integer,serviceGroup integer,editFlag integer,ObjB Blob,IPAddr ntext	
	p->BindString(1, wi->serialn());
	p->BindString(2, wi->caller());
	p->BindString(3, wi->param1());
	p->BindString(4, wi->reference());
	p->BindInt(5, wi->type());
	p->BindString(6, wi->address());
	p->BindString(7, wi->instrs());
	p->BindInt(8, wi->certflag());

	p->BindInt64(9, wi->createdts());
	p->BindInt64(10, wi->lastupdtdts());
	p->BindDouble(11, wi->lat());
	p->BindDouble(12, wi->lon());
	
	p->BindInt(13, wi->clientid());
	p->BindInt(14, wi->status());
	p->BindInt(15, wi->servicegroup());
	p->BindInt(16,/* wi->editflag() */0);

	
	string s=wi->objb();
    p->BindBlob(17,s.data(),s.length());

	p->BindString(18, wi->ipaddr());
	
	if(update)
		p->BindInt64(19, wi->id()); // must be a last one

}
void AdminCSDB::SetParams4Update(SQLiteStatement *p, PCSItem  *wi)
{
    p->BindInt(1, wi->status());
        
    string s=wi->objb();
    p->BindBlob(2,s.data(),s.length());
    
  //  p->BindInt64(3, wi->id()); // must be a last one
    
}

int64_t AdminCSDB::insertCSI(SQLiteStatement *p, PCSItem  *wi)
{
//CSSerN ntext, Caller ntext, Param1 ntext, Reference ntext, Type integer, Address ntext,Instrs ntext,CertFlag integer,CreatedTS bigint,LastUpdtdTS bigint, Latitude real, Longitude real, ClientID INTEGER,
//Status integer,serviceGroup integer,editFlag integer,ObjB Blob,IPAddr ntext	
	string sql = " INSERT INTO CH_STATIONS (CSSerN,Caller,Param1,Reference,Type,Address,Instrs,CertFlag,CreatedTS,LastUpdtdTS,Latitude,Longitude,ClientID,Status,serviceGroup,editFlag,ObjB,IPAddr)" +
		string(" VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	p->Sql(sql);
	SetParams(p, wi, false,false);
	p->ExecuteAndFree();
	return GetLastInsertedRow();
}
void AdminCSDB::saveCSI(SQLiteStatement *p, PCSItem  *wi)
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
void AdminCSDB::SaveAdminChargeStations(PCSList *alist)
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
#if 1
void AdminCSDB::UpdateAdminChargeStation(PCSItem *woi/*,bool insert*/)
{
	CREATECONNECTION
		try {
		
				PCSItem *pwi = woi;
				if (pwi != 0)saveCSI(p, pwi);
			
		
	}
	CLOSECONN
}
void AdminCSDB::UpdateAdminChargeStationStatus(PCSItem *woi/*,bool insert*/)
{

	CREATECONNECTION
	try {
		string sql = "UPDATE CH_STATIONS SET Status=?,PodSig=?,HaveSent=HaveSent|1  WHERE ROWID=?";
		p->Sql(sql);
		SetParams4Update(p, woi);
		p->ExecuteAndFree();
	}
    CLOSECONN
}
#endif
#if 0
void AdminCSDB::SetHaveSent4WorkOrder()
{
	SQLiteStatement *p = CreateConnection();
	p->SqlStatement("UPDATE CH_STATIONS SET HaveSent=1");
	CloseConn();
}
#endif
void AdminCSDB::UpdateAdminChargeStationsStatus(PCSList *alist, int status, bool have_sent_flag)
{
	CREATECONNECTION
		try {
		int len = alist->cs_items_size();
		if (len > 0)
		{
			for (int i = 0; i < len; i++)
			{
				PCSItem *pwi = alist->mutable_cs_items(i);
				string sql = "UPDATE CH_STATIONS SET Status=?,HaveSent=?,PodSig=?  WHERE ROWID=?";
				p->Sql(sql);
				pwi->set_status(status);
				SetParams4Update(p, pwi /*false have_sent_flag*/);
				p->ExecuteAndFree();
			}
		}
	}
	CLOSECONN
}
#if 0
PCSIStatusList *AdminCSDB::GetAllUnSentChargeStationsWithStatusChange()
{
	PCSIStatusList *list = new PCSIStatusList();
	CREATECONNECTION
		try {
		p->Sql("SELECT ROWID,Status,PodSig FROM CH_STATIONS WHERE HaveSent=0");
		while (p->FetchRow())
		{
			list->mutable_cs_sts_items()->AddAllocated(readChSStatus(p));
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
		return list;
}
#endif

#if 1
bool AdminCSDB::GetAdminChargeStationByID(int csid, PCSItem *woi)
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
int AdminCSDB::DeleteAdminChargeStation(int woid, int cid)
{
	CREATECONNECTION
		try {
		string sql = StringPrintf("DELETE FROM CH_STATIONS  WHERE ROWID=%d",woid);
		p->SqlStatement(sql);
	}
	CLOSECONN
		return woid;
}
int64_t AdminCSDB::InsertNewAdminChargeStationItem(PCSItem  *wi)
{
	int64_t rv = 0;
	CREATECONNECTION
		try {
		rv = insertCSI(p, wi);
	}
	CLOSECONN
		OCPPDB_PRINTF("InsertNewChargeStationItem..rv=%d\r\n",rv);
	return rv;
}
#endif


int AdminCSDB::GetLastRowIDInAdminDB()
{
	int rv= -1;
	CREATECONNECTION
	try {
		p->Sql("SELECT ROWID FROM CH_STATIONS ORDER BY ROWID DESC LIMIT 1");
		while (p->FetchRow())
		{
			rv = p->GetColumnInt(0);
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
	return rv;
}


