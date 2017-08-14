
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <stdio.h>
#include <unistd.h>

#define STDIN 0 
#define STDOUT 1 
#define STDERR 2 

int pipeArr[2];

int main(int argc, char* argv[]){
	int i;
	int debug=0;
	for (i=0; i<argc; i++)
		if (strcmp("-d",argv[i])==0) debug=1;
	printf("debug is: %d\n",debug);
	pipe(pipeArr); /* Pipe creating */
	pid_t pid;
	if (debug) fprintf(stderr,"(parent_process>forking...)\n");
    pid = fork();
	if (debug && pid>0) fprintf(stderr,"(parent_process>created process with id: %d)\n",pid);

	    if (pid==-1){
	    perror("fork error");
	    exit(0x55);
    }
    else if (pid==0) childProcess(debug);
    	else parentProcess(pid,debug);

    return 0;
}

void childProcess(int debug){
	char* argv[] = {"ls","-l",0};
	int fd;
	close(STDOUT); /* Close standart output */
	if (debug) fprintf(stderr,"(child1>redirecting stdout to write end of the pipe...)\n");

	fd=dup(pipeArr[1]); /* Redirecting first child output to the pipe */
	close (pipeArr[1]);	/* After handeling - no need */
	close (pipeArr[0]); /* Unncessary end */
	if (debug) fprintf(stderr,"(child1>going to execute cmd: ls -l...)\n");

	execvp(argv[0],argv);
}

void parentProcess(pid_t pid, int debug){
	close(pipeArr[1]);  /* Close write end - avoiding EOF: make the second child know that no more data is coming.*/ 

	int status;
	pid_t pid2;

	if (debug) fprintf(stderr,"(parent_process>forking...)\n");
    pid2 = fork();
	if (debug && pid2>0) fprintf(stderr,"(parent_process>created process with id: %d)\n",pid2);

    int fd2;
    if (pid2==-1){
		perror("fork error");
		exit(0x55);
    }
    else if (pid2==0) { /* Grandson of Grandparent (Child of parent) */
  	    char* argv2[] = {"tail","-n","2",0};
    	close (STDIN); /* Make second child understand that his input is from the pipe */
    	if (debug) fprintf(stderr,"(child2>redirecting stdin to read end of the pipe...)\n");
    	fd2 = dup (pipeArr[0]); /* Redirecting second child input to the pipe */
    	if (debug) fprintf(stderr,"(parent_process>closing the read end of the pipe...)\n");
    	close (pipeArr[0]);  /* Comment read end of pipe */
		if (debug) fprintf(stderr,"(child2>going to execute cmd: tail -n 2...)\n");
		execvp(argv2[0],argv2);
    }
    else { /* Son of Grandparent (Parent) */

    	/* Close the read end of the pipe. Not mandatory */
    	close (pipeArr[0]); /* Causing OS send SIGPIPE in case of another scenarios of another input about to come */

		/* Wait for second child to terminate */
		int status2;
		if (debug) fprintf(stderr,"(parent_process>waiting for child processes to terminate...)\n");
		waitpid (pid,&status,WUNTRACED); /* meaning wait for any child process whose process group ID is equal to that of the calling process. */
		waitpid (pid2,&status2,WUNTRACED);
		if (debug) fprintf(stderr,"(parent_process>exiting...)\n");
    }

}