
#include "send_email.h"
#include "CSmtp.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

//***********************************
//http://cboard.cprogramming.com/cplusplus-programming/125655-sending-simple-email-cplusplus.html

#define USE_SSL_EMAIL	1


//implemented in protobuf strutil.cc
//string StringReplace(const string& s, const string& oldsub,
//	const string& newsub, bool replace_all);
//int GlobalReplaceSubstring(const string& substring,
//	const string& replacement,
//	string* s);
#if 0
#if USE_SSL_EMAIL
int send_email_ssl(EmailInfo *email);
#else
int send_mail(EmailInfo *email);
#endif
#endif
#ifdef WIN32
void usleep(int d);
#endif
PSTR  get_mx_record(char *domain);

//http://archive.oreilly.com/network/excerpt/spcookbook_chap03/index3.html
int spc_email_isvalid(const char *address) {
	int        count = 0;
	const char *c, *domain;
	static char *rfc822_specials = "()<>@,;:\\\"[]";

	/* first we validate the name portion (name@domain) */
	for (c = address; *c; c++) {
		if (*c == '\"' && (c == address || *(c - 1) == '.' || *(c - 1) ==
			'\"')) {
			while (*++c) {
				if (*c == '\"') break;
				if (*c == '\\' && (*++c == ' ')) continue;
				if (*c <= ' ' || *c >= 127) return 0;
			}
			if (!*c++) return 0;
			if (*c == '@') break;
			if (*c != '.') return 0;
			continue;
		}
		if (*c == '@') break;
		if (*c <= ' ' || *c >= 127) return 0;
		if (strchr(rfc822_specials, *c)) return 0;
	}
	if (c == address || *(c - 1) == '.') return 0;

	/* next we validate the domain portion (name@domain) */
	if (!*(domain = ++c)) return 0;
	do {
		if (*c == '.') {
			if (c == domain || *(c - 1) == '.') return 0;
			count++;
		}
		if (*c <= ' ' || *c >= 127) return 0;
		if (strchr(rfc822_specials, *c)) return 0;
	} while (*++c);

	return (count >= 1);
}
static PSTR get_smtp_server(const char *mailTo)
{
	char *domain = strchr((char*)mailTo, '@');
	//free(mm);
	if (!domain)
		return 0;
	domain++;
	PSTR  smtpserver = get_mx_record(domain);
	if (smtpserver == NULL)
	{
		m_printf("Could not get MX host name resolved..%s\r\n", domain);
		return 0;
	}
	m_printf("Email..MX host name resolved..%s\r\n", smtpserver);
	return smtpserver;
}
#if 0
int main_send_register_email(char *mailTo,char *subject,const Client &c)
{
	
	PSTR  smtpserver = get_smtp_server(mailTo);
	if (smtpserver == NULL)
		return -1;
	
	string msg = "";
#if WIN32
	 read_html_file("C:\\inetpub\\www\\mkr_files\\Templates\\register.html",msg);
#else
	read_html_file("/home/mkr/www/mkr_files/Templates/register.html",msg);
#endif
	if (!msg.empty())
	{
		//google::protobuf::GlobalReplaceSubstring("#href#", LOGOUT_URL, &str);
		google::protobuf::GlobalReplaceSubstring("#DrName#", c.fullname, &msg);
		google::protobuf::GlobalReplaceSubstring("#Username#", c.user, &msg);
		google::protobuf::GlobalReplaceSubstring("#Password#", c.pass, &msg);
		google::protobuf::GlobalReplaceSubstring("#ClientID#", Int2Str(c.cid), &msg);


		//smtpserver = 0x620070 "alt2.gmail-smtp-in.l.google.com"
		//smtpserver = 0x620070 "alt1.gmail-smtp-in.l.google.com"
		EmailInfo ei(smtpserver, "info@mikerel.com", /*"skulykov@gmail.com"*/mailTo,
			subject,//"I need to test HOS server ASAP",
			"info@mikerel.com",
			/*"Hello test test test??!!."*/msg.c_str());
			
#if USE_SSL_EMAIL
		if (send_email_ssl(&ei) != 0)
#else
		if (send_mail(&ei) != 0)
#endif
			m_printf("Email has FAILED..\r\n");
		else
			m_printf("Email has successfully sent..\r\n");
		
	}
	else
	{
		m_printf("Could not read html file..\r\n");
	}
	if (smtpserver!=0)
		delete smtpserver;

	return 0;
}
int main_send_hos_combined_email(char *mailTo, char *report_file, const Client &c)
{
	PSTR  smtpserver = get_smtp_server(mailTo);
	if (smtpserver == NULL)
		return -1;
	
	string msg = "Hos and DVIR combined report for ResourceID=123456";
	
	EmailInfo ei(smtpserver, "info@mikerel.com", /*"skulykov@gmail.com"*/mailTo,
		"HOS and DVIR conbined report",
		"info@mikerel.com",
		/*"Hello test test test??!!."*/msg.c_str());
	ei.attachment_file_name = report_file;
#if USE_SSL_EMAIL
	if (send_email_ssl(&ei) != 0)
#else
	if (send_mail(&ei) != 0)
#endif
		m_printf("Comb Email has FAILED..\r\n");
	else
		m_printf("Comb Email has successfully sent..\r\n");
	if (smtpserver != 0)
		delete smtpserver;

	return 0;
}
#else
int main_send_register_email(char *mailTo, char *subject, const Client &c)
{
	int rv = 1;
	PSTR  smtpserver = get_smtp_server(mailTo);
	if (smtpserver == NULL)
		return -1;
	CSmtp mail;
	string msg = "";
#if WIN32
	read_html_file("C:\\My\\Linux\\www\\mkr_files\\Templates\\register.html", msg);
#else
	read_html_file("/home/mkr/www/mkr_files/Templates/register.html", msg);
#endif
	if (!msg.empty())
	{
		//google::protobuf::GlobalReplaceSubstring("#href#", LOGOUT_URL, &str);
		google::protobuf::GlobalReplaceSubstring("#DrName#", c.fullname, &msg);
		google::protobuf::GlobalReplaceSubstring("#Username#", c.user, &msg);
		google::protobuf::GlobalReplaceSubstring("#Password#", c.pass, &msg);
		google::protobuf::GlobalReplaceSubstring("#ClientID#", Int2Str(c.cid), &msg);
		
		mail.SetSMTPServer(smtpserver, SMTP_CLIENT_PORT,false);
		mail.SetSecurityType(USE_TLS);

		mail.SetLogin(0);
		mail.SetPassword(0);
		mail.SetSenderName("United Chargers");
		mail.SetSenderMail("info@h2evon.ca");
		mail.SetReplyTo("info@h2evon.ca");
		mail.SetSubject(subject);
		mail.AddRecipient(mailTo);
		mail.SetXPriority(XPRIORITY_NORMAL);
		mail.SetXMailer("UC-Email");
		
		int number_of_attemps = 0;

		do {
			try
			{
				m_printf("MSRE mail.Send: %d\r\n", msg.size());
				mail.Send(msg);
				rv = 1;
				m_printf("MSRE mail.Send: %d Success\r\n", msg.size());
			}
			catch (ECSmtp e)
			{
				m_printf("MSRE Ex: %d\r\n", e.GetErrorNum());
				rv = 0;
				number_of_attemps++;
				usleep(500*1000);
			}
		} while (rv == 0 && number_of_attemps < 3);
		
	}
	else
	{
		m_printf("MSRE Could not read html file..");
		rv = 0;
	}
	if (smtpserver != 0)
		delete smtpserver;

	return rv;
}
int main_send_hos_combined_email(char *mailTo, char *report_file, const Client &c)
{
	PSTR  smtpserver = get_smtp_server(mailTo);
	if (smtpserver == NULL)
		return -1;

	string msg = "Hos and DVIR combined report for ResourceID=123456";
	CSmtp mail;
	try
	{
		mail.SetSMTPServer(smtpserver, SMTP_CLIENT_PORT, false);
		mail.SetSecurityType(USE_TLS);

		mail.SetLogin(0);
		mail.SetPassword(0);
		mail.SetSenderName("Mikerel Systems");
		mail.SetSenderMail("info@mikerel.com");
		mail.SetReplyTo("info@mikerel.com");
		mail.SetSubject("HOS and DVIR conbined report");
		mail.AddRecipient(mailTo);
		mail.SetXPriority(XPRIORITY_NORMAL);
		mail.SetXMailer("MKR-Email");
		
		mail.AddAttachment(report_file);

		mail.Send(msg);
	}
	catch (ECSmtp e)
	{
		//m_printf("Comb Email Error: %s\r\n", e.GetErrorText().c_str());
	}

	if (smtpserver != 0)
		delete smtpserver;

	return 0;
}
#endif

/*
int test_mail_ssl_main()
{
	char* mailTo = "skulykov@gmail.com";

	PSTR  smtpserver = get_smtp_server(mailTo);
	if (smtpserver == NULL)
		return -1;


	char *subject = "I need to test HOS server ASAP";
	string msg = "Hello test test test??!!.";
	read_html_file("/home/mkr/www/mkr_files/Templates/register.html", msg);
	EmailInfo ei(smtpserver, "info@mikerel.com", mailTo,
		subject,
		"info@mikerel.com",
		msg.c_str());

	ei.attachment_file_name = "report_1234.pdf";
	if (send_email_ssl(&ei) != 0)

		m_printf("Email has FAILED..\r\n");
	else
		m_printf("Email has successfully sent..\r\n");
	return 1;
}
*/