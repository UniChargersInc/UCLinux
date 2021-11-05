#include "FleetTimeLogDB.h"
#include "MkrLib.h"

#include <algorithm>

FleetTimeLogDB *FleetTimeLogDB::_instance = nullptr;
#pragma region FleetTimeLogDB

FleetTimeLogDB *FleetTimeLogDB::Get()
{
	if (_instance == nullptr)
	{
		_instance = new FleetTimeLogDB();
	}
	return _instance;
}
void FleetTimeLogDB::CreateTables()
			{
	CREATECONNECTION
				try
				{
					if (!IsTableExist(p, "TIMELOG"))
					{
						CreateTimeLogTable(p);
					}
					if (!IsTableExist(p, "DeferHours"))
					{
						CreateDeferHoursTable(p);
					}
					if (!IsTableExist(p, "Recap"))
					{
						CreateRecapTable(p);
					}
					
					if (!IsTableExist(p, "RuleSelectionHistory"))
					{
						CreateHOSRulesTable(p);
					}
					if (!IsTableExist(p, "HOSAlerts"))
					{
						CreateAlertsTable(p);
					}
				}
				CLOSECONN
			}
			void FleetTimeLogDB::CreateTimeLogTable(SQLiteStatement *p)
			{
				// Create a db table to store timelog report
				string sql = "CREATE TABLE TIMELOG (UID ntext PRIMARY KEY, LogStatus INTEGER, LogTime bigint, "
					"Logbookstopid INTEGER, Odometer INTEGER, DriverId integer, HaveSent BOOLEAN, IsFromServer BOOLEAN,"
					"Latitude, Longitude, VehID ntext,OLT bigint,Editor integer,Comment ntext,Signed BOOLEAN,"
					"Type integer,Codriver ntext,TimeZone integer,Address ntext,arGeo integer,QualifyRadiusRule ntext,HosCycle integer )";

				CreateTableA(p, sql);
			}

			bool FleetTimeLogDB::GetAllTimeLogs(PTlrList& list,int DriverID,int64_t ts)
			{
				string sql = "";
				int64_t ts1 = 0;
				   // desc limit 14
				if (ts > 0)
				{
					ts1 = ts + 24 * 3600 * 1000;
				}
				else
				{
					ts = DateTime::Now().Date().getMillis();
					ts1 =ts+ (24 * 3600 * 1000);
					ts -= (14*24 * 3600 * 1000);  // 14 days back
				}
				sql = StringPrintf("SELECT * FROM TIMELOG Where DriverID=%d and (LogTime >= %lld and LogTime < %lld)  ORDER BY LogTime", DriverID, ts, ts1);
				//var sql = "SELECT * FROM TIMELOG";
				return getTTList(list,sql,0,false);
			}
			MyDailyLogsList* FleetTimeLogDB::GetDailyLogs(int DriverID, int64_t start, int64_t end)
			{
				MyDailyLogsList* list = new MyDailyLogsList();
				CREATECONNECTION
					try {
					string sql = "";
					if (start > 0 && end > 0) {

					}
					else {
						start = DateTime::Now().Date().getMillis();
						end = start + (24 * 3600 * 1000);
						start -= (14 * 24 * 3600 * 1000);  // 14 days back
						
					}
					sql = StringPrintf("SELECT LogTime,Type FROM TIMELOG Where DriverID=%d and (LogTime >= %lld and LogTime < %lld) ORDER BY LogTime", DriverID, start, end);
					p->Sql(sql);
					int mN0 = 0, mN = 0;
					MyDailyLogs *dl = 0;
					while (p->FetchRow())
					{
						int64_t ts = p->GetColumnInt64(0);
						int type = p->GetColumnInt(1);
						
						int tt = (int)(ts / 1000);
						int tt0 = (tt % 86400);                      //(24 * 3600) = 86400
						mN = tt - tt0;  // calculate current midnight time
						if (mN != mN0)
						{
							dl = new MyDailyLogs();
							dl->set_dt((int64_t)tt * 1000);
							dl->set_count(1);
							dl->set_type(type);  //TimeLogType declared in SharedObjects.h
							list->mutable_hlogs()->AddAllocated(dl);
							mN0 = mN;
						}
						else
						{
							if (dl != 0)
							{
								int x = dl->count();
								x += 1;
								dl->set_count(x);
							}
						}

					}
					p->FreeQuery();
				}
				CLOSECONN

				return list;
				
			}
			bool FleetTimeLogDB::getTTList(PTlrList& list,const std::string &sql, int _status, bool _all)
			{
				
				CREATECONNECTION
					try {
					p->Sql(sql);

					

						while (p->FetchRow())
						{
							string uid = p->GetColumnString(0);
							int status = p->GetColumnInt(1);
							int type = p->GetColumnInt(16);
							TimeLogRow *logTime = NULL;
							if (_all)
							{
								logTime = new TimeLogRow();//(TimeLogRow *) list->add_list();
							}
							else
							{
								if (type != (int)TimeLogType::Modified)
								{
									logTime = new TimeLogRow(); //(TimeLogRow *) list->add_list();
								}
							}
							if (logTime == NULL)
								continue;

							int64_t ts = p->GetColumnInt64(2);
							string vid = p->GetColumnString(10);
							logTime->init(
								uid,
								status, ts,
								p->GetColumnInt(3),
								p->GetColumnInt(4),
								p->GetColumnInt(5),
								p->GetColumnBool(6),
								p->GetColumnBool(7),
								p->GetColumnDouble(8),
								p->GetColumnDouble(9),
								
								vid         // vehID
							);
							int64_t ts1=p->GetColumnInt64(11);
							
							int editor_flag = p->GetColumnInt(12);
							std::string comment = p->GetColumnString(13);
							logTime->updateEdit(ts1, editor_flag, comment);
							bool signed_Renamed = p->GetColumnBool(14);
							logTime->set_type(p->GetColumnInt(15));
							logTime->SetSigned(signed_Renamed);
							std::string s = "";
							//try{
							s = p->GetColumnString(16);

							logTime->set_codriver(s);
							logTime->set_tz(p->GetColumnInt(17));
							
							logTime->set_addr(p->GetColumnString(18));
							logTime->set_argeo(p->GetColumnInt(19));
							int qrr = p->GetColumnInt(20);
							logTime->set_qrr(qrr);
							
							qrr = p->GetColumnInt(21);  // hos cycle
							logTime->set_cycle(qrr);

							//}
							list.mutable_list()->AddAllocated(logTime);

						}

					
					// do not forget to clean-up
					p->FreeQuery();
				}
				CLOSECONN

				return !exception_flag;
			}

			void FleetTimeLogDB::SetParams(SQLiteStatement *pStmt, /*TimeLogRow*/PTlr *timLogRow, bool sent )
			{

				pStmt->BindInt(1, (int)timLogRow->event());  //@LogStatus=@p1
				pStmt->BindInt64(2, timLogRow->logtime()); //dt   //"@LogTime" = @p2
				pStmt->BindInt(3, timLogRow->lbsid());   //"@lbsid" =@p3
				pStmt->BindInt(4, timLogRow->odo());
				pStmt->BindInt(5, timLogRow->rid());   //"@DriverId" - @p5
				bool haveSent = sent;// == false ? timLogRow->GetHaveSend() : true;
				pStmt->BindBool(6, haveSent);
				pStmt->BindBool(7, /*timLogRow->GetisFromServer()*/false);
				pStmt->BindDouble(8, timLogRow->lat());
				pStmt->BindDouble(9, timLogRow->lon());
				pStmt->BindString(10, timLogRow->vid());
				pStmt->BindInt64(11, timLogRow->olt());
				pStmt->BindInt(12, timLogRow->editor());
				pStmt->BindString(13, timLogRow->comment());
				pStmt->BindBool(14, timLogRow->signed_());
				pStmt->BindInt(15, (int)timLogRow->type());
				pStmt->BindString(16, timLogRow->codriver());
				pStmt->BindInt(17, timLogRow->tz());
				pStmt->BindString(18, timLogRow->addr());
				pStmt->BindInt(19, timLogRow->argeo());
				pStmt->BindString(20, Int2Str(timLogRow->qrr()));
				pStmt->BindString(21, Int2Str(timLogRow->cycle()));
                pStmt->BindString(22, timLogRow->uid());
			}
			int FleetTimeLogDB::saveTimeLog(SQLiteStatement *p, /*TimeLogRow*/PTlr *timLogRow)
			{
				int rv = 1;
				try
                {
                    int tmp=0;
                    string sql = "UPDATE TIMELOG SET LogStatus=?,LogTime=?,Logbookstopid=?,Odometer=?,DriverId=?,HaveSent=?,"
						"IsFromServer=?,Latitude=?, Longitude=?,VehID=?,OLT=?,Editor=?,Comment=?,Signed=?,Type=?,Codriver=?,"
						"TimeZone=?,Address=?,arGeo=?,QualifyRadiusRule=?,HosCycle=?  WHERE UID = ?";
                    p->Sql(sql);
                    // bind all three values
                    SetParams(p, timLogRow);
                    // execute the statement and reset the bindings
                    p->Execute();
                    tmp=GetNuberOfRowsUpdated();
                    p->FreeQuery();
                    
                    if (tmp == 0)
					{

						std::string sql = "INSERT INTO TIMELOG (LogStatus, LogTime, Logbookstopid, Odometer, DriverId, HaveSent, IsFromServer, Latitude, Longitude, VehID,OLT,Editor,Comment,Signed,Type,Codriver,TimeZone,Address,arGeo,QualifyRadiusRule,HosCycle,UID) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
						p->Sql(sql);
						// bind all three values
						SetParams(p, timLogRow);
						// execute the statement and reset the bindings
						p->ExecuteAndFree();
						/*
                        sql = " SELECT last_insert_rowid();";
						pStmt->Sql(sql);
						int id = -1;
						while (pStmt->FetchRow())
						{
							id = pStmt->GetColumnInt(0);
						}
						if (id > 0)
						{
							timLogRow->set_tlid(id);
						}*/
					}
					
				}

				catch (SQLiteException  &ex)
				{
					string str = ex.GetString();
					LOGE(str.c_str());
					rv = -1;
				}
				
				return rv;
			}

			int FleetTimeLogDB::SaveTimeLog(PTlr &timLogRow, bool insert_this_row)
			{
				int rv = 1;
				CREATECONNECTION
					try {
					//p->BeginTransaction();
					p->SqlStatement("BEGIN TRANSACTION;");
					rv = saveTimeLog(p, &timLogRow);
					p->SqlStatement("COMMIT;");
					//p->CommitTransaction();
				}
				CLOSECONN
					return rv;
			}

			int FleetTimeLogDB::StoreTimeLogs2DB(int cid, PTlrList *tlrl)
			{
				int rv = 0;
				//CREATECONNECTION
				SQLiteStatement *p = CreateConnection(); 
					if (p == 0) 
						goto __exit;
				try 
				{
					for (int i = 0; i < tlrl->list_size(); i++)
					{
						PTlr *tlr = tlrl->mutable_list(i);
						if (tlr)
						{
							rv = saveTimeLog(p, (TimeLogRow *)tlr);
						}
					}
				}
				//CLOSECONN
				catch (SQLiteException &ex) 
				{ 
						string msg = ex.GetString(); 
						m_printf("Exc:%s\r\n", msg.c_str()); 
				}
				__exit: 
					CloseConn();
				return rv;
			}

			bool FleetTimeLogDB::GetAllUnSent(PTlrList& list)
			{
				std::string sql = "SELECT * FROM TIMELOG WHERE HaveSent = 0 ORDER BY LogTime";
				//cmd.Parameters.AddWithValue ("@HaveSent", true);
				return getTTList(list,sql,0,true);
				
			}

			void FleetTimeLogDB::SetHaveSentDeleteOlder()
			{
				CREATECONNECTION
					try {
					p->BeginTransaction();
					p->Transaction("DELETE FROM TIMELOG WHERE LogTime < datetime('now', '-30 day');");
					//pStmt->Transaction("DELETE FROM TIMELOG WHERE LogStatus=200");
					p->Transaction("UPDATE TIMELOG SET HaveSent=1");
					p->CommitTransaction();

					/*string	sql = "UPDATE TIMELOG SET HaveSent=?";
					pStmt->Sql(sql);
					pStmt->BindBool(1,true);
					pStmt->ExecuteAndFree();*/
				}
				CLOSECONN
			}

			bool FleetTimeLogDB::GetAllUnSent4Vehicle(PTlrList& list)
			{
				std::string sql = "SELECT * FROM TIMELOG WHERE LogStatus=202 ORDER BY LogTime";
				return getTTList(list,sql,202,true);
			}
			
			bool FleetTimeLogDB::UpdateSignedStatus(vector<TimeLogRow *> tlr_list)
			{
				bool rv = true;
				CREATECONNECTION
				try{
					int count = 0;
				
					for (TimeLogRow *tlr : tlr_list)
					{
					//cmd->Parameters->Clear();

						string sql = "UPDATE TIMELOG SET Signed = ?,HaveSent=0  where UID=?";
						p->Sql(sql);
						p->BindBool(1, tlr->signed_()); 
						p->BindString(2, tlr->uid());
						p->ExecuteAndFree();
					}
				}
				
				CLOSECONN
				
				return rv;
			}


			bool FleetTimeLogDB::UpdateDriverStatus(PTlr *tlr)
			{
				bool rv = true;
				int count = 0;
				CREATECONNECTION
				
				try
				{
					string sql = "UPDATE TIMELOG SET LogStatus = ?,Signed = ?,HaveSent=?  WHERE OLT = ?";
					p->Sql(sql);
					p->BindInt(1, (int)tlr->event());
					p->BindBool(2, true);
					p->BindBool(3, false);
					p->BindString(4, TimeUtil::SkipMs(tlr->olt()).ToString());
					p->ExecuteAndFree();
					
				}
				CLOSECONN
				//Log->Debug("TLD", std::string("UpdateDriverStatus count:") + std::to_string(count));
				return rv && count > 0;
			}

			void FleetTimeLogDB::UpdateAddressInTimeLog(DateTime dt, const std::string &address)
			{
				int count = 0;
				CREATECONNECTION
				try
				{
						string sql = "UPDATE TIMELOG SET Address =?, arGeo =?  WHERE OLT =?";
						p->Sql(sql);
						DateTime odts = TimeUtil::SkipMs(dt);
						p->BindString(1, address);
						p->BindInt(2, 1);
						p->BindString(3, odts.ToString());
						p->Execute();
						count += GetNuberOfRowsUpdated();
						p->FreeQuery();
				}
				CLOSECONN

			}
			//-------------------additional--------------------------
			bool Global__getSenderIdByMid(string mid, string &senderID, int64_t &ts)
			{

				if (mid.empty())
					return false;
				vector<string> v = split(mid, '-');
				if (v.size() == 2)
				{
					senderID = v[0];
					ts = Str2Int64(v[1]);
					return true;
				}

				return false;

			}


			bool FleetTimeLogDB::getLast(string sql, PTlr &tlr)
			{
				PTlrList list;
				bool rv= getTTList(list,sql, 0, true);
				std::sort(list.mutable_list()->begin(), list.mutable_list()->end(), [](const PTlr &a, const PTlr& b) {

					string senderID;
					int64_t a_ts, b_ts;
					Global__getSenderIdByMid(a.uid(), senderID, a_ts);
					Global__getSenderIdByMid(b.uid(), senderID, b_ts);

					return /*a.uid()*/a_ts > /*b.uid()*/b_ts;

				});

				if (list.list_size() > 0)
				{
					PTlr *tlr_orig = (PTlr *)list.mutable_list(0);
					tlr.CopyFrom(*tlr_orig);
				}
				return rv;
			}
			bool FleetTimeLogDB::GetLast(int driverId, PTlr &tlr)
			{
				string sql = StringPrintf("SELECT * FROM TIMELOG WHERE DriverId=%d ORDER BY LogTime DESC LIMIT 4", driverId);
				return getLast(sql,tlr);
			}
			bool    FleetTimeLogDB::GetLastBeforeDate(int did, int64_t ts, PTlr &tlr)
			{
				if (did > 0) //(did != "" && !did.find("-1") != std::string::npos)
				{
					int64_t date_ts = DateTime(ts).Date().getMillis();
					char buf[128];
					sprintf(buf, "SELECT * FROM TIMELOG WHERE DriverId=%d AND  LogTime < %lld ORDER BY LogTime DESC LIMIT 4;", did, date_ts);
					//string sql = string_format("SELECT * FROM TIMELOG WHERE DriverId=%d AND  LogTime < %lld ORDER BY LogTime DESC LIMIT 4;", did, date_ts); //changed LIMIT 1 to LIMIT 4 -> does not return the last row when several last rows have the same LogTime values
					string sql = string(buf);
					return getLast(sql, tlr);
				}
				return false;
			}
			
			void FleetTimeLogDB::DeleteLastRecord()
			{
				/* not sure
                SQLiteStatement *p = CreateConnection();
				string sql = "DELETE FROM TIMELOG WHERE uid = (SELECT MAX(uid) FROM TIMELOG);";
				p->SqlStatement(sql);
				CloseConn(); */
			}
#pragma endregion
#pragma region RecapDB
			void FleetTimeLogDB::CreateRecapTable(SQLiteStatement *pStmt)
			{
				// Create a db table to store recap
				string sql = "CREATE TABLE Recap (Id INTEGER PRIMARY KEY AUTOINCREMENT, DriverId ntext, " 
					"date bigint, today integer, " 
					"cycleTotal integer, cycleAvailable integer,cycle ntext, HaveSent BOOLEAN)";
				CreateTableA(pStmt, sql);
			}

			void FleetTimeLogDB::Recap_SetHaveSentDeleteOlder()
			{
				/*SQLiteStatement *pStmt = CreateConnection();
				pStmt->SqlStatement( "DELETE FROM Recap WHERE date < datetime('now', '-31 day');");
				CloseConn();*/
			}

			PRecapList* FleetTimeLogDB::getAllRecap(const std::string &driverId, const std::string &sql)
			{
				PRecapList* recaps=new PRecapList();
				if (driverId.empty())
				{
					return recaps;
				}
				
				CREATECONNECTION
					try{

					p->Sql(sql);
								
							while (p->FetchRow())
							{
								PRecap *recap = new PRecap();
								int64_t ts= p->GetColumnInt64(2);
								recap->set_date(ts);
								recap->set_today(p->GetColumnInt(3));
								recap->set_total(p->GetColumnInt(4));
								recap->set_avail(p->GetColumnInt(5));
								recap->set_cycle(p->GetColumnString(6));
								//recap->set_sent(p->GetColumnBool(7));
								recaps->mutable_list()->AddAllocated(recap);
							}
							p->FreeQuery();

				}
				CLOSECONN
				return recaps;
			}

			PRecapList* FleetTimeLogDB::GetAllRecapByDriver(const std::string &driverId)
			{

				string sql = StringPrintf("Select * From Recap WHERE DriverId = '%s' order by date desc limit 14 ", driverId.c_str());
				//string sql = StringPrintf("Select * From Recap WHERE DriverId = '%s' order by date", driverId.c_str());
				return getAllRecap(driverId, sql);
			}

			PRecapList* FleetTimeLogDB::GetAllRecapByDriverForCurrentDay(const std::string &driverId)
			{
				string sql = StringPrintf("Select * From Recap WHERE DriverId = '%s' and date >= date('now','-1 day') order by date; ", driverId.c_str());
				return getAllRecap(driverId, sql);
			}

			void FleetTimeLogDB::SetParams(SQLiteStatement *p, PRecap *rr, const std::string &driverId)
			{
				p->BindInt(1, rr->today());
				p->BindInt(2, rr->total());
				p->BindInt(3, rr->avail());
				p->BindString(4, rr->cycle());
				p->BindBool(5, false);
				p->BindInt64(6, rr->date());
				p->BindString(7, driverId);                  // driveId is the last !!!
			}

			void FleetTimeLogDB::AddRecap(PRecap *rr, const std::string driverId)
			{
				std::string sql = std::string(" INSERT INTO Recap (today,cycleTotal,cycleAvailable,cycle,HaveSent,date,DriverId)  VALUES (?,?,?,?,?,?,?)");
				_doRecap(rr, driverId, sql);
			}

			void FleetTimeLogDB::_doRecap(PRecap *rr, const std::string &driverId, const std::string &sql)
			{
				if (driverId.empty())
				{
					return;
				}
				CREATECONNECTION
					try{
					p->Sql(sql);
					SetParams(p, rr, driverId);
					p->ExecuteAndFree();
				}
				
				CLOSECONN
			}

			void FleetTimeLogDB::UpdateRecap(PRecap *rr, const std::string driverId)
			{
				std::string sql = std::string(" Update Recap set today=?,cycleTotal=?,cycleAvailable=?,cycle=?,HaveSent=?  where date=? and DriverId=?");
				_doRecap(rr, driverId, sql);
			}
			void FleetTimeLogDB::CleanUpRecapTable()
			{
				/*SQLiteStatement *p = CreateConnection();
				p->SqlStatement("delete from Recap");
				CloseConn();*/
			}
#pragma endregion

#pragma region class DeferHoursRepository
			void FleetTimeLogDB::CreateDeferHoursTable(SQLiteStatement *p)
			{
				auto sql = "CREATE TABLE DeferHours (Id INTEGER PRIMARY KEY AUTOINCREMENT, DriverId ntext, day1 bigint, day2 bigint, deferMinutes integer, HaveSent BOOLEAN); ";
				CreateTableA(p, sql);
			}

			void FleetTimeLogDB::DeleteDeferHour(DeferHourRow *dhr, const std::string &driverId)
			{
				if (driverId != "")
				{
					string sql = StringPrintf("Delete From DeferHours WHERE DriverId = '%s' AND strftime('%Y-%m-%d %H:%M:%S', day1) = '%s' and  strftime('%Y-%m-%d %H:%M:%S', day2) = '%s'; ", driverId.c_str(), dhr->day1.to_string().c_str(), dhr->day2.to_string().c_str());
					CREATECONNECTION
						try {
						p->SqlStatement(sql);
					}
					CLOSECONN
				}
			}

			DeferHourRow *FleetTimeLogDB::FindDeferHourByDate(DateTime date, const std::string &driverId)
			{
				if (driverId.empty())
				{
					return nullptr;
				}
				DeferHourRow *deferhour = nullptr;
				CREATECONNECTION
				try
				{
					string s = date.ToString();
					string st1 = "strftime('%Y-%m-%d %H:%M:%S', day1)";
					string st2 = "strftime('%Y-%m-%d %H:%M:%S', day2)";
					//string sql = StringPrintf("Select * From DeferHours WHERE DriverId = '%s' AND (strftime('%Y-%m-%d %H:%M:%S', day1) = '%s' or  strftime('%Y-%m-%d %H:%M:%S', day2) = '%s') order by day2 desc limit 1; ", driverId.c_str(), str.c_str());
					/*string sql = "Select * From DeferHours WHERE DriverId = '" + driverId +"' AND (";
					sql = sql + st+", day1)="+s;*/
					string sql= StringPrintf("Select * From DeferHours WHERE DriverId = '%s' AND (%s = '%s' or  %s = '%s') order by day2 desc limit 1; ", driverId.c_str(),st1.c_str(), s.c_str(),st2.c_str(),s.c_str());
					p->Sql(sql);

					while (p->FetchRow())
					{
						deferhour = new DeferHourRow();
						int64_t ts = p->GetColumnInt64(2);
						int64_t ts1 = p->GetColumnInt64(3);
						
						int v= p->GetColumnInt(4);

						deferhour->day1 = DateTime(ts);
						deferhour->day2 = DateTime(ts1);
						deferhour->defers = TimeSpan(0, v, 0);
						deferhour->haveSend= p->GetColumnBool(5);
					}
					p->FreeQuery();
				}
				CLOSECONN
				return deferhour;
			}

			void FleetTimeLogDB::DeferHours_SetHaveSentDeleteOlder()
			{
				/*SQLiteStatement *p = CreateConnection();
				p->SqlStatement("DELETE FROM DeferHours WHERE day1 < datetime('now', '-31 day');");
				CloseConn();*/
			}

			void FleetTimeLogDB::AddDeferHour(DeferHourRow *dhr, const std::string &driverId)
			{
				if (driverId.empty())
				{
					return;
				}
				CREATECONNECTION
				try
				{
					string sql = std::string(" INSERT INTO DeferHours (DriverId, day1, day2, deferMinutes,HaveSent)  VALUES (?,?,?,?,?);");
					p->Sql(sql);
						p->BindString(1, driverId);
						p->BindInt64(2, dhr->day1.getMillis());
						p->BindInt64(3, dhr->day2.getMillis());
						p->BindInt(4, int(dhr->defers.TotalMinutes()));
						p->BindBool(5, false);
						p->ExecuteAndFree();
				}
				CLOSECONN

			}
#pragma endregion

#pragma region RuleSelectionHistory

			void FleetTimeLogDB::CreateHOSRulesTable(SQLiteStatement *pStmt)
			{
				// Create a db table to store defer hour
				string sql = "CREATE TABLE RuleSelectionHistory (Id INTEGER PRIMARY KEY AUTOINCREMENT, " 
					"DriverId ntext, ruleid integer,ruleExc integer,  selectTime bigint, country ntext, HaveSent BOOLEAN)";
				CreateTableA(pStmt, sql);

			}

			void FleetTimeLogDB::setParams(SQLiteStatement *p, PRuleSH *rsr, const std::string &driverId, bool haveSent)
			{
				int cycle = int(rsr->ruleid());
				//Log.Debug ("RSH", "AddRSH DBPar:" + cycle.ToString());
				p->BindString(1, driverId);
				p->BindInt(2, cycle);
				p->BindInt(3, int(rsr->exc()));
				p->BindInt64(4, rsr->ts());
				p->BindString(5, rsr->country());
				p->BindBool(6, haveSent);

			}

			void FleetTimeLogDB::AddRuleSelectionHistory(PRuleSH *rsr, const std::string &driverId)
			{
				if (driverId.empty())
				{
					return;
				}
				int rv = 0;
				CREATECONNECTION
				try
				{
					
						string sql = std::string(" INSERT INTO RuleSelectionHistory (DriverId, ruleid,ruleExc, selectTime, country, HaveSent) ") 
							+ std::string(" VALUES (?, ?,?,?,?,?)");
						p->Sql(sql);
						setParams(p, rsr, driverId,false);
						p->ExecuteAndFree();
				}
				CLOSECONN
				
			}

			void FleetTimeLogDB::UpdateRuleSelectionHistoryFromServer(std::vector<PRuleSH*> &rlist, const std::string &driverId)
			{
				//if (driverId.empty())
				//{
				//	return;
				//}
				//SQLiteStatement *p = CreateConnection();
				//try
				//{
				//		for (auto rule : rlist)
				//		{
				//			PRuleSH *rsh = GetLastBeforeSelectTime(p, driverId, rule->selecttime());
				//			if (rsh == nullptr || rsh->ruleid() != rule->ruleid() || rsh->ruleexception() != rule->ruleexception())
				//			{
				//				std::string sql = std::string(" INSERT INTO RuleSelectionHistory (DriverId, ruleid,ruleExc, selectTime, country, HaveSent) ")
				//					+ std::string(" VALUES (?,?,?,?,?,?);");
				//				p->Sql(sql);
				//				setParams(p, rule, driverId, true);
				//				p->ExecuteAndFree();
				//			}
				//		}
				//}
				//catch (std::exception &ex)
				//{
				//	//Log->Debug("RSH", std::string("UpdateRuleSelectionHistoryFromServer Exc:") + ex.what());
				//}
				//CloseConn();
			}

			PRuleSH *FleetTimeLogDB::GetLastBeforeSelectTime(SQLiteStatement *p, const std::string &driverId, DateTime selectTime)
			{
				PRuleSH *rsr = nullptr;
				string sql = StringPrintf("Select * From RuleSelectionHistory WHERE DriverId = '%s' AND selectTime <= ? order by selectTime DESC LIMIT 1; ", driverId.c_str());
				p->Sql(sql);
				p->BindString(1, selectTime.ToString());
				p->Execute();
				while (p->FetchRow())
				{
						rsr = readRuleRow(p);
						break;
				}
				p->FreeQuery();
				return rsr;
			}

			PRuleSH *FleetTimeLogDB::readRuleRow(SQLiteStatement *p)
			{
				PRuleSH *rsr = new PRuleSH();
				int cycle = p->GetColumnInt(2);
				rsr->set_ruleid( (int) (HOSCYCLE)(cycle));
				rsr->set_exc((int) (RuleExceptions)(p->GetColumnInt(3)));
				rsr->set_ts(p->GetColumnInt64(4));
				rsr->set_country( p->GetColumnString(5));
				//rsr->set_sent( p->GetColumnBool(6));
				return rsr;
			}

			bool FleetTimeLogDB::_getRuleSelectionHistory(const std::string &sql, PRuleSHList* rlist)
			{
				bool rv = true;
				CREATECONNECTION
				try
				{
					p->Sql(sql);
					while (p->FetchRow())
							{
								PRuleSH *rsr = readRuleRow(p);
								rlist->mutable_list()->AddAllocated(rsr);
					}
				}
				CLOSECONN
				return rv;
			}
			PRuleSH* FleetTimeLogDB::GetLastRuleByDriverCountry(string driverId, string country)
			{
				PRuleSH *rsr = 0;
				CREATECONNECTION
				try{
				string sql = StringPrintf("Select * From RuleSelectionHistory WHERE DriverId = '%s' AND country = '%s' order by selectTime DESC LIMIT 1; ",  driverId.c_str(),country.c_str());
					p->Sql(sql);
					while (p->FetchRow())
					{
						rsr = readRuleRow(p);
						break;
					}
				}
				CLOSECONN
				return rsr;
			}

			PRuleSHList* FleetTimeLogDB::GetRuleSelectionHistory(const std::string &driverId)
			{
				PRuleSHList* rlist=new PRuleSHList();
				if (driverId.empty())
				{
					return rlist;
				}
				string sql = StringPrintf("Select * From RuleSelectionHistory WHERE DriverId = '%s' order by selectTime", driverId.c_str());
				 _getRuleSelectionHistory(sql,rlist);
				 return rlist;
			}

			PRuleSHList* FleetTimeLogDB::GetUnsentRuleSelectionHistory(const std::string &driverId)
			{
				PRuleSHList* rlist =new PRuleSHList();
				if (driverId.empty())
				{
					return rlist;
				}
				string sql = StringPrintf("Select * From RuleSelectionHistory WHERE DriverId = '%s' and HaveSent=0 order by selectTime", driverId.c_str());
				 _getRuleSelectionHistory(sql, rlist);
				 return rlist;
			}

			void FleetTimeLogDB::RulesHos_SetHaveSentDeleteOlder(const std::string &drId)
			{
				//SQLiteStatement *p = CreateConnection();
				//try
				//{
				//		string sql = "DELETE FROM RuleSelectionHistory WHERE selectTime < datetime('now', '-30 day');";
				//		p->SqlStatement(sql);
				//		
				//		sql = "UPDATE RuleSelectionHistory SET HaveSent = ? WHERE DriverId = ?";
				//		
				//		p->BindBool(1, true);
				//		p->BindString(2, drId);
				//		p->ExecuteAndFree();
				//	
				//}
				//catch (std::exception &ex)
				//{
				//	//Log->Debug("RSH", std::string("RulesHos_SetHaveSentDeleteOlder Exc:") + ex.what());
				//}
				//CloseConn();
			}

#pragma endregion

#pragma region AlertDB
			void FleetTimeLogDB::CreateAlertsTable(SQLiteStatement *pStmt)
			{
				// Create a db table to store alerts
				string sql = "CREATE TABLE HOSAlerts (Id INTEGER PRIMARY KEY AUTOINCREMENT, DriverId ntext, "
					"AlertDate bigint, DrivingAvail integer,OnDutyAvail integer, "
					"AlertType integer, OnDutyRuleViolated ntext,DrivingRuleViolated ntext, HaveSent BOOLEAN)";
				CreateTableA(pStmt, sql);
			}

			void FleetTimeLogDB::Alerts_SetHaveSentDeleteOlder()
			{
				SQLiteStatement *p = CreateConnection();
				p->SqlStatement("DELETE FROM HOSAlerts WHERE AlertDate < datetime('now', '-31 day');");
				p->SqlStatement("UPDATE HOSAlerts SET HaveSent = 0 ");
				CloseConn();
			}

			bool FleetTimeLogDB::getAllAlerts(std::vector<HosAlertRow*> &alerts,  SQLiteStatement *p)
			{
				bool rv = true;
				try
				{
							while (p->FetchRow())
							{
								HosAlertRow *al = new HosAlertRow();
								al->dt = DateTime(p->GetColumnInt64(2));
								al->drivingAvailable = p->GetColumnInt(3);
								al->ondutyAvailable = p->GetColumnInt(4);
								al->alertType = (AlertTypes)(p->GetColumnInt(5));
								al->ondutyruleviolated = p->GetColumnString(6);
								al->drivingruleviolated = p->GetColumnString(7);
								al->haveSent = p->GetColumnBool(8);
								alerts.push_back(al);
							}
					
				}
				catch (std::exception &ex)
				{
					rv = false;
				}
				
				return rv;
			}

			std::vector<HosAlertRow*> FleetTimeLogDB::GetAllAlertsByDriver(const std::string &driverId)
			{
				std::vector<HosAlertRow*> alerts;
				if (driverId.empty())
					return alerts;
				
				CREATECONNECTION
				try {
					auto sql = StringPrintf("Select * From HOSAlerts WHERE DriverId = '%s' and HaveSent = 0 order by AlertDate ", driverId.c_str());
					p->Execute();
					getAllAlerts(alerts, p);
					p->FreeQuery();
				}
				CLOSECONN
				return alerts;
			}

			bool FleetTimeLogDB::isAlertExist(DateTime dt_shift, int al_type, const std::string &dr_rule, const std::string &onduty_rule, const std::string &driverId)
			{
				/*std::vector<HosAlertRow*> list;
				if (driverId.empty())
					return false;
				SQLiteStatement *p = CreateConnection();
				string sql = "Select * From HOSAlerts WHERE (DriverId=? and AlertType=? and DrivingRuleViolated=? and OnDutyRuleViolated=?) and (AlertDate>=? and AlertDate<=?) order by AlertDate ";
				p->Sql(sql);
				
				p->BindString(1, driverId);
				p->BindInt(2, al_type);
				p->BindString(3, dr_rule);
				p->BindString(4, onduty_rule);
				p->BindString(5, dt_shift.ToString());
				p->BindString(6, DateTime::UtcNow().ToString());
				p->Execute();
				bool rv = getAllAlerts(list,p);
				p->FreeQuery();
				if (rv&&list.size() > 0 && list.size() > 0)
				{
					return true;
				}
				else
				{
					return false;
				}*/
				return false;
			}

			void FleetTimeLogDB::AddAlert(HosAlertRow *rr, const std::string &driverId)
			{
				if (driverId != "")
				{
					CREATECONNECTION
						try {
						string sql = std::string(" INSERT INTO HOSAlerts (DriverId, AlertDate, DrivingAvail, OnDutyAvail, AlertType, OnDutyRuleViolated,DrivingRuleViolated,HaveSent) ")
							+ std::string(" VALUES (?, ?,?,?,?,?,?,?);");
						p->Sql(sql);
						p->BindString(1, driverId);
						p->BindInt64(2, rr->dt.getMillis());

						p->BindInt(3, rr->drivingAvailable);
						p->BindInt(4, rr->ondutyAvailable);
						p->BindInt(5, rr->drivingAvailable);
						p->BindString(6, rr->ondutyruleviolated);
						p->BindString(7, rr->drivingruleviolated);
						p->BindBool(8, false);
						p->ExecuteAndFree();
					}
					CLOSECONN
				}
				
			}
#pragma endregion
