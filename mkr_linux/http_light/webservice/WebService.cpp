
#include "WebService.h"
#include "Reports.h"
#include "db/FleetEmployeeDB.h"
#include "db/FleetAssetsDB.h"
#include "db/CentralOcppDB.h"
#include "db/HistoryDB.h"

//#include "db/AdminCSDB.h"


#include <pthread.h>

#include "udorx_server_config.h"

#if !USE_LIBEVENT
#include "server/httplib.h"
using namespace httplib;
#else
#include "server/libevhtp_server.h"
#endif
using namespace evhtp::evhtphelp;

int main_send_register_email(char *mailTo, char *subject,const Client &c);

extern const char* content_type ;
extern const char* content_disp ;
extern const char* content_len ;

extern vector<WebSession*> web_sessions;

WebSession* GetWebSessionByID(WebSession *pws)
{
	WebSession *ws = 0;
	int size = web_sessions.size();
	if (size == 0)
		return 0;
	for (int i = 0; i < size; i++)
	{
		WebSession *v = web_sessions[i];
		if (v==pws)
		{
			ws = v;
			break;
		}
	}
	return ws;
}

 WebSession* GetWebSession(string user, int cid,int &index)
{
	WebSession *ws = 0;
	int size = web_sessions.size();
	if (size == 0)
		return 0;
	for (int i = 0; i < size; i++)
	{
		WebSession *v = web_sessions[i];
		if (v->user == user&&v->cid == cid)
		{
			ws = v;
			index = i;
			break;
		}
	}
	return ws;
}

 inline static string getValue(string const &str)
 {
	 vector<string> v = split(str, '=');
	 if (v.size() >= 2)
		 return v[1];
	 return "";
 }
 static string getValueByKey(char* key, vector<string> &v)
 {
	 string rv = "";
	 for (int i = 1; i < v.size(); i++)
	 {
		 string s = v[i];
		 if (!s.empty())
		 {
			 if (strstr(s.c_str(), key) != NULL)
			 {
				 rv = getValue(s);
				 break;
			 }
		 }
	 }
	 return rv;
 }


//==========================================================================

static void generatePdfReport(evhtp_request_t * req, evhtp_kvs_t * params, int reqN)
{
	//PdfReportParams *par = new PdfReportParams();
	//PdfReport_T rr(par);

	//const char *pC = evhtp_kv_find(params, "rid");
	//if (pC)
	//	par->rid = Str2Int(string(pC));

	//pC = evhtp_kv_find(params, "vid");
	//if (pC)
	//	par->vid = string(pC);

	//pC = evhtp_kv_find(params, "cid");
	//if (pC)
	//	par->cid = Str2Int(string(pC));

	//pC = evhtp_kv_find(params, "id");
	//if (pC)
	//	par->uid = string(pC);

	//rr.par->emp = new PEmpl();
	//FleetEmployeeDB::Get()->GetEmployeeById(rr.par->emp, par->rid);

	////rr.uid = uid;
	//if (reqN == 11)
	//{
	//	m_printf("Generating hos report uid=%s\r\n", par->uid.c_str());
	//	GenerateHosReport(&rr);
	//}
	//else if (reqN == 12)
	//{
	//	GenerateInspectionReport(&rr);

	//}
	///*int length = 0;
	//rr.buf = read_bin_file("/home/mkr/local/reports/hpdf_table.pdf", length);
	//rr.buf_len = length;*/
	//if (rr.buf_len > 0)
	//{
	//	if (rr.buf)
	//	{
	//		m_printf("Generating hos report Sending response\r\n");

	//		//string contentDispV = "inline;"+par->uid+".pdf"; //+ s + ".pdf";
	//		string contentDispV = "attachment; filename=\"" + par->uid + ".pdf\""; //+ s + ".pdf";
	//		set_response_attr(req, content_type, "application/pdf", 0);
	//		set_response_attr(req, content_disp, contentDispV.c_str(), 0);
	//		string strL = Int2Str(rr.buf_len);
	//		set_response_attr(req, content_len, strL.c_str(), 0);
	//		set_response_buf(req, rr.buf, rr.buf_len);
	//	}
	//}

	//if (rr.par->emp)
	//	delete rr.par->emp;
	//if (rr.par != 0)
	//	delete rr.par;

}

//=========================================================================
//static void generatePdf_G_Label4ChargingStation(evhtp_request_t * req, evhtp_kvs_t * params, int reqN)
//{
//	PdfCSReportParams *par = new PdfCSReportParams();
//	PdfCSReport_T rr(par);
//
//	const char *pC = evhtp_kv_find(params, "rid");
//	if (pC)
//		par->rid = Str2Int(string(pC));  // charging station ID (woID)
//
//	pC = evhtp_kv_find(params, "vid");
//	if (pC)
//		par->vid = string(pC);  // 0
//
//	pC = evhtp_kv_find(params, "cid");
//	if (pC)
//		par->cid = Str2Int(string(pC));
//
//	pC = evhtp_kv_find(params, "id");
//	if (pC)
//		par->uid = string(pC);  // serial number
//
//	par->pWoi = new PCSItem();
//	AdminCSDB::Get()->GetAdminChargeStationByID(par->rid, par->pWoi);
//
//	int main_uc_G_label_pdf_report(PdfCSReport_T *rr);
//	main_uc_G_label_pdf_report(&rr);
//
//
//	/*int length = 0;
//	string fpath = REPORT_GEN_FILES + string("GRM-4021014.pdf");
//	rr.buf = read_bin_file(fpath.c_str(), length);
//	rr.buf_len = length;*/
//
//	if (rr.buf_len > 0)
//	{
//		if (rr.buf)
//		{
//			m_printf("Generating Charging station %s.pdf\r\n", par->uid.c_str());
//
//			
//			string contentDispV = "attachment; filename=\"G-" + par->uid + ".pdf\""; //+ s + ".pdf";
//			set_response_attr(req, content_type, "application/pdf", 0);
//			set_response_attr(req, content_disp, contentDispV.c_str(), 0);
//			string strL = Int2Str(rr.buf_len);
//			set_response_attr(req, content_len, strL.c_str(), 0);
//			set_response_buf(req, rr.buf, rr.buf_len);
//		}
//	}
//
//	
//	if (par->pWoi != 0)
//		delete par->pWoi;
//	if (rr.par != 0)
//		delete rr.par;
//
//}
//int main_uc_UL_label_pdf_report(PdfCSReport_T *rr);
//int main_uc_UL_Classic_label_pdf_report(PdfCSReport_T *rr);
//static void generatePdf_UL_Label4ChargingStation(evhtp_request_t * req, evhtp_kvs_t * params, int reqN)
//{
//	PdfCSReportParams *par = new PdfCSReportParams();
//	PdfCSReport_T rr(par);
//	const char *pC = evhtp_kv_find(params, "rid");
//	if (pC)
//		par->rid = Str2Int(string(pC));  // charging station ID (woID)
//
//	pC = evhtp_kv_find(params, "vid");
//	if (pC)
//		par->vid = string(pC);  // 0
//
//	pC = evhtp_kv_find(params, "cid");
//	if (pC)
//		par->cid = Str2Int(string(pC));
//
//	pC = evhtp_kv_find(params, "id");
//	if (pC)
//		par->uid = string(pC);  // serial number
//
//	par->pWoi = new PCSItem();
//	AdminCSDB::Get()->GetAdminChargeStationByID(par->rid, par->pWoi);
//
//	
//	/*int type=par->pWoi->type();
//	type--;
//	if (type > 0)
//	{
//	   main_uc_UL_label_pdf_report(&rr);
//	}
//	else if (type == 0) */
//	{
//		main_uc_UL_Classic_label_pdf_report(&rr);
//	}
//
//	/*int length = 0;
//	string fpath = REPORT_GEN_FILES + string("GRM-4021014.pdf");
//	rr.buf = read_bin_file(fpath.c_str(), length);
//	rr.buf_len = length;*/
//
//	if (rr.buf_len > 0)
//	{
//		if (rr.buf)
//		{
//			m_printf("Generating Charging station %s.pdf\r\n", par->uid.c_str());
//
//
//			string contentDispV = "attachment; filename=\"UL-" + par->uid + ".pdf\""; //+ s + ".pdf";
//			set_response_attr(req, content_type, "application/pdf", 0);
//			set_response_attr(req, content_disp, contentDispV.c_str(), 0);
//			string strL = Int2Str(rr.buf_len);
//			set_response_attr(req, content_len, strL.c_str(), 0);
//			set_response_buf(req, rr.buf, rr.buf_len);
//		}
//	}
//
//
//	if (par->pWoi != 0)
//		delete par->pWoi;
//	if (rr.par != 0)
//		delete rr.par;
//
//}


//-------------------------------------------------------------------------

//------------------------------------------------------------------------------
//static void SendHOSCycleStrToFromBrowser(evhtp_request_t * req, vector<string> &v)
//{
//	string cID = getValueByKey("cycle", v);
//	int cycle = Str2Int(cID);
//
//	std::string response = Cycle2Str(cycle);
//	set_response_buf(req, response.data(), response.size());
//}

//-------------------------------------------------------------------------
void process_web_request(evhtp_request_t * req)
{

	int req_method = evhtp_request_get_method(req);
	//to_log_file("Web Request ..");
	if (req_method == htp_method_GET)
	{
		evhtp_kvs_t * params = req->uri->query;

		if (params)
		{
			/*for (auto v1 : par)
			{
				string s = v1.second;
				v.push_back(s);
			}*/
			const char* pC = evhtp_kv_find(params, "req");
			int reqN = Str2Int(string(pC));
			if (reqN == 11 || reqN == 12)
			{
				generatePdfReport(req, params, reqN);
			}else if (reqN == 13)
			{
				//generatePdf_G_Label4ChargingStation(req, params, reqN);
			}
			else if (reqN == 14)
			{
				//generatePdf_UL_Label4ChargingStation(req, params, reqN);
			}
		}
	}
	else if (req_method == htp_method_POST)
	{
		evbuf_t *in_buf = req->buffer_in;
		int len = evbuffer_get_length(in_buf);
		//m_printf(" method POST content len=%d\r\n",len);

		char buf[1024] = { 0 };

		//req->GetContent(buf, len, false);
		evbuffer_copyout(in_buf, buf, len);


		string strR(buf);
		//to_log_file("::" + strR);
		vector<string> v = split(strR, '&');
		int rSize = 0;
		bool flag((rSize = v.size()) > 0);

		
		if (flag)
		{
			string s = getValue(v[0]); // get reqNumber
			int reqN = Str2Int(s);
			switch (reqN)
			{

			case 7:
			{
				
			}
			break;
			
			case 35:
				//SendHOSCycleStrToFromBrowser(req,v);
				break;
			//case  40:   // from Web browser
			//	SaveInspectionCategoriesFromBrowser(req, res);
			//	break;
			//case 41:
			//	GetMaxInspectionCategoryGroup(req, res);
			//	break;
				
			default:
			{
				string resp = string("NOT");
				set_response_printf(req, resp);
			}
			break;
			}
		}
        
	}

}

#ifdef WIN32
int main_uc_G_label_pdf_report(PdfCSReport_T *rr);
int main_uc_UL_label_pdf_report(PdfCSReport_T *rr);
int main_uc_UL_Classic_label_pdf_report(PdfCSReport_T *rr);
#if 0
int main_pdf_label()
{
	
	PdfCSReportParams *p = new PdfCSReportParams();

	PdfCSReport_T *rr = new PdfCSReport_T(p);
	rr->save_to_file = true;
	rr->filename = "save_UL_Classic_label.pdf";



	p->rid = 2129;//14;  // charging station ID (woID)

	p->vid = "0";  // 0

	p->cid = -1;

	p->uid = "GRS-4011000850";// "GR1-401100000e";  // serial number

	p->pWoi = new PCSItem();
	AdminCSDB::Get()->GetAdminChargeStationByID(p->rid, p->pWoi);

	
	//main_uc_UL_Classic_label_pdf_report(rr);
	
	main_uc_UL_label_pdf_report(rr);




	return 0;
}
#endif

int test_cs_statistics()
{
	std::time_t getEpochTime(const std::string& dateTime, const std::string& dateTimeFormat);
	std::time_t tss= getEpochTime("2021-09-21T00:00:00:000Z",/*"%Y-%m-%d"*/"%Y-%m-%dT%H:%M:%SZ");  //  SELECT strftime('%s',datetime('2021-09-21','utc'))
//	tss -= (3600*4);
	if (tss != 0)
	{
		tss = tss + 1;
	}

	//std::vector<CS_Statistics_T> stats;
	std::map<int64_t, std::vector<CS_Statistics_T>> stat_map;
	int cid = 3166;
	const char *sern = "GRS-4011002447"; //"GRS-40110009ef";
	int rv= HistoryDB::Get()->GetAllChargeStationsStatistics(cid, sern, stat_map);

	udorx_pbf::TrDataList trdatalist;
	
	for (auto& x : stat_map)
	{
		udorx_pbf::TrData *trdata = trdatalist.add_tdlist();;
		int64_t id = x.first;
		int connId = (id >> 32)&0xffffffff;
		int transId = (id & 0xffffffff);
		trdata->set_sern(sern);
		trdata->set_transid(transId);
		trdata->set_connid(connId);
		for (auto &xx : x.second)
		{
			udorx_pbf::MeterValues *mvv = trdata->add_mvalues();
			mvv->ParseFromString(xx.meterValues);
			
			//v.push_back(pbf_tr_data);
		}

	}


	//=========print trdatalist==================
	
		int size = trdatalist.tdlist_size();
		for (int i = 0; i < size; i++)
		{
			TrData *trdata = trdatalist.mutable_tdlist(i);
			printf("%s %d %d \r\n", trdata->sern().c_str(), trdata->transid(), trdata->connid());
			int mvsize = trdata->mvalues_size();
			for (int j = 0; j < mvsize; j++)
			{
				MeterValues *mvv = trdata->mutable_mvalues(j);
				int mvv_size = mvv->transactiondata_size();
				for (int k = 0; k < mvv_size; k++)
				{
					MeterValue *mv = mvv->mutable_transactiondata(k);
					printf("MeterValue at ts=%lld\r\n", mv->timestamp());
					int svsize = mv->sampledvalue_size();
					for (int n = 0; n < svsize; n++)
					{
						SampledValue *sv = mv->mutable_sampledvalue(n);
						printf("SV val:%s U:%s\r\n", sv->value().c_str(),sv->unit().c_str());
					}
				}
			}
		}
	


	return rv;
}
#endif
