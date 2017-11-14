
#include "signal_handlers.h"
#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void catch_sigint(int signalNo){

	fflush(stdout);
	signal(SIGINT,SIG_IGN);
  // TODO: File this!
}

void catch_sigtstp(int signalNo)
{
	fflush(stdout);
	signal(SIGTSTP,SIG_IGN);

  // TODO: File this!

}
