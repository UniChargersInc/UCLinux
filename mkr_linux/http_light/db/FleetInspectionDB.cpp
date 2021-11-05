#include "FleetInspectionDB.h"
#include "MkrLib.h"

FleetInspectionDB *FleetInspectionDB::_instance = nullptr;

FleetInspectionDB *FleetInspectionDB::Get()
{
	if (_instance == nullptr)
	{
		_instance = new FleetInspectionDB();
	}
	return _instance;
}
void FleetInspectionDB::CreateTables()
{
	CREATECONNECTION
	try
	{
		if (!IsTableExist(p, "CATEGORY"))
		{
			CreateCategoryTable(p);
		}
		if (!IsTableExist(p, "CATEGORY_GROUP"))
		{
			CreateCategoryGroupTable(p);
		}
		if (!IsTableExist(p, "CATEGORY_VEHICLE"))
		{
			CreateCategoryVehicleAssignmentTable(p);
		}
		
		if (!IsTableExist(p, "INSPECTION_ITEM"))
		{
			CreateInspectionItemTable(p);
		}
		
		if (!IsTableExist(p, "INSPECTION_REPORT"))
		{
			CreateInspectionReportTable(p);
		}
	}
	CLOSECONN
}
void FleetInspectionDB::CreateCategoryTable(SQLiteStatement *pStmt)
{
	// Create a db table to store categories
	string sql = "CREATE TABLE CATEGORY (CatID INTEGER PRIMARY KEY AUTOINCREMENT, Description ntext,attID ntext,GroupID integer,BarCodeID integer);";

	pStmt->SqlStatement(sql);

	// Insert all default categories
	auto commands = std::vector<std::string>{ "Air Brake System", "Cab", "Cargo Securement", "Coupling Devices", "Dangerous Goods/Hazardous Materials", "Driver controls", "Driver Seat", "Electric Brake System", "Emergency Equipment and Safety Devices", "Exhaust System", "Frame and Cargo Body", "Fuel System", "General", "Glass and Mirrors", "Heater/Defroster", "Horn", "Hydraulic Brake System", "Lamps and Reflectors", "Steering", "Suspension System", "Tires", "Wheels, Hubs and Fasteners", "Windshield Wiper/Washer", "Refrigerated Trailer", "Trailer brake connections" };

    //int i=1;
	for (string command : commands)
	{
        sql=StringPrintf("INSERT INTO CATEGORY ( Description,attID,GroupID,BarCodeID) VALUES ('%s','',0,-1)",command.c_str());
        pStmt->SqlStatement(sql);
       // i++;
	}

}
void FleetInspectionDB::CreateCategoryGroupTable(SQLiteStatement *p)
{
	// Create a db table to store categories
	string sql = "CREATE TABLE CATEGORY_GROUP (GroupID integer PRIMARY KEY AUTOINCREMENT,ClientID integer, Name ntext,TS bigint);";

	CreateTableA(p, sql);

}

PICatList* FleetInspectionDB::GetAllCategories(string vehID, bool att_query )
{
	PICatList *list=new PICatList();
	CategoryVehicleAssignment vv; //CategoryVehicleAssignment *vv = new CategoryVehicleAssignment();

	CREATECONNECTION
		try{
	//1.
		string sql = StringPrintf("SELECT GroupIDs, updateTS FROM CATEGORY_VEHICLE where VehId='%s'", vehID.c_str());
				p->Sql(sql);
				while (p->FetchRow())
				{
					vv.groupIDs = p->GetColumnString(0);
					vv.updateTS = p->GetColumnString(1);
					
					break;
				}
			std::vector<int> grIDS;
			if (vv.groupIDs != "")
			{
				//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
				std::vector<std::string> s1 = split(vv.groupIDs,',');
				if (s1.size() > 0 && s1.size() > 0)
				{
					for (int i = 0; i < s1.size(); i++)
					{
						if (!(s1[i]).empty())
						{
							int x = Str2Int(s1[i]);
							if (x >= 0)
							{
								grIDS.push_back(x);
							}
						}
					}
				}
			}
			else
			{
				if (!att_query)
				{
					grIDS.push_back(0); // when there is no attachment (BoxID for attachment) info
				}
				// DO NOT get default categories !!!
			}
			p->FreeQuery();

			for (int grID : grIDS)
			{
				sql = StringPrintf("SELECT CatID, Description,attID,BarCodeID FROM CATEGORY where GroupID=%d", grID);
				p->Sql(sql);
				
					
					while (p->FetchRow())
					{
						short catID = p->GetColumnInt(0);
						std::string desc = p->GetColumnString(1);
						std::string barcodeLoc = p->GetColumnString(2);
						int barcodeid = p->GetColumnInt(3);
						PICat *icr = new PICat();
						icr->set_cid(catID);
						icr->set_desc(desc);
						icr->set_groupid(grID);
						icr->set_barcodeid(barcodeid);
						icr->set_bcloc(barcodeLoc);
						//, desc, grID, barcodeid, barcodeLoc
						icr->set_color(0);                        // color is a required field !!!!!
						list->mutable_list()->AddAllocated(icr);
					}
				p->FreeQuery();
				
			}
			for (int i = 0; i < list->list_size();i++)
			{
				PICat *icr = list->mutable_list(i);
				int cid = icr->cid();
				PIItemList* iiL = GetInspectionItemsByCategoryId(cid, p);
				//PIItemList iiL1=icr->items();
				if (iiL->list_size() > 0) {

					for (int i = 0; i < iiL->list_size();i++) {
						PIItem *ii = iiL->mutable_list(i);
						if (ii->parentid() == 0)
							icr->mutable_items()->mutable_list()->AddAllocated(ii);

					}
					
					/*if (this.items.Count > 0) {
						int levels = 0;
						createAllLevels(this.items, all_list, ref levels);
					}*/
				}
			}
	}//try
	CLOSECONN
	return list;
}

////, cat->Category->Replace("'", "''"), cat->BarCodeID, cat->CatID, cat->GroupID, cat->BarCodeLocation
void FleetInspectionDB::SetCatParams(SQLiteStatement *pStmt, PICat *icr)
{
	std::string str = icr->desc();
	pStmt->BindString(1, str);					
	pStmt->BindInt(2, icr->barcodeid());
	pStmt->BindString(3, icr->bcloc());
	pStmt->BindInt(4, icr->cid());				
	pStmt->BindInt(5, icr->groupid());        
	       
	
}
int FleetInspectionDB::UpdateCategory(PICat *icr, SQLiteStatement *p)
{
	string sql = "UPDATE CATEGORY SET Description=?,BarCodeID=?, attID=? WHERE CatID = ? and GroupID=? ";
	p->Sql(sql);
	SetCatParams(p, icr);
	p->Execute();
	int tmp = GetNuberOfRowsUpdated();
	p->FreeQuery();
	
	return tmp;
}
int64_t FleetInspectionDB::InsertCategory(PICat *icr, SQLiteStatement *p)
{
	
		string sql = "INSERT INTO CATEGORY (Description,BarCodeID,attID,CatID, GroupID) VALUES (?, ?,?,?,?)";
		p->Sql(sql);
		SetCatParams(p, icr);

		p->Execute();
		int64_t tmp = GetNuberOfRowsUpdated();
		p->FreeQuery();
	
	return tmp;
}
void FleetInspectionDB::SaveCategories(PICatList* list, bool delete_flag, std::vector<int> &grIDs)
{
	int tmp = -1;
	//just for test
	std::string s = "";
	for (auto v : grIDs)
	{
		s = s + Int2Str(v) + std::string(",");
	}
	
	CREATECONNECTION
		try{
			if (delete_flag)
			{
				if (grIDs.size() > 0)
				{
					for (auto groupId : grIDs)
					{
						string sql = StringPrintf("DELETE FROM CATEGORY WHERE GroupID=%d", groupId);
						p->SqlStatement(sql);
					}
				}
			}
			tmp = 0;
			int size = list->list_size();
			for (int i = 0; i < size;i++)
			{
				PICat *icr=list->mutable_list(i);
				if(UpdateCategory(icr, p)==0)
					InsertCategory(icr,p);
			}
		
	}
	
	CLOSECONN
}
void FleetInspectionDB::CreateCategoryVehicleAssignmentTable(SQLiteStatement *p)
{
	auto sql = "CREATE TABLE CATEGORY_VEHICLE (VehId ntext , GroupIDs ntext,  updateTS datetime); ";
	CreateTableA(p, sql);
}
void FleetInspectionDB::SaveCategory2VehicleAssignment(string vehId, const std::string &groupIDs)
{
	
	int tmp = -1;
	CREATECONNECTION
		try{
		string sql = "UPDATE CATEGORY_VEHICLE SET GroupIDs = ?,updateTS=? WHERE VehId = ?";
		p->Sql(sql);
			tmp = 0;
			p->BindString(1, groupIDs);
			p->BindString(2, DateTime::UtcNow().ToString());
			p->BindString(3,vehId );
			p->Execute();
			tmp = GetNuberOfRowsUpdated();
			p->FreeQuery();
			if (tmp == 0)
			{
				// Do an insert
				sql="INSERT INTO CATEGORY_VEHICLE (GroupIDs,updateTS,BoxId) VALUES (?,?,?)";
				p->Sql(sql);
				p->BindString(1, groupIDs);
				p->BindString(2, DateTime::UtcNow().ToString());
				p->BindString(3, vehId);
				p->Execute();
				tmp = GetNuberOfRowsUpdated();
				p->FreeQuery();

			}
	}
	CLOSECONN
}
CategoryVehicleAssignment *FleetInspectionDB::GetVehicleCategories(string vehID)
{
	CategoryVehicleAssignment *vv = new CategoryVehicleAssignment();
	auto sql = StringPrintf("SELECT GroupIDs, updateTS FROM CATEGORY_VEHICLE where VehId='%'", vehID.c_str());
	
	CREATECONNECTION
		try{
		p->Sql(sql);
				while (p->FetchRow())
				{
					vv->groupIDs = p->GetColumnString(0);
					vv->updateTS = p->GetColumnString(1);
					break;
				}
		p->FreeQuery();
	}
	CLOSECONN
	
	return vv;
}
//--------------------------------Inspection Items-----------------------------------------------

void FleetInspectionDB::CreateInspectionItemTable(SQLiteStatement *pStmt)
{
	// Create a db table to store inspection items
	string sql = "CREATE TABLE INSPECTION_ITEM (IIID INTEGER PRIMARY KEY AUTOINCREMENT, CatID INTEGER, Defect ntext, DefectAbbr ntext, DefectLevel INTEGER,parentId INTEGER,hasChildren integer,attID ntext);";
	pStmt->SqlStatement(sql);

	// Insert all default categories
    auto commands = std::vector<std::string>{ " (1,'Audible air leak', 'Audible air leak', 0,0,0,'')", 
		" (1, 'Slow air pressure build-up rate', 'Slow build-up rate', 0,0,0,'')", 
		" (1, 'Pushrod stroke of any brake exceeds the adjustment limit', 'Excess brake stroke', 1,0,0,'')", 
		" (1, 'Air loss rate exceeds prescribed limit', 'Excessive air loss', 1,0,0,'')", 
		" (1, 'Inoperative towing vehicle (tractor) protection system', 'Protection valve test failed', 1,0,0,'')", 
		" (1, 'Low air warning system fails or system is activated', 'Pressure warning failure', 1,0,0,'')", 
		" (1, 'Inoperative service, parking or emergency brake', 'Any brake failure', 1,0,0,'')", 
		" (2,'Occupant compartment door fails to open', 'Door malfunction', 0,0,0,'')", 
		" (2, 'Any door fails to close securely', 'Latch malfunction', 1,0,0,'')", 
		" (3, 'Insecure of improper load covering', 'Ineffective load cover', 0,0,0,'')", 
		" (3, 'Insecure cargo', 'Insecure Cargo', 1,0,0,'')",
		" (3, 'Absence, failure, malfunction or deterioration of required cargo securement device or load covering', 'Ineffective securement device', 1,0,0,'')", 
		" (4, 'Coupler or mounting has loose or missing fastener', 'Defective fastener', 0,0,0,'')", 
		" (4, 'Coupler is insecure of movement exceeds prescribed limit', 'Excessive free-play', 1,0,0,'')", 
		" (4, 'Coupling or locking mechanism is damaged or fails to lock', 'Lock failure/damaged', 1,0,0,'')", 
		" (4, 'Defective, incorrect or missing safety chain/cable', 'Ineffective chain/cable', 1,0,0,'')", 
		" (5, 'Dangerous goods/Hazardous Materials requirement not met', 'DG deficiency', 1,0,0,'')", 
		" (6, 'Accelerator pedal, clutch, gauges, audible and visual indicators or instruments fail to function properly', 'Instrument/Control malfunction', 0,0,0,'')", 
		" (7, 'Seat is damaged or fails to remain in set position', 'Damage/Adj. failure', 0,0,0,'')", 
		" (7, 'Seatbelt or tether belt is insecure, missing or malfunctions', 'Seatbelt/Tether defective', 1,0,0,'')", 
		" (8, 'Loose or insecure wiring or electrical connection', 'Deficient wiring or connections', 0,0,0,'')", 
		" (8, 'Inoperative breakaway device', 'Inoperative breakaway device', 1,0,0,'')", 
		" (8, 'Inoperative break', 'Inoperative break', 1,0,0,'')", 
		" (9, 'Emergency equipment is missing, damaged or defective', 'Deficent safty equipment', 0,0,0,'')", 
		" (10, 'Exhaust leak', 'Exhaust leak', 0,0,0,'')", 
		" (10, 'Leak that causes exhaust gas to enter the occupant compartment', 'Exhaust enters cabin', 1,0,0,'')", 
		" (11, 'Damaged frame or cargo body', 'Damaged frame/cargo body', 0,0,0,'')", 
		" (11, 'Visibly shifted, cracked, collapsing or sagging frame members', 'Failed frame member', 1,0,0,'')", 
		" (12, 'Missing fuel tank cap', 'Missing tank cap', 0,0,0,'')", 
		" (12, 'Insecure fuel tank', 'Insecure fuel tank', 1,0,0,'')", 
		" (12, 'Dripping fuel leak', 'Dripping fuel leak', 1,0,0,'')", 
		" (13, 'Serious damage or deterioration that is noticeable and may affect the safe operation of vehicle', 'Serious vehicle cond.', 1,0,0,'')", 
		" (14, 'Required mirror or window glass fails to provide the required view to the driver as a result of being cracked, broken, damaged, missing or maladjusted', 'Defective view', 0,0,0,'')", 
		" (14, 'Required mirror or glass has broken or damaged attachments onto vehicle body', 'Broken mount', 0,0,0,'')", 
		" (15, 'Control or system failure', 'Heater/defroster malfunction', 0,0,0,'')", 
		" (15, 'Defroster fails to provide unobstructed view through the windshield', 'Ineffective defroster', 1,0,0,'')", 
		" (16, 'Vehicle has no operative horn', 'Inoperative horn', 0,0,0,'')", 
		" (17, 'Brake fluid level is below indicated minimum level', 'Low fluid', 0,0,0,'')", 
		" (17, 'Parking brake is inoperative', 'Inoperative parking brake', 0,0,0,'')", 
		" (17, 'Brake boost or power assist is not operative', 'Inoperative power assist', 1,0,0,'')", 
		" (17, 'Brake fluid leak', 'Brake fluid leak', 1,0,0,'')", 
		" (17, 'Brake pedal fade or insufficient brake pedal reserve', 'Brake pedal fades', 1,0,0,'')", 
		" (17, 'Activated (other than ABS) warning device', 'Active brake warning', 1,0,0,'')", 
		" (17, 'Brake fluid reservoir is less than a quarter full', 'Missing brake fluid', 1,0,0,'')", 
		" (18, 'Required lamp does not function as intended', 'Lamp malfunction', 0,0,0,'')", 
		" (18, 'Required reflector is missing or partially missing', 'Missing reflector', 0,0,0,'')", 
		" (18, 'Failure of both low-beam headlamps', 'Headlamps failed', 1,0,0,'')", 
		" (18, 'Failure of both rearmost tail lamps', 'Tail lamps failed', 1,0,0,'')", 
		" (18, 'Failure of a rearmost turn-indicator lamp', 'Turn signal failed', 1,0,0,'')", 
		" (18, 'Failure of both rearmost brake lamps', 'Brake lamps failed', 1,0,0,'')", 
		" (19, 'Steering wheel lash (free-play) is greater than normal', 'Steering steering lash', 0,0,0,'')", 
		" (19, 'Steering wheel is insecure, or does not respond normally', 'Steering wheel malfunction', 1,0,0,'')", 
		" (19, 'Steering wheel lash (free-play) exceeds required limit', 'Steering lash exceeded', 1,0,0,'')", 
		" (20, 'Air leak in air suspension system', 'Air suspension leak', 0,0,0,'')", 
		" (20, 'Broken spring leaf', 'Broken spring', 0,0,0,'')", 
		" (20, 'Suspension fastener is loose, missing or broken', 'Defective anchor point', 0,0,0,'')", 
		" (20, 'Damaged, deflated air bag', 'Defective air bag', 1,0,0,'')", 
		" (20, 'Cracked or broken main spring leaf or more than one broken spring leaf', 'Spring failure', 1,0,0,'')", 
		" (20, 'Part of spring leaf or suspension is missing, shifted out of place or in ccontact with another vehicle component', 'Defective suspension mount', 1,0,0,'')", 
		" (20, 'Loose U-bolt', 'Loose U-bolt', 1,0,0,'')", 
		" (21, 'Damaged tread or sidewall of tire', 'Any tire damage', 0,0,0,'')", 
		" (21, 'Tire leaking', 'Tire leaking', 0,0,0,'')", 
		" (21, 'Flat tire', 'Flat tire', 1,0,0,'')", 
		" (21, 'Tire tread depth is less than wear limit', 'Visible tread wear bar', 1,0,0,'')", 
		" (21, 'Tire is in contact with another tire or any vehicle component other than mud-flap', 'Insufficient tire clearance', 1,0,0,'')", 
		" (21, 'Tire is marked ''Not for highway use''', 'Improper tire', 1,0,0,'')", 
		" (21, 'Tire has exposed cords in the tread or outer side wall area', 'Exposed cords', 1,0,0,'')", 
		" (22, 'Hub oil below minimum level (when fitted with sight glass)', 'Low hub oil', 0,0,0,'')", 
		" (22, 'Leaking wheel seal', 'Leaking wheel seal', 0,0,0,'')", 
		" (22, 'Wheel has loose, missing or ineffective fastener', 'Ineffective fastener', 1,0,0,'')", 
		" (22, 'Damaged, cracked or broken wheel, rim or attaching part', 'Defective rim/mount', 1,0,0,'')", 
		" (22, 'Evidence of imminent wheel, hub or bearing failure', 'Probable bearing failure', 1,0,0,'')", 
		" (23, 'Control or system malfunction', 'Wiper not operation', 0,0,0,'')", 
		" (23, 'Wiper blade damaged, missing or fails to adequately clear driver''s field of vision', 'Wiper ineffective', 0,0,0,'')", 
		" (23, 'Wiper or washer fails to adequately clear driver''s field of vision in area swept by driver''s side wiper', 'Fail to clear window', 1,0,0,'')", 
		" (24, 'Next service due date or mileage has exceed limit or will by end of current trip', 'Trailer service overdue', 0,0,0,'')", 
		" (24, 'Next service due date or engine hours have exceed limit or will by end of current trip', 'Reefer service overdue', 0,0,0,'')", 
		" (24, 'Fluid levels not within specified limits. Damaged or worn belts, hoses or wiring. Engine will not start or operate smoothly', 'Reefer engine operation', 0,0,0,'')", 
		" (24, 'Not able to operate temperature control or read temperature setting', 'Temperature control', 0,0,0,'')", 
		" (24, 'Not able to achieve or maintain set operating temperature', 'Operating temperature', 0,0,0,'')", 
		" (25, 'General', 'General', 0,0,0,'')" };

	for (auto command : commands)
	{
        sql="INSERT INTO INSPECTION_ITEM (CatID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID)  VALUES "+command;
        pStmt->SqlStatement(sql);
	}

}

PIItem * FleetInspectionDB::read_pitem(SQLiteStatement *p, PIItem *pII,int index)
{
	//PIItem *pII = new PIItem();
	pII->set_iid(p->GetColumnInt(index++));  //0
	pII->set_cid(p->GetColumnInt(index++));
	string str = p->GetColumnString(index++);
	pII->set_desc(str);
	str = p->GetColumnString(index++);    //3
//	pII->set_defabbr(str);
	pII->set_deflevel(p->GetColumnInt(index++));
	pII->set_parentid(p->GetColumnInt(index++));   // 5
	pII->set_haschildren(p->GetColumnInt(index++));
//	pII->set_attid(p->GetColumnString(index++));
	return pII;
}
PIItem *FleetInspectionDB::GetInspectionItemById(int id)
{
	PIItem *rv = 0;
	auto sql = StringPrintf("SELECT * FROM INSPECTION_ITEM WHERE IIID = %d", id);
	CREATECONNECTION
	try {

		p->Sql(sql);
		if (p->FetchRow())
		{
			rv = read_pitem(p, new PIItem());
		}
		p->FreeQuery();
	}
	CLOSECONN
	return rv;
}
PIItemList* FleetInspectionDB::GetInspectionItemsByCategoryId(int CatID, SQLiteStatement *p)
{
	string sql = StringPrintf("SELECT * FROM INSPECTION_ITEM WHERE CatID = %d", CatID);
	PIItemList* list = new PIItemList();
	p->Sql(sql);
	while (p->FetchRow())
	{
		PIItem *pII = new PIItem();
		list->mutable_list()->AddAllocated(read_pitem(p, pII));
	}
	p->FreeQuery();
	return list;
}
PIItemList* FleetInspectionDB::GetInspectionItemsByCategoryId(int CatID)
{
	PIItemList* list = 0;
	CREATECONNECTION
	try {
		list = this->GetInspectionItemsByCategoryId(CatID, p);
	}
	CLOSECONN
	return list;
}
PIItemList* FleetInspectionDB::GetInspectionItemChildren(int inspItemId)
{
	string sql = StringPrintf("SELECT * FROM INSPECTION_ITEM WHERE parentId= %d", inspItemId);
	PIItemList* list = new PIItemList();
	CREATECONNECTION
	try {
		p->Sql(sql);
		while (p->FetchRow())
		{
			PIItem *pII = new PIItem();
			list->mutable_list()->AddAllocated(read_pitem(p, pII));
		}

		p->FreeQuery();
	}
	CLOSECONN
	return list;
}
void FleetInspectionDB::SetIIParams(SQLiteStatement *pStmt, PIItem *ii, bool do_uldate)
{
	pStmt->BindInt(1, ii->cid());
	pStmt->BindString(2, ii->desc());
	pStmt->BindString(3, /*ii->defabbr()*/"");
	pStmt->BindInt(4, ii->deflevel());
	pStmt->BindInt(5, ii->parentid()); // parentid
	pStmt->BindInt(6, ii->haschildren());
	if(do_uldate)
		pStmt->BindInt(7, ii->iid());
}
//void FleetInspectionDB::SaveInspectionItem(PIItem *ii)
//{
//	CREATECONNECTION
//	try {
//
//		
//		if (tmp == 0)
//		{
//			
//
//		}
//	}
//	CLOSECONN
//}
int FleetInspectionDB::UpdateInspectionItem(PIItem *ii, SQLiteStatement *p)
{
	string sql = "UPDATE INSPECTION_ITEM SET CatID=?,Defect=?,DefectAbbr=?,DefectLevel=?,parentId=?,hasChildren=? WHERE IIID=?";
	p->Sql(sql);
	SetIIParams(p, ii);
	p->Execute();
	int tmp = GetNuberOfRowsUpdated();
	p->FreeQuery();
	return tmp;
}
int FleetInspectionDB::InsertInspectionItem(PIItem *ii, SQLiteStatement *p)
{
	// Do an insert
	string sql = "INSERT INTO INSPECTION_ITEM (CatID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren) VALUES (?,?,?,?,?,? );";
	p->Sql(sql);
	SetIIParams(p, ii,false);
	p->Execute();
	int rv =(int) GetLastInsertedRow();
	p->FreeQuery();
	return rv;
}
//-----------------------------------------InspectionReportDB-------------------------------------------------
void FleetInspectionDB::CreateInspectionReportTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE INSPECTION_REPORT (UID ntext PRIMARY KEY, ITS bigint, VID ntext,RID integer,RNAME ntext, CID integer,  Odo INTEGER, Flags INTEGER, cb_cats ntext,Addr ntext, Lat, Lon, Sent BOOLEAN); ";
	CreateTableA(p, sql);

	sql= "CREATE TABLE INSPECTION_REPORT_DEFFECTS(IRUID ntext, IIID INTEGER, Comments ntext, Clr ntext, ClrDriverID ntext, ClrDriverName ntext, attID ntext, MFILES ntext)";
	CreateTableA(p, sql);
}
PIRpt *FleetInspectionDB::readInspRow(SQLiteStatement *p)
{
	PIRpt *ir = new PIRpt();
		
	ir->set_uid(p->GetColumnString(0));
	ir->set_its(p->GetColumnInt64(1));
    ir->set_vid(p->GetColumnString(2));  // EqID
	ir->set_rid(p->GetColumnInt(3));
    ir->set_rname(p->GetColumnString(4));
	ir->set_cid(p->GetColumnInt(5));
    ir->set_odo(p->GetColumnInt(6));
    ir->set_flags( p->GetColumnInt(7));
    
	string str = p->GetColumnString(8);  // cb_cats + cb_a_cats
    ir->set_cb_cats(str);
	/*
    std::vector<std::string> s1 = split(str,'|');
	ir->set_cb_cats("");
	ir->set_cb_a_cats("");
	if (s1.size() ==2)
	{
		ir->set_cb_cats(s1[0]);
		ir->set_cb_a_cats(s1[1]);
	}
	else if (s1.size() == 1) {
		ir->set_cb_cats(s1[0]);
		
	}*/
	
	string addr = p->GetColumnString(9);
	ir->set_addr(addr);
	ir->set_lat( p->GetColumnDouble(10));
    ir->set_lon( p->GetColumnDouble(11));
    
	return ir;
}
void FleetInspectionDB::PopulateDeffectListInIR(PIRpt *ir, SQLiteStatement *p)
{
	string sql = StringPrintf(" SELECT * FROM INSPECTION_REPORT_DEFFECTS R left join INSPECTION_ITEM T ON R.IIID=T.IIID WHERE IRUID = '%s'", ir->uid().c_str());
	p->Sql(sql);
	//int count = sqlite3_data_count(p);
	while (p->FetchRow())
	{

		/*try
		{*/
			//IRUID INTEGER, IIID INTEGER, Comments ntext, Clr ntext, ClrDriverID ntext, ClrDriverName ntext, attID ntext, MFILES ntext
			PIItem *pII = new PIItem();
			pII->set_comments(p->GetColumnString(2));
			pII->set_clrd(p->GetColumnString(3));
			string str = p->GetColumnString(4);
			pII->set_clrdid(str);
			str = p->GetColumnString(5);
			pII->set_clrdname(str);
			str = p->GetColumnString(6);
			//pII->set_attid(str);

			//PIItem *pII = read_pitem(p);


			std::string s = p->GetColumnString(7); //MFILES
			if (!s.empty() && s.find(",") != string::npos)
			{
				//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
				std::vector<std::string> a = split(s, ',');
				if (a.size() > 0)
				{
					//pII->media_files = std::vector<std::string>();
					for (int i = 0; i < a.size(); i++)
					{
						string ss = a[i];
						if (!ss.empty())
						{
							pII->add_media_files(ss);
						}
					}
				}
			}
			read_pitem(p, pII, 8);
			if (pII->comments() != "" || (pII->media_files_size() > 0))
			{
				ir->mutable_defects()->AddAllocated(pII);
			}


		/*}
		catch (std::exception &ex)
		{
		}*/
	}
	p->FreeQuery();

}
void FleetInspectionDB::PopulateDeffectList(PIRptList* list, SQLiteStatement *p)
{
	int len = list->list_size();
	if (len == 0) return;
	for (int i = 0; i < len; i++)
	{
		PIRpt *ir = list->mutable_list(i);
		PopulateDeffectListInIR(ir, p);
	}//for (int i = 0; i < len; i++)
		
}

void FleetInspectionDB::SetIRParams(SQLiteStatement *p, PIRpt *ir,bool sent)
{
   	p->BindInt64(1,ir->its());
	p->BindString(2,ir->vid());
	p->BindInt(3, ir->rid());
    p->BindString(4, ir->rname());
    p->BindInt(5, ir->cid());
    p->BindInt(6, ir->odo());
 //   p->BindInt(6, ir->signed());
    p->BindInt(7, ir->flags());
    
	std::string checkedCats = ir->cb_cats() /*+ std::string("|") + ir->cb_a_cats()*/;
	p->BindString(8, checkedCats);

	p->BindString(9, ir->addr());

    p->BindDouble(10, ir->lat());
	p->BindDouble(11, ir->lon());
    p->BindBool(12, sent);
	
		p->BindString(13, ir->uid());

}
void FleetInspectionDB::SetIRDParams(SQLiteStatement *pStmt, PIItem *ii,string insRUID,string mfiles)
{
	//inspectionRow->InsRowID, ii->iID, ii->Comments, ii->Clrd, ii->ClrdId, ii->ClrdName, ii->attID, mfiles
	pStmt->BindString(1, insRUID);
	pStmt->BindInt(2, ii->iid());
	pStmt->BindString(3, ii->comments());
	pStmt->BindString(4, ii->clrd());
	pStmt->BindString(5, ii->clrdid()); 
	pStmt->BindString(6, ii->clrdname());
	pStmt->BindString(7, /*ii->attid()*/"");
	pStmt->BindString(8, mfiles);
}
void FleetInspectionDB::InsertDefects(PIRpt *ir, SQLiteStatement *p)
{
	int len = ir->defects_size();
	if (len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			PIItem *ii = ir->mutable_defects(i);
			std::string mfiles = adjustFiles(ii);
			//ii->Comments = ii->Comments->Replace("'", "''"); // escape '
			string sql = "INSERT INTO INSPECTION_REPORT_DEFFECTS (IRUID,IIID,Comments,Clr,ClrDriverId,ClrDriverName,attID,MFILES) VALUES (?,?,?,?,?,?,?,?)";
			p->Sql(sql);
			SetIRDParams(p, ii, ir->uid(), mfiles);
			p->ExecuteAndFree();
		}
	}
}
bool FleetInspectionDB::saveInspectionReport(SQLiteStatement *p,PIRpt *ir)
{
	bool rv = true;
    /*UID ntext PRIMARY KEY, ITS integer, VID ntext,RID integer, CID integer,  Odo INTEGER, Signed integer,Type INTEGER, cb_cats ntext, Lat, Lon, Sent BOOLEAN)*/
	try
	{
		
        int tmp=0;
        
            // Do an update
            string sql = "UPDATE INSPECTION_REPORT SET ITS=?,VID=?,RID=?,RNAME=?,CID=?,Odo=?,Flags=?,cb_cats=?,Addr=?,Lat=?, Lon=?, Sent=? WHERE UID=?";
            p->Sql(sql);
            SetIRParams(p, ir,false);
            p->Execute();
            tmp = GetNuberOfRowsUpdated();
            p->FreeQuery();
			if (tmp > 0)
			{
				// First delete all the deffects for this report and re-add them for updating procedure
				sql = StringPrintf("DELETE FROM INSPECTION_REPORT_DEFFECTS WHERE IRUID = '%s' ", ir->uid().c_str());
				p->SqlStatement(sql);

				InsertDefects(ir, p);
			}
        if (tmp==0)
			{
				// Do an insert
				string sql = "INSERT INTO INSPECTION_REPORT (ITS,VID,RID,RNAME,CID,Odo, Flags,cb_cats,Addr,Lat,Lon,Sent,UID) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)";
				p->Sql(sql);
				// bind all three values
				SetIRParams(p, ir,false);
				// execute the statement and reset the bindings
				p->ExecuteAndFree();
				/*
                sql = "SELECT last_insert_rowid();";
				p->Sql(sql);
                int id = -1;
				while (p->FetchRow())
				{
					id = p->GetColumnInt(0);
				}
				if (id > 0)
				{
					ir->set_insrid(id);
				}
				p->FreeQuery();
                */
				InsertDefects(ir, p);
			}
	}
	catch (SQLiteException &e)
	{
		/*string str = e.GetString();
		LOGE(str.c_str());*/
		rv = false;
	}
	catch (std::exception &ex)
	{/*
		string str = string(ex.what());
		if (!str.empty())
		{
		}*/
		rv = false;
	}
	
	return rv;
}


bool FleetInspectionDB::SaveInspectionReports(PIRptList *irList)
{
	bool rv = true;
	CREATECONNECTION
	try
	{
		
		for (int i = 0; i < irList->list_size(); i++)
		{
			PIRpt *r = irList->mutable_list(i);
			if (r)
			{
				if (!saveInspectionReport(p, r))
					rv = false;
			}
		}
	}
	CLOSECONN
	return rv;
}

std::string FleetInspectionDB::adjustFiles(PIItem *ii)
{
	std::string files = "";
	if (ii->media_files_size() > 0)
	{
		for (std::string s : ii->media_files())
		{
			files = files + s + std::string(","); // list of files -> comma delimited
		}
	}
	return files;
}

//---------------------------------------------------------------
PIRptList* FleetInspectionDB::get_all_insp_reports(string &sql/* , int type*/)
{
    PIRptList* list = 0;
	CREATECONNECTION
	try {

		p->Sql(sql);
		while (p->FetchRow())
		{
			PIRpt *ir = readInspRow(p);

			// if(type==0 || (type==ir->flags()&0xf) ) // take all
			{
				if (list == 0)
					list = new PIRptList();
				list->mutable_list()->AddAllocated(ir);

			}
			//else
			//    if(ir) delete ir;

		}
		p->FreeQuery();
		if (list != 0)
			PopulateDeffectList(list, p);
	}
	CLOSECONN
	return list;
}
PIRptList* FleetInspectionDB::GetAllUnSent()
{
	string sql = " SELECT * FROM INSPECTION_REPORT WHERE Sent = 0 ORDER BY ITS; ";
//string sql = " SELECT * FROM INSPECTION_REPORT ";
//string sql = " SELECT * FROM INSPECTION_REPORT where attid not like '%-%'";
return get_all_insp_reports(sql);
}
PIRptList* FleetInspectionDB::GetAllInspReportsByDriverID(int rid, int64_t from, int64_t to)
{//// type=0 - pretrip, 1- posttrip / flag 0-recent, 1 -old
#if 0  
	DateTime dt = TimeUtil::CurrentTime();
	int64_t ts = dt.getMillis();
	ts -= (int64_t)(24 * (int64_t)(3600 * 1000)); // one day in millis
	string tsStr = Int642Str(ts);
	string sql = StringPrintf("SELECT * FROM INSPECTION_REPORT WHERE VID='%s' and (Flags & %d)", VehID.c_str(), type);

	if (flag == 0)
		sql = sql + " and ITS >= " + tsStr;//sql =sql+ " and ITS >= datetime('now', '-1 day') ";
	else if (flag == 1)
		sql = sql + " and ITS < " + tsStr;//sql =sql+ " and ITS < datetime('now', '-1 day') ";
	sql = sql + " ORDER BY ITS Desc ";
#endif    
	//string sql = "SELECT * FROM INSPECTION_REPORT ";
	//string sql = StringPrintf("SELECT * FROM INSPECTION_REPORT WHERE VID='%s' ", VehID.c_str());
	string sql = StringPrintf("SELECT * FROM INSPECTION_REPORT WHERE RID=%d and ITS >= %lld and ITS <= %lld", rid,from,to);
	return get_all_insp_reports(sql);
}
PIRptList* FleetInspectionDB::GetAllInspReportsByVID(string vid, int64_t from, int64_t to)
{
	string sql = "SELECT * FROM INSPECTION_REPORT WHERE VID=";
	if(from!=0&&to!=0)
		sql = StringPrintf("%s '%s' and ITS >= %lld and ITS <= %lld",sql.c_str(), vid.c_str(), from, to);
	else
		sql = StringPrintf("%s '%s'",sql.c_str(), vid.c_str());
	return get_all_insp_reports(sql);
}
PIRpt* FleetInspectionDB::GetInspReport(string InsRUID)
{
	PIRpt *insRow = 0;
	string sql = StringPrintf(" SELECT * FROM INSPECTION_REPORT WHERE UID = '%s'  ", InsRUID.c_str());
	CREATECONNECTION
		try {
		p->Sql(sql);
		while (p->FetchRow())
		{
			insRow = readInspRow(p);
			break;;
		}
		p->FreeQuery();

		if (insRow != 0)
			PopulateDeffectListInIR(insRow, p);
	}
	CLOSECONN
		return insRow;
}

void FleetInspectionDB::SetHaveSentDeleteOlder()
{
	CREATECONNECTION
		try {

		DateTime dt = TimeUtil::CurrentTime();
		int64_t ts = dt.getMillis();
		ts -= (int64_t)(30 * 24 * (int64_t)(3600 * 1000)); // 30 days in millis
		string tsStr = Int642Str(ts);

		string sql = "DELETE FROM INSPECTION_REPORT_DEFFECTS WHERE IRUID IN (SELECT UID FROM INSPECTION_REPORT WHERE ITS < " + tsStr + ")";
		p->SqlStatement(sql);
		sql = "DELETE FROM INSPECTION_REPORT WHERE ITS < " + tsStr;
		p->SqlStatement(sql);

		sql = "UPDATE INSPECTION_REPORT SET Sent = 1; ";
		p->SqlStatement(sql);
	}
	CLOSECONN
}

//========================================================
int64_t FleetInspectionDB::InsertInspectionCategoryGroupFromBrowser(int cid, int gid, string name, PCatTree *insp_cats)
{
	int64_t groupID = -1;
	CREATECONNECTION
		try
	{
		//1.
		int64_t ts = DateTime::UtcNow().getMillis();
		string sql = StringPrintf("INSERT INTO Category_Group (ClientID,Name,TS) VALUES (%d,'%s',%lld)", cid, name.c_str(), ts);
		p->SqlStatement(sql);
		groupID = GetLastInsertedRow();
		//2. Insert Categories
		int size = insp_cats->cats_size();
		if (size > 0)
		{
			for (int i = 0; i < size; i++)
			{
				PCatTreeItem *pI = insp_cats->mutable_cats(i);
				if (pI == 0)
					continue;
				PICat icr;

				icr.set_desc(pI->value());
				icr.set_groupid(groupID);
				//icr.set_cid(pI->catid());
				icr.set_barcodeid(0);
				icr.set_bcloc("");
				icr.set_color(0);
				int catID = (int)InsertCategory(&icr, p);
				icr.set_cid(catID);

				//3. Check to see if this Category tree item has a subitems If so Include them to InspectionItem table
				int size1 = pI->data_size();
				if (size1 > 0)
				{
					for (int j = 0; j < size1; j++)
					{
						PCatTreeItem *pIi = pI->mutable_data(j);
						if (pIi == 0) continue;
						PIItem ii;
						ii.set_cid(catID);
						ii.set_desc(pIi->value());
						ii.set_deflevel(0);
						int rv=InsertInspectionItem(&ii, p);
					}

				}
			}
		}

	}
	CLOSECONN
	return groupID;
}

int FleetInspectionDB::GetMaxInspectionCategoryGroup()
{
	int groupID = -1;
	CREATECONNECTION
	try
	{
		groupID=(int)GetLastInsertedRow();
	}
	CLOSECONN
	return groupID;
}