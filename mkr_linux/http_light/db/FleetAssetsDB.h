#ifndef MKR_ASSETSDB_H_INCLUDED
#define MKR_ASSETSDB_H_INCLUDED
#include "BaseDB.h"

#include "HosObjects/Definitions.h"
#include "HosObjects/TimeUtil.h"

#include "models/models.h"

#include "proto/udorx_comm.pb.h"
#include "proto/udorx_common.pb.h"

#include "proto/udorx_ws_comm.pb.h"

#include "models/myassettypes.h"

using namespace std;

using namespace udorx_pbf;


using namespace HosEngine;


class FleetAssetsDB :public BaseDB
{
private:
	static FleetAssetsDB *_instance;
	int x;


	void createTables();
	void CreateAssetsTable(SQLiteStatement *pStmt);
	void CreateLocHistTable(SQLiteStatement *pStmt);
	void CreateAssetGroupTable(SQLiteStatement *pStmt);

	void ModifyAssetGroupTable(SQLiteStatement *pStmt);
	void ModifyLocHistTable(SQLiteStatement *pStmt);

	//void CreateDevicesTable(SQLiteStatement *pStmt);
	
	void CreateAssetConfigTable(SQLiteStatement *pStmt);

	void CreatePOIsTable(SQLiteStatement *pStmt);
	void CreatePOIGroupTable(SQLiteStatement *pStmt);
	void ModifyPOIGroupTable(SQLiteStatement *pStmt);

	void SetAssetParams(SQLiteStatement *p, AssetData *ad, bool update);
	AssetData *readAsset(SQLiteStatement *p);
	PoiData *readPoi(SQLiteStatement *p);
	ClsGroup *readClsGroup(SQLiteStatement *p);
	void getAssetsFromDB(string &sql, RepeatedPtrField<AssetData> *list);
	void insertAssetIntoGroup(SQLiteStatement *p, PEmpl *emp,int groupID);
	int readRttHBList(SQLiteStatement *p, int rid,std::string Did, RttHBList &hb_list);
public:
	FleetAssetsDB() : BaseDB("uc_fleet_assets.db")
	{
		createTables();
		x = 1;
	}


	//No, destructors are called automatically in the reverse order of construction. (Base classes last). 
	//Do not call base class destructors.
	//!! you do not have to declare the destructor virtual in the derived class
	~FleetAssetsDB()
	{
		x = 0;
	}

	static FleetAssetsDB *Get() {
		if (_instance == 0)
			_instance = new FleetAssetsDB();
		return _instance;
	}
	static void Clear() {
		if (_instance != 0)
		{
			delete _instance;
		}
		_instance = 0;
	}

	void InsertNewAsset(PEmpl *emp, ASSETTYPE atype);
	bool removeAssetByMobileID(int rid);
	AssetData * InsertAssetIntoGroup( PEmpl *emp, int groupID);

	void GetAssetsFromDB(int cid, RepeatedPtrField<AssetData> *list);
	void GetAssetGroupList(int cid, RepeatedPtrField<ClsGroup> *list);

	void GetPoisFromDB(int cid, RepeatedPtrField<PoiData> *list);
	void GetPoiGroupList(int cid, RepeatedPtrField<ClsGroup> *list);

	int SavePoiToDB(int cid, int grId,PoiData *poi);

	int listenForIncomingRTTEvents(int cid, int64_t ts, RttHBList *hb);
	int getRttHBList(int cid,int rid, int64_t ts0, int64_t ts1, RttHBList &hb_list,int asset_type);
//	int getRttTrips4Rpl(int cid, std::string didStr, int64_t ts0, int64_t ts1, RttTrips &rtt_trips);
	int InsertHB(int cid,RttHB *mhb);
};

#endif