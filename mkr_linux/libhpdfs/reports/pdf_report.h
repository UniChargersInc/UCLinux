#ifndef PDF_REPORT_H
#define PDF_REPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !(defined _WIN32 || defined __WIN32__)
#include <unistd.h>
#endif
#include <math.h>
#include <setjmp.h>
#include <hpdf.h>
#include <time.h>
#if !(defined _WIN32 || defined __WIN32__)
#include <sys/utsname.h>
#endif

// These two includes should always be used
#include "hpdf_table.h"
#include "hpdf_errstr.h"

#include "udorx_server_config.h"


// The output after running the program will be written to this file

/*
#ifdef _WIN32
#define OUTPUT_FILE "hpdf_table.pdf"
#define WEB_VEH_IMG_DIR	"C:\\My\\Linux\\www\\mkr_files\\common\\v_img"
#define WEB_RES_IMG_DIR "C:\\My\\Linux\\www\\mkr_files\\common\\resImg"
#else
#define OUTPUT_FILE "/home/mkr/service/reports/hpdf_table.pdf"
#define WEB_VEH_IMG_DIR	"/home/mkr/www/mkr_files/common/v_img"
#define WEB_RES_IMG_DIR	"/home/mkr/www/mkr_files/common/resImg"
#endif
*/


#define TRUE 1
#define FALSE 0

//-------------proto definitions----------------------------
#include "HosObjects/Definitions.h"
#include "HosObjects/TimeUtil.h"
#include "MkrLib.h"
#include "./CommonUtils/my_time.h"
#include "./CommonUtils/my_utils.h"

using namespace HosEngine;
using namespace udorx_pbf;

//--------------------------------------------------------



// Utility macro to create a HPDF color constant from integer RGB values
#ifdef	__cplusplus
#define _TO_HPDF_RGB(r,g,b) {r/255.0f,g/255.0f,b/255.0f}
#else
#define _TO_HPDF_RGB(r,g,b) (HPDF_RGBColor){r/255.0f,g/255.0f,b/255.0f}
#endif

typedef struct {
	float x, y;
	float width, height;
}MyRect_T;
typedef struct
{
	float x, y;
}Point_T;


#if 0
typedef unordered_map<int64_t, PTlrList*>  hos_map_t;
typedef unordered_map<int64_t, PIRptList*>  insp_map_t;
#endif
enum class ReportType
{
	NONE=0,
	HOS=1,
	INSPECTION=2,
	CHARGING_STATION
};
#if 0
struct PdfReportParams {
	ReportType rtype;
	int cid,  rid;
	int64_t from_ts, to_ts;  // date timestamp range
	string options;          //e.g. "1,1,1"  -> include HOS LogSheets, PreTRip, PostTrip inspections  
	string vid, uid;

	
	PVehicle * (*get_veh_info_cb)(int cid, string vid);
	PICatList*  (*get_cats_info_cb)(string vid);

	PEmpl *emp = 0;
	PVehicle *veh = 0;
	PICatList* cat_list = 0;

	void * report_data=0;  //HOS - PTlrList*  , Inspection - PIRpt

	PdfReportParams() {
		rtype = ReportType::NONE;
		emp = 0;
		get_veh_info_cb = 0;
		get_cats_info_cb;
	}
	/*~PdfReportParams()
	{
		if (emp)
		{
			delete emp;
			emp = 0;
		}
	}*/
	void setReportData(void* list) {
		report_data = list;
	}
	void *getReportData()
	{
		return report_data;
	}
};
#endif

struct PdfReportBase_T
{
	unsigned int buf_len;
	unsigned char *buf;

	const char *filename;
	bool save_to_file;
	PdfReportBase_T() {
		save_to_file = false;
		filename = 0;// OUTPUT_FILE;
		buf_len = 0;
		buf = 0;
	}
	virtual ~PdfReportBase_T()
	{
		if (buf != 0)
			delete[] buf;

	}
	virtual void setReportData(void* data) = 0;
};
#if 0
struct PdfReport_T : public PdfReportBase_T
{
	PdfReportParams *par;

	PdfReport_T(PdfReportParams *p)
	{
		par = p;
	}
	virtual ~PdfReport_T()
	{
		/*if(buf!=0)
			delete[] buf;*/
		
	}
	virtual void setReportData(void* data) {}
};

class CombinedReport_T
{
public:
	PdfReportParams *par;
	hos_map_t hos_map_list;
	/* insp_map_t  insp_map_list; */
	PIRptList* insp_list=0;
	
	//const char *filename=0;
	string rpt_filename = "";
	
	CombinedReport_T(PdfReportParams *par)
	{
		this->par = par;
	}
	
};
#endif

struct PdfCSReportParams {
	ReportType rtype;
	int cid, rid;
	int64_t from_ts, to_ts;  // date timestamp range
	string options;          //e.g. "1,1,1"  -> include HOS LogSheets, PreTRip, PostTrip inspections  
	string vid, uid;


	PCSItem  *pWoi = 0;

	void * report_data = 0;  //HOS - PTlrList*  , Inspection - PIRpt

	PdfCSReportParams() {
		rtype = ReportType::NONE;
		pWoi = 0;
	}
	
};
struct PdfCSReport_T : public PdfReportBase_T
{
	PdfCSReportParams *par;
	
	PdfCSReport_T(PdfCSReportParams *p)
	{
		par = p;
	}
	virtual ~PdfCSReport_T()
	{
		/*if (buf != 0)
			delete[] buf;*/

	}
	virtual void setReportData(void* data) {}
};


//
//svk
#ifdef	__cplusplus
extern "C" {
#endif

	
	/*int main_insp_pdf_report(PdfReport_T *rr);
	int main_hos_pdf_report(PdfReport_T *rr);
	int main_combined_pdf_report(CombinedReport_T *rr);*/

	void stroke_page_result_to(PdfReportBase_T *rr, HPDF_Doc pdf );
	void stroke_page_result_to_file(const char *filename, HPDF_Doc pdf);

	/*void my_hos_tbl2(int64_t date_ts,PdfReportParams *par,  HPDF_Doc pdf_doc,HPDF_Page pdf_page, int yPos=0);
	void drawHosGraph(HPDF_Doc pdf_doc,HPDF_Page pdf_page, PTlrList *tlr_list,float x, float y, float width, float height);*/

	void draw_image(HPDF_Doc     pdf,
		const char  *filename,
		float        x,
		float        y,
		float width,
		float height,
		const char  *text);
	void draw_image_from_blob(HPDF_Doc     pdf,const std::string& imgBlob,float x,float y,float width,float height);
	void draw_line_text(HPDF_Page    page,
		float        x,
		float        y,
		const char  *label);

	void draw_text(HPDF_Page    page, const char  *label, float x, float y);
	void draw_line(HPDF_Page    page, float x0, float y0, float x1, float y1);

	void showCenteredTitle(HPDF_Page    page,  const char *title,int y,int x0=0);
	void draw_text_pair_underline(HPDF_Page    page, float x, float y,
		const char  *label, const char  *value, int min_width=0);


	//----------------------inspection----------------------
	//void my_insp_tbl_cats(PdfReportParams *par,HPDF_Doc pdf_doc, HPDF_Page pdf_page, int yPos = 0);
	//void my_insp_tbl_defects(PdfReportParams *par,HPDF_Doc pdf_doc, HPDF_Page pdf_page, int yPos = 0);

	void draw_signature(HPDF_Page    page, const short *p, int len, MyRect_T &rect);

#ifdef	__cplusplus
}

#endif

#endif

