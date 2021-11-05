#include "Reports.h"
#include "db/FleetEmployeeDB.h"
#include "db/FleetAssetsDB.h"

#include <pthread.h>

int main_send_hos_combined_email(char *mailTo, char *report_file, const Client &c);
#if 0
////=================callback functions=======================================
static PVehicle *get_veh_info(int cid, string vid)
{
	return NULL;// FleetEmployeeDB::Get()->GetVehicleFromDB(cid, vid);
}
static PICatList*  get_cats_info(string vid)
{
	return NULL;// FleetInspectionDB::Get()->GetAllCategories(vid, false);
}
//static bool get_employee_info(PEmpl *emp, int rid)
//{
//	return FleetEmployeeDB::Get()->GetEmployeeById(emp, rid);
//}

//==========================================================================


int GenerateHosReport(PdfReport_T *rr)
{
	//PTlrList tlr_list ;
	//rr->par->rtype = ReportType::HOS;
	//vector<string> v = split(rr->par->uid, '-');
	//int64_t ts = 0;
	//if (v.size() > 1)
	//{
	//	ts = Str2Int64(v[1]);
	//	rr->par->from_ts = ts;
	//}
	//if (rr->par->rid > 0 && ts > 0)
	//{
	//	bool rv = FleetTimeLogDB::Get()->GetAllTimeLogs(tlr_list,rr->par->rid, ts);
	//	if (rv)
	//	{
	//		rr->par->setReportData(&tlr_list);

	//	}
	//}
	////m_printf("GenerateHosReport len=%d\r\n", tlr_list? tlr_list->list_size():0);
	//main_hos_pdf_report(rr);
	
	
	return 1;
}

//---------------------------------------------------------

int GenerateInspectionReport(PdfReport_T *rr)
{
	//rr->par->rtype = ReportType::INSPECTION;
	//PIRpt *ir = FleetInspectionDB::Get()->GetInspReport(rr->par->uid);
	//if (!ir)
	//	return - 1;
	//rr->par->setReportData(ir);
	///*PVehicle *veh = db->GetVehicleFromDB(par->cid, par->vid);
	//PICatList* cats = FleetInspectionDB::Get()->GetAllCategories(par->vid, false);
	//if (cats != 0 && veh != 0)
	//{
	//((InspReport_T*)rr)->setCategoryData(cats);
	//((InspReport_T*)rr)->setVehicleData(veh);
	//}*/

	//rr->par->get_cats_info_cb = get_cats_info;
	//rr->par->get_veh_info_cb = get_veh_info;
	//main_insp_pdf_report(rr);
	//
	//if (ir)
	//	delete ir;
	return 1;
}

//=====================================================================================
//===================Combined Report===================================================

static void create_hos_map_list(CombinedReport_T &rr)
{
	// /*unordered_map<int64_t, PTlrList*>*/hos_map_t map_list;
	//MyDailyLogsList* dlogs = FleetTimeLogDB::Get()->GetDailyLogs(rr.par->rid, /*int64_t start*/rr.par->from_ts, /*int64_t end*/rr.par->to_ts);
	//int size = dlogs->hlogs_size();
	//if (size > 0)
	//{
	//	for (int i = 0; i < size; i++)
	//	{
	//		MyDailyLogs *dl = dlogs->mutable_hlogs(i);
	//		if (dl)
	//		{
	//			int len = dl->count();
	//			if (len == 1 && dl->type() == (int)TimeLogType::Auto)
	//				continue;
	//			else
	//			{
	//				int64_t ts = dl->dt();
	//				PTlrList tlr_list;
	//				bool rv= FleetTimeLogDB::Get()->GetAllTimeLogs(tlr_list,rr.par->rid, ts);
	//				if (rv)
	//				{
	//					PTlrList *pTlrL = new PTlrList();
	//					pTlrL->CopyFrom(tlr_list);
	//					rr.hos_map_list.insert({ ts, pTlrL });//map_list[ts] = tlr_list;
	//				}
	//			}
	//		}
	//	}
	//}

	//if (dlogs)
	//	delete dlogs;

}


int GenerateNewEmailsReport(int cid, int rid, const vector<string> &v)
{
//	int rv = 0;
//	PdfReportParams *par = new PdfReportParams();
//	CombinedReport_T rr(par);
//		
//	par->cid = cid;
//	par->rid = rid;
//	par->vid = "1001-1571935691266";
//	par->uid = "1001-1572836488771";  // inspection report uid
//	rr.rpt_filename = "rpt_"+par->uid+".pdf";
//	par->from_ts = Str2Int64(v[1]);
//	par->to_ts = Str2Int64(v[2]);
//	par->options = v[3];
//	
//	int opt = 0;
//	if (!par->options.empty())
//	{
//		vector<string> v1 = split(par->options, ',');
//		if (v1.size() >= 3)
//		{
//			if (v1[0] == "1")
//				opt |= 1;
//			if (v1[1] == "1")
//				opt |= 2;
//			if (v1[2] == "1")
//				opt |= 4;
//		}
//	}
//	if (opt & 1)
//	{
//		create_hos_map_list(rr);
//	}
//	if (opt & 2)
//	{
//		rr.insp_list = FleetInspectionDB::Get()->GetAllInspReportsByDriverID(rid, par->from_ts, par->to_ts);
//	}
//
//	par->emp = new PEmpl();      // do not forget to delete !!!!!!
//	if(!FleetEmployeeDB::Get()->GetEmployeeById(par->emp, rid))
//		goto __cleanup;
//	
//	
//	
//	par->get_veh_info_cb = get_veh_info;
//	par->get_cats_info_cb = get_cats_info;
//	
//	main_combined_pdf_report(&rr);
//	if (true)
//	{
//		
//		// attach file to email and send it to e_addr
//		string email_addr = v[0];
//		Client c;
//		c.ResourceID = rid;
//		c.cid = cid;
//		m_printf("ProcessEmailsFromMobile ...GenerateNewEmailsReport--GOTO AttFile:%s Email Sending to: %s\r\n", rr.rpt_filename.c_str(), email_addr.c_str());
//		int rv1=main_send_hos_combined_email((char*)email_addr.c_str(),(char*) rr.rpt_filename.c_str(), c );
//		if (rv1 == 0)
//			rv = 1;
//		
//	}
//__cleanup:
//	//clean up
//	stdH::ClearMapWithDynamicallyAllocatedObjects(rr.hos_map_list);
//
//	if (par->emp)
//		delete par->emp;
//
//	
//	delete par;
//	return rv;

	return 0;
}


void test_email_report()
{
	string s = "skulykov@gmail.com|1486252800000|1486422120000|1,1,1";
	vector<string> v = split(s, '|');
	string resp = "NOT";
	if (v.size() >= 4)
	{
		
		int rv = GenerateNewEmailsReport(2601, 1001, v);
		if (rv)
			resp = "OK";
	}
}
void test_single_report(int reqN)
{
	PdfReportParams *par = new PdfReportParams();
	PdfReport_T rr(par);

	par->rid = 1001;;

	par->cid = 2601;

	par->vid = "1001-1571935691266";
	par->uid = "1001-1572836488771";

	rr.filename = "single_report_1234.pdf";
	rr.save_to_file = false;//true;

	rr.par->emp = new PEmpl();
	FleetEmployeeDB::Get()->GetEmployeeById(rr.par->emp, par->rid);

	//rr.uid = uid;
	if (reqN == 11)
	{
		m_printf("Generating hos report uid=%s\r\n", par->uid.c_str());
		GenerateHosReport(&rr);
	}
	else if (reqN == 12)
	{
		GenerateInspectionReport(&rr);

	}
	/*int length = 0;
	rr.buf = read_bin_file("/home/mkr/local/reports/hpdf_table.pdf", length);
	rr.buf_len = length;*/
	if (rr.buf_len > 0)
	{
		if (rr.buf)
		{
			m_printf("Generating hos report Sending response\r\n");

			//string contentDispV = "inline;" + par->uid + ".pdf"; //+ s + ".pdf";
			//set_response_attr(req, content_type, "application/pdf", 0);
			//set_response_attr(req, content_disp, contentDispV.c_str(), 0);
			//string strL = Int2Str(rr.buf_len);
			//set_response_attr(req, content_len, strL.c_str(), 0);
			//set_response_buf(req, rr.buf, rr.buf_len);
		}
	}
	
	if (rr.par->emp)
		delete rr.par->emp;
	if (rr.par != 0)
		delete rr.par;
	
}
void test_report()
{
	//test_email_report();
	test_single_report(12);
}

#endif
//=============================================================================================================


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hpdf.h"
#if 0
jmp_buf env;

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler(HPDF_STATUS   error_no,
	HPDF_STATUS   detail_no,
	void         *user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
		(HPDF_UINT)detail_no);
	longjmp(env, 1);
}

void
draw_line(HPDF_Page    page,
	float        x,
	float        y,
	const char  *label)
{
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x, y - 10);
	HPDF_Page_ShowText(page, label);
	HPDF_Page_EndText(page);

	HPDF_Page_MoveTo(page, x, y - 15);
	HPDF_Page_LineTo(page, x + 220, y - 15);
	HPDF_Page_LineTo(page, x + 240, y - 35);
	HPDF_Page_LineTo(page, x, y - 15);
	HPDF_Page_Stroke(page);

	x += 20;
	y += 20;
	HPDF_Page_MoveTo(page, x, y - 15);
	HPDF_Page_LineTo(page, x + 220, y - 15);
	HPDF_Page_LineTo(page, x + 240, y - 35);
	HPDF_Page_LineTo(page, x, y - 15);
	HPDF_Page_Stroke(page);
}

void
draw_line2(HPDF_Page    page,
	float       x,
	float       y,
	const char  *label)
{
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x, y);
	HPDF_Page_ShowText(page, label);
	HPDF_Page_EndText(page);

	HPDF_Page_MoveTo(page, x + 30, y - 25);
	HPDF_Page_LineTo(page, x + 160, y - 25);
	HPDF_Page_Stroke(page);
}

void
draw_rect(HPDF_Page     page,
	double        x,
	double        y,
	const char   *label)
{
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x, y - 10);
	HPDF_Page_ShowText(page, label);
	HPDF_Page_EndText(page);

	HPDF_Page_Rectangle(page, x, y - 40, 220, 25);
}

int pdf_main_test_gr()
{
	const char* page_title = "Line Example";

	HPDF_Doc  pdf;
	HPDF_Font font;
	HPDF_Page page;
	char fname[256];

	const HPDF_UINT16 DASH_MODE1[] = { 3 };
	const HPDF_UINT16 DASH_MODE2[] = { 3, 7 };
	const HPDF_UINT16 DASH_MODE3[] = { 8, 7, 2, 7 };

	double x;
	double y;
	double x1;
	double y1;
	double x2;
	double y2;
	double x3;
	double y3;

	float tw;

	strcpy(fname, "pdftest_graphics");
	strcat(fname, ".pdf");

	pdf = HPDF_New(error_handler, NULL);
	if (!pdf) {
		printf("error: cannot create PdfDoc object\n");
		return 1;
	}

	if (setjmp(env)) {
		HPDF_Free(pdf);
		return 1;
	}

	/* create default-font */
	font = HPDF_GetFont(pdf, "Helvetica", NULL);

	/* add a new page object. */
	page = HPDF_AddPage(pdf);

	/* print the lines of the page. */
	HPDF_Page_SetLineWidth(page, 1);
	HPDF_Page_Rectangle(page, 50, 50, HPDF_Page_GetWidth(page) - 100,
		HPDF_Page_GetHeight(page) - 110);
	HPDF_Page_Stroke(page);

	/* print the title of the page (with positioning center). */
	HPDF_Page_SetFontAndSize(page, font, 24);
	tw = HPDF_Page_TextWidth(page, page_title);
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_Page_GetWidth(page) - tw) / 2,
		HPDF_Page_GetHeight(page) - 50);
	HPDF_Page_ShowText(page, page_title);
	HPDF_Page_EndText(page);

	HPDF_Page_SetFontAndSize(page, font, 10);

	/* Draw verious widths of lines. */
	HPDF_Page_SetLineWidth(page, 0);
	draw_line(page, 60, 770, "line width = 0");

	HPDF_Page_SetLineWidth(page, 1.0);
	draw_line(page, 60, 740, "line width = 1.0");

	HPDF_Page_SetLineWidth(page, 2.0);
	draw_line(page, 60, 710, "line width = 2.0");

	/* Line dash pattern */
	HPDF_Page_SetLineWidth(page, 1.0);

	HPDF_Page_SetDash(page, DASH_MODE1, 1, 1);
	draw_line(page, 60, 680, "dash_ptn=[3], phase=1 -- "
		"2 on, 3 off, 3 on...");

	HPDF_Page_SetDash(page, DASH_MODE2, 2, 2);
	draw_line(page, 60, 650, "dash_ptn=[7, 3], phase=2 -- "
		"5 on 3 off, 7 on,...");

	HPDF_Page_SetDash(page, DASH_MODE3, 4, 0);
	draw_line(page, 60, 620, "dash_ptn=[8, 7, 2, 7], phase=0");

	HPDF_Page_SetDash(page, NULL, 0, 0);

	HPDF_Page_SetLineWidth(page, 30);
	HPDF_Page_SetRGBStroke(page, 0.0, 0.5, 0.0);

	/* Line Cap Style */
	HPDF_Page_SetLineCap(page, HPDF_BUTT_END);
	draw_line2(page, 60, 570, "PDF_BUTT_END");

	HPDF_Page_SetLineCap(page, HPDF_ROUND_END);
	draw_line2(page, 60, 505, "PDF_ROUND_END");

	HPDF_Page_SetLineCap(page, HPDF_PROJECTING_SCUARE_END);
	draw_line2(page, 60, 440, "PDF_PROJECTING_SCUARE_END");

	/* Line Join Style */
	HPDF_Page_SetLineWidth(page, 30);
	HPDF_Page_SetRGBStroke(page, 0.0, 0.0, 0.5);

	HPDF_Page_SetLineJoin(page, HPDF_MITER_JOIN);
	HPDF_Page_MoveTo(page, 120, 300);
	HPDF_Page_LineTo(page, 160, 340);
	HPDF_Page_LineTo(page, 200, 300);
	HPDF_Page_Stroke(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 60, 360);
	HPDF_Page_ShowText(page, "PDF_MITER_JOIN");
	HPDF_Page_EndText(page);

	HPDF_Page_SetLineJoin(page, HPDF_ROUND_JOIN);
	HPDF_Page_MoveTo(page, 120, 195);
	HPDF_Page_LineTo(page, 160, 235);
	HPDF_Page_LineTo(page, 200, 195);
	HPDF_Page_Stroke(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 60, 255);
	HPDF_Page_ShowText(page, "PDF_ROUND_JOIN");
	HPDF_Page_EndText(page);

	HPDF_Page_SetLineJoin(page, HPDF_BEVEL_JOIN);
	HPDF_Page_MoveTo(page, 120, 90);
	HPDF_Page_LineTo(page, 160, 130);
	HPDF_Page_LineTo(page, 200, 90);
	HPDF_Page_Stroke(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 60, 150);
	HPDF_Page_ShowText(page, "PDF_BEVEL_JOIN");
	HPDF_Page_EndText(page);

	/* Draw Rectangle */
	HPDF_Page_SetLineWidth(page, 2);
	HPDF_Page_SetRGBStroke(page, 0, 0, 0);
	HPDF_Page_SetRGBFill(page, 0.75, 0.0, 0.0);

	draw_rect(page, 300, 770, "Stroke");
	HPDF_Page_Stroke(page);

	draw_rect(page, 300, 720, "Fill");
	HPDF_Page_Fill(page);

	draw_rect(page, 300, 670, "Fill then Stroke");
	HPDF_Page_FillStroke(page);

	/* Clip Rect */
	HPDF_Page_GSave(page);  /* Save the current graphic state */
	draw_rect(page, 300, 620, "Clip Rectangle");
	HPDF_Page_Clip(page);
	HPDF_Page_Stroke(page);
	HPDF_Page_SetFontAndSize(page, font, 13);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 290, 600);
	HPDF_Page_SetTextLeading(page, 12);
	HPDF_Page_ShowText(page,
		"Clip Clip Clip Clip Clip Clipi Clip Clip Clip");
	HPDF_Page_ShowTextNextLine(page,
		"Clip Clip Clip Clip Clip Clip Clip Clip Clip");
	HPDF_Page_ShowTextNextLine(page,
		"Clip Clip Clip Clip Clip Clip Clip Clip Clip");
	HPDF_Page_EndText(page);
	HPDF_Page_GRestore(page);

	/* Curve Example(CurveTo2) */
	x = 330;
	y = 440;
	x1 = 430;
	y1 = 530;
	x2 = 480;
	y2 = 470;
	x3 = 480;
	y3 = 90;

	HPDF_Page_SetRGBFill(page, 0, 0, 0);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 300, 540);
	HPDF_Page_ShowText(page, "CurveTo2(x1, y1, x2. y2)");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x + 5, y - 5);
	HPDF_Page_ShowText(page, "Current point");
	HPDF_Page_MoveTextPos(page, x1 - x, y1 - y);
	HPDF_Page_ShowText(page, "(x1, y1)");
	HPDF_Page_MoveTextPos(page, x2 - x1, y2 - y1);
	HPDF_Page_ShowText(page, "(x2, y2)");
	HPDF_Page_EndText(page);

	HPDF_Page_SetDash(page, DASH_MODE1, 1, 0);

	HPDF_Page_SetLineWidth(page, 0.5);
	HPDF_Page_MoveTo(page, x1, y1);
	HPDF_Page_LineTo(page, x2, y2);
	HPDF_Page_Stroke(page);

	HPDF_Page_SetDash(page, NULL, 0, 0);

	HPDF_Page_SetLineWidth(page, 1.5);

	HPDF_Page_MoveTo(page, x, y);
	HPDF_Page_CurveTo2(page, x1, y1, x2, y2);
	HPDF_Page_Stroke(page);

	/* Curve Example(CurveTo3) */
	y -= 150;
	y1 -= 150;
	y2 -= 150;

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 300, 390);
	HPDF_Page_ShowText(page, "CurveTo3(x1, y1, x2. y2)");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x + 5, y - 5);
	HPDF_Page_ShowText(page, "Current point");
	HPDF_Page_MoveTextPos(page, x1 - x, y1 - y);
	HPDF_Page_ShowText(page, "(x1, y1)");
	HPDF_Page_MoveTextPos(page, x2 - x1, y2 - y1);
	HPDF_Page_ShowText(page, "(x2, y2)");
	HPDF_Page_EndText(page);

	HPDF_Page_SetDash(page, DASH_MODE1, 1, 0);

	HPDF_Page_SetLineWidth(page, 0.5);
	HPDF_Page_MoveTo(page, x, y);
	HPDF_Page_LineTo(page, x1, y1);
	HPDF_Page_Stroke(page);

	HPDF_Page_SetDash(page, NULL, 0, 0);

	HPDF_Page_SetLineWidth(page, 1.5);
	HPDF_Page_MoveTo(page, x, y);
	HPDF_Page_CurveTo3(page, x1, y1, x2, y2);
	HPDF_Page_Stroke(page);

	/* Curve Example(CurveTo) */
	y -= 150;
	y1 -= 160;
	y2 -= 130;
	x2 += 10;

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 300, 240);
	HPDF_Page_ShowText(page, "CurveTo(x1, y1, x2. y2, x3, y3)");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x + 5, y - 5);
	HPDF_Page_ShowText(page, "Current point");
	HPDF_Page_MoveTextPos(page, x1 - x, y1 - y);
	HPDF_Page_ShowText(page, "(x1, y1)");
	HPDF_Page_MoveTextPos(page, x2 - x1, y2 - y1);
	HPDF_Page_ShowText(page, "(x2, y2)");
	HPDF_Page_MoveTextPos(page, x3 - x2, y3 - y2);
	HPDF_Page_ShowText(page, "(x3, y3)");
	HPDF_Page_EndText(page);

	HPDF_Page_SetDash(page, DASH_MODE1, 1, 0);

	HPDF_Page_SetLineWidth(page, 0.5);
	HPDF_Page_MoveTo(page, x, y);
	HPDF_Page_LineTo(page, x1, y1);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, x2, y2);
	HPDF_Page_LineTo(page, x3, y3);
	HPDF_Page_Stroke(page);

	HPDF_Page_SetDash(page, NULL, 0, 0);

	HPDF_Page_SetLineWidth(page, 1.5);
	HPDF_Page_MoveTo(page, x, y);
	HPDF_Page_CurveTo(page, x1, y1, x2, y2, x3, y3);
	HPDF_Page_Stroke(page);

	/* save the document to a file */
	HPDF_SaveToFile(pdf, fname);

	/* clean up */
	HPDF_Free(pdf);

	return 0;
}

#else
jmp_buf env;

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler(HPDF_STATUS   error_no,
	HPDF_STATUS   detail_no,
	void         *user_data)
{
	//printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
	//	(HPDF_UINT)detail_no);
	longjmp(env, 1);
}

void
draw_line(HPDF_Page    page,
	float        x,
	float        y,
	const char  *label)
{
	/*HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x, y - 10);
	HPDF_Page_ShowText(page, label);
	HPDF_Page_EndText(page);
*/
	HPDF_Page_MoveTo(page, x, y - 15);
	HPDF_Page_LineTo(page, x + 220, y - 15);
	HPDF_Page_LineTo(page, x + 240, y - 35);
	HPDF_Page_LineTo(page, x, y - 15);
	HPDF_Page_Stroke(page);

	x += 20;
	y -= 40;
	HPDF_Page_SetLineWidth(page, 2.5);
	HPDF_Page_MoveTo(page, x, y - 15);
	HPDF_Page_LineTo(page, x + 220, y - 15);
	HPDF_Page_LineTo(page, x + 240, y - 35);
	HPDF_Page_LineTo(page, x, y - 15);
	HPDF_Page_Stroke(page);
}


int pdf_main_test_gr()
{
	const char* page_title = "Line Example";

	HPDF_Doc  pdf;
	HPDF_Font font;
	HPDF_Page page;
	char fname[256];

	const HPDF_UINT16 DASH_MODE1[] = { 3 };
	const HPDF_UINT16 DASH_MODE2[] = { 3, 7 };
	const HPDF_UINT16 DASH_MODE3[] = { 8, 7, 2, 7 };

	double x;
	double y;
	double x1;
	double y1;
	double x2;
	double y2;
	double x3;
	double y3;

	float tw;

	strcpy(fname, "pdftest_graphics");
	strcat(fname, ".pdf");

	pdf = HPDF_New(error_handler, NULL);
	if (!pdf) {
		//printf("error: cannot create PdfDoc object\n");
		return 1;
	}

	if (setjmp(env)) {
		HPDF_Free(pdf);
		return 1;
	}

	/* create default-font */
	font = HPDF_GetFont(pdf, "Helvetica", NULL);

	/* add a new page object. */
	page = HPDF_AddPage(pdf);

	/* print the lines of the page. */
	HPDF_Page_SetLineWidth(page, 1);
	HPDF_Page_Rectangle(page, 50, 50, HPDF_Page_GetWidth(page) - 100,
		HPDF_Page_GetHeight(page) - 110);
	HPDF_Page_Stroke(page);

	/* print the title of the page (with positioning center). */
	HPDF_Page_SetFontAndSize(page, font, 24);
	tw = HPDF_Page_TextWidth(page, page_title);
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_Page_GetWidth(page) - tw) / 2,
		HPDF_Page_GetHeight(page) - 50);
	HPDF_Page_ShowText(page, page_title);
	HPDF_Page_EndText(page);

	HPDF_Page_SetFontAndSize(page, font, 10);

	/* Draw verious widths of lines. */
	HPDF_Page_SetLineWidth(page, 0);
	draw_line(page, 60, 770, "line width = 0");


	/* save the document to a file */
	HPDF_SaveToFile(pdf, fname);

	/* clean up */
	HPDF_Free(pdf);

	return 0;
}

#endif