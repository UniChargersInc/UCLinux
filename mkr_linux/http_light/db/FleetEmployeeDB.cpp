#include "MkrLib.h"
#include "CommonUtils/my_utils.h"
#include "FleetEmployeeDB.h"
void m_printf(const char *format, ...);
FleetEmployeeDB *FleetEmployeeDB::_instance = 0;  // initialize static var member

void FleetEmployeeDB::createTables()
{
	CREATECONNECTION
	try
	{
		//m_printf("FleetEmployeeDB::createTables()\r\n");
		if (!IsTableExist(p, "EMPLOYEE"))
		{
			CreateEmployeeTable(p);
			//m_printf("CreateEmployeeTable..DONE\r\n");
		}
		else {
			ModifyEmployeeTable(p);
		}
		
		if (!IsTableExist(p, "CLIENTS"))
		{
			createClientsTable(p);
			m_printf("createClientsTable..DONE\r\n");
		}
		//m_printf("FleetEmployeeDB::createTables()..DONE\r\n");
	}
	CLOSECONN
}

/*
Looks like you can insert and delete a row to set whatever you want as the starting number:

sqlite> create table t1 (oid integer primary key autoincrement, a);
sqlite> insert into t1 values (100, 'foo');
sqlite> delete from t1;
sqlite> insert into t1 (a) values ('bar');
sqlite> select * from t1;
101|bar

*/


void FleetEmployeeDB::CreateEmployeeTable(SQLiteStatement *pStmt)
{
	std::string sql = "CREATE TABLE EMPLOYEE (ID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"DriverName ntext,Username ntext,Password ntext,License ntext,HomeAddress ntext,"
		"IsSupervisor BOOLEAN,Signature blob,Cycle integer,HOSExcept integer,ClientID integer,"
		"OrgName ntext,OrgAddr ntext,TimeZone integer,Email ntext,Photo ntext,LoginSts integer,LoginFCM ntext)";        
	CreateTableA(pStmt, sql); 

	//to get the ID starting number !!!!
	pStmt->SqlStatement("insert into EMPLOYEE (ID,DriverName) values (1000, 'foo')");
	pStmt->SqlStatement("delete from EMPLOYEE");
}
void FleetEmployeeDB::ModifyEmployeeTable(SQLiteStatement *pStmt)
{
	//std::string sql = "ALTER TABLE employee ADD LoginSts integer;";
	std::string sql = "ALTER TABLE employee ADD LoginFCM ntext;";
	try {
		pStmt->SqlStatement(sql);
	}
    catch (SQLiteException &ex) 
	{ 
		/*string msg = ex.GetString(); 
		m_printf("Exc:%s\r\n", msg.c_str()); */
	}
}





//*************************************************************************************************************************
//*************************************************************************************************************************
bool FleetEmployeeDB::get_employee(SQLiteStatement *pStmt, PEmpl *emp)
{
	
	//emp = new PEmpl();
	emp->set_id(pStmt->GetColumnInt(0));
	emp->set_drname(pStmt->GetColumnString(1));
	emp->set_user(pStmt->GetColumnString(2));
	emp->set_pass(pStmt->GetColumnString(3));
	emp->set_license(pStmt->GetColumnString(4));

	emp->set_haddr(pStmt->GetColumnString(5));
	emp->set_superv(pStmt->GetColumnBool(6));
	/*string sign = pStmt->GetColumnString(7);
	int len = sign.size();
	if(len>0)
	 emp->set_signature(sign);
	else
		emp->set_signature("");*/
	
	int len = pStmt->GetColumnBytes(7);
	const void *buf = pStmt->GetColumnBlob(7);
	string s = "";
	if (len>0 && buf != 0)
		s = string((const char*)buf, len);
	emp->set_sign(s);

	emp->set_cycle((int)(HOSCYCLE)pStmt->GetColumnInt(8));
	emp->set_h_exc(pStmt->GetColumnInt(9));

	emp->set_cid(pStmt->GetColumnInt(10));
	emp->set_orgname(pStmt->GetColumnString(11));
	emp->set_orgaddr(pStmt->GetColumnString(12));
	emp->set_tz(pStmt->GetColumnInt(13));
	emp->set_email(pStmt->GetColumnString(14));
	emp->set_photo(pStmt->GetColumnString(15));

	return true;
}
bool FleetEmployeeDB::get_employee(PEmpl *emp,std::string sql)
{
	bool rv = false;
	CREATECONNECTION
	try
	{
		p->Sql(sql);
		
		while (p->FetchRow())
		{
			get_employee(p, emp);
			break; // it will not allow to get more then 1 Employee
		}

		// do not forget to clean-up
		p->FreeQuery();
		rv = true;
	}
	CLOSECONN

	return rv;
}
bool FleetEmployeeDB::GetEmployeeById(PEmpl *emp,int id)
{
	std::string sql = StringPrintf("SELECT * FROM EMPLOYEE WHERE ID = %d ", id); 
	return get_employee(emp,sql);
}
int FleetEmployeeDB::GetLoginStsById(int id)
{
	std::string sql = StringPrintf("SELECT LoginSts FROM EMPLOYEE WHERE ID = %d ", id);

	int rv = -1;
	CREATECONNECTION
	try
	{
		p->Sql(sql);

		while (p->FetchRow())
		{
			rv=p->GetColumnInt(0);
			break;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
	return rv;

}
bool FleetEmployeeDB::GetEmployeeByUserPass(PEmpl *emp,string user, string pass)
{
	std::string sql = StringPrintf("SELECT * FROM EMPLOYEE WHERE Username='%s' and Password='%s'", user.c_str(),pass.c_str());
	return get_employee(emp,sql);
}

void FleetEmployeeDB::SetParams(SQLiteStatement *pStmt, PEmpl *emp, bool update, bool from_mobile)
{
	if (update && !from_mobile)
	{
		//"UPDATE EMPLOYEE SET DriverName=?,Password=?,Username=?,Email=?,Photo=? WHERE Id=? and ClientID=?"
		pStmt->BindString(1, emp->drname());					// DriverName
		pStmt->BindString(2, emp->pass());			// Password=@p2
		pStmt->BindString(3, emp->user());				// Username=@p3
		pStmt->BindString(4, emp->email());        //p17
		pStmt->BindString(5, emp->photo());        //p16

		pStmt->BindInt(6, emp->id());
		pStmt->BindInt(7, emp->cid());

		return;
	}
	std::string str = emp->drname();
	pStmt->BindString(1, str);					// DriverName=@p1
	pStmt->BindString(2, emp->pass());			// Password=@p2
	pStmt->BindString(3, emp->user());				// Username=@p3
	pStmt->BindString(4, emp->license());        //License=@p4
	
	pStmt->BindString(5, emp->haddr());        //p6
	pStmt->BindBool(6, emp->superv());                      //p7
	string sign = emp->sign();
	pStmt->BindBlob(7, sign.data(), sign.length());
	
	pStmt->BindInt(8, (int)emp->cycle());                      //p10
	

	pStmt->BindInt(9, emp->h_exc());                      //p11
	pStmt->BindInt(10, emp->cid());        //p12
	pStmt->BindString(11, emp->orgname());        //p13
	pStmt->BindString(12, emp->orgaddr());        //p14
	pStmt->BindInt(13, emp->tz());        //p15
	pStmt->BindString(14, emp->email());        //p17
	pStmt->BindString(15, emp->photo());        //p16
	if(update)
		pStmt->BindInt(16, emp->id());        //p18
}
void FleetEmployeeDB::SaveEmployee(PEmpl *emp, bool from_mobile)
{
	int tmp = 0;
	string sql = "";
	CREATECONNECTION
	try
	{
		if (emp->id() > 0)
		{
			/* If The request comes from mobworkers.js      -> The Update sql query should be added accordingly
			em.ID = rid;  //-1 -- em will be inserted
			em.CID = gL.cid;
			em.User = vv.uname;
			em.Pass = vv.pass;
			em.DrName = vv.fname;
			em.Email = vv.email;
			em.photo = upl_file_name;
			*/
			//std::string _HomeAddress = emp->haddr() + "|" + emp->state();  // !!!!
			if (from_mobile)
			{
				
				sql = "UPDATE EMPLOYEE SET DriverName=?,Password=?,Username=?,License=?,"
					"HomeAddress=?,IsSupervisor=?,Signature=?,Cycle=?,HOSExcept=?,ClientID=?,OrgName=?,"
					"OrgAddr=?,TimeZone=?,Email=?,Photo=? WHERE Id=?";
			}
			else {
				
				sql = "UPDATE EMPLOYEE SET DriverName=?,Password=?,Username=?,Email=?,Photo=? WHERE Id=? and ClientID=?";
			}
			p->Sql(sql);
			// bind all three values
			SetParams(p, emp, true, from_mobile);
			// execute the statement and reset the bindings
			p->Execute();
			tmp = GetNuberOfRowsUpdated();
			p->FreeQuery(); // do not forget to clean-up
		}
		if (tmp == 0)
		{
			sql = " INSERT INTO EMPLOYEE "
				"(DriverName,Password,Username, License,  HomeAddress, IsSupervisor,Signature,"
				"Cycle,HOSExcept,ClientID,OrgName,OrgAddr,TimeZone,Email,Photo) "
				"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
			p->Sql(sql);
			SetParams(p, emp, false, from_mobile);
			p->ExecuteAndFree();

			int id = GetLastInsertedRow();

			emp->set_id(id);   // !!!!!

		}
	}
	CLOSECONN
}
bool FleetEmployeeDB::RemoveEmployee(int empId)
{
	bool rv = false;
	CREATECONNECTION
	try
	{
		string sql = StringPrintf("DELETE FROM EMPLOYEE WHERE Id=%d",  empId);
		p->SqlStatement(sql);
		rv = true;
	}
	CLOSECONN
	return rv;
}
void FleetEmployeeDB::UpdateLoginStsById(int id, int sts)
{
	string sql;
	CREATECONNECTION
	try
	{ 
		sql = StringPrintf("UPDATE EMPLOYEE SET LoginSts=%d WHERE ID=%d ",sts,id);
		p->SqlStatement(sql);
	}
	CLOSECONN
		
}
void FleetEmployeeDB::UpdateFCMLoginStsById(int id,std::string msg, int sts)
{
	string sql;
	CREATECONNECTION
		try
	{
		sql = StringPrintf("UPDATE EMPLOYEE SET LoginFCM='%s' WHERE ID=%d ", msg.c_str(), id);
		p->SqlStatement(sql);
	}
	CLOSECONN

}
PEmplList * FleetEmployeeDB::getEmployeesFromDB(SQLiteStatement * p, string & sql)
{
	PEmplList *list = new PEmplList();
	p->Sql(sql);

	while (p->FetchRow())
	{
		PEmpl *v = new PEmpl();
		get_employee(p, v);
		list->mutable_list()->AddAllocated(v);
	}
	return list;
}
PEmplList * FleetEmployeeDB::GetEmployeesFromDB(int cid)
{
	PEmplList *list = 0;
	CREATECONNECTION
	try
	{
		string sql = StringPrintf("SELECT * FROM EMPLOYEE WHERE ClientID =  %d", cid);
		list = getEmployeesFromDB(p, sql);
	}
	CLOSECONN
	return list;
}
