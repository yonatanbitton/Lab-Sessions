#define DONE -1
#define RUNNING 1
#define SUSPENDED 0
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "LineParser.h"
#include "JobControl.h"
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

int pipeArr[2];

job** job_list_ptr;

void execute(cmdLine *pCmdLine, job* jobToRun){
    char* program = pCmdLine->arguments[0];
    char* args[PATH_MAX-1];
    int i, fd;
    for (i=0; i<pCmdLine->argCount; i++){
        args[i]=pCmdLine->arguments[i];
    }
    if (pCmdLine->next!=NULL)
    	pipe(pipeArr);
    
    pid_t pid;
    pid = fork();
    if (pid==-1){
    perror("fork error");
    exit(0x55);
    }
    else if (pid==0){ /* Child */
/*        printf("first first child\n");
*/        signal(SIGQUIT, SIG_DFL); 
        signal(SIGCHLD, SIG_DFL); 
        signal(SIGTSTP, SIG_DFL); 
        int in_des = 0;
        int out_des = 1;
        /* Set child group id -> group id */
        pid_t childPid = getpid();
        setpgid(childPid,childPid);

        if (pCmdLine->next!=NULL){
        	close(STDOUT); /* Close standart output */
	        fd=dup(pipeArr[1]); /* Redirecting child output to the pipe */
	        close (pipeArr[1]);
	        close (pipeArr[0]);
	        /* Handle pipe of first child */
        }  
        int retVal=1;
        retVal=execvp(program,args);
        if (retVal==-1){
            printf("Program is: %s",program);
            write(STDERR,"Error on execv\n",strlen("Error on execv\n"));
            exit(0x55);
        }
        
    }
    else{
        /* Set child group id -> group id, and make the parent acknowledge it. */
        
        jobToRun->pgid = pid; /* Update the parent group id as the childs' */ 

        int status;

        if (pCmdLine->next!=NULL)
        	close(pipeArr[1]);  /* The child was the only one writing to the pipe.  */ 
        if (pCmdLine->next!=NULL){
/*            printf("Going to fork again\n");
*/            forkAgain(pCmdLine->next, pid, status, jobToRun);
        }

/*        printf("%d\n", getpid());*/
        if (jobToRun->cmd[strlen(jobToRun->cmd)-2]!='&')
            runJobInForeground (job_list_ptr,jobToRun,0,defaultAttribute, getpid());                     
    }
}

void forkAgain(cmdLine *nextCmd, pid_t pid, int status, job* jobToRun){

   
    pid_t pid2;
    pid2 = fork();
    char* program = nextCmd->arguments[0];
    char* args[PATH_MAX-1];
    int i;
    for (i=0; i<nextCmd->argCount; i++){
        args[i]=nextCmd->arguments[i];
    }
    int fd2;
    if (pid2==-1){
        perror("fork error");
        exit(0x55);
    }
    else if (pid2==0) { /* Grandson of Grandparent (Child of parent) */
        close (STDIN); /* Redirecting second child input to be from the pipe */
        fd2 = dup (pipeArr[0]);
        close (pipeArr[0]);  
        int retVal=1;
        retVal=execvp(program,args);
        if (retVal==-1){
            printf("Program is: %s",program);
            write(STDERR,"Error on execv\n",strlen("Error on execv\n"));
            exit(0x55);
        }    
    }
    else { /* Son of Grandparent (Parent) */

        /* Close the read end of the pipe */
        close (pipeArr[0]);

        /* Wait for second child to terminate */
        int status2;
        waitpid (pid,&status,WUNTRACED); /* meaning wait for any child process whose process group ID is equal to that of the calling process. */
        waitpid (pid2,&status2,WUNTRACED);

    }
}

void handleSignal(int signo)
{
    if (signo == SIGUSR1)
        printf("received SIGUSR1\n");
    else if (signo == SIGKILL)
        printf("received SIGKILL\n");
    else if (signo == SIGSTOP)
        printf("received SIGSTOP\n");
    else if (signo == SIGTSTP){
        printf("received SIGTSTP\n");
        signal(SIGTSTP, handleSignal);
    }
}



int main(int argc, char *argv[]){
	job* job_list = NULL;
	job_list_ptr = &job_list;
	
	char input[2048];
   
    signal(SIGTSTP, handleSignal);
    signal(SIGUSR1, handleSignal);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    /* Set the process group of the shell to its process id (getpid). */	
    pid_t processId = getpid();
    setpgid(processId, processId);

    /* Save default terminal attributes to restore them back when a process running in the foreground ends.*/
    defaultAttribute = malloc (sizeof(struct termios));

    if (tcgetattr(STDIN_FILENO, defaultAttribute) != 0) /* Save the terminal attributes so we can restore them later. */
    	error("tcgetatt() error");

    while (1){      	
        cmdLine* box;
        char buff2[PATH_MAX + 1];
        char* cwd2;
        cwd2 = getcwd( buff2, PATH_MAX + 1 );
        printf ("%s> ",cwd2);
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
                    runJobInForeground (job_list_ptr,requiredJob,1,defaultAttribute, processId);               
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
    	else {
            job* jobToRun;
            jobToRun = addJob(job_list_ptr, input);
            cmdLine* currCmd;
            currCmd = parseCmdLines(jobToRun->cmd); 
            execute(currCmd, jobToRun);           
        
        } 
	}

	freeJobList(job_list_ptr);
    free(defaultAttribute);

    return 0;
}