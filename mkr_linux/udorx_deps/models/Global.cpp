#include "Global.h"
#include "db_interface.h"

#define SIMULATE_MY_LOCATION    1

Global* Global::_instance = 0;
int Global::GetDID() 
{
#ifdef WIN32
	return 1001;
#endif
	/*if (this->pL != 0)
		return pL->resourceid();
	else
		return -1;*/
	return did;
}
int Global::GetCID()
{
	return cid;
}
void Global::SetRegister(PEmpl *pE)
{

}
void Global::SetLogin(ClsLoginMD *pL) 
{ 
	//this->pL = pL;
	this->did = pL->rid();
	this->DrName = pL->drname();
	this->cid = pL->mutable_lgn()->cid();
	string DID = GetDriverID();
	//store login response to DB
	
	PVehicleList vList = 0;// pL->mutable_vehicles();
	if (vList != 0)
	{
		//int len = vList->vitem_size();
		//m_printf("SetLogin ..DriverName:%s DID=%s vehicles:%d", this->DrName.c_str(),DID.c_str(), len);

		//if (len > 0)
		//{   /*
		//	for (int i = 0; i < len; i++)
		//	{
		//		PVehicle *vv = vList->mutable_vitem(i);
		//		m_printf("SetLogin ..Vehicle Desc:%s ", vv->desc().c_str());
		//	}*/
		//	//EmployeeDB::Get()->SaveVehicleList2DB(vList, DID);
		//}
	}//if (vList != 0)
	if(pL->has_driverprof())
	{
		PEmpl emp = pL->driverprof();
		m_printf("SetLogin ..DriverProf Name:%s ", emp.drname().c_str());

		EmployeeRow *_prof = new EmployeeRow();
		_prof->CopyFrom(emp);
		_prof->SetAutoLogin(true);
		_prof->set_cid(this->cid);

		UpdateCurrentProfile(_prof);
	}
}
void Global::CleanUp()
{
	did = 0;
	cid = 0;
	DrName = "";
}
/*
 * Number of seconds will be valid till the year 2038
 * 4294967295 = 0xFFFFFFFF
 4294967295000 = 0x3E7FFFFFC18 -> takes 42 bits
 we have available 64-42=22 bits -> max value=0x3FFFFF -> 4194303 - max number of Drivers
 * */

int64_t Global::GenerateUID()
{
    int64_t rv = DateTime::UtcNow().getMillis();
    int64_t did = GetDID();
    //if (did < 4194303)
        rv =(int64_t)(did << 42) | rv;
    return rv;    
}
string Global::GenerateTextUID()
{
    
    int64_t ts = DateTime::UtcNow().getMillis();
    string s = GetDriverID();
    string ts_txt=Int642Str(ts);
    s=s+"-"+ts_txt;
    return s;
}

string Global::GetDriverID()
{
#ifdef WIN32
	return "1001";
#endif
/*#if SIMULATE_MY_LOCATION
    return "1003";
#endif*/
	int v = GetDID();
	char buf[32] = { 0 };
	sprintf(buf, "%d", v);  //itoa is not supported
	return string(buf);
}
string Global::GetDriverName()
{
	/*if (this->pL != 0)
		return pL->drname();
	else
		return "";*/
	return DrName;
}

EmployeeRow *Global::GetCurrentProfile() 
{
	if (this->prof == 0)
	{
		std::string id = GetDriverID();
		EmployeeDB *db = EmployeeDB::Get();
		if (db != 0)
		{
			this->prof = db->GetEmployeeById(id);
		}
	}
	return this->prof;
}
void Global::SetCurrentProfile(EmployeeRow *_prof)
{
	if (this->prof != 0)
		delete this->prof;
	this->prof = _prof;
	if(_prof == NULL)
		CleanUp();
	else
	{
		this->cid = _prof->cid();
		this->did = _prof->id();
		this->DrName = _prof->drname();
/*
		if (_prof->tz() == -1)
		{
			int dst = 0; // daylight saving time;
			string tzStr = "";
			
			float tz = TimeUtil::getTimeZoneOffset(tzStr, dst, false);
			int tz1=TimeUtil::ComposeTZ((int) tz, dst);
			_prof->set_tz(tz1);
			TimeUtil::SetTimeZoneFromServer(tz, 1);
		}
		else
		{
			int dls = _prof->GetDLSavings();
			int tz = _prof->GetTZ();
			//	MKR_LOGI("TimeUtil::SetTimeZoneFromServer tz=%d\r\n", tz);
			TimeUtil::SetTimeZoneFromServer(tz, dls);
		}
*/
		int dst = 0; // daylight saving time;
		string tzStr = "";

		float tz = TimeUtil::getTimeZoneOffset(tzStr, dst, false);
		int tz1 = TimeUtil::ComposeTZ((int)tz, dst);
		this->prof->set_tz(tz1);
		TimeUtil::SetTimeZoneFromServer(tz, 1);

	}
}
//this call can come from two places 1. JniHos 2. From server
int Global::UpdateCurrentProfile(EmployeeRow *_prof)
{
	SetCurrentProfile(_prof);
	EmployeeDB *db = EmployeeDB::Get();
	if (db != 0)
	{
		db->SaveEmployee(this->prof);
	}
	return 1;
}


//-------------Vehicle--------------------------------------
void Global::setSelectedVehicle(string vid)
{
    //int flags;
    //if(this->selectedVeh!=0)
    //{
    //    flags=this->selectedVeh->flag();
    //    flags &= 0x7fffffff;  // clear bit 31
    //    EmployeeDB::Get()->setVehicleFlag(vid, flags);
    //    delete this->selectedVeh;
    //}
    //this->selectedVeh=EmployeeDB::Get()->GetVehicleById(vid);
    //flags=this->selectedVeh->flag();
    //flags|=0x80000000;
    //this->selectedVeh->set_flag(flags);
    //EmployeeDB::Get()->setVehicleFlag(this->selectedVeh->uid(), flags);
}
string Global::getSelectedVehicleID()
{
   /* if(this->selectedVeh==0)
    {
        PVehicle *v=EmployeeDB::Get()->getSelectedVehicle();
        if(v==0)
         return "";
        else
        {
            this->selectedVeh=v;
        }
    }*/
	return "";// this->selectedVeh->uid();
}
PVehicle *Global::getSelectedVehicle()
{
   /* if(this->selectedVeh==0)
    {
        PVehicle *v=EmployeeDB::Get()->getSelectedVehicle();
        if(v)
        {
            this->selectedVeh=v;
        }
    }
    return this->selectedVeh;*/
	return 0;
}

//-----------------Messaging--------------
MsgUserCls::MsgUserCls(/*string did,string name*/PMsg *pm)
{
    Global *g=Global::Get();
    senderId="";
    ts=-1;
    senderName=pm->sn();
    g->getSenderIdByMid(pm->mid(),senderId,ts); //m->did();
}

bool Global::addMsgUser(MsgUserCls &user)
{
    bool rv=false;
    size_t size = this->msg_users.size();
    bool found=false;
    if(size>0)
    {
        for(MsgUserCls u:this->msg_users)
        {
            if(u.senderId==user.senderId)
            {
                found=true;
                break;
            }
        }
    }
    if(!found)
    {
        this->msg_users.push_back(user);
        rv=true;
    }
    return rv;
}
bool Global::getMsgUserById(string did,MsgUserCls &user)
{
    bool found=false;
    size_t size = this->msg_users.size();
    if(size>0)
    {
        for(MsgUserCls u:this->msg_users)
        {
            if(u.senderId==did)
            {
                user.senderId=u.senderId;
                user.senderName=u.senderName;
                found=true;
                break;
            }
        }
    }
    return found;
}
bool Global::getSenderIdByMid(string mid,string &senderID,int64_t &ts)
{
    
    if(mid.empty())
        return false;
    vector<string> v= split(mid,'-');
    if(v.size()==2)
    {
        senderID=v[0];
        ts=Str2Int64(v[1]);
        return true;
    }
    
    return false;
        
}
std::string Global::getUserInitials(std::string name)
{
    std::string rv="";
    vector<string> v= split(name,' ');
    size_t len=v.size();
    if(len>1)
    {
        for(int i=0;i<len;i++)
        {
            std::string str=v[i];
            if(!str.empty())
            {
                rv+=(str.substr(0,1));
            }
            if(rv.size()>=2)
                break;         // Only two symbols
        }
    }else if(len==1)
    {
        rv=name.substr(0,2);
    }
    return rv;
}

//=========Location=====================
void Global::setLocation(double lat, double lon,float speed, float cog, int flag)
{
	mloc.lat = lat;
	mloc.lon = lon;
	mloc.speed = speed;
	mloc.cog = cog;
	mloc.flags = flag;// == 1 ? true : false;

}
#define SIMULATE_MY_LOCATION    0
MyLocation_T &Global::getCurrentLocation()
{
#if SIMULATE_MY_LOCATION
    setLocation(43.8499, -79.50193,0, 0, 1);
#endif
    return mloc;
    
}
