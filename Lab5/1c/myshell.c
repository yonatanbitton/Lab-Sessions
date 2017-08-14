#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include "LineParser.h"

#define STDOUT 1
#define STDERR 2

void waitpid(int a,int* b, int c);


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

void handleSignal(int signalNum){
    printf("Ignoring this signal: %s\n",strsignal(signalNum));
}

void changeDir(char* directory){
    char* cwd;
    char buff[PATH_MAX + 1];
    cwd = getcwd( buff, PATH_MAX + 1 );
    printf ("Before, working diretory is: %s\n",cwd);
    printf("directory: %s\n",directory);
    int ret;
    strcat(cwd,"/");
    strcat(cwd,directory);
    printf("Path is: %s\n",cwd);
    ret = chdir (cwd);
    if (ret == -1)
    	fprintf(stderr, "Error on changing the directory (Maybe not exists!)\n");
    char* cwd2;
    char buff2[PATH_MAX + 1];
    cwd2 = getcwd( buff2, PATH_MAX + 1 );
    printf ("After, working diretory is: %s\n",cwd2);
}

int main(int argc, char *argv[]){


    char input[2048];
    cmdLine* box;
	
    
    while (1){
        signal(SIGQUIT, handleSignal); 
        signal(SIGTSTP, handleSignal); /* CTRL+Z */     
        signal(SIGCHLD, handleSignal);
    	fgets(input,2048,stdin);    	
    	if (input[0]=='q') {
    		break;
    	}
    	box = parseCmdLines(input);  
    	if (strncmp(input,"cd ",3)==0){
    		changeDir(box->arguments[1]);           
        } else {  	
	    	pid_t pid;
	    	pid = fork();
	    	if (pid==-1){
	            perror("fork error");
	            exit(0x55);
	        }
	        else if (pid==0)
		   		execute(box);
	        else{
	            int status;
	            if (box->blocking==1){
	                printf("I'm waiting! \n");
	                (void)waitpid(pid,&status,0);
	                printf("Back! \n");
	                char* cwd2;
		            char buff2[PATH_MAX + 1];
		            cwd2 = getcwd( buff2, PATH_MAX + 1 );
		            printf ("After child finished, working diretory is: %s\n",cwd2);
	            }
	            freeCmdLines(box);
	        }
    	}
	}
  	   
    return 0;
}