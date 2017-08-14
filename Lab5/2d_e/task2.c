#define DONE -1
#define RUNNING 1
#define SUSPENDED 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LineParser.h"
#include "JobControl.h"

/*void handleSignal(int signalNum){
    printf("Ignoring this signal: %s\n",strsignal(signalNum));
}*/

job** job_list_ptr;

void handleSignal(int signo)
{
    if (signo == SIGUSR1)
        printf("received SIGUSR1\n");
    else if (signo == SIGKILL)
        printf("received SIGKILL\n");
    else if (signo == SIGSTOP)
        printf("received SIGSTOP\n");
    else if (signo == SIGTSTP){
        /*printf("received SIGTSTP\n");*/
        job* firstJob = *job_list_ptr;
        suspendLast(firstJob);
    }
}

void suspendLast(job* currJob){
    if (currJob!=NULL)
    {
        if (currJob->next==NULL){
            printf("job suspending:\n");
            printOneJob(currJob);
            currJob->status=SUSPENDED; 
            kill(currJob->pgid,SIGTSTP);
        }
        else suspendLast(currJob->next);
    }

}

int main(int argc, char *argv[]){
	job* job_list = NULL;
	job_list_ptr = &job_list;
	
	char input[2048];
    if (signal(SIGUSR1, handleSignal) == SIG_ERR)
        printf("\ncan't catch SIGUSR1\n");
    /*if (signal(SIGKILL, handleSignal) == SIG_ERR)
        printf("\ncan't catch SIGKILL\n");
    if (signal(SIGSTOP, handleSignal) == SIG_ERR)
        printf("\ncan't catch SIGSTOP\n");*/
    if (signal(SIGTSTP, handleSignal) == SIG_ERR)
        printf("\ncan't catch SIGSTOP\n");
	/*signal(SIGTTIN, SIG_IGN); 
    signal(SIGTTOU, SIG_IGN); 	
  	signal(SIGQUIT, SIG_DFL); 
	signal(SIGCHLD, SIG_DFL); 
    signal(SIGTSTP, SIG_DFL); */

    /* Set the process group of the shell to its process id (getpid). */	
    pid_t processId = getpid();
    setpgid(processId, processId);

    /* Save default terminal attributes to restore them back when a process running in the foreground ends.*/
    defaultAttribute = malloc (sizeof(struct termios));

    if (tcgetattr(STDIN_FILENO, defaultAttribute) != 0) /* Save the terminal attributes so we can restore them later. */
    	error("tcgetatt() error");

    while (1){      	

    	fgets(input,2048,stdin);    	
    	/*if (input[0]=='q' && input[1]==0) {*/
    	if (strcmp(input,"q\n")==0 || (strncmp(input,"quit",4)==0)){
    		break;
    	}

        else if (strncmp(input,"fg ",3)==0){
            
   
            cmdLine* line;
            int number;
            line=parseCmdLines(input);
            number = atoi(line->arguments[1]);
            job* requiredJob = findJobByIndex(job_list, number);
            /*if (requiredJob!=NULL) printOneJob(requiredJob);*/
            if (requiredJob!=NULL){ 
                            
                if(requiredJob->cmd[strlen(requiredJob->cmd)-2]!='&') {
                    runJobInForeground (job_list_ptr,requiredJob,1,defaultAttribute, processId);
                }
                else printf("fg command was required, but the required job runs in the background\n");
            }
        }
         else if (strncmp(input,"bg ",3)==0){
            cmdLine* line;
            int number;
            line=parseCmdLines(input);
            number = atoi(line->arguments[1]);            
            runJobInBackground (*job_list_ptr,number);
        }
    	else if (strcmp(input,"jobs\n")==0){
    		/*printf("inJobs%s\n");*/
    		printJobs(job_list_ptr);
    	}
    	else addJob(job_list_ptr, input);
    	/*i-=1;*/
	}

	freeJobList(job_list_ptr);
    free(defaultAttribute);

    return 0;
}