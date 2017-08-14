#define DONE -1
#define RUNNING 1
#define SUSPENDED 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LineParser.h"
#include "JobControl.h"
#include <signal.h>
#include <unistd.h>

/*void handleSignal(int signalNum){
    printf("Ignoring this signal: %s\n",strsignal(signalNum));
}*/


int main(int argc, char *argv[]){
	job* job_list = NULL;
	job** job_list_ptr = &job_list;
	char input[2048];
	signal(SIGTTIN, SIG_IGN); 
    signal(SIGTTOU, SIG_IGN); 	
  	signal(SIGQUIT, SIG_DFL); 
	signal(SIGCHLD, SIG_DFL); 
    signal(SIGSTP, SIG_DFL); 
    /* Set the process group of the shell to its process id (getpid). */	
    pid_t groupId = getpid();
    setgpid(groupId);
    /* Save default terminal attributes to restore them back when a process running in the foreground ends.*/
    *defaultAttribute = malloc (sizeof(struct termios));

    if (tcgetattr(STDIN_FILENO, defaultAttribute) != 0)
    	error("tcgetatt() error");

    /*int i=5;*/
    while (1){

    	fgets(input,2048,stdin);    	
    	/*if (input[0]=='q' && input[1]==0) {*/
    	if (strcmp(input,"q\n")==0){
    		break;
    	}

    	if (strcmp(input,"jobs\n")==0){
    		printf("inJobs%s\n");
    		printJobs(job_list_ptr);
    	}
    	printf("Debugging: input%s\n",input);
    	addJob(job_list_ptr, input);
    	/*i-=1;*/
	}
	freeJobList(job_list_ptr);
    free(defaultAttribute);
    return 0;
}