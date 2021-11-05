#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif


#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cassert>

#include <ucontext.h>


//#include <filesystem>
#include <sstream>
#include <fstream>
#include <signal.h>     // ::signal, ::raise
#include <boost/stacktrace.hpp>

/*
BOOST_STACKTRACE_USE_BACKTRACE
BOOST_STACKTRACE_USE_ADDR2LINE

boost_stacktrace_addr2line
boost_stacktrace_backtrace
*/


//const char* backtraceFileName = "./backtraceFile.dump";
//https://stackoverflow.com/questions/13428910/how-to-set-the-environmental-variable-ld-library-path-in-linux
//set LD_LIBRARY_PATH on Ubuntu 
/*export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/your/custom/path/
You can add it to your ~/.bashrc:
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/your/custom/path/' >> ~/.bashrc

Don't ask me why, but this did not work, until I made this small change: mv ~/.bashrc ~/.bash_profile with otherwise the same code and same file privileges. It then worked. 
This is on Ubuntu 20.04.1 LTS. Without this file rename, LD_LIBRARY_PATH would still be empty on re-login. .bashrc was rwx for user, so it should execute fine. Executing it manually would also not set LD_LIBRARY_PATH (somewhat expected), and sourcing it (. ./.bashrc) did set it. Very odd, and I expect it's Ubuntu 20.04.1 LTS specific. –
*/

static const char* log_path_segv = "/home/mkr/service/logs/http_segv.log";
void stop_http_light_main_run();
void signalHandler(int signum)
{
	//::signal(SIGSEGV, SIG_DFL);
	//::signal(SIGABRT, SIG_DFL);
	//boost::stacktrace::safe_dump_to(backtraceFileName);
	//std::cout << boost::stacktrace::stacktrace();
	stop_http_light_main_run();
	{
		time_t timeval = time(NULL);
		
		std::ofstream myfile;
		myfile.open(log_path_segv, std::ios::out | std::ios::app);
		myfile << "SIGN: " <<signum << " ** " << ctime(&timeval) << std::endl;
		myfile << boost::stacktrace::stacktrace();
		
	}
	usleep(100*1000);
	//::raise(SIGABRT);
	exit(EXIT_FAILURE);
}

//void sendReport()
//{
//	if (/*std::filesystem::exists(backtraceFileName)*/true)
//	{
//		std::ifstream file(backtraceFileName);
//
//		auto st = boost::stacktrace::stacktrace::from_dump(file);
//		std::ostringstream backtraceStream;
//		backtraceStream << st << std::endl;
//
//		// sending the code from st
//
//		file.close();
//		//std::filesystem::remove(backtraceFileName);
//	}
//}




#define CP_NAME    "signaln"

#define ITERATIONS 100
#define INTERVAL   5

/*
 * function prototypes
 */
void do_checkpoint(void);
void checkpoint_term(int);
void checkpoint_only(int);
int set_sigsegv();

void event_log_func(int severity, const char *msg);

int main_sig_setup()
{
	int i;        /* loop index */

	/*
	 * attach signal handlers as described in the tutorial (note we are
	 * attaching the same handler to multiple signals)
	 */
	signal(SIGTERM, checkpoint_term);
	signal(SIGINT, checkpoint_term);
	
	signal(SIGALRM, checkpoint_term);
	signal(SIGHUP, checkpoint_only);
	signal(SIGPIPE, checkpoint_only);
	
	//set_sigsegv();
	signal(SIGSEGV, signalHandler);
	signal(SIGABRT, signalHandler);
	return(0);
}


void do_checkpoint(int signum)
{
	static int cp_num = 1;    /* sequential numbering of checkpoints */
	char filename[256];    /* buffer for construction of filenames */
	time_t timeval;           /* current time */
	FILE *fp;                 /* reference to file for checkpoint ouptut */

	{
		if(signum == SIGINT || signum == SIGTERM)
			stop_http_light_main_run();
	}


	/*
	 * construct a unique file name (numbered sequentially in this example)
	 * for this checkpoint output; this allows for multiple checkpoints from
	 * a single run that do not interfere with one another
	 */
	snprintf(filename, 256, "/home/mkr/service/logs/%s_%d.txt", CP_NAME, cp_num);
	if ((fp = fopen(filename, "w")) == NULL)
	{
		//fprintf(stderr, "[ERROR]: unable to open file (%s) for checkpoint output\n", filename);
		event_log_func(0, "unable to open file for checkpoint output");
		return;
	}
	else
	{
		/*
		 * output relevant checkpointing data here (this example
		 * records the date/time of the checkpoint, and outputs
		 * the sequential checkpoint number)
		 */
		timeval = time(NULL);
		fprintf(fp, "- SIGNAL -\n");
		fprintf(fp, "time: %s", ctime(&timeval));
		fprintf(fp, "signal number = %d\n", signum);
		fclose(fp);
		cp_num++;
	}
}


void checkpoint_term(int signum)
{
	do_checkpoint(signum);
	//exit(1);
}


void checkpoint_only(int signum)
{
	do_checkpoint(signum);
}

