#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include "LineParser.h"
void waitpid(int a,int* b, int c);

#define STDOUT 1
#define STDERR 2

void execute(cmdLine *pCmdLine){
    char* cwd;
    char buff[PATH_MAX + 1];
	char* program = pCmdLine->arguments[0];
	char* args[PATH_MAX-1];
	int i;
	for (i=1; i<pCmdLine->argCount; i++){
		args[i]=pCmdLine->arguments[i];
	}
	int returnVal=1;
	cwd = getcwd( buff, PATH_MAX + 1 );
    int size = strlen(cwd)+strlen(program)+1;  
    char fullPath[size];
    int k,l;
    for (k=0; k<strlen(cwd); k++)
    	fullPath[k]=cwd[k];
    fullPath[strlen(cwd)]='/';
    for (l=0; l<strlen(program); l++){	
    	fullPath[1+l+strlen(cwd)]=program[l];
    }
    fullPath[50]=0;
    printf("%s\n",fullPath);
	returnVal=execvp(program,args);
	if (returnVal==-1){
		write(STDERR,"Error on execv\n",strlen("Error on execv\n"));
		exit(0x55);
	}
}

void changeDir(){
    char* cwd;
    char buff[PATH_MAX + 1];
    cwd = getcwd( buff, PATH_MAX + 1 );
    printf ("Before, working diretory is: %s\n",cwd);

    char *directory = "/tmp";
    int ret;
    ret = chdir (directory);

    char* cwd2;
    char buff2[PATH_MAX + 1];
    cwd2 = getcwd( buff2, PATH_MAX + 1 );
    printf ("After, working diretory is: %s\n",cwd2);
}

int main(int argc, char *argv[]){


    char input[2048];
    cmdLine* box;
	int j;
    
    while (1){
    	fgets(input,2048,stdin);
    	/*printf("Hello %s",input);*/
        box = parseCmdLines(input);

    	if (input[0]=='q') {
    		break;
    	}
        if (input[0]=='c' && input[1]=='d' && input[2]==' '){
            char* cwd;
            char buff[PATH_MAX + 1];
            cwd = getcwd( buff, PATH_MAX + 1 );
            printf ("Before, working diretory is: %s\n",cwd);
            char* directory = box->arguments[1];
            /*printf("directory: %s\n",directory);*/
            int ret;
            ret = chdir (directory);
            char* cwd2;
            char buff2[PATH_MAX + 1];
            cwd2 = getcwd( buff2, PATH_MAX + 1 );
            printf ("After, working diretory is: %s\n",cwd2);
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
                printf("I'm waiting!\n");
                (void)waitpid(pid,&status,0);
                printf("Back! \n");
            }
            freeCmdLines(box);
            }
    	
        }
    }
    return 0;
}