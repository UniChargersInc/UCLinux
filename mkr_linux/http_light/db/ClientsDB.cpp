#include "FleetEmployeeDB.h"
#include "CommonUtils/my_utils.h"
#include "CommonUtils/my_time.h"
#include "MkrLib.h"
void m_printf(const char *format, ...);
void FleetEmployeeDB::createClientsTable(SQLiteStatement *pStmt)
{
	string sql = "CREATE TABLE CLIENTS ("
		"CID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"Username ntext, Password ntext,Email ntext,FullName ntext,Phone ntext,TS ntext, "
		"Status integer)";
	CreateTableA(pStmt, sql);

	//to get the ID starting number !!!!
	pStmt->SqlStatement("insert into CLIENTS (CID,Username) values (2600, 'foo')");
	pStmt->SqlStatement("delete from CLIENTS");
}
void FleetEmployeeDB::setCParams(SQLiteStatement *p, Client &c,unsigned int cid)
{
	if (cid > 0)
	{
		std::string cidS = std::to_string(cid);
		c.user = c.user + cidS;
		c.pass = c.pass + cidS;
	}
	p->BindString(1, c.user);
	p->BindString(2, c.pass);
	p->BindString(3, c.email);
	p->BindString(4, c.fullname);
	p->BindString(5, c.phone);
	DateTime dt = DateTime::UtcNow();
	p->BindString(6, dt.to_string());
	p->BindInt(7, 1);
}
int FleetEmployeeDB::InsertNewClient(Client &c)
{
	int rv = 0;
	string sql = "";
	CREATECONNECTION
	try {
		
		sql = StringPrintf("SELECT Username FROM CLIENTS WHERE Username = '%s' ", c.user.c_str());
		string vv = getStringValue(p, sql);
		if (!vv.empty())
		{
			rv= -1;   // Username is already in use
			goto __exit__;
		}
		
		//sql = StringPrintf("SELECT Email FROM CLIENTS WHERE Email = '%s' ", c.email.c_str());
		//vv = getStringValue(p, sql);
		//if (!vv.empty())
		//{
		//	rv= -2;   // Email is already in use
		//	goto __exit__;
		//}

		
		/*id = GetLastInsertedAutoIncrementID("CLIENTS");
		id+=1;*/
		sql = " INSERT INTO CLIENTS (Username,Password,Email,FullName,Phone,TS,Status)"
			" VALUES (?,?,?,?,?,?,?)";
		p->Sql(sql);
		setCParams(p, c, 0/*id*/);
		p->ExecuteAndFree();

	   rv = GetLastInsertedRow();  //immediately after your insert.  !!!

		c.cid = (rv);   // !!!!!

	__exit__: {}
	}
	
	CLOSECONN
		return rv; // == id) ? 1 : 0;
}
void FleetEmployeeDB::RemoveClient(int cid)
{
	CREATECONNECTION
	try
	{
		string sql = StringPrintf("DELETE FROM CLIENTS WHERE CID=%d", cid);
		p->SqlStatement(sql);
	}
	CLOSECONN
}
Client * FleetEmployeeDB::readClient(SQLiteStatement *p)
{
	Client *c = new Client();
	c->cid=(p->GetColumnInt(0));
	c->user = p->GetColumnString(1);
	c->pass = p->GetColumnString(2);
	c->email = p->GetColumnString(3);
	c->fullname=p->GetColumnString(4);
	c->phone= p->GetColumnString(5);
	c->timestamp = p->GetColumnString(6);
	c->status = (p->GetColumnInt(7));


	return c;
}
int FleetEmployeeDB::getAllClients(vector<Client *> &list,string &sql)
{
	int rv = 0;
	CREATECONNECTION
	try {
		p->Sql(sql);

		while (p->FetchRow())
		{
			list.push_back(readClient(p));
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
	return rv;
}
int FleetEmployeeDB::GetAllClients(vector<Client *> &list)
{
	string sql = " SELECT * FROM CLIENTS ";//ORDER BY StartTime DESC; ";
	return getAllClients(list, sql);
}
bool FleetEmployeeDB::ValidateWebClient(Client *c)
{
	vector<Client *> list;
	string sql = StringPrintf("SELECT * FROM CLIENTS Where Username='%s' and Password='%s'", c->user.c_str(), c->pass.c_str());
	int rv = getAllClients(list, sql);
	if (rv == 1)
	{
		Client *c1 = list[0];
		c->cid = c1->cid;
		c->email = c1->email;
		c->fullname = c1->fullname;
		c->status = c1->status;
		c->phone = c1->phone;
		c->timestamp = c1->timestamp;
	}
	stdH::clearVectorDynItems(list);//cleanUp list
	return rv == 1;
}
bool FleetEmployeeDB::ValidateMobileClient(Client *c, PEmpl *emp)
{
	bool rv = true;// ValidateWebClient(c);
	
	int rv1 = 0;
	if (rv)
	{
		if (c->ResourceID > 0)
			 rv=GetEmployeeById(emp,c->ResourceID);
		else
			rv=GetEmployeeByUserPass(emp,c->user, c->pass);

		if (rv&&emp->id() > 0)
		{
			c->cid = emp->cid();
			c->ResourceID = emp->id();
			c->drivername = emp->drname();         // 
			rv1 = 1;

		}
	}
	return (rv&&(rv1==1));
}