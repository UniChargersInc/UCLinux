#ifndef SEND_EMAIL_H
#define SEND_EMAIL_H


#include <stdio.h>

#include "MkrLib.h"
#include "CommonUtils/my_utils.h"
#include "models/models.h"
#include <google/protobuf/stubs/strutil.h>  //GlobalReplaceSubstring


int main_send_hos_combined_email(char *mailTo, char *report_file, const Client &c);
int main_send_register_email(char *mailTo, char *subject, const Client &c);
int test_mail_ssl_main();
#endif