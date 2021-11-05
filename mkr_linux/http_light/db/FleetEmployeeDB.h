#ifndef MKR_EMPLOYEEDB_H_INCLUDED
#define MKR_EMPLOYEEDB_H_INCLUDED
#include "BaseDB.h"

#include "HosObjects/Definitions.h"
#include "HosObjects/TimeUtil.h"

#include "models/models.h"


using namespace HosEngine;
using namespace udorx_pbf;

class FleetEmployeeDB:public BaseDB
{
private:
	static FleetEmployeeDB *_instance;
	int x;
	
	
	void createTables();
	void CreateEmployeeTable(SQLiteStatement *pStmt);
	void ModifyEmployeeTable(SQLiteStatement *pStmt);
	
	
	bool get_employee(SQLiteStatement *pStmt,PEmpl *emp);
	bool get_employee(PEmpl *emp,std::string sql);
	void SetParams(SQLiteStatement *pStmt, PEmpl *emp,bool update,bool from_mobile);
	PEmplList *getEmployeesFromDB(SQLiteStatement *p, string &sql);
	

	//=========================Clients=========================
	void createClientsTable(SQLiteStatement *pStmt);
	void setCParams(SQLiteStatement *p, Client &c,unsigned int cid);
	Client *readClient(SQLiteStatement *p);
	int getAllClients(vector<Client *> &list, string &sql);

	

public:
	FleetEmployeeDB() : BaseDB("uc_fleet_employee.db")
	{
		createTables();
		x = 1;
	}


	//No, destructors are called automatically in the reverse order of construction. (Base classes last). 
	//Do not call base class destructors.
	//!! you do not have to declare the destructor virtual in the derived class
	~FleetEmployeeDB()
	{
		x = 0;
	}

	static FleetEmployeeDB *Get(){
		if (_instance == 0)
			_instance = new FleetEmployeeDB();
		return _instance; 
	}
	static void Clear() {
		if (_instance != 0)
		{
			delete _instance;
		}
		_instance=0;
	}

	bool GetEmployeeById(PEmpl *emp,int id);
	int GetLoginStsById(int id);
	bool GetEmployeeByUserPass(PEmpl *emp,string user,string pass);
	void SaveEmployee(PEmpl *emp,bool from_mobile);
	bool RemoveEmployee(int empId);
	
	void UpdateLoginStsById(int id, int sts);
	void UpdateFCMLoginStsById(int id, std::string msg, int sts);


	//===========================Vehicles================================
	/*bool SaveVehicle2DB(PVehicle *vi,string &did, bool haveSent = true);
	bool SaveVehicleList2DB(PVehicleList *vList, string &did);
	PVehicleList * GetVehicleListFromDB(int cid);
	PVehicle * GetVehicleFromDB(int cid, string vid);
	bool RemoveVehicle(std::string  uid);*/
	PEmplList * GetEmployeesFromDB(int cid);


	//=======================Clients====================================
	int InsertNewClient(Client &c);
	void RemoveClient(int cid);
	int GetAllClients(vector<Client *> &list);
	bool ValidateMobileClient(Client *c, PEmpl *emp = 0);
	bool ValidateWebClient(Client *c);

	//====================Emails=======================================
	/*void CreateEmailTable(SQLiteStatement *pStmt);
	int InsertNewEmails(int cid,int rid,const vector<string> &v);*/


};

#endif