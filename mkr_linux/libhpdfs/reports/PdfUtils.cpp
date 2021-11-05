
#include "pdf_report.h"

//#include <iostream>   // to use std::string
//#include <sstream>
//#include <vector>
//using namespace std;
#include "CommonUtils/my_utils.h"

#define USE_BEZIER_CURVE	1
#if  USE_BEZIER_CURVE
//#include "bezier/bezier.h"
#endif
#include "bezier/bezier.h"

void draw_image(HPDF_Doc     pdf,
	const char  *filename,
	float        x,
	float        y,
	float width,
	float height,
	const char  *dir)
{

	char filename1[255];

	HPDF_Page page = HPDF_GetCurrentPage(pdf);
	HPDF_Image image;

	snprintf(filename1, 255, "%s%s", dir, filename);

	bool M_PathFileExists(char *path, bool remove_file_name);
	if (!M_PathFileExists((char*)filename1, false))
	{
		return;
	}

	image = HPDF_LoadJpegImageFromFile(pdf, filename1);

	/* Draw image to the canvas. */
	HPDF_Page_DrawImage(page, image, x, y, /*HPDF_Image_GetWidth(image),HPDF_Image_GetHeight(image)*/width,height);
}
void draw_image_from_blob(HPDF_Doc pdf, const std::string& imgBlob, float x, float y, float width, float height)
{
	HPDF_Page page = HPDF_GetCurrentPage(pdf);
	HPDF_Image image;

	image = HPDF_LoadJpegImageFromMem(pdf, (HPDF_BYTE*)imgBlob.c_str(),imgBlob.size());

	/* Draw image to the canvas. */
	HPDF_Page_DrawImage(page, image, x, y, /*HPDF_Image_GetWidth(image),HPDF_Image_GetHeight(image)*/width, height);
}

void draw_line_text(HPDF_Page    page,
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
	HPDF_Page_Stroke(page);
}
void draw_text_pair_underline(HPDF_Page    page,float x,float y,
	const char  *label, const char  *value, int min_width )
{
	HPDF_Page_SetLineWidth(page, 0.7);
	HPDF_Page_SetRGBStroke(page, 0, 0, 0);

	HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0); // change text color
	draw_text(page, label, x, y);
	float tw = HPDF_Page_TextWidth(page, label);
	x += tw + 6;

	//HPDF_Page_SetRGBFill(page, 0.0, 0.0, 1.0); // change text color
	float y1=y + 1;
	draw_text(page, value, x, y1);

	
	y1=y - 1;
	if(min_width==0)
	{
		tw = HPDF_Page_TextWidth(page, value);
		draw_line(page, x - 2, y1, x + tw + 6, y1);
	}
	else
	{
		draw_line(page, x - 2, y1, x + min_width, y1);
	}
}

void draw_text(HPDF_Page    page, const char  *label,float x,float y)
{
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, x, y);
	HPDF_Page_ShowText(page, label);
	HPDF_Page_EndText(page);
}
void draw_line(HPDF_Page    page, float x0, float y0, float x1, float y1)
{
	HPDF_Page_MoveTo(page, x0, y0);
	HPDF_Page_LineTo(page, x1, y1);
	HPDF_Page_Stroke(page);
}

void showCenteredTitle(HPDF_Page    page, const char *title,int y,int xOffset)
{
	
	float tw = HPDF_Page_TextWidth(page, title);
	HPDF_Page_BeginText(page);
	HPDF_Page_TextOut(page, (HPDF_Page_GetWidth(page) - tw) / 2 - xOffset,
		y, title);
	HPDF_Page_EndText(page);
}

//---------------------------------------------------------------
void stroke_page_result_to_file(const char *filename, HPDF_Doc pdf)
{
	char buf[128];
	snprintf(buf, 128, "%s%s", REPORT_GEN_FILES, filename);
	if (HPDF_OK != HPDF_SaveToFile(pdf, buf)) {
		fprintf(stderr, "ERROR: Cannot save to file!");
	}
}
// Stroke the generated PDF to a fill
void stroke_page_result_to(PdfReportBase_T *rr, HPDF_Doc pdf) {

	//HPDF_Doc gPdfDoc = getPdfDoc();
	if (rr->save_to_file)
	{
		stroke_page_result_to_file(rr->filename, pdf);
	}
	else
	{
		if (HPDF_OK != HPDF_SaveToStream(pdf)) {
			fprintf(stderr, "ERROR: Cannot save to stream!");
		}
		rr->buf_len = HPDF_GetStreamSize(pdf);
		if (rr->buf_len > 0)
		{
			rr->buf = new HPDF_BYTE[rr->buf_len];
			HPDF_STATUS sts = HPDF_ReadFromStream(pdf, rr->buf, &rr->buf_len);
			if (sts == HPDF_OK)
			{

			}
		}
	}
	HPDF_Free(pdf);
}

#if OLD_IMPL
int process_multiline_text_output(const hpdf_table_t t, hpdf_table_cell_t *cell, float xpos, char* content)
{
	//char* cnt = "Hey This is for test\r\nThe second line!!";
	int nRows = 2;
	if (cell->textwidth > cell->width * 2)
		nRows = 3;
	int sLen = strlen(content);
	int nCharsPerRow = sLen / nRows;
	char *buf = new char[nCharsPerRow + 1];//calloc(nCharsPerRow + 1, 1);


	float yOffset = 2;
	float d_y = cell->height / nRows;
	float yCellPos = t->posy + cell->delta_y;
	float y = yCellPos + yOffset + d_y*(nRows - 1);
	for (int i = 0; i < nRows; i++)
	{

		int bytesToCopy = sLen - i*nCharsPerRow;
		if (bytesToCopy > nCharsPerRow)
			bytesToCopy = nCharsPerRow;
		memset(buf, 0, nCharsPerRow + 1);
		memcpy(buf, (content + i*nCharsPerRow), bytesToCopy);

		hpdf_table_encoding_text_out(t->pdf_page, xpos, y, buf);

		y -= d_y;
	}

	delete[] buf;
	return 0;
}
#else
//void split(const std::string &s, char delim, std::vector<std::string> &elems) {
//	std::stringstream ss;
//	ss.str(s);
//	std::string item;
//	while (std::getline(ss, item, delim)) {
//		elems.push_back(item);
//	}
//}
//std::vector<std::string> split(const std::string &s, char delim) {
//	std::vector<std::string> elems;
//	split(s, delim, elems);
//	return elems;
//}
int process_multiline_text_output(const hpdf_table_t t, hpdf_table_cell_t *cell, float xpos, char* content)
{
	//char* cnt = "Hey This is for test\r\nThe second line!!";
	string str = string(content);
	int nRows = 2;
	int x = cell->textwidth / cell->width;
	int x1 = cell->textwidth - x*cell->width;
	if (x1 > 0)
		x += 1;
	nRows = x;

	float yOffset = 2;
	float d_y = cell->height / nRows;
	float yCellPos = t->posy + cell->delta_y;
	float y = yCellPos + yOffset + d_y*(nRows - 1);


	int nRowDrawn = 0;

	std::vector<std::string> v = split(str, ' ');
	if (v.size() > 1)
	{
		string s = v[0];
		float tw = HPDF_Page_TextWidth(t->pdf_page, s.c_str());
		for (int i = 1; i < v.size(); i++)
		{
			string si = v[i];
			float ti = HPDF_Page_TextWidth(t->pdf_page, si.c_str());
			if (tw >= cell->width)
			{
				//show this part
				if (nRowDrawn<nRows)
				{
					hpdf_table_encoding_text_out(t->pdf_page, xpos, y, (char*)s.c_str());
					nRowDrawn++;
				}
				y -= d_y;
				s = si;
				tw = ti;
			}
			else
			{
				if ((tw + ti) >= cell->width)
				{
					if (nRowDrawn<nRows)
					{
						hpdf_table_encoding_text_out(t->pdf_page, xpos, y, (char*)s.c_str());
						nRowDrawn++;
					}
					y -= d_y;
					s = si;
					tw = ti;
				}
				else
				{
					string s1 = s + " " + si;
					tw = HPDF_Page_TextWidth(t->pdf_page, s1.c_str());
					if (tw >= cell->width)
					{
						if (nRowDrawn<nRows)
						{
							hpdf_table_encoding_text_out(t->pdf_page, xpos, y, (char*)s.c_str());
							nRowDrawn++;
						}
						y -= d_y;
						s = si;
						tw = ti;
					}
					else
					{
						s = s1;
					}
				}
			}
		}//for (int i = 1; i < v.size(); i++)
		if (!s.empty())
		{
			if (nRowDrawn<nRows)
			{
				hpdf_table_encoding_text_out(t->pdf_page, xpos, y, (char*)s.c_str());
				nRowDrawn++;
			}
		}
	}
	else
	{
		int sLen = strlen(content);
		int nCharsPerRow = sLen / nRows;
		char *buf = new char[nCharsPerRow + 1];//calloc(nCharsPerRow + 1, 1);


		for (int i = 0; i < nRows; i++)
		{

			int bytesToCopy = sLen - i*nCharsPerRow;
			if (bytesToCopy > nCharsPerRow)
				bytesToCopy = nCharsPerRow;
			memset(buf, 0, nCharsPerRow + 1);
			memcpy(buf, (content + i*nCharsPerRow), bytesToCopy);
			hpdf_table_encoding_text_out(t->pdf_page, xpos, y, buf);
			y -= d_y;
		}

		delete[] buf;
	}
	return 0;
}
#endif

#if !USE_BEZIER_CURVE
void draw_signature(HPDF_Page    page, const short *p, int len, MyRect_T &rect)
{
	HPDF_Page_SetLineWidth(page, 1.2);


	if (len>0 && p != 0)
	{
		vector<BezPoint>  points;

		bool new_path = true;
		len = len / 4;

		float xMax = 0, yMax = 0, xScale = 1.0, yScale = 1.0;
		for (int i = 0; i < len; i++)
		{
			int x = (*p++);
			int y = (*p++);
			BezPoint p(x, y);
			points.push_back(p);

			if (xMax < x) xMax = x;
			if (yMax < y) yMax = y;

		}

		xScale = (rect.width * 1.0) / (xMax * 1.0);
		yScale = (rect.height * 1.0) / (yMax * 1.0);


		int path_count = 0;
		for (BezPoint &p1 : points)
		{
			BezPoint pnt(p1.x * xScale, p1.y*yScale);
			if (pnt.x < 0 && pnt.y < 0)
			{
				new_path = true;
				if(path_count>=2)
					HPDF_Page_Stroke(page); // close path and stroke
				path_count = 0;
				continue;
			}
			if (new_path)
			{
				HPDF_Page_MoveTo(page, rect.x + pnt.x, rect.y - pnt.y);   // started a new path
				new_path = false;
				path_count++;
			}
			else //if (pnt.x != 0 && pnt.y != 0)
			{
				//if(pnt.x<rect.width&&pnt.y<rect.height)
					HPDF_Page_LineTo(page, rect.x + pnt.x, rect.y - pnt.y);
					path_count++;
			}
			
		}
		if (path_count >= 2)
			HPDF_Page_Stroke(page);//HPDF_Page_Stroke(page);
	}

}
#else
enum MOUSE_STATE {
	DOWN =0,
	MOVING =1,
	UP =2
};

static void quadraticBezierDraw(BezPoint p[], HPDF_Page    page, MyRect_T &rect)
{
	//BezPoint p[], float t
	
	for (double t = 0; t <= 1.0; t+=0.2)
	{
		BezPoint bp = quadraticBezier(p, t);
		HPDF_Page_LineTo(page, rect.x + bp.x, rect.y - bp.y);
	}
}

void draw_signature(HPDF_Page    page, const short *p, int len, MyRect_T &rect)
{
	vector<BezPoint>  pts;
	char endPointCount = 0;
	
	int endPoint=1;
	double deltaT=0.1;

	MOUSE_STATE state = MOUSE_STATE::DOWN;

	len = len / 4;
	int ptCount =len;

	if (!(len > 0 && p != 0))
		return;
	float xMax = 0, yMax = 0, xScale = 1.0, yScale = 1.0;
	for (int i = 0; i < len; i++)
	{
		int x = (*p++);
		int y = (*p++);
		BezPoint p(x, y);
		pts.push_back(p);
		
		if (xMax < x) xMax = x;
		if (yMax < y) yMax = y;

	}

	xScale = (rect.width * 1.0) / (xMax * 1.0);
	yScale = (rect.height * 1.0) / (yMax * 1.0);


	HPDF_Page_SetLineWidth(page, 1.2);
	

	BezPoint P0;
	BezPoint xyLast , xyAddLast;
	for (BezPoint &p1 : pts)
	{
		BezPoint p(p1.x * xScale, p1.y*yScale);

		if (p.x < 0 || p.y < 0) {
			state = MOUSE_STATE::UP;  // mouseUp
		}
		switch (state)
		{

			case MOUSE_STATE::DOWN:
				//mouse down
				//ctx.beginPath();  // reset path
				HPDF_Page_MoveTo(page, rect.x + p.x, rect.y - p.y);//ctx.moveTo(x, y);
				xyLast.x= p.x, xyLast.y = p.y ;
				state = MOUSE_STATE::MOVING;
				P0.x = p.x, P0.y = p.y;
				break;
			case MOUSE_STATE::MOVING:
			{
				
				BezPoint xyAdd (
						 (xyLast.x + p.x) / 2,
						 (xyLast.y + p.y) / 2
				);
				
				BezPoint points[3] = { P0,xyLast ,xyAdd };
				quadraticBezierDraw(points,page,rect);
				//ctx.quadraticCurveTo(xyLast.x, xyLast.y, xyAdd.x, xyAdd.y);
				
				HPDF_Page_Stroke(page);//ctx.stroke();

				//ctx.beginPath();
				HPDF_Page_MoveTo(page, rect.x + xyAdd.x, rect.y - xyAdd.y);//ctx.moveTo(xyAdd.x, xyAdd.y);
				P0.x=xyAdd.x, P0.y = xyAdd.y;
				
				xyLast.x=p.x, xyLast.y = p.y ;


			}
				break;

			case MOUSE_STATE::UP:
				HPDF_Page_Stroke(page);//ctx.stroke();
				
				state = MOUSE_STATE::DOWN;
				break;
		}
	}



}
#endif