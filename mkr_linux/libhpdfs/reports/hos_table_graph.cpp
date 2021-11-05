
#include "pdf_report.h"
#include <stdint.h>  // for int64_t

//extern HPDF_Page pdf_page;
static bool hos_list_contains_midnight_auto_event = false;
//-----------------------svk-----------------------------
static const char *hdr_content[3] =
{
	"Time",
	"Event",
	"Address"
};

static string my_str_in_cb = "";
static char * hpdf_table_content_callback_hos(void *table_tag, size_t r, size_t c)
{
	
	PdfReportParams *par = (PdfReportParams *)table_tag;
	if (!table_tag) return 0;
	PTlrList *tlr_list = (PTlrList *)par->getReportData();
	if (tlr_list == 0)
		return 0;
	if (r == 0) // last row is a header !!!
		return 0;
	int index = hos_list_contains_midnight_auto_event ? r: r - 1;
	if (!(index >= 0 && index < tlr_list->list_size()))
		return 0;
	PTlr *tlr = 0;
	if (index >= 0 && index < tlr_list->list_size())
	{
		tlr = tlr_list->mutable_list(index);
	}
	if (tlr == 0)
		return 0;
	switch (c)
	{
	case 0:
	{
		my_str_in_cb = DateTime::TimeHMStringFromMillis(tlr->logtime());
	}
		break;
	case 1:
	{
		int v = tlr->event();
		switch (v)
		{
			case 101:
				my_str_in_cb = "OFF DUTY";
				break;
			case 102:
				my_str_in_cb = "SLEEPING";
				break;
			case 103:
				my_str_in_cb = "DRIVING";
				break;
			case 104:
				my_str_in_cb = "ON DUTY";
				break;
		}
	}
	break;
	case 2:
	{
		my_str_in_cb = tlr->addr();
		if (my_str_in_cb.empty())
		{
			my_str_in_cb = "> " + std::to_string(tlr->lat()) + " : " + std::to_string(tlr->lon());
		}
	}
	break;
	}
	return (char*)my_str_in_cb.c_str();
}
static bool table_content_style_cb(void *ttag, size_t r, size_t c, hpdf_text_style_t *content_style)
{
	if (c == 0 || c == 1)
	{
		content_style->halign = (hpdf_table_text_align)CENTER;  //does not help -> TODO
		return true;
	}
	return false;
}
//
void my_hos_tbl2(int64_t date_ts,PdfReportParams *par,  HPDF_Doc pdf_doc, HPDF_Page pdf_page, int yPos)
{
	PTlrList *tlr_list = (PTlrList *)par->getReportData();
	if (tlr_list == 0)
		return;
	hos_list_contains_midnight_auto_event = false;
	int size = 0;
	if (tlr_list)
		size = tlr_list->list_size();
	if (size == 0)
		return;
	int num_rows = size + 1;
	PTlr *tlr = tlr_list->mutable_list(0);
	if(tlr->type()== (int)TimeLogType::Auto)
	{
		hos_list_contains_midnight_auto_event = true;
		num_rows--;  // DO NOT SHOW Auto Event !!!
	}
	
	int num_cols = 3;
	string table_title="HOS Events for "+ DateTime::DateStrFromMillis(/*par->from_ts*/ date_ts);
	hpdf_table_t t = hpdf_table_create(num_rows, num_cols,(char*) table_title.c_str());

	// Use a red title and center the text
	HPDF_RGBColor color = { 0.6f,0,0 };
	HPDF_RGBColor background = { 0.9f,1.0f,0.9f };
	hpdf_table_set_title_style(t, HPDF_FF_HELVETICA_BOLD, 14, color, background);
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
	hpdf_table_set_colwidth_percent(t, 1, 20);
	hpdf_table_set_colwidth_percent(t, 2, 60);

	hpdf_table_set_header_content(t,(char**) hdr_content);
	hpdf_table_set_tag(t, par);
	hpdf_table_set_content_callback(t, hpdf_table_content_callback_hos);
	hpdf_table_set_content_style_callback(t, table_content_style_cb);
	//hpdf_table_set_content(t, content);
	//hpdf_table_set_labels(t, labels);


	HPDF_REAL xpos = 40;// 100;
	HPDF_REAL ypos = yPos==0?630:yPos;
	HPDF_REAL width = 500;
	HPDF_REAL height = 0; // Calculate height automatically
	hpdf_table_stroke(pdf_doc, pdf_page, t, xpos, ypos, width, height);

	//svk
	hpdf_table_destroy(t);
}


//--------------------------Graphic-----------------------------------------
//--------------------------------------------------------------------------
//typedef long long int64_t;

//typedef struct {
//	int64_t logtime;
//	int ev;
//
//}TimeLog;
////1478723660 sec => 11/09/2016 @ 8:34pm (UTC) 
//TimeLog g_timeLogList[]=
//{
//	{
//		(int64_t)(1478723660 - 3600 * 4) * 1000,
//		104
//	},
//	{
//		(int64_t)(1478723660-3600*2) * 1000,
//		102
//	},
//	{
//		(int64_t)1478723660*1000,  // ts in millis
//		103
//	}
//
//};


void initTotalHours();
void UpdateTotalHours(int ts, int ev);
void UpdateTotalHours(int ts, int ev);
int64_t get_extend_current_date(/*int64_t ts*/PTlr *tlr);

float OriginY = 0;
static void m_draw_text(HPDF_Page    page, const char  *label, float x, float y)
{
	//float hp = HPDF_Page_GetHeight(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x, OriginY-y);
	HPDF_Page_ShowText(page, label);
	HPDF_Page_EndText(page);
}
static void m_draw_line(HPDF_Page    page, float x0, float y0, float x1, float y1)
{
	//float hp = HPDF_Page_GetHeight(page);

	HPDF_Page_MoveTo(page, x0, OriginY -y0);
	HPDF_Page_LineTo(page, x1, OriginY -y1);
	HPDF_Page_Stroke(page);
}


static int mWidth, mHeight;
static int LeftOffset = 24, TopOffset = 30, RightOffset = 30, BottomOffset = 2;
static float gw, gh, gLeft, gTop, gRight, segmentHeight;
static float _gridunit = 0;
static int dutyStatusCount = 4;
static char *header[24] = { " M", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", " N", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11"};
static char *gutter[4] = { "OF", "SB", "D ", "ON" };

static int headerTimeSlide = 0, bottomTimeSlide = 0;

/*int64_t*/ int totaloff = 0, totalon = 0, totaldrv = 0, totalslp = 0;

static void initialize(float x, float y, float width, float height)
{

	mWidth = width;
	mHeight = height;
	mHeight -= (headerTimeSlide + bottomTimeSlide);
	gw = (float)(mWidth - (LeftOffset + RightOffset));
	gh = (float)(mHeight - (TopOffset + BottomOffset));
	gLeft = (float)x + LeftOffset;
	OriginY = y + height;
	gTop = /*(float)y +*/ TopOffset;
	segmentHeight = gh / dutyStatusCount;

	segmentHeight = gh / dutyStatusCount;

}
static void drawGrid(HPDF_Doc pdf_doc,HPDF_Page page,float x,float y,float width,float height)
{
	HPDF_Font font;
	font = HPDF_GetFont(pdf_doc, "Helvetica", NULL);
	HPDF_Page_SetFontAndSize(page, font, 12);
	HPDF_Page_SetLineWidth(page, 1);
	HPDF_Page_SetRGBStroke(page, 0, 0, 0);
	HPDF_Page_SetRGBFill(page, 0.25, 0.25, 0.75);

	//HPDF_Page_Rectangle(page, x, y, width, height);
	//HPDF_Page_Stroke(page);
	for (int i = 0; i <= 4; i++)
	{
		m_draw_line(page, gLeft, gTop + (float)(i * segmentHeight), gLeft + gw, gTop + (float)(i * segmentHeight));
	}


	/*HPDF_Page_SetLineWidth(page, 0.5);
	float yc, d_y = height / 4;
	for (int i = 1; i < 3; i++)
	{
		yc = y + d_y*i;
		m_draw_line(page, x, yc, x + width, yc);
	}*/

	// draw the tick lines
	float tickXdelta = gw / 96;
	for (int i = 0; i <= 96; i++)
	{
		float tickx = gLeft + (i * tickXdelta);
		float tickheight = segmentHeight / 4;
		float delta = 0;
		if (i % 2 == 0)
			tickheight = segmentHeight / 2;
		if (i % 4 == 0) {
			tickheight = segmentHeight;
			delta = 4;
			// draw header text
			int j = i / 4;
			if (j<24/*[header count]*/)
			{
				char *str = header[j];
				int x = (int)(tickx - LeftOffset / 3);
				int y = (int)(gTop - TopOffset / 3);
				 m_draw_text(page,str, x, y);
			}
		}

		m_draw_line(page, tickx,  gTop - delta,   tickx,  gTop + tickheight );
		m_draw_line(page,  tickx,  gTop + segmentHeight,  tickx,  gTop + segmentHeight + tickheight );
		m_draw_line(page,  tickx,  gTop + segmentHeight * 3,  tickx,  gTop + segmentHeight * 3 - tickheight );
		m_draw_line(page,  tickx,  gTop + segmentHeight * 4 + delta,  tickx,  gTop + segmentHeight * 4 - tickheight );

	}
	font = HPDF_GetFont(pdf_doc, "Helvetica-Bold", NULL);
	HPDF_Page_SetFontAndSize(page, font, 14);
	// draw lables
	m_draw_text(page, gutter[0], gLeft - LeftOffset + 2, gTop + BottomOffset + segmentHeight / 2 );
	m_draw_text(page, gutter[1], gLeft - LeftOffset + 2, gTop + BottomOffset + segmentHeight * 3 / 2 );
	m_draw_text(page, gutter[2], gLeft - LeftOffset + 2,gTop + BottomOffset + segmentHeight * 5 / 2 );
	m_draw_text(page, gutter[3], gLeft - LeftOffset + 2, gTop + BottomOffset + segmentHeight * 7 / 2 );
	m_draw_text(page," TOT",gw+gLeft, (gTop-TopOffset/3) );

}
int fake_GetMinutesFromMidNight(int64_t ts)
{
	/*int t = 1478723660;// current time in  sec = > 11 / 09 / 2016 @ 8:34pm(UTC)
	int rv = t % (24*3600);
	rv = t - rv;  //result is 1478649600 -> 11/09/2016 @ 12:00am (UTC)
	return rv; */

	int rv=ts / 1000;
	rv = rv % (24 * 3600); // seconds since midnight
	rv /= 60;
	return rv;
}
void DrawLog(HPDF_Page  pdf_page, PTlrList *tlrl,int size=0)
{
	initTotalHours();
	//grid pixel size per minute gWidth/(24*60)
	float gridunit = gw / 1440;
	_gridunit = 1440 / gw;
	size = tlrl->list_size();
	if(size == 0)
		return;
	float x1, x2, y1, y2;
	PTlr *tlr0 = tlrl->mutable_list(0);

	x1 = gLeft+ fake_GetMinutesFromMidNight(tlr0->logtime()) * gridunit;// + (DateTime::GetMinutesFromMidNight(tlr0->logtime())* gridunit);
	y1 = gTop + (tlr0->event() - 101) * segmentHeight + segmentHeight / 2;
	//int size = logdata->list_size();
	//boolean extend_current_date=false;
	int64_t ts = 0, ts0 = 0, extend_current_date = 0;
	HPDF_Page_SetRGBStroke(pdf_page, 0, 0, 1.0);
	HPDF_Page_SetLineWidth(pdf_page, 2);
	if (size == 1)
	{
		extend_current_date = get_extend_current_date(tlr0);
	}
	else
		for (int i = 1; i < size; i++)
		{
			PTlr *tlr = tlrl->mutable_list(i);
			long TotalMinutes = 0;
			ts = tlr->logtime();
			ts0 = tlr0->logtime();
			if (i == (size - 1))
			{
				extend_current_date = get_extend_current_date(tlr);
			}
			TotalMinutes = (ts - ts0) / (60 * 1000);
			UpdateTotalHours ((int) /*(ts - ts0)*/TotalMinutes,  tlr0->event());

			x2 = x1 + (float)(TotalMinutes * gridunit);
			y2 = gTop + (tlr->event() - 101) * segmentHeight + segmentHeight / 2;

			//horizantal log line

			// CGContextSetLineWidth(gc,4);
			m_draw_line(pdf_page, x1, y1,   x2,  y1);

			//vertical log line
			m_draw_line(pdf_page,  x2, y1, x2, y2);

			/*
			if (edit_flag&&i==1)
			{
			DrawDriverStatus.drawEdit (g,(int)x2, (int)y1, 16, UIColor.Red);
			}*/
			x1 = x2;
			y1 = y2;
			tlr0 = tlr;
		}
	//last correction
	if (extend_current_date>0)
	{
		ts0 = tlr0->logtime();
		long TotalMinutes = (extend_current_date - ts0) / (60 * 1000);
		UpdateTotalHours(TotalMinutes, tlr0->event() );
		x2 = x1 + (float)(TotalMinutes * gridunit);
		y2 = gTop + (tlr0->event() - 101) * segmentHeight + segmentHeight / 2;
		//horizantal log line

		m_draw_line(pdf_page, x1 , y1  , x2 , y1 );
	}

}
int64_t get_extend_current_date(/*int64_t ts*/PTlr *tlr)
{
	int64_t ts = tlr->logtime();
	int64_t extend_current_date = 0;

	DateTime now = TimeUtil::DateTime_Now(tlr->tz());
	int64_t cur = now.getMillis();

	if (cur>ts)
	{
		int64_t cur_dd = now.Date().getMillis();
		DateTime dt1(ts);
		int64_t ts_dd = dt1.Date().getMillis();
		if (cur_dd == ts_dd)
			extend_current_date = cur;
		else {
			ts_dd = ts_dd + 24 * 3600 * 1000;    //DateUtil.addDays(ts_dd, 1);
			extend_current_date = ts_dd;
		}
	}
	return extend_current_date;
}

void initTotalHours()
{
	totaloff = 0;
	totalon = 0;
	totaldrv = 0;
	totalslp = 0;
}
void UpdateTotalHours(int ts, int ev)
{
	switch (ev)
	{
	case 101/*LOGSTATUS.OffDuty*/:
		totaloff += ts;
		break;
	case 104/*LOGSTATUS.OnDuty*/:
		totalon += ts;
		break;
	case 103/*LOGSTATUS.Driving*/:
		totaldrv += ts;
		break;
	case 102/*LOGSTATUS.Sleeping*/:
		totalslp += ts;
		break;
	}
}
static void _do_hour_min(int v,char *buf)
{
	int h = v / 60;
	int m = v - h * 60;
	sprintf(buf, "%02d:%02d", v / 60, m);
}
void DrawTotalHours(HPDF_Page page)
{
	char buf[32] = { 0 };
	
	float x = gLeft + gw +4;
	_do_hour_min(totaloff, buf);
	m_draw_text(page, buf,  x,  gTop + BottomOffset + segmentHeight / 2);
	_do_hour_min(totalslp, buf); //sprintf(buf, "%d:%d", totalslp/60, totalslp );
	m_draw_text(page, buf, x, gTop + BottomOffset + segmentHeight * 3 / 2);
	_do_hour_min(totaldrv, buf);//sprintf(buf, "%d:%d", totaldrv/60, totaldrv );
	m_draw_text(page, buf, x, gTop + BottomOffset + segmentHeight * 5 / 2);
	_do_hour_min(totalon, buf);//sprintf(buf, "%d:%d", totalon/60, totalon );
	m_draw_text(page, buf, x, gTop + BottomOffset + segmentHeight * 7 / 2);
}

void drawHosGraph(HPDF_Doc pdf_doc,HPDF_Page pdf_page, PTlrList *tlrl,float x,float y, float width, float height)
{
	initialize(x, y, width, height);
	drawGrid(pdf_doc,pdf_page,x, y, width, height);
	DrawLog(pdf_page,   tlrl);
	DrawTotalHours(pdf_page);
}