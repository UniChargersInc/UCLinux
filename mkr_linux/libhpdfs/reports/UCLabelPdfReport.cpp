
#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <cstdint>
#include <iostream>

#include "pdf_report.h"
#include "CommonUtils/my_time.h"


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
add_a4page(HPDF_Doc pdf,HPDF_REAL width,HPDF_REAL height) {
	HPDF_Page pdf_page = HPDF_AddPage(pdf);
	//HPDF_Page_SetSize(pdf_page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
	//HPDF_Page_SetSize(pdf_page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);

	HPDF_Page_SetHeight(pdf_page, /*280*/height);
	HPDF_Page_SetWidth(pdf_page, /*120*/width);
	return pdf_page;
}

const char* str_charger_type[] = { "Grizzl-E Classic", "Grizzl-E Smart","Grizzl-E Mini", "Grizzl-E Extreme Edition", "Grizzl-E Avalanche", "Grizzl-E Duo", "Alpha ", "PCPH Set" };

void add_uc_G_label_report_content(int64_t date_ts,PdfCSReportParams *par, HPDF_Doc pdf, HPDF_Page pdf_page)
{
	
	//HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	
	HPDF_REAL hp = HPDF_Page_GetHeight(pdf_page), wp = HPDF_Page_GetWidth(pdf_page);  // 120 x 280

	string tmp_str = "";
	
	
	//**********************Hos header************************************
	string sN = "";
	int cert_flag = 0;
	if (par->pWoi)
	{
		sN = par->uid;//par->pWoi->locationname();
		cert_flag =par->pWoi->certflag();
		/*if (!cert.empty())
		{
			try {
				cert_flag = std::stoi(cert);
			}catch(std::exception &ex)
			{ }
		}*/
	}

	//draw company logo
	tmp_str = "Logo1.jpg";
	draw_image(pdf, tmp_str.c_str(), 0, hp-32, wp / 2 - 10,32, WEB_LABEL_IMG_DIR);


	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
	tmp_str = "Made in Canada";
	float x0 = wp / 2 - 10, y0 = 8;
	draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);

	y0 += 7;
	tmp_str = "unitedchargers.com";
	draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);

	y0 += 7;
	tmp_str = "90 Gough Rd, Unit2";
	draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);

	y0 += 7;
	tmp_str = "Markham ON,L3R 5V5";
	draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);

	y0 += 7;
	tmp_str = "1-833-240-8118";
	draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);

	HPDF_Page_SetFontAndSize(pdf_page, font, 8);
	x0 = 2;
	int type = par->pWoi->type();
	type--;
	//printf("G type %d \r\n",type);
	if (type >= 0 && type <= 6)
	{
		std::string typeStr = str_charger_type[type];
		y0 += 10;
		
		//HPDF_Page_SetFontAndSize(pdf_page, font, 8);
		draw_text(pdf_page, typeStr.c_str(), x0, hp - y0);
	}
	if (!par->pWoi->param1().empty()) {
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 10;
		draw_text(pdf_page, par->pWoi->param1().c_str(), x0, hp - y0);
	}
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white

	y0 = 94;// 82;
	if (par->pWoi->certflag() & 1) {
		//draw UL logo
		tmp_str = "cULLogo.jpg";
		draw_image(pdf, tmp_str.c_str(), 0, hp - y0, wp / 2 - 10, 32, WEB_LABEL_IMG_DIR);

		x0 = wp / 2;
		y0 = 94/*82*/ - 16;
		HPDF_Page_SetFontAndSize(pdf_page, font, 8);
		tmp_str = "File number";
		draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 10;
		tmp_str = "[E510712]";
		draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);
	}
	//resrore black color
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white


	y0 = /*82*/94+26;
	//draw FCC logo
	tmp_str = "FCC.jpg";
	draw_image(pdf, tmp_str.c_str(), 4, hp - y0, 16, 16, WEB_LABEL_IMG_DIR);

	HPDF_Page_SetFontAndSize(pdf_page, font, 7);
	y0 += 10;
	tmp_str = "Contains FCC ID:";
	draw_text(pdf_page, tmp_str.c_str(), 4, hp - y0);
	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	y0 += 8;
	tmp_str = "2ABPY-5B5BD9";
	draw_text(pdf_page, tmp_str.c_str(), 4, hp - y0);


	if (par->pWoi->certflag() & 2) {
		//draw Energy star logo
		y0 = /*82*/94 + 42;
		tmp_str = "ESlogo1.jpg";
		draw_image(pdf, tmp_str.c_str(), wp / 2 + 10, hp - y0, 32, 32, WEB_LABEL_IMG_DIR);
	}
	

	// draw QR code
	tmp_str = sN;
	if (tmp_str.empty())
		tmp_str = "blank.jpg";
	else
		tmp_str += ".jpg";

	draw_image(pdf, tmp_str.c_str(), 0, 20,100,100, WEB_QRCODE_IMG_DIR);

	// ==serial number

	HPDF_Page_SetFontAndSize(pdf_page, font, 10);
	y0 = 15 + 110;
	x0 = 4;
	tmp_str = "Serial Number";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	//HPDF_Page_SetRGBFill(pdf_page,1.0,0,0);
	y0 -= 10;
	

	tmp_str = "["+sN+"]";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);



	
	
	
	//footer
	HPDF_Page_SetFontAndSize(pdf_page, font, 5);
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white
	x0 = 4;
	y0 =22;
	tmp_str = "Type 4X Enclusure";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);



	//M.F.G: YYYY/MM/DD
	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
    int64_t ts= par->pWoi->createdts();
	string tsStr=DateTime::DateStrFromMillis_1(ts);
	tmp_str = "MFG:"+tsStr;
	draw_text(pdf_page, tmp_str.c_str(), wp/2, y0);

	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white
	HPDF_Page_SetFontAndSize(pdf_page, font, 5);

	y0 -= 6;
	tmp_str = "In/Out 208-240 VAC, 60Hz, 40A Max";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	y0 -= 6;
	tmp_str = "Temperature rating -22F to 122F (-30C to 50C)";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);


		
}
int main_uc_G_label_pdf_report(PdfCSReport_T *rr)
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
	HPDF_Page pdf_page=add_a4page(pdf,120,280);
	add_uc_G_label_report_content(rr->par->from_ts,rr->par, pdf, pdf_page);
	stroke_page_result_to((PdfReportBase_T*)rr,pdf);

	//m_printf("main_hos_pdf_report ..DONE\r\n");

	return (EXIT_SUCCESS);
}

//***********************************************************************************
//***********************************************************************************
void add_uc_UL_label_report_content(int64_t date_ts, PdfCSReportParams *par, HPDF_Doc pdf, HPDF_Page pdf_page)
{

	//HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);

	HPDF_REAL hp = HPDF_Page_GetHeight(pdf_page), wp = HPDF_Page_GetWidth(pdf_page);  // 120 x 280

	string tmp_str = "";


	//**********************Hos header************************************
	string sN = "";
	int cert_flag = 0;
	if (par->pWoi)
	{
		sN = par->uid;//par->pWoi->locationname();
		cert_flag = par->pWoi->certflag();
		/*if (!cert.empty())
		{
			try {
				cert_flag = std::stoi(cert);
			}catch(std::exception &ex)
			{ }
		}*/
	}


	HPDF_Page_SetFontAndSize(pdf_page, font, 8);

	int x0 = 2, y0 = 9,Y0=56;
	int type = par->pWoi->type();
	type--;
	//printf("UL type %d \r\n",type);
	if (type >= 0 && type <= 6)
	{
		std::string typeStr = str_charger_type[type];
		
		x0 = 2;
		//HPDF_Page_SetFontAndSize(pdf_page, font, 9);
		draw_text(pdf_page, typeStr.c_str(), x0, hp - y0);
	}
	if (!par->pWoi->param1().empty()) {
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 10;
		draw_text(pdf_page, par->pWoi->param1().c_str(), x0, hp - y0);
	}
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white

	y0 = Y0;
	if (par->pWoi->certflag() & 1) {
		//draw UL logo
		tmp_str = "cULLogo.jpg";
		draw_image(pdf, tmp_str.c_str(), 0, hp - y0, wp / 2 - 10, 32, WEB_LABEL_IMG_DIR);

		x0 = wp / 2 -5;
		y0 = Y0 - 16;
		HPDF_Page_SetFontAndSize(pdf_page, font, 8);
		tmp_str = "File number";
		draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 10;
		tmp_str = "[E510712]";//"[UL Control#]";
		draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);
	}
	//resrore black color
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white


	y0 = Y0 + 26;
	//draw FCC logo
	tmp_str = "FCC.jpg";
	draw_image(pdf, tmp_str.c_str(), 4, hp - y0, 16, 16, WEB_LABEL_IMG_DIR);

	HPDF_Page_SetFontAndSize(pdf_page, font, 7);
	y0 += 10;
	tmp_str = "Contains FCC ID:";
	draw_text(pdf_page, tmp_str.c_str(), 4, hp - y0);
	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	y0 += 8;
	tmp_str = "2ABPY-5B5BD9";
	draw_text(pdf_page, tmp_str.c_str(), 4, hp - y0);


	if (par->pWoi->certflag() & 2) {
		//draw Energy star logo
		y0 = Y0 + 42;
		tmp_str = "ESlogo1.jpg";
		draw_image(pdf, tmp_str.c_str(), wp / 2 + 10, hp - y0, 32, 32, WEB_LABEL_IMG_DIR);
	}


	// draw QR code
	tmp_str = sN;
	if (tmp_str.empty())
		tmp_str = "blank.jpg";
	else
		tmp_str += ".jpg";

	draw_image(pdf, tmp_str.c_str(), 0, 20, 100, 100-8, WEB_QRCODE_IMG_DIR);

	// ==serial number

	HPDF_Page_SetFontAndSize(pdf_page, font, 10);
	y0 = 15 + 110 -8;
	x0 = 4;
	tmp_str = "Serial Number";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	y0 -= 10;


	tmp_str = "[" + sN + "]";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);






	//footer
	HPDF_Page_SetFontAndSize(pdf_page, font, 5);
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white
	x0 = 4;
	y0 = 22-4;
	tmp_str = "Type 4X Enclusure";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	//M.F.G: YYYY/MM/DD
	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
	int64_t ts = par->pWoi->createdts();
	string tsStr = DateTime::DateStrFromMillis_1(ts);
	tmp_str = "MFG:" + tsStr;
	draw_text(pdf_page, tmp_str.c_str(), wp / 2, y0);

	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white
	HPDF_Page_SetFontAndSize(pdf_page, font, 5);


	y0 -= 6;
	tmp_str = "In/Out 208-240 VAC, 60Hz, 40A Max";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	y0 -= 6;
	tmp_str = "Temperature rating -22F to 122F (-30C to 50C)";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);



}
int main_uc_UL_label_pdf_report(PdfCSReport_T *rr)
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
	HPDF_Page pdf_page = add_a4page(pdf, 120-8, 240-8);
	add_uc_UL_label_report_content(rr->par->from_ts, rr->par, pdf, pdf_page);
	stroke_page_result_to((PdfReportBase_T*)rr, pdf);

	//m_printf("main_hos_pdf_report ..DONE\r\n");

	return (EXIT_SUCCESS);
}
//======================================================Label UL- Classic---------------------===============

void add_uc_UL_Classic_label_report_content_with_energy_star(int64_t date_ts, PdfCSReportParams *par, HPDF_Doc pdf, HPDF_Page pdf_page)
{

	//HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);

	HPDF_REAL hp = HPDF_Page_GetHeight(pdf_page), wp = HPDF_Page_GetWidth(pdf_page);  // 120 x 280

	string tmp_str = "";


	//**********************Hos header************************************
	string sN = "";
	int cert_flag = 0;
	if (par->pWoi)
	{
		sN = par->uid;//par->pWoi->locationname();
		cert_flag = par->pWoi->certflag();
		/*if (!cert.empty())
		{
			try {
				cert_flag = std::stoi(cert);
			}catch(std::exception &ex)
			{ }
		}*/
	}


	HPDF_Page_SetFontAndSize(pdf_page, font, 8);

	int x0 = 2, y0 = 9, Y0 = 56;
	int type = par->pWoi->type();
	type--;
	//printf("UL type %d \r\n",type);
	if (type >= 0 && type <= 6)
	{
		std::string typeStr = str_charger_type[type];
		if (type == 3)   //Grizzl-E Avalanche Edition
		{
			typeStr = "Grizzl-E Avalanche";
		}
		x0 = 2;
		//HPDF_Page_SetFontAndSize(pdf_page, font, 9);
		draw_text(pdf_page, typeStr.c_str(), x0, hp - y0);
	}
	if (!par->pWoi->param1().empty()) {
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 10;
		draw_text(pdf_page, par->pWoi->param1().c_str(), x0, hp - y0);
	}
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white

	font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	y0 = Y0;
	if (par->pWoi->certflag() & 1) {
		//draw UL logo
		tmp_str = "cULLogo.jpg";
		draw_image(pdf, tmp_str.c_str(), 0, hp - y0, wp / 2 - 10, 32, WEB_LABEL_IMG_DIR);

		x0 = wp / 2 - 5;
		y0 = Y0 - /*16*/24;
		HPDF_Page_SetFontAndSize(pdf_page, font, 7);
		//tmp_str = "File number";
		//draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);
		////HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		//y0 += 10;
		tmp_str = "[E510712]";//"[UL Control#]";
		draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);

		y0 = Y0 - 16;
		HPDF_Page_SetFontAndSize(pdf_page, font, 6);
		tmp_str = "EV Supply ";//"[UL Control#]";
		draw_text(pdf_page, tmp_str.c_str(), x0 - 2, hp - y0);

		y0 = Y0 - 16 + 7;
		HPDF_Page_SetFontAndSize(pdf_page, font, 6);
		tmp_str = "Equipment/Charger";//"[UL Control#]";
		draw_text(pdf_page, tmp_str.c_str(), x0 - 2, hp - y0);

	}
	//resrore black color
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white


	y0 = Y0 + /*26*/20;
	//draw FCC logo
	tmp_str = "FCC.jpg";
	draw_image(pdf, tmp_str.c_str(), 2, hp - y0, 16, 16, WEB_LABEL_IMG_DIR);

	font = HPDF_GetFont(pdf, "Helvetica", NULL);

	HPDF_Page_SetFontAndSize(pdf_page, font, 7);
	bool green_cert = par->pWoi->certflag() & 2;
	if (green_cert)
	{
		y0 = Y0 + 12 - 4;
		tmp_str = "FCC ID:";
		draw_text(pdf_page, tmp_str.c_str(), wp / 3-14, hp - y0);
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 8;
		tmp_str = "2ABPY-5B5BD9";
		draw_text(pdf_page, tmp_str.c_str(), wp / 3-14, hp - y0);
	}

	if (green_cert) {
		//draw Energy star logo
		y0 = Y0 + 32/*42*/;
		tmp_str = "energystar_logo.jpg";
		draw_image(pdf, tmp_str.c_str(), wp / 2 + 20, hp - y0, 30, 30, WEB_LABEL_IMG_DIR);
	}
	else {
		y0 = Y0 + 12 - 4;
		tmp_str = "Contains FCC ID:";
		draw_text(pdf_page, tmp_str.c_str(), wp / 3, hp - y0);
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 8;
		tmp_str = "2ABPY-5B5BD9";
		draw_text(pdf_page, tmp_str.c_str(), wp / 3, hp - y0);
	}


	// draw QR code
	tmp_str = sN;
	if (tmp_str.empty()) {
		tmp_str = "blank.jpg";
	}
	else {
		tmp_str += ".jpg";
	}
	if (par->pWoi->objb().size() < 10)  // we do not have QR code as a blob
	{
		draw_image(pdf, tmp_str.c_str(), 0, 28, 32, 32, WEB_QRCODE_IMG_DIR);
	}
	else {
		draw_image_from_blob(pdf, par->pWoi->objb(), 0, 28, 32, 32);
	}

	// ==serial number
#if 1
	font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font, 8);
	y0 = 30 + 32 + 8;
	x0 = 4;
	tmp_str = "Serial Number";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	y0 -= 10;


	tmp_str = "[" + sN + "]";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);
#endif





	//footer
	font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white
	x0 = 40;
	y0 = (28 + 16);
	tmp_str = "Type 4X Enclusure";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	//M.F.G: YYYY/MM/DD
	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	y0 -= 8;
	font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
	int64_t ts = par->pWoi->createdts();
	string tsStr = DateTime::DateStrFromMillis_1(ts);
	tmp_str = "MFG:" + tsStr;
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white




	font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
	y0 = 28;//================
	x0 = 1;
	y0 -= 7;
	tmp_str = "In/Out 208-240VAC, 60Hz, 40A Max";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	x0 = 2;
	y0 -= 7;
	tmp_str = "Temperature rating:";// -22F to 122F(-30C to 50C)";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	y0 -= 7;
	tmp_str = "-22F to 122F (-30C to 50C)";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

}
//===========================================================================================================
void add_uc_UL_Classic_label_report_content(int64_t date_ts, PdfCSReportParams *par, HPDF_Doc pdf, HPDF_Page pdf_page)
{

	//HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);

	HPDF_REAL hp = HPDF_Page_GetHeight(pdf_page), wp = HPDF_Page_GetWidth(pdf_page);  // 120 x 280

	string tmp_str = "";


	//**********************Hos header************************************
	string sN = "";
	int cert_flag = 0;
	if (par->pWoi)
	{
		sN = par->uid;//par->pWoi->locationname();
		cert_flag = par->pWoi->certflag();
		/*if (!cert.empty())
		{
			try {
				cert_flag = std::stoi(cert);
			}catch(std::exception &ex)
			{ }
		}*/
	}


	HPDF_Page_SetFontAndSize(pdf_page, font, 8);

	int x0 = 2, y0 = 9, Y0 = 56;
	int type = par->pWoi->type();
	type--;
	//printf("UL type %d \r\n",type);
	if (type >= 0 && type <= 6)
	{
		std::string typeStr = str_charger_type[type];
		if (type == 3)   //Grizzl-E Avalanche Edition
		{
			typeStr = "Grizzl-E Avalanche";
		}
		x0 = 2;
		//HPDF_Page_SetFontAndSize(pdf_page, font, 9);
		draw_text(pdf_page, typeStr.c_str(), x0, hp - y0);
	}
	if (!par->pWoi->param1().empty()) {
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 10;
		draw_text(pdf_page, par->pWoi->param1().c_str(), x0, hp - y0);
	}
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white

	font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	y0 = Y0;
	if (par->pWoi->certflag() & 1) {
		//draw UL logo
		tmp_str = "cULLogo.jpg";
		draw_image(pdf, tmp_str.c_str(), 0, hp - y0, wp / 2 - 10, 32, WEB_LABEL_IMG_DIR);

		x0 = wp / 2 - 5;
		y0 = Y0 - /*16*/24;
		HPDF_Page_SetFontAndSize(pdf_page, font, 7);
		//tmp_str = "File number";
		//draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);
		////HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		//y0 += 10;
		tmp_str = "[E510712]";//"[UL Control#]";
		draw_text(pdf_page, tmp_str.c_str(), x0, hp - y0);

		y0 = Y0 - 16;
		HPDF_Page_SetFontAndSize(pdf_page, font, 6);
		tmp_str = "EV Supply ";//"[UL Control#]";
		draw_text(pdf_page, tmp_str.c_str(), x0-2, hp - y0);

		y0 = Y0 - 16+7;
		HPDF_Page_SetFontAndSize(pdf_page, font, 6);
		tmp_str = "Equipment/Charger";//"[UL Control#]";
		draw_text(pdf_page, tmp_str.c_str(), x0-2, hp - y0);

	}
	//resrore black color
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white


	y0 = Y0 + /*26*/20;
	//draw FCC logo
	tmp_str = "FCC.jpg";
	draw_image(pdf, tmp_str.c_str(), 4, hp - y0, 16, 16, WEB_LABEL_IMG_DIR);

	font = HPDF_GetFont(pdf, "Helvetica", NULL);

	HPDF_Page_SetFontAndSize(pdf_page, font, 7);
	bool green_cert = par->pWoi->certflag() & 2;
	if (green_cert)
	{
		y0 += 10;
		tmp_str = "Contains FCC ID:";
		draw_text(pdf_page, tmp_str.c_str(), 4, hp - y0);
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 8;
		tmp_str = "2ABPY-5B5BD9";
		draw_text(pdf_page, tmp_str.c_str(), 4, hp - y0);
	}

	if (green_cert) {
		//draw Energy star logo
		y0 = Y0 + 42;
		tmp_str = "ESlogo1.jpg";
		draw_image(pdf, tmp_str.c_str(), wp / 2 + 10, hp - y0, 32, 32, WEB_LABEL_IMG_DIR);
	}
	else {
		y0 = Y0 + 12 -4;
		tmp_str = "Contains FCC ID:";
		draw_text(pdf_page, tmp_str.c_str(), wp/3, hp - y0);
		//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
		y0 += 8;
		tmp_str = "2ABPY-5B5BD9";
		draw_text(pdf_page, tmp_str.c_str(), wp/3, hp - y0);
	}


	// draw QR code
	tmp_str = sN;
	if (tmp_str.empty()) {
		tmp_str = "blank.jpg";
	}
	else {
		tmp_str += ".jpg";
	}
	if (par->pWoi->objb().size() < 10)  // we do not have QR code as a blob
	{
		draw_image(pdf, tmp_str.c_str(), 0, 28, 32, 32, WEB_QRCODE_IMG_DIR);
	}
	else {
		draw_image_from_blob(pdf, par->pWoi->objb(), 0, 28, 32, 32);
	}

	// ==serial number
#if 1
	font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font, 8);
	y0 = 30+32+8;
	x0 = 4;
	tmp_str = "Serial Number";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	y0 -= 10;


	tmp_str = "[" + sN + "]";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);
#endif





	//footer
	font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white
	x0 = 40;
	y0 = (28+16);
	tmp_str = "Type 4X Enclusure";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	//M.F.G: YYYY/MM/DD
	//HPDF_Page_SetRGBFill(pdf_page, 1.0, 0, 0);
	y0 -= 8;
	font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
	int64_t ts = par->pWoi->createdts();
	string tsStr = DateTime::DateStrFromMillis_1(ts);
	tmp_str = "MFG:" + tsStr;
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	HPDF_Page_SetRGBFill(pdf_page, 0.0, 0.0, 0.0); // black   -> 1,1,1 white
	



	font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Page_SetFontAndSize(pdf_page, font, 6);
	y0 = 28;//================
	x0 = 1;
	y0 -= 7;
	tmp_str = "In/Out 208-240VAC, 60Hz, 40A Max";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	x0 = 2;
	y0 -= 7;
	tmp_str = "Temperature rating:";// -22F to 122F(-30C to 50C)";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

	y0 -= 7;
	tmp_str = "-22F to 122F (-30C to 50C)";
	draw_text(pdf_page, tmp_str.c_str(), x0, y0);

}
int main_uc_UL_Classic_label_pdf_report(PdfCSReport_T *rr)
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
	//HPDF_Page pdf_page = add_a4page(pdf, 120 - 8, 240 - 8);    // 1.56x3.22in    =  1.56x25.4 = 39.624 x 81.788  mm  
	HPDF_Page pdf_page = add_a4page(pdf, 100, 156);    //1.39x2.17in should be    36x55mm   
	if(rr->par->pWoi->certflag()==1)
		add_uc_UL_Classic_label_report_content(rr->par->from_ts, rr->par, pdf, pdf_page);
	else if (rr->par->pWoi->certflag() == 3)
		add_uc_UL_Classic_label_report_content_with_energy_star(rr->par->from_ts, rr->par, pdf, pdf_page);
	stroke_page_result_to((PdfReportBase_T*)rr, pdf);

	//m_printf("main_hos_pdf_report ..DONE\r\n");

	return (EXIT_SUCCESS);
}