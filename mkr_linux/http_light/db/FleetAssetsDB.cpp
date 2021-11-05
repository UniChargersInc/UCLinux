#include "FleetAssetsDB.h"
#include "CommonUtils/my_utils.h"
#include "CommonUtils/my_time.h"
#include "MkrLib.h"


static std::string getDidOnType(ASSETTYPE atype, int rid)
{
	string ridS = Int2Str(rid);

	string Did = "";
	if (atype == ASSETTYPE::MOBILE_WORKERS)
		Did = "MOBID-" + ridS;
	else if (atype == ASSETTYPE::ASSETS)   // assets device -> like satellite gps tracking
		Did = "ASTID-" + ridS;
	else if (atype == ASSETTYPE::FLEETS)  // GPS tracking for fleets
		Did = "FLTID-" + ridS;
	return Did;
}

ASSETTYPE  getAssetTypeOnDidType(std::string did_type)
{
	ASSETTYPE atype = ASSETTYPE::ASSETS;
	const char *pStr = did_type.c_str();
	if (strstr(pStr, "MOBID") != NULL)
		atype = ASSETTYPE::MOBILE_WORKERS;
	else if (strstr(pStr, "FLTID") != NULL)
		atype = ASSETTYPE::FLEETS;
	
	return atype;
}

//-------------------------------------------------------
FleetAssetsDB *FleetAssetsDB::_instance = 0;  // initialize static var member


void FleetAssetsDB::createTables()
{
	SQLiteStatement *p = CreateConnection();
	if (p == 0)
		goto __exit;
	try
	{
		if (!IsTableExist(p, "Assets"))
		{
			CreateAssetsTable(p);
		}
		if (!IsTableExist(p, "LocHist"))
		{
			CreateLocHistTable(p);
		}
		else {
			ModifyLocHistTable(p);
		}
		
		if (!IsTableExist(p, "AssetGroup"))
			CreateAssetGroupTable(p);
		else {
			ModifyAssetGroupTable(p);
		}
		
		if (!IsTableExist(p, "AssetConfig"))
			CreateAssetConfigTable(p);
		if (!IsTableExist(p, "POIs"))
			CreatePOIsTable(p);
		if (!IsTableExist(p, "POIGroup"))
			CreatePOIGroupTable(p);
		else {
			//ModifyPOIGroupTable(p);
		}
	}
	catch (SQLiteException &ex)
	{
		string msg = ex.GetString();
		m_printf("Exc:%s\r\n", msg.c_str());
	}
__exit:
	CloseConn();
}

void FleetAssetsDB::CreateAssetsTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE Assets ("
		"AssetID INTEGER PRIMARY KEY AUTOINCREMENT,GrID integer,"
		"CID integer,Username ntext, DeviceID ntext, Desc ntext, "
		"Lat real,Lon real,Speed integer,COG integer,TS bigint,ITS bigint,Sensors ntext,Address ntext,"
		"AssetType integer)";
	CreateTableA(p, sql);
}
void FleetAssetsDB::CreateLocHistTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE LocHist ("
		"CID integer, DID ntext,Lat real,Lon real,Speed integer,COG integer,TS bigint,Sensors ntext,AssetType integer,IgnSts integer)";
	CreateTableA(p, sql);
}
void FleetAssetsDB::ModifyLocHistTable(SQLiteStatement *pStmt)
{
	std::string sql = "ALTER TABLE LocHist ADD IgnSts integer;";     
	try {
		pStmt->SqlStatement(sql);
	}
	catch (SQLiteException &ex)
	{
		/*string msg = ex.GetString();
		m_printf("Exc:%s\r\n", msg.c_str()); */
	}
}


void FleetAssetsDB::CreateAssetGroupTable(SQLiteStatement *p)
{
	
	string sql = "CREATE TABLE AssetGroup ("
		"AGID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"CID integer, Desc ntext,Img ntext,GroupType integer)";
			
		CreateTableA(p, sql);


		
	
}
void FleetAssetsDB::ModifyAssetGroupTable(SQLiteStatement *pStmt)
{
	std::string sql = "ALTER TABLE AssetGroup ADD GroupType integer;";     // GroupType - ( parentID in a asset tree) 1-Assets, 2- Fleets, 3- Mobile workers, 4 - POIs
	try {
		pStmt->SqlStatement(sql);
	}
	catch (SQLiteException &ex)
	{
		/*string msg = ex.GetString();
		m_printf("Exc:%s\r\n", msg.c_str()); */
	}
}

//void FleetAssetsDB::CreateDevicesTable(SQLiteStatement *p)
//{
//
//}
void FleetAssetsDB::CreateAssetConfigTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE AssetConfig ("
		"DeviceID ntext, Email ntext"
		")";
	CreateTableA(p, sql);
}
void FleetAssetsDB::CreatePOIsTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE POIs ("
		"PoiID ntext,GrID integer,"
		"CID integer, PoiName ntext, Radius integer,GeoData ntext,"
		"PoiType integer,Color integer)";
	CreateTableA(p, sql);
}
void FleetAssetsDB::CreatePOIGroupTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE POIGroup ("
		"POIGID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"CID integer, Desc ntext,Img ntext,GroupType integer"
		")";
	CreateTableA(p, sql);
}
void FleetAssetsDB::ModifyPOIGroupTable(SQLiteStatement *pStmt)
{
	std::string sql = "ALTER TABLE POIGroup ADD GroupType integer;";     // GroupType - ( parentID in a asset tree) 1-Assets, 2- Fleets, 3- Mobile workers, 4 - POIs
	try {
		pStmt->SqlStatement(sql);
	}
	catch (SQLiteException &ex)
	{
		/*string msg = ex.GetString();
		m_printf("Exc:%s\r\n", msg.c_str()); */
	}
}
//--------------------------------------------------------------------------------
/*"AssetID INTEGER PRIMARY KEY AUTOINCREMENT,GrID integer,"
		"CID integer,Username ntext, DeviceID ntext, Desc ntext, "
		"Lat real,Lon real,Speed integer,COG real,TS integer,ITS integer,Sensors ntext,Address ntext,"
		"AssetType integer)";
*/
void FleetAssetsDB::SetAssetParams(SQLiteStatement *p, AssetData *ad,bool update)
{
	//p->BindInt(1, ad->grid());  -> must be last
	p->BindInt(1, ad->cid());
	p->BindString(2, ad->username());
	p->BindString(3, ad->did());
	p->BindString(4, ad->desc());
	p->BindDouble(5, ad->lat());
	p->BindDouble(6, ad->lon());
	p->BindInt(7, ad->spd());
	p->BindInt(8,(int) ad->cog());

	p->BindInt64(9, ad->ts());
	p->BindInt64(10,ad->its());

	p->BindString(11, ad->sensors());
	p->BindString(12, ad->address());
	
	p->BindInt(13, (int)ad->type());
	
	if (update)
	{
		p->BindInt(14, (int)ad->id());
		p->BindInt(15, ad->grid());
	}
	else {
		p->BindInt(14, ad->grid());
	}
}
void FleetAssetsDB::InsertNewAsset(PEmpl *emp, ASSETTYPE atype)
{
	CREATECONNECTION
	try
	{
		int cid = emp->cid(), rid = emp->id();
		string cidS = Int2Str(cid);
		string agn = "Group - " + cidS;//string agn = "Assets for " + cidS;
		string sql = StringPrintf("INSERT INTO AssetGroup (CID,Desc,Img) VALUES (%d,'%s','m115.png')", cid, agn.c_str());
		p->SqlStatement(sql);
		int grid = GetLastInsertedRow();
#if 1
		//create assets
		AssetData *ad = new AssetData();
		
		ad->set_grid(grid);
		ad->set_cid(cid);
		ad->set_username(emp->user());
		string Did = getDidOnType((ASSETTYPE)atype, rid);
		ad->set_did(Did);
		ad->set_desc(emp->drname());//ad->set_desc("Mobile Worker ( " + ridS + " )");
		ad->set_type((int)atype);

		sql = "INSERT INTO Assets (CID,Username,DeviceID,Desc,Lat,Lon,Speed,COG,TS,ITS,Sensors,Address,AssetType,GrID) "
			"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

		p->Sql(sql);
		SetAssetParams(p, ad, false);
		p->ExecuteAndFree();
		delete ad;

		sql = StringPrintf("INSERT INTO AssetConfig (DeviceID,Email) VALUES ('%s','%s')", Did.c_str(), emp->email().c_str());
		p->SqlStatement(sql);


		agn = "Default POIs " + cidS;
		sql = StringPrintf("INSERT INTO POIGroup (CID,Desc,Img) VALUES (%d,'%s','p117.png')", cid, agn.c_str());
		p->SqlStatement(sql);
		grid = GetLastInsertedRow();

		/*agn = "Default POI for " + cidS;
		string gdata = "POINT (-80.601879 35.74659065)";
		sql = StringPrintf("INSERT INTO POIs (GrID,CID,PoiName,Radius,GeoData,PoiType) VALUES (%d,%d,'%s',%d,'%s',%d)", grid, cid, agn.c_str(), 12, gdata.c_str(), 1);
		p->SqlStatement(sql);*/
#endif
	}
	CLOSECONN
}
bool FleetAssetsDB::removeAssetByMobileID(int rid)
{
	bool rv = false;
	CREATECONNECTION
	try
	{
		string Did =  getDidOnType(ASSETTYPE::MOBILE_WORKERS, rid);

		string sql = StringPrintf("delete from AssetConfig where DeviceID='%s'", Did.c_str());
		p->SqlStatement(sql);

		sql = StringPrintf("delete from Assets where DeviceID='%s'", Did.c_str());
		p->SqlStatement(sql);


		rv = true;
	}
	CLOSECONN
	return rv;
}
AssetData * FleetAssetsDB::InsertAssetIntoGroup( PEmpl *emp, int grid)
{
	AssetData *ad = 0;
	CREATECONNECTION
	try
	{
		int cid = emp->cid(), rid = emp->id();

		//create assets
		ad = new AssetData();  
		string ridS = Int2Str(rid);
		ad->set_grid(grid);
		ad->set_cid(cid);
		ad->set_username(emp->user());
		string Did = "MOBID-" + ridS;
		ad->set_did(Did);
		ad->set_desc(emp->drname());//ad->set_desc("Mobile Worker ( " + ridS + " )");
		ad->set_type(1);
		////message_ = "CHECK failed: IsInitialized(): Can't serialize message of type \"udorx_pbf.AssetData\" because it is missing required fields: ID, lat, lon, Spd, Cog, TS, ITS, Sensors, Address"
		ad->set_id(-1);
		ad->set_lon(0.1); ad->set_lat(0.1); ad->set_spd(1); ad->set_cog(0.1); ad->set_ts(1); ad->set_its(1); ad->set_sensors("*"); ad->set_address("*");


		string sql = StringPrintf("Select DeviceID from Assets where DeviceID='%s'",Did);
		string readDid = "";
		try {

			p->Sql(sql);
			while (p->FetchRow())
			{
				readDid=p->GetColumnString(0);
			}
			// do not forget to clean-up
			p->FreeQuery();
		}
		catch (SQLiteException &ex)
		{
			string msg = ex.GetString();
			m_printf("Exc:%s\r\n", msg.c_str());
		}
		if (readDid == Did)
		{
			// we need to update the row since Did already exists
			sql = "UPDATE Assets Set(GrID) VALUES (?) where DeviceID=? and CID=?";
			p->Sql(sql);
			p->BindInt(1, ad->grid());
			p->BindString(2, ad->did());
			p->BindInt(3, ad->cid());
		}
		else
		{
			sql = "INSERT INTO Assets (CID,Username,DeviceID,Desc,Lat,Lon,Speed,COG,TS,ITS,Sensors,Address,AssetType,GrID) "
				"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
			p->Sql(sql);
			SetAssetParams(p, ad, false);
		}
		
		p->ExecuteAndFree();
		//delete ad;

		sql = StringPrintf("INSERT INTO AssetConfig (DeviceID,Email) VALUES ('%s','%s')", Did.c_str(), emp->email().c_str());
		p->SqlStatement(sql);
		
	}
	CLOSECONN
	return ad;
}
//------------------------------------------------------
AssetData * FleetAssetsDB::readAsset(SQLiteStatement *p)
{
	//GrID,CID,Username,DeviceID,Desc,Lat,Lon,Speed,COG,TS,ITS,Sensors,Address,AssetType
	AssetData *a = new AssetData();
	a->set_id(p->GetColumnInt(0));
	a->set_grid(p->GetColumnInt(1));
	a->set_cid(p->GetColumnInt(2));
	a->set_username(p->GetColumnString(3));
	a->set_did(p->GetColumnString(4));
	a->set_desc(p->GetColumnString(5));

	a->set_lat(p->GetColumnDouble(6));
	a->set_lon(p->GetColumnDouble(7));

	a->set_spd(p->GetColumnInt(8));
	a->set_cog((float)p->GetColumnInt(9));

	a->set_ts(p->GetColumnInt64(10));
	a->set_its(p->GetColumnInt64(11));
	a->set_sensors(p->GetColumnString(12));
	a->set_address(p->GetColumnString(13));
	a->set_type(p->GetColumnInt(14));

	
	return a;
}
void FleetAssetsDB::getAssetsFromDB(string &sql, RepeatedPtrField<AssetData> *list)
{
	CREATECONNECTION
	try {

		p->Sql(sql);
		while (p->FetchRow())
		{
			list->/*mutable_alist()->*/AddAllocated(readAsset(p));       //mutable_wo_items()->AddAllocated(readAct(p));
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
   CLOSECONN
	
}
void FleetAssetsDB::GetAssetsFromDB(int cid, RepeatedPtrField<AssetData> *list)
{
	string sql = StringPrintf("Select * from Assets where CID=%d", cid);
	getAssetsFromDB(sql,list);
}
int FleetAssetsDB::listenForIncomingRTTEvents(int cid, int64_t ts,RttHBList *hb)
{
	int rv = 0;
	RepeatedPtrField<AssetData> list;
	// SELECT * FROM Assets WHERE  (ClientID=@cid and LastUpdatedTimeStamp>=@ts)
	string sql = StringPrintf("Select * from Assets where CID=%d and TS>=%lld", cid,ts);
	getAssetsFromDB(sql,&list);
	for (int i = 0; i < list.size(); i++)
	{
		AssetData ad = list.Get(i);
		//if (ad)
		{
			RttHB *rt = new RttHB();
			rt->set_lat(ad.lat());
			rt->set_lon(ad.lon());
			rt->set_spd(ad.spd());
			rt->set_cog(ad.cog());
			rt->set_ts(ad.ts());
			rt->set_rid(-1);
			rt->set_did(ad.did());
			rt->set_devtype(ad.type());

			
			hb->mutable_events()->AddAllocated(rt);
			rv++;
		}
	}
	

	return rv;
}
int FleetAssetsDB::readRttHBList(SQLiteStatement *p, int rid, std::string Did, RttHBList &hb_list)
{
	//"CID integer, DID ntext,Lat real,Lon real,Speed integer,COG integer,TS bigint,Sensors ntext,AssetType integer)";
	int cid1 = p->GetColumnInt(0);
	string did1 = p->GetColumnString(1);
	float Lat = p->GetColumnDouble(2);
	float Lon = p->GetColumnDouble(3);

	int speed = p->GetColumnInt(4);
	int cog = p->GetColumnInt(5);
	int64_t ts = p->GetColumnInt64(6);
	string sensors = p->GetColumnString(7);
	int atype = p->GetColumnInt(8);

	RttHB *rt = hb_list.add_events();
	rt->set_lat(Lat);
	rt->set_lon(Lon);
	rt->set_spd(speed);
	rt->set_cog(cog);
	rt->set_ts(ts); 
	rt->set_rid(rid);
	rt->set_did(Did);
	rt->set_devtype(atype);
	rt->set_sensors(sensors);
	return 1;
}
int FleetAssetsDB::getRttHBList(int cid, int rid, int64_t ts0, int64_t ts1, RttHBList &hb_list, int asset_type)
{
	int rv = 0;
	string Did = getDidOnType((ASSETTYPE)asset_type,  rid);//"MOBID-" + std::to_string(rid);
	CREATECONNECTION
	try
	{
		string sql = StringPrintf("Select * from LocHist where CID=%d and DID='%s' and TS > %lld and TS < %lld ;", cid, Did.c_str(),ts0,ts1);  // rowid is automatically created !  -> This gets the last row
		p->Sql(sql);
		while (p->FetchRow())
		{
			readRttHBList(p,rid,Did, hb_list);
			
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}

	CLOSECONN
	return rv;
}

int FleetAssetsDB::getRttTrips4Rpl(int cid, std::string didStr, int64_t ts0, int64_t ts1, RttTrips &rtt_trips)
{
	int rv = 0;
	int rid = -1;
	CREATECONNECTION
	try
	{
		string sql = StringPrintf("Select * from LocHist where CID=%d and DID='%s' and IgnSts>0 order by TS asc", cid, didStr.c_str());
		p->Sql(sql);
		RttHBList *rtt_list = rtt_trips.add_trips();//new RttHBList();
		while (p->FetchRow())
		{
			readRttHBList(p, rid, didStr, *rtt_list);
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}

	CLOSECONN

	

	//rtt_trips. addAllocated(rtt_list);

	return rv;
}

int FleetAssetsDB::InsertHB(int cid,RttHB *r)  // this is actually coming from a mobile worker
{
	int rv = 0;

	//AssetData *ad = new AssetData();
	int atype = r->devtype();

	std::string Did = getDidOnType((ASSETTYPE) atype, r->rid());
	
	float lat = r->lat();
	float lon = r->lon();
	int spd = r->spd();
	int cog = r->cog();
	int64_t ts = r->ts();  // should be in UTC !!!
	string sensors = r->sensors();
	
	string sql = "";
	CREATECONNECTION
	try
	{
		//1. Update Mobile Assets info -> Actually it's coming from Mobile worker phone
		sql = StringPrintf("UPDATE Assets Set Lat=%f,Lon=%f,Speed=%d,COG=%d,TS=%lld, Sensors='%s'  where CID=%d and DeviceID='%s' and AssetType=%d", lat, lon, spd, cog, ts,sensors.c_str(), cid, Did.c_str(), atype );
		p->SqlStatement(sql);

		//2. Update location history
		//2.1 Get last location
		double Lat = 0, Lon = 0;
		std::string prev_sensors = "";
		sql = StringPrintf("Select * from LocHist where CID=%d and DID='%s' ORDER BY rowid DESC LIMIT 1;", cid, Did.c_str());  // rowid is automatically created !  -> This gets the last row
		p->Sql(sql);
		while (p->FetchRow())
		{
			//"CID integer, DID ntext,Lat real,Lon real,Speed integer,COG integer,TS bigint,Sensors ntext,AssetType integer)";
			int cid1=p->GetColumnInt(0);
			string did1 = p->GetColumnString(1);
			Lat=p->GetColumnDouble(2);
			Lon = p->GetColumnDouble(3);

			prev_sensors= p->GetColumnString(7);
			
			break;

		}
		// do not forget to clean-up
		p->FreeQuery();

		// detect if Ignition state has been changed
		int ign_state =0 ;
		{
			std::vector < std::string> v0, v;
			v0 = split(prev_sensors, ',');
			v = split(sensors, ',');
			if (v.size() >= 1 && v0.size() >= 1)
			{
				bool f = v[0] == "1" || v[0] == "0";
				bool f0 = v0[0] == "1" || v0[0] == "0";
				if (f && f0 && (v0[0] != v[0]))
				{
					ign_state = v[0] == "1" ? 1 : 2;   // 1 - ignition (OFF -> ON) - TRIP_START , 2 - ignition (ON->OFF) TRIP_END
				}
			}
		}

		//2.2 Compute the distance and insert if > limit
		//double measure(double lat1, double lon1, double lat2, double lon2);
		//if (lat != 0 && lon != 0)
		//{
		//	double d = measure(Lat, Lon, lat, lon); // distance in meters
		//	if (d > 100)
		//	{
		//		string sql = StringPrintf("INSERT INTO LocHist (CID,DID,Lat,Lon,Speed,COG,TS,Sensors,AssetType,IgnSts) VALUES (%d,'%s',%f,%f,%d,%d,%lld,'%s',%d,%d)", cid, Did.c_str(), lat, lon, spd, cog, ts,sensors.c_str(),atype, ign_state);
		//		p->SqlStatement(sql);
		//	}
		//}
	}
	CLOSECONN
	return rv;
}
//===================================================================



void FleetAssetsDB::GetAssetGroupList(int cid, RepeatedPtrField<ClsGroup> *list)
{
	
	CREATECONNECTION
	try {
		string sql = StringPrintf("Select * from AssetGroup where CID=%d", cid);

		p->Sql(sql);
		while (p->FetchRow())
		{
			ClsGroup *g = readClsGroup(p);
			list->AddAllocated(g);
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
	
}
PoiData *FleetAssetsDB::readPoi(SQLiteStatement *p)
{//"PoiID ntext,GrID integer,CID integer, PoiName ntext, Radius integer,GeoData ntext,PoiType integer
	PoiData *a = new PoiData();
	a->set_id(p->GetColumnString(0));
	a->set_grid(p->GetColumnInt(1));
	//a->set_cid(p->GetColumnInt(2));
	a->set_name(p->GetColumnString(3));
	a->set_radius(p->GetColumnInt(4));
	string geodata=p->GetColumnString(5);

	vector<string> v = split(geodata, ',');
	if (v.size() > 0 && (v.size()%2)==0)
	{
		for (int i = 0; i < v.size(); i += 2)
		{
			PPointLL *pp = a->add_pnts();
			string lonStr = v[i], latStr = v[i + 1];
			float lat=0, lon=0;
			try {
				lon = std::stof(lonStr);
				lat = std::stof(latStr);
			}
			catch (...) {}
			pp->set_lon(lon);
			pp->set_lat(lat);
		}
	}
	a->set_type(p->GetColumnInt(6));
	a->set_color(p->GetColumnInt(7));
	return a;
}
void FleetAssetsDB::GetPoisFromDB(int cid, RepeatedPtrField<PoiData> *list)
{
	
	CREATECONNECTION
	try {
		string sql = StringPrintf("Select * from Pois where CID=%d", cid);

		p->Sql(sql);
		while (p->FetchRow())
		{
			list->AddAllocated(readPoi(p));       //mutable_wo_items()->AddAllocated(readAct(p));
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN

}
int FleetAssetsDB::SavePoiToDB(int cid, int grId, PoiData *poi)
{//"PoiID ntext,GrID integer,CID integer, PoiName ntext, Radius integer,GeoData ntext,PoiType integer
	
	int rv = 0;
	string sql = "";
	string data = "";
	int len = poi->pnts_size();
	if (len <= 0)
		return -1;
	for (int i = 0; i < len; i++)
	{
		PPointLL *pp = poi->mutable_pnts(i);
		string s = std::to_string(pp->lon());
		if (i != 0)
			data += ",";
		data += s + ",";

		s = std::to_string(pp->lat());
		data += s ;
	}
	int radius = poi->radius();
	string pid = poi->id();
	int tmp = 0;
	CREATECONNECTION
	try
	{
		//1. Update POIs 
		sql = StringPrintf("UPDATE POIs Set PoiName='%s',GeoData='%s', PoiType=%d , Radius=%d, Color=%d  where CID=%d and GrID=%d and PoiID='%s'", poi->name().c_str(), data.c_str(), poi->type(), radius,poi->color(), cid, grId,pid.c_str());
		p->SqlStatement(sql,true);// postpone to free query -> we do it after GetNuberOfRowsUpdated()
		tmp = GetNuberOfRowsUpdated();
		p->FreeQuery(); // do not forget to clean-up
		
		if (tmp == 0)
		{
			
				string sql = StringPrintf("INSERT INTO POIs (PoiID,GrID,CID,PoiName,Radius,GeoData,PoiType,Color) VALUES ('%s',%d,%d,'%s',%d,'%s',%d,%d)", pid.c_str(), grId, cid, poi->name().c_str(), radius, data.c_str(), poi->type(),poi->color());
				p->SqlStatement(sql);
			
		}
		rv = 1;
	}
	CLOSECONN
	return rv;


}
ClsGroup *FleetAssetsDB::readClsGroup(SQLiteStatement *p)
{
	ClsGroup *g = new ClsGroup();
	g->set_id(p->GetColumnInt(0));
	g->set_desc(p->GetColumnString(2));
	g->set_img(p->GetColumnString(3));
	g->set_type(p->GetColumnInt(4));

	return g;
}
void FleetAssetsDB::GetPoiGroupList(int cid, RepeatedPtrField<ClsGroup> *list)
{
	
	CREATECONNECTION
	try {
		string sql = StringPrintf("Select * from PoiGroup where CID=%d", cid);

		p->Sql(sql);
		while (p->FetchRow())
		{
			ClsGroup *g = readClsGroup(p);
			list->AddAllocated(g);
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN

}