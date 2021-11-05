
#include <stdio.h>
#include <stdlib.h>

#include "pdf_report.h"

void add_hos_report_content(int64_t date_ts,PdfReportParams *par,  HPDF_Doc pdf, HPDF_Page pdf_page);
void add_insp_report_content(PdfReportParams *par, HPDF_Doc pdf, HPDF_Page pdf_page);

// A standard hpdf error handler which also translates the hpdf error code to a human
// readable string
static void
error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data) {
	fprintf(stderr, "*** PDF ERROR: \"%s\", [0x%04X : %d]\n", hpdf_errstr(error_no), (unsigned int)error_no, (int)detail_no);
	jmp_buf *env=(jmp_buf*)user_data;
	longjmp(*env, 1);
}


// Setup a PDF document with one page
static HPDF_Page
add_a4page(HPDF_Doc pdf) {
	HPDF_Page pdf_page = HPDF_AddPage(pdf);
	HPDF_Page_SetSize(pdf_page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
	HPDF_REAL hp = HPDF_Page_GetHeight(pdf_page);
	hp *= 1.8;
	HPDF_Page_SetHeight(pdf_page, hp);
	return pdf_page;
}



void print_page(HPDF_Page   page, int page_num)
{
	char buf[50];

	HPDF_Page_SetWidth(page, 200);
	HPDF_Page_SetHeight(page, 300);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 50, 250);
#ifdef __WIN32__
	_snprintf(buf, 50, "Page:%d", page_num);
#else
	snprintf(buf, 50, "Page:%d", page_num);
#endif
	HPDF_Page_ShowText(page, buf);
	HPDF_Page_EndText(page);
}

int main_combined_pdf_report(CombinedReport_T *rr)
{
	// For simulated exception handling
	jmp_buf env;
	HPDF_Doc pdf;
	
	pdf = HPDF_New(error_handler, /*NULL*/&env);

	if (setjmp(env)) {
		HPDF_Free(pdf);
		return EXIT_FAILURE;
	}


	// Setup the PDF document
	
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	
	/* Set page mode to use outlines. */
	HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_OUTLINE);

	PdfReportParams *par = rr->par;
	int size=rr->hos_map_list.size();
	if (size > 0)
	{
		HPDF_Outline root = HPDF_CreateOutline(pdf, NULL, "HOS Logsheets", NULL);
		HPDF_Outline_SetOpened(root, HPDF_TRUE);

		vector<HPDF_Page> hos_pages;
		vector<HPDF_Outline> outline_arr;
		for (auto v : rr->hos_map_list)
		{
			HPDF_Page page = add_a4page(pdf);
			PTlrList *tlrL = v.second;
			par->setReportData(tlrL);
			add_hos_report_content(v.first,par, pdf, page);
			hos_pages.push_back(page);
			PTlr *tlr = tlrL->mutable_list(0);
			string tmp_str = DateTime::DateStrFromMillis(tlr?tlr->logtime():0);
			HPDF_Outline outline = HPDF_CreateOutline(pdf, root, tmp_str.c_str(), NULL);
			outline_arr.push_back(outline);

			HPDF_Destination dst = HPDF_Page_CreateDestination(page);
			HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
			HPDF_Outline_SetDestination(outline, dst);
		}
		

		
	}
	PIRptList *rL = rr->insp_list;
	size = rL ? rL->list_size() : 0;
	if (size > 0)
	{
		HPDF_Outline root = HPDF_CreateOutline(pdf, NULL, "DVIRs", NULL);//Inspection reports
		HPDF_Outline_SetOpened(root, HPDF_TRUE);

		vector<HPDF_Page> hos_pages;
		vector<HPDF_Outline> outline_arr;
		string prev_vid = "";
		rr->par->veh = 0;
		rr->par->cat_list = 0;
		for (int i = 0; i < size; i++)
		{
			PIRpt *r = rL->mutable_list(i);
			HPDF_Page page = add_a4page(pdf);
			
			par->setReportData(r);

			//we need vehicle info and Categories info
			//these requests uses database query ->  minimize request ( do it ONLY when VehicleID changed)
			string vid = r->vid();
			if (prev_vid != vid)
			{   //clean up the previus one
				if (rr->par->veh) delete rr->par->veh;
				if (rr->par->cat_list) delete rr->par->cat_list;

				rr->par->veh = rr->par->get_veh_info_cb(rr->par->cid, vid);
				rr->par->cat_list = rr->par->get_cats_info_cb(vid);

				prev_vid = vid;
			}
			if (rr->par->veh&&rr->par->cat_list)
			{
				add_insp_report_content(rr->par, pdf, page);
				hos_pages.push_back(page);
				string tmp_str = DateTime::DateHMStrFromMillis(r ? r->its() : 0);
				HPDF_Outline outline = HPDF_CreateOutline(pdf, root, tmp_str.c_str(), NULL);
				outline_arr.push_back(outline);

				HPDF_Destination dst = HPDF_Page_CreateDestination(page);
				HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
				HPDF_Outline_SetDestination(outline, dst);
			}

		}//for (int i = 0; i < size; i++)

		 //final clean up
		if (rr->par->veh) delete rr->par->veh;
		if (rr->par->cat_list) delete rr->par->cat_list;

	}


#if 0
	HPDF_Page page[4],page1[4];

	//1. Add first page
	page[0] = add_a4page(pdf);
	add_inspection_report_content(rr->inspR,pdf,page[0]);


	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);

	page[1] = HPDF_AddPage(pdf);
	HPDF_Page_SetFontAndSize(page[1], font, 20);
	print_page(page[1], 2);

	page[2] = HPDF_AddPage(pdf);
	HPDF_Page_SetFontAndSize(page[2], font, 20);
	print_page(page[2], 3);

	//----------------------------------------------
	//1. Add first page
	page1[0] = add_a4page(pdf);
	add_hos_report_content(rr->hosR, pdf, page1[0]);

		
	page1[1] = HPDF_AddPage(pdf);
	HPDF_Page_SetFontAndSize(page1[1], font, 20);
	print_page(page1[1], 2);

	page1[2] = HPDF_AddPage(pdf);
	HPDF_Page_SetFontAndSize(page1[2], font, 20);
	print_page(page1[2], 3);




	HPDF_Outline root,root1;
	HPDF_Outline outline[4], outline1[4];
	HPDF_Destination dst,dst1;

	/* create outline root. */
	root = HPDF_CreateOutline(pdf, NULL, "HOS Logsheets", NULL);
	HPDF_Outline_SetOpened(root, HPDF_TRUE);

	outline[0] = HPDF_CreateOutline(pdf, root, "page1", NULL);
	outline[1] = HPDF_CreateOutline(pdf, root, "page2", NULL);

	/* create outline with test which is  encoding */
	/*outline[2] = HPDF_CreateOutline(pdf, root, "SAMP_TXT",
		HPDF_GetEncoder(pdf, "90ms-RKSJ-H"));*/
	outline[2] = HPDF_CreateOutline(pdf, root, "page23+++BBBBB", NULL);

	/* create destination objects on each pages
	* and link it to outline items.
	*/
	dst = HPDF_Page_CreateDestination(page[0]);
	HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page[0]), 1);
	HPDF_Outline_SetDestination(outline[0], dst);

	dst = HPDF_Page_CreateDestination(page[1]);
	HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page[1]), 1);
	HPDF_Outline_SetDestination(outline[1], dst);

	dst = HPDF_Page_CreateDestination(page[2]);
	HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page[2]), 1);
	HPDF_Outline_SetDestination(outline[2], dst);


	//----------------------------------------------------------------
	/* create outline root. */
	root1 = HPDF_CreateOutline(pdf, NULL, "Inspection reports", NULL);
	HPDF_Outline_SetOpened(root1, HPDF_TRUE);

	outline1[0] = HPDF_CreateOutline(pdf, root1, "DVIR-1", NULL);
	outline1[1] = HPDF_CreateOutline(pdf, root1, "DVIR-2", NULL);

	/* create outline with test which is  encoding */
	/*outline[2] = HPDF_CreateOutline(pdf, root, "SAMP_TXT",
	HPDF_GetEncoder(pdf, "90ms-RKSJ-H"));*/
	outline1[2] = HPDF_CreateOutline(pdf, root1, "DVIR-3", NULL);

	/* create destination objects on each pages
	* and link it to outline items.
	*/
	dst1 = HPDF_Page_CreateDestination(page1[0]);
	HPDF_Destination_SetXYZ(dst1, 0, HPDF_Page_GetHeight(page1[0]), 1);
	HPDF_Outline_SetDestination(outline1[0], dst1);

	dst1 = HPDF_Page_CreateDestination(page1[1]);
	HPDF_Destination_SetXYZ(dst1, 0, HPDF_Page_GetHeight(page1[1]), 1);
	HPDF_Outline_SetDestination(outline1[1], dst1);

	dst1 = HPDF_Page_CreateDestination(page1[2]);
	HPDF_Destination_SetXYZ(dst1, 0, HPDF_Page_GetHeight(page1[2]), 1);
	HPDF_Outline_SetDestination(outline1[2], dst1);


#endif

	stroke_page_result_to_file(rr->rpt_filename.c_str(),pdf);

	//printf("Done. \"%s\"\n", OUTPUT_FILE);

	return (EXIT_SUCCESS);
}

