#include "CommonUtils/my_utils.h"//#include "CommonUtils/my_utils.h"
#include "HistoryDB.h"
#include "MkrLib.h"


void m_printf(const char *format, ...);
#define OCPPDB_PRINTF m_printf
using namespace lgpl3::ocpp16;
using namespace udorx_pbf;

HistoryDB *HistoryDB::_instance = 0;  // initialize static var member

void HistoryDB::createTables()
{
	CREATECONNECTION
	try {
		
		if (!IsTableExist(p, "CSHistTbl"))
		{
			CreateChargeStationHistTable(p);
			//OCPPDB_PRINTF("CSHistTbl created..\r\n");
		}
		
		if (!IsTableExist(p, "StartTrTbl"))
		{
			CreateStartTransactionTable(p);
		}
		if (!IsTableExist(p, "StopTrTbl"))
		{
			CreateStopTransactionTable(p);
		}
		if (!IsTableExist(p, "MeterVTbl"))
		{
			CreateMeterValuesTable(p);
		}
	}
	CLOSECONN
}


//========================================================================

void HistoryDB::CreateStopTransactionTable(SQLiteStatement *p)
{
	//transactionDataID rowid of the MeterValue ( in the MeterVTbl)
	string sql = "CREATE TABLE StopTrTbl ("
		"HRowId integer,transactionId integer,idTag ntext,meterStop integer,TS bigint,reason ntext,transactionData blob)";
	CreateTableA(p, sql);
}
void HistoryDB::setStopTransactionParams(SQLiteStatement *p, StopTrData_T &data)
{ //( HRowId,transactionId,idTag,meterStop,TS,reason,transactionData) VALUES (?,?,?,?,?,?,?)
	p->BindInt(1, data.HRowId);
	p->BindInt(2, data.transactionId);
	p->BindString(3, data.idTag);
	p->BindInt(4, data.meterStop);
	p->BindInt64(5, data.TS);
	p->BindString(6, data.reason);
	
	p->BindBlob(7, data.transactionData.data(), data.transactionData.length());

}
void HistoryDB::setMeterValuesParams(SQLiteStatement *p, MeterValuesData_T &data,int g_ts)
{ //INSERT INTO MeterVTbl (CSSerN,HRowId,transactionId,connectorId,MeterValue,Flag) VALUES (?,?,?,?)";
	p->BindString(1, data.CSSerN);
	p->BindInt(2, data.HRowId);
	p->BindInt(3, data.transactionId);
	p->BindInt(4, data.connectorId);
	p->BindBlob(5, data.meterValueData.data(), data.meterValueData.length());
	p->BindInt(6, g_ts); // Flag to keep last inserted timestamp
}
bool HistoryDB::readStopTrData(SQLiteStatement *p, StopTrData_T &data)
{
	//HRowId, transactionId, idTag, meterStop, TS, reason, transactionData
	data.HRowId=(p->GetColumnInt(0));
	data.transactionId=(p->GetColumnInt(1));  
	data.idTag=(p->GetColumnString(2));
	data.meterStop=(p->GetColumnInt(3));   
	
	data.TS=(p->GetColumnInt64(4));
	data.reason=(p->GetColumnString(5));

	int len = p->GetColumnBytes(6);
	const void *buf = p->GetColumnBlob(6);
	data.transactionData = "";
	if (len > 0 && buf != 0)
		data.transactionData = string((const char*)buf, len);
	
	return true;
}
int HistoryDB::GetStopTransactionRequestDataFromDB(int transactionId, StopTransactionRequest &tr_data)
{
	int rv = 0;
	StopTrData_T data;
	string sql = StringPrintf("select * from StopTrTbl where transactionId=%d", transactionId);
	CREATECONNECTION
		try {
		p->Sql(sql);
		while (p->FetchRow())
		{
			readStopTrData(p, data);
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
		tr_data.meterStop = data.meterStop;
		tr_data.idTag = data.idTag;
		//cereal::schema_data_time dt;
		//dt.set_millis(data.TS);
		//tr_data.timestamp = dt;// data.TS;
		tr_data.timestamp.set_millis(data.TS);
		tr_data.transactionId = data.transactionId;
		tr_data.reason = StopTransactionRequest::ReasonEnum::from_string(data.reason);
		if (!data.transactionData.empty())
		{
			udorx_pbf::MeterValues/*TransactionData*/ pbf_tr_data;
			pbf_tr_data.ParseFromString(data.transactionData);
			int tr_data_size = pbf_tr_data.transactiondata_size();
			
			schema_array<TransactionDataEntry, 65536, 0> tr_data__transactionData;
			for (int i = 0; i < tr_data_size; i++)
			{
				TransactionDataEntry tde;
				auto& v = pbf_tr_data.transactiondata(i);
				tde.timestamp.set_millis(v.timestamp());
				for (int j = 0; j < v.sampledvalue_size(); j++)
				{
					SampledValueEntry sv;
					auto &v1 = v.sampledvalue(j);
					sv.value = v1.value();
					sv.context = SampledValueEntry::ContextEnum::from_string(v1.context());
					sv.format= SampledValueEntry::FormatEnum::from_string(v1.format());
					sv.location = SampledValueEntry::LocationEnum::from_string(v1.location());
					sv.measurand = SampledValueEntry::MeasurandEnum::from_string(v1.measurand());
					sv.phase = SampledValueEntry::PhaseEnum::from_string(v1.phase());
					sv.unit = SampledValueEntry::UnitEnum::from_string(v1.unit());

					tde.sampledValue.push_back(sv);
				}
				tr_data__transactionData.push_back(tde);
			}
			tr_data.transactionData = tr_data__transactionData;

		}
	return rv;
}
int HistoryDB::insertStopTransactionReqMsg2DB(SQLiteStatement *p, int hist_rowid, void *ptr)
{
	int rv = 0;
	StopTransactionRequest* pSts = (StopTransactionRequest *)ptr;
	StopTrData_T g_tr_data;
	g_tr_data.HRowId = hist_rowid;
	g_tr_data.meterStop = pSts->meterStop;
	g_tr_data.transactionId = pSts->transactionId;
	g_tr_data.TS = pSts->timestamp.value().counter() / 1000000;   // to convert to millis /1000000
	
	if (pSts->idTag.has_value())
		g_tr_data.idTag = pSts->idTag.value().to_string();

	
	if (pSts->reason.has_value())
		g_tr_data.reason = pSts->reason.value().to_string();

	if (pSts->transactionData.has_value())
	{
		//std::string tr_data_str = pSts->transactionData.value();
		udorx_pbf::MeterValues tr_data;
		auto &vv = pSts->transactionData.value();
		
		int size = vv.size();
		for (int i=0;i<size;i++)
		{
			auto &v = vv[i];
			int64_t ts = v.timestamp.value().counter() / 1000000;
			udorx_pbf::MeterValue *mv = tr_data.add_transactiondata();
			mv->set_timestamp(ts);

			int svs = v.sampledValue.size();
			if (svs > 0)
			{
				for (int j=0;j<svs;j++)
				{
					auto &v1 = v.sampledValue[j];
					udorx_pbf::SampledValue *sv = mv->add_sampledvalue();
					sv->set_value(v1.value);  /*Required. Value as a “Raw” (decimal) number or*/
					if (v1.context.has_value())
						sv->set_context(v1.context.value().to_string());
					if (v1.format.has_value())
						sv->set_format(v1.format.value().to_string());
					if (v1.measurand.has_value())
						sv->set_measurand(v1.measurand.value().to_string());
					if (v1.phase.has_value())
						sv->set_phase(v1.phase.value().to_string());
					if (v1.location.has_value())
						sv->set_location(v1.location.value().to_string());
					if (v1.unit.has_value())
						sv->set_unit(v1.unit.value().to_string());
				}
			}
		}
		
		tr_data.SerializeToString(&g_tr_data.transactionData);
	}

	/*std::string sql = StringPrintf("INSERT INTO StopTrTbl ( HRowId,transactionId,idTag,meterStop,TS,reason,transactionDataID) VALUES (%d,%d,'%s',%d,%lld,'%s',%d)",
		hist_rowid, pSts->transactionId, idTag.c_str(), pSts->meterStop, ts,reason.c_str(), transactionDataID);
	p->SqlStatement(sql);*/

	std::string sql = "INSERT INTO StopTrTbl (HRowId,transactionId,idTag,meterStop,TS,reason,transactionData) VALUES (?,?,?,?,?,?,?)";
	p->Sql(sql);
	setStopTransactionParams(p, g_tr_data);
	p->ExecuteAndFree();


	rv = GetLastInsertedRow();  //immediately after your insert.  !!!;

	return rv;  

}
//************************************MeterValues*************************
void HistoryDB::CreateMeterValuesTable(SQLiteStatement *p)
{
	//transactionDataID rowid of the MeterValue ( in the MeterVTbl)
	string sql = "CREATE TABLE MeterVTbl (CSSerN ntext,HRowId integer,transactionId integer,connectorId integer,MeterValue blob,Flag integer);";  //CREATE INDEX index_meters on MeterVTbl(HRowId, CSSerN); 
	CreateTableA(p, sql);

	/*sql = "CREATE INDEX index_meters on MeterVTbl (HRowId, CSSerN);";
	p->SqlStatement(sql);*/
}
bool HistoryDB::readMeterVData(SQLiteStatement *p, MeterValuesData_T &data)
{
	//HRowId integer,transactionId integer,connectorId integer,MeterValue blob
	data.CSSerN = (p->GetColumnString(0));
	data.HRowId = (p->GetColumnInt(1));
	data.transactionId = (p->GetColumnInt(2));
	data.connectorId = (p->GetColumnInt(3));
	
	int len = p->GetColumnBytes(4);
	const void *buf = p->GetColumnBlob(4);
	data.meterValueData = "";
	if (len > 0 && buf != 0)
		data.meterValueData = string((const char*)buf, len);

	
	return true;
}
#if 0
int HistoryDB::GetMeterValuesRequestDataFromDB(int transactionId, std::vector<MeterValuesRequest> &tr_data_arr)
{
	int rv = 0;
	std::vector<MeterValuesData_T> data_arr;
	string sql = StringPrintf("select * from MeterVTbl where transactionId=%d", transactionId);
	CREATECONNECTION
		try {
		p->Sql(sql);
		while (p->FetchRow())
		{
			MeterValuesData_T data;
			readMeterVData(p, data);
			rv++;
			data_arr.push_back(data);
			//break;   // there must be several Metervalues for the same transactionId
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
		for (int i = 0; i < data_arr.size(); i++)
		{
			MeterValuesRequest tr_data;
			tr_data.transactionId = data_arr[i].transactionId;
			tr_data.connectorId = data_arr[i].connectorId;

			if (!data_arr[i].meterValueData.empty())
			{
				udorx_pbf::MeterValues pbf_tr_data;
				pbf_tr_data.ParseFromString(data_arr[i].meterValueData);
				int tr_data_size = pbf_tr_data.transactiondata_size();

				//schema_array<TransactionDataEntry, 65536, 0> tr_data__transactionData;
				for (int i = 0; i < tr_data_size; i++)
				{
					MeterValueEntry tde;
					auto& v = pbf_tr_data.transactiondata(i);
					tde.timestamp.set_millis(v.timestamp());
					for (int j = 0; j < v.sampledvalue_size(); j++)
					{
						SampledValueEntry sv;
						auto &v1 = v.sampledvalue(j);
						sv.value = v1.value();
						sv.context = SampledValueEntry::ContextEnum::from_string(v1.context());
						sv.format = SampledValueEntry::FormatEnum::from_string(v1.format());
						sv.location = SampledValueEntry::LocationEnum::from_string(v1.location());
						sv.measurand = SampledValueEntry::MeasurandEnum::from_string(v1.measurand());
						sv.phase = SampledValueEntry::PhaseEnum::from_string(v1.phase());
						sv.unit = SampledValueEntry::UnitEnum::from_string(v1.unit());

						tde.sampledValue.push_back(sv);
					}
					tr_data.meterValue.push_back(tde);
				}
			}
			tr_data_arr.push_back(tr_data);
		}
	return rv;
}
#endif

int HistoryDB::insertMeterValuesReqMsg2DB(SQLiteStatement *p, int hist_rowid,const char *cs_sern, void *ptr)
{
	int rv = 0;
	MeterValuesRequest* pSts = (MeterValuesRequest *)ptr;
	
	MeterValuesData_T g_mv_data;
	g_mv_data.HRowId = hist_rowid;
	g_mv_data.connectorId= pSts->connectorId;   // was missing
	if (pSts->transactionId.has_value())
		g_mv_data.transactionId = pSts->transactionId.value();
	int g_ts = 0;
	if (/*pSts->meterValue.has_value()*/true)  // meterValue is a required field here !!!
	{
		//std::string tr_data_str = pSts->transactionData.value();
		udorx_pbf::MeterValues tr_data;
		auto &vv = pSts->meterValue;

		int size = vv.size();
		
		for (int i = 0; i < size; i++)
		{
			auto &v = vv[i];
			int64_t ts = v.timestamp.value().counter() / 1000000;
			udorx_pbf::MeterValue *mv = tr_data.add_transactiondata();
			mv->set_timestamp(ts);
			if (g_ts == 0)
				g_ts = ts / 1000;   // in seconds
			int svs = v.sampledValue.size();
			if (svs > 0)
			{
				for (int j = 0; j < 1;/* svs; */ j++)
				{
					auto &v1 = v.sampledValue[j];
					udorx_pbf::SampledValue *sv = mv->add_sampledvalue();
					sv->set_value(v1.value);  /*Required. Value as a “Raw” (decimal) number or*/
					if (v1.context.has_value())
						sv->set_context(v1.context.value().to_string());
					if (v1.format.has_value())
						sv->set_format(v1.format.value().to_string());
					if (v1.measurand.has_value())
						sv->set_measurand(v1.measurand.value().to_string());
					if (v1.phase.has_value())
						sv->set_phase(v1.phase.value().to_string());
					if (v1.location.has_value())
						sv->set_location(v1.location.value().to_string());
					if (v1.unit.has_value())
						sv->set_unit(v1.unit.value().to_string());
				}
			}
		}

		tr_data.SerializeToString(&g_mv_data.meterValueData);
	}
	
	if (g_ts > 0)
	{
		int ts_flag = -1;
		string sql;
		//1.
		if (cs_sern!=NULL)
		{
			sql = StringPrintf("SELECT Flag from MeterVTbl where CSSerN='%s' order by ROWID DESC limit 1", cs_sern);
			p->Sql(sql);
			while (p->FetchRow())
			{
				ts_flag = p->GetColumnInt(0);
			}
			// do not forget to clean-up
			p->FreeQuery();
		}
		//OCPPDB_PRINTF("MeterV sn:%s flag:%d g_ts:%d \r\n", cs_sern, ts_flag,g_ts);
		if (ts_flag == -1 || ((g_ts - ts_flag) >= 120))
		{

			sql = "INSERT INTO MeterVTbl (CSSerN,HRowId,transactionId,connectorId,MeterValue,Flag) VALUES (?,?,?,?,?,?)";
			p->Sql(sql);
			g_mv_data.CSSerN = std::string(cs_sern);
			setMeterValuesParams(p, g_mv_data, g_ts);
			p->ExecuteAndFree();
			rv = GetLastInsertedRow();  //immediately after your insert.  !!!;
			//OCPPDB_PRINTF("MeterV Insert rv:%d \r\n", cs_sern, rv);
		}
	}

	return rv;
}

//------------------------------------------------------------------------
void HistoryDB::CreateStartTransactionTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE StartTrTbl ("
		"HRowId integer,connectorId integer,idTag ntext,meterStart integer,reservationId integer,TS bigint)";
	CreateTableA(p, sql);
}

int HistoryDB::insertStartTransactionReqMsg2DB(SQLiteStatement *p, int hist_rowid, void *ptr)
{
	int rv = 0;
	StartTransactionRequest* pSts = (StartTransactionRequest *)ptr;
	uint64_t ts = 0;
	ts = pSts->timestamp.value().counter() / 1000000;   // to convert to millis /1000000
	int reservationId = 0;
	if (pSts->reservationId.has_value())
		reservationId = pSts->reservationId.value();

	
	std::string sql = StringPrintf("INSERT INTO StartTrTbl ( HRowId,connectorId,idTag,meterStart,reservationId,TS) VALUES (%d,%d,'%s',%d,%d,%lld)",
		hist_rowid, pSts->connectorId, pSts->idTag.to_string().c_str(), pSts->meterStart, reservationId,ts);
	p->SqlStatement(sql);
	rv= GetLastInsertedRow();  //immediately after your insert.  !!!;

	return rv;  // this is a transactionId value which should be used in StartTransactionResponse !!!
}

int HistoryDB::OnStartTransactionRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, void *ptr, std::string& sern)
{
	int rv = 0;
	CREATECONNECTION
	try {
		int hist_rowid = insertOcppHistory2DB(p, req, rtt, sern);
		if (hist_rowid > 0)
		{
			rv = insertStartTransactionReqMsg2DB(p, hist_rowid, ptr);

			//--------------------------------------------------------------------------
			StartTransactionRequest* pSts = (StartTransactionRequest *)ptr;

			int meterStart = pSts->meterStart;
			int transactionId = rv;

			rtt.errCode = std::to_string(meterStart);   // added !!! in v_05.37
			rtt.status = std::to_string(transactionId);
			updateHistOnStartTransaction(p, req, rtt, hist_rowid);
		}
		//-------------------------------------------------------------------------------	
	}
	CLOSECONN
	return rv;
}
//------------------------------------------------------------------------
int HistoryDB::OnStopTransactionRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, void *ptr, std::string& sern)
{
	int rv = 0;
	CREATECONNECTION
		try {
		//--------------------------------------------------------------------------
		StopTransactionRequest* pSts = (StopTransactionRequest *)ptr;

		int meterStop = pSts->meterStop;
		int transactionId = pSts->transactionId;

		rtt.errCode = std::to_string(meterStop);   // added !!! in v_05.37
		rtt.status = std::to_string(transactionId);
		//-------------------------------------------------------------------------------
		int hist_rowid = insertOcppHistory2DB(p, req, rtt, sern);
		if (hist_rowid > 0)
		{
			rv = insertStopTransactionReqMsg2DB(p, hist_rowid, ptr);
		}
		//-------------------------------------------------------------------------------	
	}
	CLOSECONN
		return rv;
}
//------------------------------------------------------------------------
int HistoryDB::OnMeterValuesRequestMessage2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, void *ptr, std::string& sern)
{
	int rv = 0;
	CREATECONNECTION
	try {
		int hist_rowid = insertOcppHistory2DB(p, req, rtt, sern);
		if (hist_rowid > 0)
		{
			rv = insertMeterValuesReqMsg2DB(p, hist_rowid, rtt.ser_number.c_str(), ptr);
			//OCPPDB_PRINTF("insertMeterValuesReqMsg2DB rv:%d \r\n", rv);
		}
	}
	CLOSECONN
	return rv;
}



//------------------------------------------------------------------------

void HistoryDB::CreateChargeStationHistTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE CSHistTbl ("
		"CSSerN ntext,TS bigint,IPAddr ntext,Action ntext,CSSts ntext,ErrCode ntext,RTValues ntext)";
	CreateTableA(p, sql);
}

int HistoryDB::updateHistOnStartTransaction(SQLiteStatement *p, OCPP_Request &req, Rtt_Ocpp_T &rtt,int hist_rowid)
{
	int rv = 0;
	if (!rtt.ser_number.empty())
	{
		std::string sql = StringPrintf("UPDATE CSHistTbl SET CSSts='%s',ErrCode='%s' WHERE rowid=%d", rtt.status.c_str(), rtt.errCode.c_str(), hist_rowid);
		p->SqlStatement(sql);
		rv = GetNuberOfRowsUpdated();
		
		//OCPPDB_PRINTF("InsertOcpp to Hist sern:%s", sern.c_str());
	}
	else {
		OCPPDB_PRINTF("updateHistOnStartTransaction empty SN:");
		rv = -1;
	}
	return rv;
}

std::string HistoryDB::getLastStatusNoficationFromHistTable(std::string sern)
{
	//GRS-4011000010
	std::string res = "";
	std::string sql = StringPrintf("select RTValues from cshisttbl where cssern='%s' and action='StatusNotification' order by rowid desc limit 1", sern.c_str());
	CREATECONNECTION
	try {
		p->Sql(sql);
		while (p->FetchRow())
		{
			res = p->GetColumnString(0);
			break;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
	return res;
}
//*************************************************************************************************************************

//========================================================
int HistoryDB::GetAllChargeStationsHistory(int cid, const char *sern, std::vector<CS_History_T> &histv)
{
	int rv = 0;
	//OCPPDB_PRINTF("AllChargeStationsHistory: sern:%s \r\n", sern);
	//select * from CSHistTbl where  CSSerN='GRS-40110009ef' order by TS desc
	string sql	= StringPrintf("select TS,IPAddr,Action,CSSts,ErrCode,RTValues from CSHistTbl where  CSSerN='%s' and Action<>'MeterValues' order by TS desc ", sern);  
	CREATECONNECTION
	try {
		p->Sql(sql);
		while (p->FetchRow())
		{
			CS_History_T h;
			h.ts = p->GetColumnInt64(0);
			h.IPAddr = p->GetColumnString(1);
			h.Action = p->GetColumnString(2);
			h.CSSts = p->GetColumnString(3);
			h.ErrCode = p->GetColumnString(4);
			h.RTValues = p->GetColumnString(5);
			histv.push_back(h);
			rv++;
		}
		// do not forget to clean-up
		p->FreeQuery();
	}
	CLOSECONN
	return rv;
}
int HistoryDB::ClearAllChargeStationsHistory(int cid, const char *sern)
{
	int rv = 0;
	CREATECONNECTION
		try {
		string sql = StringPrintf("DELETE FROM CSHistTbl where CSSerN='%s' ", sern);
		p->SqlStatement(sql);
		rv = 1;
	}
	CLOSECONN
		return rv;
}

//========================================================================================
int HistoryDB::GetAllChargeStationsStatistics(int cid, const char *sern, std::map<int64_t, std::vector<CS_Statistics_T>> &stat_map)
{
	int rv = 0;
	std::vector<int> v0, v1;
	string sql = "";
	CREATECONNECTION
	try {
#if 0
		//1 read number of StartTransaction indexes
		sql = StringPrintf("SELECT rowid FROM cshisttbl WHERE action='StartTransaction' and CSSerN='%s'", sern);
		p->Sql(sql);
		while (p->FetchRow())
		{
			
			int rowid = p->GetColumnInt64(0);
			v0.push_back(rowid);
			
		}
		// do not forget to clean-up
		p->FreeQuery();

		//2 read number of StopTransaction indexes
		sql = StringPrintf("SELECT rowid FROM cshisttbl WHERE action='StopTransaction' and CSSerN='%s'", sern);
		p->Sql(sql);
		while (p->FetchRow())
		{

			int rowid = p->GetColumnInt64(0);
			v1.push_back(rowid);

		}
		// do not forget to clean-up
		p->FreeQuery();

		//3 read statistics  Start/Stop transactions
		
		//ideally the nimber of v0 should be the same as v1
		int size0 = v0.size(), size1 = v1.size();
		if (size0 > size1)
		{
			// if for some reason StartTransaction does not match StopTransaction 
			size0 = size1;
		}
		if (size0 > 0 && size1 > 0)
		{
			for (int i = 0; i < size0; i++)
			{
				CS_Statistics_T stat;
				stat.x0 = v0[i];
				stat.x1 = v1[i];
				if (stat.x1 <= stat.x0)   // something need to be corrected here !!!
				{
					bool found = false;
					for (int j = i; j < size1; j++)
					{
						if (v1[j] > stat.x0)
						{
							stat.x1 = v1[j];
							found = true;
							break;
						}
					}
					if (!found)
						//stat.x1 = -1;
						continue;
				}

				sql = StringPrintf("SELECT transactionId,connectorId,MeterValue from metervtbl where hrowid>=%d and hrowid<=%d and CSSerN='%s'", stat.x0, stat.x1, sern);
				p->Sql(sql);
				while (p->FetchRow())
				{

					stat.transactionId = p->GetColumnInt(0);
					stat.connectorId = p->GetColumnInt(1);
					int len = p->GetColumnBytes(2);
					const void *buf = p->GetColumnBlob(2);
					stat.meterValues = "";
					if (len > 0 && buf != 0)
						stat.meterValues = string((const char*)buf, len);
					//stats.push_back(stat);
					int64_t id = (int64_t)((int64_t)stat.connectorId << 32) + (int64_t)stat.transactionId;
					stat_map[id].push_back(stat);
					rv++;
				}
				// do not forget to clean-up
				p->FreeQuery();
			}
		}
#else
		
			//1 read number of StartTransactionId from StartTransaction events
		sql = StringPrintf("SELECT CSSts FROM cshisttbl WHERE action='StartTransaction' and CSSerN='%s'", sern);
		p->Sql(sql);
		while (p->FetchRow())
		{

			int rowid = p->GetColumnInt64(0);
			v0.push_back(rowid);

		}
		// do not forget to clean-up
		p->FreeQuery();

		//2 read number of StopTransaction indexes
		sql = StringPrintf("SELECT CSSts FROM cshisttbl WHERE action='StopTransaction' and CSSerN='%s'", sern);
		p->Sql(sql);
		while (p->FetchRow())
		{

			int rowid = p->GetColumnInt64(0);
			v1.push_back(rowid);

		}
		// do not forget to clean-up
		p->FreeQuery();

		//ideally the nimber of v0 should be the same as v1
		int size0 = v0.size(), size1 = v1.size();
		if (size1 > size0)
		{
			// if for some reason StartTransaction does not match StopTransaction 
			size1 = size0;
		}
		if (size0 > 0 && size1 > 0)
		{
			for (int i = 0; i < size1; i++)
			{
				CS_Statistics_T stat;
				stat.x0 = 0;// v0[i];
				stat.x1 = 0;// v1[i];
				//if (stat.x1 <= stat.x0)   // something need to be corrected here !!!
				//{
				//	bool found = false;
				//	for (int j = i; j < size1; j++)
				//	{
				//		if (v1[j] > stat.x0)
				//		{
				//			stat.x1 = v1[j];
				//			found = true;
				//			break;
				//		}
				//	}
				//	if (!found)
				//		//stat.x1 = -1;
				//		continue;
				//}
				int trId = v1[i];
				//sql = StringPrintf("SELECT transactionId,connectorId,MeterValue from metervtbl where hrowid>=%d and hrowid<=%d and CSSerN='%s'", stat.x0, stat.x1, sern);
				sql = StringPrintf("SELECT connectorId,MeterValue from metervtbl where transactionId=%d and CSSerN='%s'", trId, sern);
				p->Sql(sql);
				while (p->FetchRow())
				{

					stat.transactionId = trId;// p->GetColumnInt(0);
					stat.connectorId = p->GetColumnInt(0);
					int len = p->GetColumnBytes(1);
					const void *buf = p->GetColumnBlob(1);
					stat.meterValues = "";
					if (len > 0 && buf != 0)
						stat.meterValues = string((const char*)buf, len);
					//stats.push_back(stat);
					int64_t id = (int64_t)((int64_t)stat.connectorId << 32) + (int64_t)stat.transactionId;
					stat_map[id].push_back(stat);
					rv++;
				}
				// do not forget to clean-up
				p->FreeQuery();
			}
		}

#endif
	}
	CLOSECONN
		
	return rv;
}

int HistoryDB::insertOcppHistory2DB(SQLiteStatement *p, OCPP_Request &req, Rtt_Ocpp_T &rtt, std::string& sern)
{
	int rv = 0;
	
		if (!sern.empty())
		{
			std::string sql = StringPrintf("INSERT INTO CSHistTbl (CSSerN,TS,IPAddr,Action,CSSts,ErrCode,RTValues) VALUES ('%s',%lld,'%s','%s','%s','%s','%s')", sern.c_str(), rtt.ts, req.remoteIP.c_str(), req.action.c_str(), rtt.status.c_str(), rtt.errCode.c_str(), rtt.sensors.c_str());
			p->SqlStatement(sql);
			rv = GetDB()->GetLastInsertRowId();  //immediately after your insert.  !!!;
			rtt.ser_number = sern;
			//OCPPDB_PRINTF("InsertOcpp to Hist sern:%s", sern.c_str());
		}
		else {
			OCPPDB_PRINTF("InsertOcpp_2 sern empty IP:%s", req.remoteIP.c_str());
			// Probably because the Charging Station with this Serial Number has not been registered prio to use !!!!
			rv = -1;
		}
		return rv;
}

int HistoryDB::InsertOcppHistory2DB(OCPP_Request &req, Rtt_Ocpp_T &rtt, std::string& sern)
{
	int rv = 0;
	CREATECONNECTION
	try {
		rv = insertOcppHistory2DB(p, req, rtt, sern);
	}
	CLOSECONN
	return rv;
}