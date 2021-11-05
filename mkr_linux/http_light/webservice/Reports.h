#ifndef REPORTS_INC_H
#define REPORTS_INC_H


#include <MkrLib.h>
#include <CommonUtils/my_utils.h>

#include <mutex>


#include "proto/udorx_comm.pb.h"
#include "proto/udorx_ws_comm.pb.h"

#include "pdf_report.h"

using namespace std;

using namespace udorx_pbf;


//int GenerateHosReport(PdfReport_T *rr);
//int GenerateInspectionReport(PdfReport_T *rr);

int GenerateNewEmailsReport(int cid, int rid, const vector<string> &v);

#endif