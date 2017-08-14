#include "JobControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include "LineParser.h"

#define STDOUT 1
#define STDERR 2

void execute(cmdLine *pCmdLine){
    char buff[PATH_MAX + 1];
    char* program = pCmdLine->arguments[0];
    char* args[PATH_MAX-1];
    int i;
    for (i=0; i<pCmdLine->argCount; i++){
        args[i]=pCmdLine->arguments[i];
    }
    int returnVal=1;
    returnVal=execvp(program,args);
    if (returnVal==-1){
        write(STDERR,"Error on execv\n",strlen("Error on execv\n"));
        exit(0x55);
    }
}

/**
* Receive a pointer to a job list and a new command to add to the job list and adds it to it.
* Create a new job list if none exists.
**/
job* addJob(job** job_list, char* cmd){
	job* job_to_add = initializeJob(cmd);
	printf("Debugging: cmd%s\n",cmd);

	if (*job_list == NULL){
		*job_list = job_to_add;
		job_to_add -> idx = 1;
	}	
	else{
		int counter = 2;
		job* list = *job_list;
		while (list -> next !=NULL){
			printf("adding %d\n", list->idx);
			list = list -> next;
			counter++;
		}
		job_to_add ->idx = counter;
		list -> next = job_to_add;
	}
	return job_to_add;
}


/**
* Receive a pointer to a job list and a pointer to a job and removes the job from the job list 
* freeing its memory.
**/
void removeJob(job** job_list, job* tmp){
	if (*job_list == NULL)
		return;
	job* tmp_list = *job_list;
	if (tmp_list == tmp){
		*job_list = tmp_list -> next;
		freeJob(tmp);
		return;
	}
		
	while (tmp_list->next != tmp){
		tmp_list = tmp_list -> next;
	}
	tmp_list -> next = tmp -> next;
	freeJob(tmp);
	
}

/**
* receives a status and prints the string it represents.
**/
char* statusToStr(int status)
{
  static char* strs[] = {"Done", "Suspended", "Running"};
  return strs[status + 1];
}


/**
*   Receive a job list, and print it in the following format:<code>[idx] \t status \t\t cmd</code>, where:
    cmd: the full command as typed by the user.
    status: Running, Suspended, Done (for jobs that have completed but are not yet removed from the list).
  
**/
void printJobs(job** job_list){

	job* tmp = *job_list;
	updateJobList(job_list, FALSE);
	while (tmp != NULL){
		printf("[%d]\t %s \t\t %s", tmp->idx, statusToStr(tmp->status),tmp -> cmd); 
		
		if (tmp -> cmd[strlen(tmp -> cmd)-1]  != '\n')
			printf("\n");
		job* job_to_remove = tmp;
		tmp = tmp -> next;
		if (job_to_remove->status == DONE)
			removeJob(job_list, job_to_remove);
		
	}
 
}

void printOneJob(job* currJob){
 		printf("[%d]\t %s \t\t %s", currJob->idx, statusToStr(currJob->status),currJob -> cmd); 
		
		if (currJob -> cmd[strlen(currJob -> cmd)-1]  != '\n')
			printf("\n");
}


/**
* Receive a pointer to a list of jobs, and delete all of its nodes and the memory allocated for each of them.
*/
void freeJobList(job** job_list){
	while(*job_list != NULL){
		job* tmp = *job_list;
		*job_list = (*job_list) -> next;
		freeJob(tmp);
	}
	
}


/**
* receives a pointer to a job, and frees it along with all memory allocated for its fields.
**/
void freeJob(job* job_to_remove){
	free(job_to_remove->tmodes);
	free(job_to_remove->cmd);
	free(job_to_remove);
}


/**
* Receive a command (string) and return a job pointer. 
* The function needs to allocate all required memory for: job, cmd, tmodes
* to copy cmd, and to initialize the rest of the fields to NULL: next, pigd, status 
**/

job* initializeJob(char* cmd){
	job *newJob = (struct job*)malloc(sizeof(job));
	newJob->cmd = (char*)malloc(strlen(cmd));
	strcpy(newJob->cmd,cmd);
	newJob->idx=0;
	newJob->pgid=NULL;
	newJob->status=RUNNING;
	newJob->tmodes = (struct termios*)malloc(sizeof(struct termios));
	newJob->next=NULL;
	return newJob;
}


/**
* Receive a job list and and index and return a pointer to a job with the given index, according to the idx field.
* Print an error message if no job with such an index exists.
**/
job* findJobByIndex(job* job_list, int idx){
   if (job_list->next==NULL){
	   	if (job_list->idx==idx)
	   		return job_list;
	   	else printf("No job with such index!\n");
   }
   else if (job_list->idx == idx)
   			return job_list;
   		else findJobByIndex(job_list->next, idx);
}


/**
* Receive a pointer to a job list, and a boolean to decide whether to remove done
* jobs from the job list or not. 
**/
void updateJobList(job **job_list, int remove_done_jobs){
	int done=0;
	job *currJob = *job_list; /* Segmentation Fault */

	while (!done){
		int i=0;
		if (currJob->next==NULL)
			done=1;
		cmdLine* currCmd;
		currCmd = parseCmdLines(currJob->cmd);  
		
			pid_t pid;
	    	pid = fork();
	    	if (pid==-1){
	            perror("fork error");
	            exit(0x55);
	        }
	        else if (pid==0){
	        	/* set signals to default */ 
	        	signal(SIGQUIT, SIG_DFL); 
				signal(SIGCHLD, SIG_DFL); 
				signal(SIGSTP, SIG_DFL);

				/* Set the group id of the child to be as the curr job's.*/
				pid_t childId = getpid();
				setgpid(childId);

	        	if (currJob->status==RUNNING)
		   			execute(currCmd);
	        }
	        else{
	            int status;

	            /* Set the group id of the child to be as the child's group id.*/
	            /*setgpid(pid);*/
	            currJob->pgid = pid;

	            if (currCmd->blocking==1){ /* & causes the parent to be not blocking */
	                printf("I'm waiting! \n");
	                int retStatus;
	                retStatus=waitpid(pid,&status,WNOHANG);
	                if (retStatus==-1) 
	                	printf("no process with the given process group id, then waitpid returns -1. \n");
   	                printf("Back! \n");  
   	                /* Restore the shell condition to the initial state */	
   	                if (tcsetattr(STDIN_FILENO,0,defaultAttribute) != 0)
    					error("tcsetattr() error");    
   	                            
   	                if (remove_done_jobs==1)
					{
						printOneJob(currJob);
						freeJob(currJob);
					}
	            }
	            currJob->status=DONE;
	            freeCmdLines(currCmd);
	        }
        
	    currJob = currJob->next;
	}
}

/** 
* Put job j in the foreground.  If cont is nonzero, restore the saved terminal modes and send the process group a
* SIGCONT signal to wake it up before we block.  Run updateJobList to print DONE jobs.
**/

void runJobInForeground (job** job_list, job *j, int cont, struct termios* shell_tmodes, pid_t shell_pgid){
 	retStatus=waitpid(j->pgid,&status,WNOHANG);
	if (retStatus==-1){ /* Finished: no process with the given process group id, then waitpid returns -1. */
			printOneJob(j);
			removeJob(j);
	} 
	if (retStatus==0){ /* Haven't finished - waitpid returns 0. */
		tcsetpgrp(STDIN_FILENO,j->pgid);
	}
 	if (cont && j->status==SUSPENDED){
 		tcsetattr (STDIN_FILENO, TCSADRAIN, j->tmodes);
 		kill(j->pgid,SIGCONT);
 	}
 		
	retStatus=0;
	retStatus=waitpid(j->pgid,&status,WIFSTOPPED | WUNTRACED); /* flag WIFSTOPPED | WUNTRACED - catch the relevant signals*/
    if (WIFEXITED(status)) { /*returns true if the child terminated normally*/
        j->status==DONE;
    } else if (WIFSIGNALED(status)) { /*returns true if the child process was terminated by a signal.*/
        j->status==DONE;
    } else if (WIFSTOPPED(status)) { /* Caused by WUNTRACED, hopefully catches SIGINT as well */
        j->status==SUSPENDED;
    } 
	
}

/** 
* Put a job in the background.  If the cont argument is nonzero, send
* the process group a SIGCONT signal to wake it up.  
**/

void runJobInBackground (job *j, int cont){	
 
}
