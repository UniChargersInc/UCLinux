
#include "pdf_report.h"
#include <stdint.h>  // for int64_t
extern HPDF_Doc gPdfDoc;
//extern HPDF_Page pdf_page;
#include "CommonUtils/my_utils.h"

//-----------------------svk-----------------------------
static const char *hdr_cats_content[3] =
{
	"ID",  //"CategoryID",
	"Category description",
	"Test"
};
static const char *hdr_defects_content[3] =
{
	"CategoryID",
	"Defect description",
	"Comments"
};

static string my_str_in_cb = "";
static char * hpdf_table_content_callback_insp_cats(void *table_tag, size_t r, size_t c)
{

	PdfReportParams *par = (PdfReportParams *)table_tag;
	if (!par) return 0;
	PICatList *list = par->cat_list;
	if (!list) return 0;

	if (r == 0) // last row is a header !!!
		return 0;
	int index = r - 1;

	PICat *icr = 0;
	if (index >= 0 && index < list->list_size())
	{
		icr = list->mutable_list(index);
	}

	switch (c)
	{
	case 0:
		my_str_in_cb = Int2Str(icr->cid());
		break;
	case 1:
	{
		
		
		if (icr)
		{
			my_str_in_cb=icr->desc();
		}

	}
		break;
	case 2:
	{
		my_str_in_cb = "Passed";
		PIRpt *ir = (PIRpt *)par->getReportData();
		PIItem *ii = 0;
		int size = ir->defects_size();
		if (icr&&size>0)
		{
			for (int i = 0; i < size; i++)
			{
				PIItem *ii = ir->mutable_defects(i);
				if (ii&&ii->cid() == icr->cid())
				{
					my_str_in_cb = "Failed";
				}
			}
		}	
	}
		break;
	}

	return (char*)my_str_in_cb.c_str();
}
static bool table_content_style_cb(void *ttag, size_t r, size_t c, hpdf_text_style_t *content_style)
{
	if (c == 0 || c == 2)
	{
		content_style->halign = (hpdf_table_text_align)CENTER;  //does not help -> TODO
		return true;
	}
	return false;
}
bool table_content_defects_style_cb(void *ttag, size_t r, size_t c, hpdf_text_style_t *content_style)
{
	if (c == 0 )
	{
		content_style->halign = (hpdf_table_text_align)CENTER;  //does not help -> TODO
		return true;
	}
	return false;
}
static char * hpdf_table_content_callback_insp_defects(void *table_tag, size_t r, size_t c)
{

	PdfReportParams *par = (PdfReportParams *)table_tag;
	if (!par) return 0;
	PIRpt *ir =(PIRpt *) par->getReportData();
	if (!ir) return 0;

	if (r == 0) // last row is a header !!!
		return 0;
	int index = r - 1;
	if (! (index >= 0 && index < ir->defects_size()) )
		return 0;
	PIItem *ii=ir->mutable_defects(index);
	if (ii == 0)
		return 0;
	switch (c)
	{
		case 0:
		{
			int catId=ii->cid();
			my_str_in_cb = Int2Str(catId);
		}
		break;
		case 1:
			my_str_in_cb = ii->desc();
			break;
		case 2:
			my_str_in_cb = ii->comments();
			break;
	}

	return (char*)my_str_in_cb.c_str();
}

void my_insp_tbl_cats(PdfReportParams *par,HPDF_Doc pdf_doc, HPDF_Page pdf_page,  int yPos)
{
	PICatList *list=par->cat_list;
	if (list == 0)
		return;
	int size = list->list_size();
	int num_rows = size + 1;  // +1 to include header !!
	int num_cols = 3;
	char *table_title = "Inspection categories";
	hpdf_table_t t = hpdf_table_create(num_rows, num_cols, table_title);

	// Use a red title and center the text
	HPDF_RGBColor color = { 0.6f,0,0 };
	HPDF_RGBColor background = { 0.9f,1.0f,0.9f };
	hpdf_table_set_title_style(t, HPDF_FF_HELVETICA_BOLD, 12, color, background);
	hpdf_table_set_title_halign(t, CENTER);


	// Use specially formatted header row
	hpdf_table_use_header(t, TRUE);

	// Use full grid and not just the short labelgrid
	hpdf_table_use_labelgrid(t, FALSE);

	// Use bold font for content. Use the C99 way to specify constant structure constants
#ifdef	__cplusplus
	hpdf_table_set_content_style(t, HPDF_FF_COURIER_BOLD, 10, { 0.1f, 0.1f, 0.1f }, { 1.0, 1.0, 0.9f });
#else
	hpdf_table_set_content_style(t, HPDF_FF_COURIER_BOLD, 10, (HPDF_RGBColor) { 0.1f, 0.1f, 0.1f }, (HPDF_RGBColor) { 1.0, 1.0, 0.9f });
#endif


	hpdf_table_set_colwidth_percent(t, 0, 10);
	hpdf_table_set_colwidth_percent(t, 1, 75);
	hpdf_table_set_colwidth_percent(t, 2, 15);

	hpdf_table_set_header_content(t, (char**)hdr_cats_content);

	hpdf_table_set_tag(t, par);
	hpdf_table_set_content_callback(t, hpdf_table_content_callback_insp_cats);
	hpdf_table_set_content_style_callback(t, table_content_style_cb);
	//hpdf_table_set_content(t, content);
	//hpdf_table_set_labels(t, labels);


	HPDF_REAL xpos = 40;// 100;
	HPDF_REAL ypos = yPos == 0 ? 630 : yPos;
	HPDF_REAL width = 400;
	HPDF_REAL height = 500; // Calculate height automatically
	hpdf_table_stroke(pdf_doc, pdf_page, t, xpos, ypos, width, height);


	//svk
	hpdf_table_destroy(t);

	
}



void my_insp_tbl_defects(PdfReportParams *par,HPDF_Doc pdf_doc, HPDF_Page pdf_page, int yPos)
{
	PIRpt *ir =(PIRpt *) par->getReportData();
	if (!ir) return ;
	int size = ir->defects_size();
	int num_rows = size + 1;  // +1 to include header !!
	int num_cols = 3;

	
	char *table_title = "Inspection defects";
	hpdf_table_t t = hpdf_table_create(num_rows, num_cols, table_title);

	// Use a red title and center the text
	HPDF_RGBColor color = { 0.6f,0,0 };
	HPDF_RGBColor background = { 0.9f,1.0f,0.9f };
	hpdf_table_set_title_style(t, HPDF_FF_HELVETICA_BOLD, 12, color, background);
	hpdf_table_set_title_halign(t, CENTER);


	// Use specially formatted header row
	hpdf_table_use_header(t, TRUE);

	// Use full grid and not just the short labelgrid
	hpdf_table_use_labelgrid(t, FALSE);

	// Use bold font for content. Use the C99 way to specify constant structure constants
#ifdef	__cplusplus
	hpdf_table_set_content_style(t, HPDF_FF_COURIER_BOLD, 10, { 0.1f, 0.1f, 0.1f }, { 1.0, 1.0, 0.9f });
#else
	hpdf_table_set_content_style(t, HPDF_FF_COURIER_BOLD, 10, (HPDF_RGBColor) { 0.1f, 0.1f, 0.1f }, (HPDF_RGBColor) { 1.0, 1.0, 0.9f });
#endif


	hpdf_table_set_colwidth_percent(t, 0, 20);
	hpdf_table_set_colwidth_percent(t, 1, 40);
	hpdf_table_set_colwidth_percent(t, 2, 40);

	hpdf_table_set_header_content(t, (char**)hdr_defects_content);

	hpdf_table_set_tag(t, par);
	hpdf_table_set_content_callback(t, hpdf_table_content_callback_insp_defects);
	hpdf_table_set_content_style_callback(t, table_content_defects_style_cb);
	//hpdf_table_set_content(t, content);
	//hpdf_table_set_labels(t, labels);


	HPDF_REAL xpos = 40;// 100;
	HPDF_REAL ypos = yPos == 0 ? 630 : yPos;
	HPDF_REAL width = 400;
	HPDF_REAL height = 0; // Calculate height automatically
	hpdf_table_stroke(pdf_doc, pdf_page, t, xpos, ypos, width, height);


	//svk
	hpdf_table_destroy(t);
}




