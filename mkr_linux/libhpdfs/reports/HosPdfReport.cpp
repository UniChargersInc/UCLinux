
#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <cstdint>
#include <iostream>

#include "pdf_report.h"

//// For simulated exception handling
//jmp_buf env;
//
//// Global handlers to the HPDF document and page
//HPDF_Doc pdf;
//HPDF_Page pdf_page;



// A standard hpdf error handler which also translates the hpdf error code to a human
// readable string
static void
error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data) {
	fprintf(stderr, "*** PDF ERROR: \"%s\", [0x%04X : %d]\n", hpdf_errstr(error_no), (unsigned int)error_no, (int)detail_no);
	//longjmp(env, 1);
	jmp_buf *env = (jmp_buf*)user_data;
	longjmp(*env, 1);
}


// Setup a PDF document with one page
static HPDF_Page
add_a4page(HPDF_Doc pdf) {
	HPDF_Page pdf_page = HPDF_AddPage(pdf);
	HPDF_Page_SetSize(pdf_page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
	return pdf_page;
}


int get_table_height(/*HosReport_T *rr*/PTlrList *list)
{
	int table_height = 0;
	//PTlrList *list = rr->getReportData();
	if (list)
	{
		int size = list->list_size();
		if (size > 0)
		{
			table_height = (size + 1) * 25 + 60;
		}
	}
	return table_height;
}
void add_hos_report_content(int64_t date_ts,PdfReportParams *par, HPDF_Doc pdf, HPDF_Page pdf_page)
{
	PTlrList *tlr_list =(PTlrList *) par->getReportData();
	if (tlr_list == 0)
		return;
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	int table_height = get_table_height(tlr_list);
	int total_height = 390 + table_height + 120 + 120 + 60;
	HPDF_Page_SetHeight(pdf_page, total_height);

	HPDF_REAL hp = HPDF_Page_GetHeight(pdf_page), wp = HPDF_Page_GetWidth(pdf_page);

	string tmp_str = "";
	if (par->emp->has_photo())
	{
		tmp_str = par->emp->photo();
	}
	if(tmp_str.empty())
		tmp_str = "blank.jpg";
	
	draw_image(pdf, /*"Andrew_Fuller.jpg"*/tmp_str.c_str(), wp - 90, hp - 90,40,40, WEB_RES_IMG_DIR);
	//HPDF_Image image = HPDF_LoadPngImageFromFile(pdf, "C:\\My\\Linux\\libevent2_win32_proj\\libharu-master\\demo\\pngsuite\\maskimage.png");
	//HPDF_Page_DrawImage(pdf_page, image, 240, 40, 64, 64);

	//**********************Hos header************************************
	HPDF_Font  font1 = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font1, 18);
	tmp_str = "Hos Report: " + par->uid;
	showCenteredTitle(pdf_page, tmp_str.c_str(), hp - 20, 140);


	float Yc = 50;
	HPDF_Page_SetFontAndSize(pdf_page, font1, 14);
	tmp_str = DateTime::DateStrFromMillis(/*par->from_ts*/date_ts);
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Date:", tmp_str.c_str());

	Yc += 20;
	HPDF_Page_SetFontAndSize(pdf_page, font, 12);
	
	tmp_str = par->emp->drname();
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Employee:", tmp_str.c_str());
	// tmp_str=rr->  co-driver ???
	draw_text_pair_underline(pdf_page, 40 + 200, hp - Yc, "Co-driver:", "           ");
	Yc += 20;
	tmp_str = Cycle2Str(par->emp->cycle());
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Cycle:", /*"Canada_7h/8d"*/tmp_str.c_str());
	Yc += 20;
	tmp_str = par->emp->haddr();
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Home terminal:", tmp_str.c_str());
	Yc += 20;
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Shiipping document:", "", 200);

	Yc += 20;
	HPDF_Page_SetFontAndSize(pdf_page, font1, 14);
	draw_text(pdf_page, "Odometer:", 40, hp - Yc);
	HPDF_Page_SetFontAndSize(pdf_page, font, 12);
	draw_text_pair_underline(pdf_page, 40 + 100, hp - Yc, "Start:", "11234");
	draw_text_pair_underline(pdf_page, 40 + 100 + 100, hp - Yc, "End:", "11334");
	draw_text_pair_underline(pdf_page, 40 + 200 + 100, hp - Yc, "Total:", "100");

	Yc += 20;

	Yc += 200; // left bottom conner of the HOS graph  height=200;


	drawHosGraph(pdf, pdf_page, tlr_list, 20, hp - Yc, wp - 40, 200);
	Yc += 20;

	//calculate hos events table height !!!

	Yc += table_height; // 200 = table height -> must be calculated  -> HERE WE RESERVE SPACE FOR A TABLE since it's growing from the bottom to the top
	my_hos_tbl2(date_ts,par,pdf, pdf_page,  hp - Yc);


	Yc += 100; // 100 - signature height
	char *sDriver = "Driver's signature:";

	draw_text_pair_underline(pdf_page, 40, hp - Yc, sDriver, "", 400);


	MyRect_T rect;
	rect.x = 40 + HPDF_Page_TextWidth(pdf_page, sDriver) + 10;
	rect.y = hp - Yc + 100; // top left corner
	rect.width = 200;
	rect.height = 100;

	int len = par->emp->sign().size();
	short *data = (short*)par->emp->sign().data();
	draw_signature(pdf_page, data, len, rect);


	Yc += 120;

	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Supervisor's signature:", "", 400);
	//********************Hos page footer*****************
	HPDF_Page_SetFontAndSize(pdf_page, font, 10);
	HPDF_Page_SetRGBFill(pdf_page, 0.3, 0.3, 0.3);
	//float angle1 = 0;                   /* A rotation of 30 degrees. */
	//float rad1 = angle1 / 180 * 3.141592; /* Calcurate the radian value. */

	//HPDF_Page_BeginText(pdf_page);
	//HPDF_Page_SetTextMatrix(pdf_page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),
	//	40,  2);
	//HPDF_Page_ShowText(pdf_page, "Mikerel System Inc. All rights reserved");
	//HPDF_Page_EndText(pdf_page);
	showCenteredTitle(pdf_page, "Mikerel System Inc. All rights reserved", 2);

	//--------------------My HOS stuff-----------
	//add_a4page();
	//my_hos_tbl();
	//my_hos_tbl2(pdf, pdf_page);
	//--------------------------------------------
	//stroke_page_tofile();
	
}
int main_hos_pdf_report(PdfReport_T *rr)
{
	jmp_buf env;
	HPDF_Doc pdf;


	bool need_height_adjustment = false;

	pdf = HPDF_New(error_handler, /*NULL*/&env);
	if (setjmp(env)) {
		HPDF_Free(pdf);
		return EXIT_FAILURE;
	}
	
	// Setup the PDF document
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	

	//1. Add first page
	HPDF_Page pdf_page=add_a4page(pdf);
	add_hos_report_content(rr->par->from_ts,rr->par, pdf, pdf_page);
	stroke_page_result_to(rr,pdf);

	//m_printf("main_hos_pdf_report ..DONE\r\n");

	return (EXIT_SUCCESS);
}

