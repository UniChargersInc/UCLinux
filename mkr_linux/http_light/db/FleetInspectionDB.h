#ifndef MKR_INSPECTIONDB_H_INCLUDED
#define MKR_INSPECTIONDB_H_INCLUDED

#include <string>
#include <vector>
#include <stdexcept>

#include "BaseDB.h"
#include "HosObjects/Definitions.h"
#include "HosObjects/TimeUtil.h"

#include "CommonUtils/my_utils.h"

using namespace HosEngine;
using namespace udorx_pbf;
struct CategoryVehicleAssignment
{
	string groupIDs = "";
	/*int64_t*/string updateTS = "";
};

class FleetInspectionDB : public BaseDB
{
private:
	static FleetInspectionDB *_instance;
	void CreateTables();
	void CreateCategoryTable(SQLiteStatement *pStmt);
	void CreateCategoryGroupTable(SQLiteStatement *pStmt);
	void CreateCategoryVehicleAssignmentTable(SQLiteStatement *p);
	
	
	
public:
	FleetInspectionDB() : BaseDB("uc_fleet_inspections.db")
	{
		CreateTables();
	}
	static FleetInspectionDB *Get();
	PICatList* GetAllCategories(string vehID, bool att_query = false);
	void SaveCategories(PICatList* list, bool delete_flag, std::vector<int> &grIDs);
	void SaveCategory2VehicleAssignment(string vehId, const std::string &groupIDs);
	CategoryVehicleAssignment *GetVehicleCategories(string vehID);

	//-----------------Inspection Items------------------------
	void CreateInspectionItemTable(SQLiteStatement *pStmt);
	PIItemList* GetInspectionItemsByCategoryId(int categoryId);
	PIItemList* GetInspectionItemsByCategoryId(int categoryId, SQLiteStatement *p);
	PIItemList* GetInspectionItemChildren(int inspItemId);
	
	PIItem *GetInspectionItemById(int id);
	//void SaveInspectionItem(PIItem *inspectionItem);

	//-----------------------------------------InspectionReportDB-------------------------------------------------
	void CreateInspectionReportTable(SQLiteStatement *p);
	PIRptList* GetAllUnSent();
	void SetHaveSentDeleteOlder();

	
	PIRpt* GetInspReport(string InsRUID);
	PIRptList* GetAllInspReportsByVID(string vid, int64_t from=0, int64_t to=0);
	PIRptList* GetAllInspReportsByDriverID(int rid, int64_t from, int64_t to);
	bool SaveInspectionReports(PIRptList *irList);

	//========================================================
	
	int UpdateCategory(PICat *icr, SQLiteStatement *p);
	int64_t InsertCategory(PICat *icr, SQLiteStatement *p);
	int64_t InsertInspectionCategoryGroupFromBrowser(int cid, int gid,string name, PCatTree *insp_cats);

	int GetMaxInspectionCategoryGroup();
};

#endif

