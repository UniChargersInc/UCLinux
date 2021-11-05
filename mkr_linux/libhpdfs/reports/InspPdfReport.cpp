
#include <stdio.h>
#include <stdlib.h>

#include "pdf_report.h"

//// For simulated exception handling
//static jmp_buf env;
//
//// Global handlers to the HPDF document and page
//extern HPDF_Doc pdf;
//extern HPDF_Page pdf_page;


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
	HPDF_REAL hp = HPDF_Page_GetHeight(pdf_page);
	hp *= 1.8;
	HPDF_Page_SetHeight(pdf_page, hp);
	return pdf_page;
}

void add_insp_report_content(PdfReportParams *par, HPDF_Doc pdf, HPDF_Page pdf_page)
{
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_REAL hp = HPDF_Page_GetHeight(pdf_page), wp = HPDF_Page_GetWidth(pdf_page);

	//draw_image(pdf, "Andrew_Fuller.jpg", wp - 90, hp - 90, WEB_RES_IMG_DIR);
	draw_image(pdf, "toyota.jpg", wp - 160, hp - 120,40,40, MEDIA_VEH);
	//**********************Hos header************************************
	HPDF_Font  font1 = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font1, 18);
	std::string title = "Inspection Report: " + par->uid;
	showCenteredTitle(pdf_page, title.c_str(), hp - 20, 100);

	PIRpt *ir = (PIRpt *)par->getReportData();
	if (!ir)
		return;
	string tmp_str = DateTime::DateHMStrFromMillis(ir->its());

	float Yc = 50;
	HPDF_Page_SetFontAndSize(pdf_page, font1, 14);
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Date:", tmp_str.c_str());

	
	Yc += 20;
	HPDF_Page_SetFontAndSize(pdf_page, font, 12);
	tmp_str = par->emp->drname();
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Employee:", tmp_str.c_str());
	draw_text_pair_underline(pdf_page, 40 + 200, hp - Yc, "Organization:", "           ");
	Yc += 20;
	PVehicle *veh = par->veh;
	tmp_str = veh != 0 ? veh->desc() : "";
	tmp_str = "Name:" + tmp_str + " ID:" + ir->vid();
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Vehicle:", tmp_str.c_str());
	tmp_str = *ir->mutable_addr();//ir->addr;
	if(tmp_str.empty())
		tmp_str = Double2Str(ir->lat()) + "--" + Double2Str(ir->lon());
	Yc += 20;
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Inspection place:",/* "1000 Yonge Street, Toronto, Canada"*/tmp_str.c_str());
	Yc += 20;
	tmp_str = Int2Str(ir->odo());
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Odometer value:", tmp_str.c_str(), 100);

	Yc += 20;
	HPDF_Page_SetFontAndSize(pdf_page, font1, 14);
	draw_text(pdf_page, "Vehicle:", 40, hp - Yc);
	HPDF_Page_SetFontAndSize(pdf_page, font, 12);
	tmp_str = veh->vl();
	draw_text_pair_underline(pdf_page, 40 + 100, hp - Yc, "License plate:", tmp_str.c_str());
	draw_text_pair_underline(pdf_page, 40 + 100 + 200, hp - Yc, "Trailer license:", "n/a");
	Yc += 30;
	draw_text(pdf_page, "I declare that the above vehicle has been inspected with aplicable requirements", 40, hp - Yc);

	Yc += 20;

	Yc += 100; // 100 - signature height
	char *sInsp = "Inspector's signature:";
	draw_text_pair_underline(pdf_page, 40, hp - Yc, sInsp, "", 400);


	MyRect_T rect;
	rect.x = 40 + HPDF_Page_TextWidth(pdf_page, sInsp) + 10;
	rect.y = hp - Yc + 95; // top left corner
	rect.width = 200;
	rect.height = 95;

	////fake data
	//unsigned short *data = new  unsigned short[10 * 2];
	//unsigned short *p = data;
	//for (int i = 0; i < 10; i++)
	//{
	//	*p++ = (i * 10) + 40;
	//	*p++ = i * 10;
	//}

	int len = par->emp->sign().size();
	short *data = (short*)par->emp->sign().data();
	draw_signature(pdf_page, data, len, rect);

	Yc += (500 + 60);


	my_insp_tbl_cats(par,pdf, pdf_page, hp - Yc);
	Yc += 20;
	int size = ir->defects_size();
	if (size > 0)
	{
		int table_height = (size + 1) * 25 + 60;
		Yc += table_height; // 200 = table height -> must be calculated  -> HERE WE RESERVE SPACE FOR A TABLE since it's growing from the bottom to the top
		my_insp_tbl_defects(par,pdf, pdf_page,  hp - Yc);
	}

	Yc += 30;
	HPDF_Page_SetFontAndSize(pdf_page, font1, 10);
	draw_text(pdf_page, "I declare that the above noted defects have been fixed", 40, hp - Yc);

	Yc += 100; // 100 - signature width
	draw_text_pair_underline(pdf_page, 40, hp - Yc, "Authorized signature:", "", 400);

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


}

int main_insp_pdf_report(PdfReport_T *rr)
{
	jmp_buf env;
	HPDF_Doc pdf;
	HPDF_Font font;

	pdf = HPDF_New(error_handler, /*NULL*/&env);

	if (setjmp(env)) {
		HPDF_Free(pdf);
		return EXIT_FAILURE;
	}
	

	// Setup the PDF document
	
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	font = HPDF_GetFont(pdf, "Helvetica", NULL);



	//1. Add first page
	HPDF_Page pdf_page=add_a4page(pdf);

	//we need vehicle info and Categories info
	PVehicle *veh = rr->par->get_veh_info_cb(rr->par->cid, rr->par->vid);
	rr->par->veh = veh;

	PICatList *clist = rr->par->get_cats_info_cb(rr->par->vid);
	rr->par->cat_list = clist;
	if (veh&&clist)
	{
		add_insp_report_content(rr->par, pdf, pdf_page);
		stroke_page_result_to(rr, pdf);
	}
	//printf("Done. \"%s\"\n", OUTPUT_FILE);
	if (veh)
		delete veh;
	if (clist)
		delete clist;
	

	return (EXIT_SUCCESS);
}

