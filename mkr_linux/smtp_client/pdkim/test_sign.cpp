/*
 *  PDKIM - a RFC4871 (DKIM) implementation
 *
 *  Signing test & sample code.
 *
 *  Copyright (C) 2009  Tom Kistner <tom@duncanthrax.net>
 *  http://duncanthrax.net/pdkim/
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>

#include "src/pdkim.h"
#include "../src/smtpfuncs.h"
//#include "logfile.h"

#if 0
#define RSA_PRIVKEY "-----BEGIN RSA PRIVATE KEY-----\n\
MIICXQIBAAKBgQC5+utIbbfbpssvW0TboF73Seos+1ijdPFGwc/z8Yu12cpjBvRb\n\
5/qRJd83XCySRs0QkK1zWx4soPffbtyJ9TU5mO76M23lIuI5slJ4QLA0UznGxfHd\n\
fXpK9qRnmG6A4HRHC9B93pjTo6iBksRhIeSsTL94EbUJ625i0Lqg4i6NVQIDAQAB\n\
AoGBAIDGqJH/Pta9+GTzGovE0N0D9j1tUKPl/ocS/m4Ya7fgdQ36q8rTpyFICvan\n\
QUmL8sQsmZ2Nkygt0VSJy/VOr6niQmoi97PY0egxvvK5mtc/nxePCGwYLOMpB6ql\n\
0UptotmvJU3tjyHZbksOf6LlzvpAgk7GnxLF1Cg/RJhH9ubBAkEA6b32mr52u3Uz\n\
BjbVWez1XBcxgwHk8A4NF9yCpHtVRY3751FZbrhy7oa+ZvYokxS4dzrepZlB1dqX\n\
IXaq7CgakQJBAMuwpG4N5x1/PfLODD7PYaJ5GSIx6BZoJObnx/42PrIm2gQdfs6W\n\
1aClscqMyj5vSBF+cWWqu1i7j6+qugSswIUCQA3T3BPZcqKyUztp4QM53mX9RUOP\n\
yCBfZGzl8aCTXz8HIEDV8il3pezwcbEbnNjen+8Fv4giYd+p18j2ATSJRtECQGaE\n\
lG3Tz4PYG/zN2fnu9KwKmSzNw4srhY82D0GSWcHerhIuKjmeTw0Y+EAC1nPQHIy5\n\
gCd0Y/DIDgyTOCbML+UCQQClbgAoYnIpbTUklWH00Z1xr+Y95BOjb4lyjyIF98B2\n\
FA0nM8cHuN/VLKjjcrJUK47lZEOsjLv+qTl0i0Lp6giq\n\
-----END RSA PRIVATE KEY-----"
#else

#define RSA_PRIVKEY "-----BEGIN RSA PRIVATE KEY-----\n\
MIICXQIBAAKBgQDmhEcoXX2sIZNUPYsq6MA23RgfuAkup160yZeZzCPwL8pgaA2+\n\
COqwQU1MgZMPwxSWXNFtrUMNNok+Uz4jsY828yI80/dgXU8WIHr1tmnDk5QKm1b+\n\
QbgcWq1ZHQYG3aZC4WUww+N/8npWr8uG7tca2Vt/LCcLTK1zwEGeyP9ePwIDAQAB\n\
AoGBAJxuQ8O2viPjnRFbumdiD56rp0VPJJwOZIyXCVQLXpAOPm3x7vF5J5p4ljZG\n\
8D+QCRaI4m5lV81NpxEueErMofxEargmnYLING0BTvgWPG0PMAN3mpMMVCInWx2T\n\
2C9M5duloNtpolOhvsSoOBLCGcChuDEwMivGkrZ0Z4ZLPmShAkEA+Ze/ODE3+KpY\n\
+QxZ6N4tFNhsscDSwZogsLtjyOzOE76zSptjcNcN88Nof4hrvjbMrkXxrDP3gyhc\n\
7q9ys2+z1wJBAOxvKykxkkkKnb0GC6NuHs2tqP1RioSlbErD6kJ2hdWm54jmyL1a\n\
5/kbEvEW9laCqCbdrZgPX/LVuikuF2Rh29kCQHsTrjKKarxPXBJrWcawEHDwDCFX\n\
IVZgp3Ae56TDyRcJeaX3nACfu3yRxmzlG6Qvp5zkVrkA4zOk6ra0g44iLf8CQQDb\n\
xUIDWna1uk6BQ7tRwKyVVtT8Eu2E+xc8L/vyp6Ujwa76jDsP+A4keUwUGdoE9BJF\n\
fk08SR5Q5OXUHWnU/zmRAkBJURpjPTD0AWyDGyDzNFE9JqwsfmB665CVmFPCM32R\n\
MDGMkMaXUFPbU5SEXXOTjix54c2luS+jucVQLAmmSF53\n\
-----END RSA PRIVATE KEY-----"

#endif

#define DOMAIN   "mikerel.com"
#define SELECTOR "mkrsel"

/* This small function signs the following message using the domain,
 * selector and RSA key above. It prints the signature header
 * followed by the message on STDOUT, so we can simply pipe that
 * to the verification test program (test_verify.c).
 *
 * Please note that this sample code skips on some error checking
 * and handling for the sake of clarity and brevity.
 */

#if MY_TEST_PDKIM
#if 0
char *test_message[] = {
  "From: Tom Kistner <tom@duncanthrax.net>\r\n",
  "X-Folded-Header: line one\r\n\tline two\r\n",
  "To: PDKIM\r\nSubject: PDKIM Test\r\n\r\nTes",
  "t 3,4\r\nHeute bug ich, morgen fix ich.\r\n",
  NULL
};
#else
/*
char *test_message[] = {
	"From: <info@mikerel.com>\r\n",
	"X-Folded-Header: line one\r\n\tline two\r\n",
	"To: skulykov@gmail.com\r\nSubject: PDKIM Test\r\n\r\nTes",
	"t 3,4\r\nHeute bug ich, morgen fix ich.\r\n",
	NULL
};*/

char *test_message[] = {
	"From: <info@mikerel.com>\r\n",
	//"X-Folded-Header: line one\r\n\tline two\r\n",
	"To: skulykov@gmail.com\r\n",
	"Subject: PDKIM Test\r\n",
	"\r\nTes",
	"t 3,4\r\nHeute bug ich, morgen fix ich.\r\n",
	NULL
};
#endif

int main_test_sign(/*int argc, char *argv[]*/) {
//  FILE *debug=0;
  int i;

  pdkim_ctx       *ctx;
 // pdkim_signature *signature;

  delete_log_file();

  /* pdkim_ctx *pdkim_init_sign(int mode,
   *                            char *domain,
   *                            char *selector,
   *                            char *rsa_privkey)
   *
   * Initialize context for signing.
   *
   *    int mode
   *      PDKIM_INPUT_NORMAL or PDKIM_INPUT_SMTP. When SMTP
   *      input is used, the lib will deflate double-dots at
   *      the start of atline to a single dot, and it will
   *      stop processing input when a line with and single
   *      dot is received (Excess input will simply be ignored).
   *
   *    char *domain
   *      The domain to sign as. This value will land in the
   *      d= tag of the signature.
   *
   *    char *selector
   *      The selector string to use. This value will land in
   *      the s= tag of the signature.
   *
   *    char *rsa_privkey
   *      The private RSA key, in ASCII armor. It MUST NOT be
   *      encrypted.
   *
   * Returns: A pointer to a freshly allocated pdkim_ctx
   *          context.
   */
  ctx = pdkim_init_sign(PDKIM_INPUT_NORMAL,  /* Input type */
                        DOMAIN,              /* Domain   */
                        SELECTOR,            /* Selector */
                        RSA_PRIVKEY          /* Private RSA key */
                       );

  /* void pdkim_set_debug_stream(pdkim_ctx *ctx,
   *                             FILE *debug)
   *
   * Set up debugging stream.
   *
   * When PDKIM was compiled with DEBUG defined (which is the
   * recommended default), you can set up a stream where it
   * sends debug output to. In this example, we simply use
   * STDERR (fd 2) for that purpose. If you don't set a debug
   * stream, no debug output is generated.
   */
//  debug = fdopen(2,"a");
//  pdkim_set_debug_stream(ctx,debug);

  /* int pdkim_set_optional(pdkim_ctx *ctx,
   *                        char *sign_headers,
   *                        char *identity,
   *                        int canon_headers,
   *                        int canon_body,
   *                        long bodylength,
   *                        int algo,
   *                        unsigned long created,
   *                        unsigned long expires)
   *
   * OPTIONAL: Set additional optional signing options. If you do
   * not use this function, sensible defaults (see below) are used.
   * Any strings you pass in are dup'ed, so you can safely release
   * your copy even before calling pdkim_free() on your context.
   *
   *    char *sign_headers (default NULL)
   *      Colon-separated list of header names. Headers with
   *      a name matching the list will be included in the
   *      signature. When this is NULL, the list of headers
   *      recommended in RFC4781 will be used.
   *
   *    char *identity (default NULL)
   *      An identity string as described in RFC4781. It will
   *      be put into the i= tag of the signature.
   *
   *    int canon_headers (default PDKIM_CANON_SIMPLE)
   *      Canonicalization algorithm to use for headers. One
   *      of PDKIM_CANON_SIMPLE or PDKIM_CANON_RELAXED.
   *
   *    int canon_body (default PDKIM_CANON_SIMPLE)
   *      Canonicalization algorithm to use for the body. One
   *      of PDKIM_CANON_SIMPLE or PDKIM_CANON_RELAXED.
   *
   *    long bodylength (default -1)
   *      Amount of canonicalized body bytes to include in
   *      the body hash calculation. A value of 0 means that
   *      the body is not included in the signature. A value
   *      of -1 (the default) means that there is no limit.
   *
   *    int algo (default PDKIM_ALGO_RSA_SHA256)
   *      One of PDKIM_ALGO_RSA_SHA256 or PDKIM_ALGO_RSA_SHA1.
   *
   *    unsigned long created (default 0)
   *      Seconds since the epoch, describing when the signature
   *      was created. This is copied to the t= tag of the
   *      signature. Setting a value of 0 (the default) omits
   *      the tag from the signature.
   *
   *    unsigned long expires (default 0)
   *      Seconds since the epoch, describing when the signature
   *      expires. This is copied to the x= tag of the
   *      signature. Setting a value of 0 (the default) omits
   *      the tag from the signature.
   *
   *  Returns: 0 (PDKIM_OK) for success or a PDKIM_ERR_* constant
   */
  pdkim_set_optional(ctx, NULL, NULL,
                     PDKIM_CANON_SIMPLE, PDKIM_CANON_SIMPLE,
                     -1, PDKIM_ALGO_RSA_SHA256, 0, 0);

  /* int pdkim_feed(pdkim_ctx *ctx,
   *                char *data,
   *                int data_len)
   *
   * (Repeatedly) feed data to the signing algorithm. The message
   * data MUST use CRLF line endings (like SMTP uses on the
   * wire). The data chunks do not need to be a "line" - you
   * can split chunks at arbitrary locations.
   *
   *    char *data
   *      Pointer to data to feed. Please note that despite
   *      the example given below, this is not necessarily a
   *      C string.
   *
   *    int data_len
   *      Length of data being fed, in bytes.
   *
   * Returns: 0 (PDKIM_OK) for success or a PDKIM_ERR_* constant
   */

  /*svk
  i = 0;
  while (test_message[i] != NULL) {
    if (pdkim_feed(ctx,
                   test_message[i],
                   strlen(test_message[i])) != PDKIM_OK) {
      printf("pdkim_feed() error\n");
      goto BAIL;
    }
    i++;
  }*/

#define BUFSIZE  1024
#define BUFSIZE2  100
#define MAIL_AGENT  "mikerel mail"
  char s_buf2[BUFSIZE2] = { 0 };
  time_t t_now = time(NULL);
  int n_ret;

  char s_buf[BUFSIZE] = { 0 };
  // First prepare the envelope 
  strftime(s_buf2, BUFSIZE2, "%a, %d %b %Y  %H:%M:%S +0000", gmtime(&t_now));

  char *subject = "Hey, How are you doing??";
  char *replyto = "info@mikerel.com";

  snprintf(s_buf, BUFSIZE, "Date: %s\r\nFrom: %s\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\nReply-To: %s\r\n\r\n",
	  s_buf2, "<info@mikerel.com>", "skulykov@gmail.com", subject, MAIL_AGENT, replyto);
  if (pdkim_feed(ctx,
	  s_buf,
	  strlen(s_buf)) != PDKIM_OK) {
	  printf("pdkim_feed() error\n");
	  goto BAIL;
  }


  /* int pdkim_feed_finish(pdkim_ctx *ctx,
   *                       pdkim_signature **signature,
   *
   * Signal end-of-message and retrieve the signature block.
   *
   *    pdkim_signature **signature
   *      Pass in a pointer to a pdkim_signature pointer.
   *      If the function returns PDKIM_OK, it will be set
   *      up to point to a freshly allocated pdkim_signature
   *      block. See pdkim.h for documentation on what that
   *      block contains. Hint: Most implementations will
   *      simply want to retrieve a ready-to-use
   *      DKIM-Signature header, which can be found in
   *      *signature->signature_header. See the code below.
   *
   * Returns: 0 (PDKIM_OK) for success or a PDKIM_ERR_* constant
   */
  if (pdkim_feed_finish(ctx,/* &signature */ 0) == PDKIM_OK) {

    /* Print signature to STDOUT, followed by the original
     * message. We can then pipe the output directly to
     * test_verify.c.
     */
   /* printf(signature->signature_header);
	to_log_file(signature->signature_header);
    printf("\r\n");
	to_log_file("\r\n");*/

    /*
	i = 0;
	while (test_message[i] != NULL) {
      printf(test_message[i]);
	  to_log_file(test_message[i]);
      i++;
    }*/
	char envelop[2048] = { 0 };
	snprintf(envelop, 2048, "%s\r\n%s", ctx->sig->signature_header, s_buf);
	to_log_file(envelop);
	
  }

  BAIL:
  /* void pdkim_free_ctx(pdkim_ctx *ctx)
   *
   *  Free all allocated memory blocks referenced from
   *  the context, as well as the context itself.
   */
  pdkim_free_ctx(ctx);

// if(debug) fclose(debug);
  return 0;
}

#endif
//==================================================================================
void /*sk_log_to_file*/m_printf(const char *format, ...);
int dkim_sign_and_send(int n_sock,char *s_buf) {
	int rv = 0;
	pdkim_ctx       *ctx;
	
	ctx = pdkim_init_sign(PDKIM_INPUT_NORMAL,  /* Input type */
		DOMAIN,              /* Domain   */
		SELECTOR,            /* Selector */
		RSA_PRIVKEY          /* Private RSA key */
	);

	pdkim_set_optional(ctx, NULL, NULL,
		PDKIM_CANON_SIMPLE, PDKIM_CANON_SIMPLE,
		-1, PDKIM_ALGO_RSA_SHA256, 0, 0);
	
	if (pdkim_feed(ctx,
		s_buf,
		strlen(s_buf)) != PDKIM_OK) {
		m_printf("pdkim_feed() error\n");
		rv = -1;
		goto BAIL;
	}

	if (pdkim_feed_finish(ctx,/* &signature */ 0) == PDKIM_OK) {
		char envelop[2048] = { 0 };
		snprintf(envelop, 2048, "%s\r\n%s", ctx->sig->signature_header, s_buf);
		//to_log_file(envelop);

		/* Send the envelope */
		if (send(n_sock, envelop, strlen(envelop), 0) == SOCKET_ERROR) {
			m_printf("Could not send DKIM Envelope: %s\r\n", envelop);
			rv=-3;
		}

	}
	else
		rv = -2;

BAIL:
	
	pdkim_free_ctx(ctx);
	return rv;
}




