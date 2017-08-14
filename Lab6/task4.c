#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include "LineParser.h"
#include <fcntl.h>

#define STDOUT 1
#define STDERR 2

struct pair {
    char* name;
    char* value; 
};

typedef struct pair pair;
typedef struct myLink myLink;


struct myLink {
    pair* p;
    myLink* next;
};


/* Print the data of every myLink in list. Each item followed by a newline character. */
void list_print(myLink *pair_list){
    myLink* curr = pair_list;
    int k=1;
    while(curr != NULL){
        printf("Pair name: %s\n",curr->p->name);
        printf("Pair value: %s\n",curr->p->value);
        curr = curr->next;
        k++;

    }
}

/* If the list was succesful - return 1 (and overrides). Else - returns 0 */
int list_search(myLink* pair_list, pair* pairToInsert){
    myLink* curr = pair_list;
    if (curr==NULL) return 0;

    /* Else: list is not null, the curr will be the first node */
    while (curr!= NULL){
        pair* comparedPair = curr->p;
        if (strcmp(comparedPair->name,pairToInsert->name)==0) 
        {
            printf("Override\n");
            curr->p->value = pairToInsert->value;
            return 1;
        }

        curr = curr->next;
    }
    return 0;
}

/* If the list was succesful - return 1 (and overrides). Else - returns 0 */
char* getMappedVal(myLink* pair_list, char* arg){

    char* newArg = (char*)malloc(strlen(arg));
    int i;
 
    strcpy(newArg,arg+1);
    newArg[strlen(arg)]=0;

    myLink* curr = pair_list;
    if (curr==NULL) {
        printf("curr==null on getMappedVal\n");
        return 0;
    }
    while (curr!= NULL){
        pair* comparedPair = curr->p;

        if (strncmp(comparedPair->name,newArg,strlen(newArg))==0) 
        {
            printf("found \n");
                return comparedPair->value;
            
        }

        curr = curr->next;
    }
    return 0;
}

/* handleVal runs the value that was paired with the name parameter.
   But if the name was "~", the function returns home directory.
*/
char* handleVal(myLink* pair_list, char* name){
    myLink* curr = pair_list;
    cmdLine* box;
    if (curr==NULL) {
        fprintf(stderr, "%s\n", "null list, variable not found");
        return NULL; 
    }
    int found=0;
    /* Else: list is not null, the curr will be the first node */
    while (curr!= NULL){
        pair* comparedPair = curr->p;
        if (strncmp(comparedPair->name,name,strlen(comparedPair->name)-1)==0) 
        {
        	if (strcmp(name,"~")==0){
        		return comparedPair->value;
        	}
        	else {
        		char* val = comparedPair->value;
	            box = parseCmdLines(val);  
	            execute(box);
	            found=1;
        	} 
        }

        curr = curr->next;
    }
    free(name);
    if (found==0) fprintf(stderr, "%s\n", "variable not found");
    return NULL;
}

void deletePair(myLink* pair_list, char* name){
    printf("Before: \n");
    list_print(pair_list);      
    myLink* curr = pair_list;
    myLink* currNext = curr->next;
    cmdLine* box;
    if (curr==NULL) {
        fprintf(stderr, "%s\n", "variable not found");
        return; 
    }
    /* Else: list is not null, the curr will be the first node */
    else if (strncmp(curr->p->name,name,strlen(name))==0){
        list_print(pair_list);
        pair_list=pair_list->next;
        printf("\n\n\n");
        return;
    }
    else while (currNext!= NULL){
        pair* comparedPair = currNext->p;
        if (strncmp(comparedPair->name,name,strlen(name))==0) 
        {
            /* Here we want to remove the current */
            curr->next = currNext->next;
            free(currNext->p);
            free(currNext);
            free(name);
            return; 
        }
        currNext = currNext->next;
        curr = curr->next;
    }
    free(name);
    printf("\n\n\n");
    printf("After (No change): \n");
    list_print(pair_list);
    return;
}

/* Add a new myLink with the given data to the list 
    (either at the end or the beginning, depending on what your TA tells you),
    and return a pointer to the list (i.e., the first myLink in the list).
    If the list is null - create a new entry and return a pointer to the entry. */
myLink* list_append(myLink* pair_list, pair* pairToInsert){
    if (list_search(pair_list,pairToInsert)==0)
        {
            myLink* curr = pair_list;
            myLink* pairNode=(struct myLink*)malloc(sizeof(struct myLink));
            pairNode->p=pairToInsert;
            pairNode->next=NULL;
            if (curr==NULL) return pairNode;
            /* Else: list is not null, the curr will be the first node */
            while ((curr->next)!= NULL)
                curr = curr->next;
            curr->next=pairNode;
            return pair_list; /* The first node will still be the first node */
        }
    else return pair_list; 
}

/* Free the memory allocated by the list. I allocated place for each virus and for each myLink!*/
void list_free(myLink *pair_list) {
    if (pair_list == NULL) return;
    else {
        list_free(pair_list->next);
        free(pair_list->p);
        free(pair_list);
    }
}


void waitpid(int a,int* b, int c);

void execute(cmdLine *pCmdLine){
    int in_des=0;
    int out_des=1;
    pid_t pid;
    pid = fork();
    if (pid==-1){
        perror("fork error");
        exit(0x55);
    }
    else if (pid==0){
        char* program = pCmdLine->arguments[0];
        char* args[PATH_MAX-1];
        int i;
        for (i=0; i<pCmdLine->argCount; i++){
            args[i]=pCmdLine->arguments[i];
        }
        int returnVal=1;

        /* Handle input and output if exists */
        if (pCmdLine->inputRedirect != NULL){
            in_des = open(pCmdLine->inputRedirect,O_RDONLY);

            dup2(in_des,STDIN_FILENO);
        }
        if (pCmdLine->outputRedirect != NULL){
            out_des = open(pCmdLine->outputRedirect,O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
            dup2(out_des,STDOUT_FILENO);
        }

        returnVal=execvp(program,args);
        if (returnVal==-1){
            write(STDERR,"Error on execv\n",strlen("Error on execv\n"));
            exit(0x55);
        }
    }
    else{
        int status;
        if (pCmdLine->blocking==1){
            printf("I'm waiting! \n");
            (void)waitpid(pid,&status,0);
            printf("Back! \n");
            char* cwd2;
            char buff2[PATH_MAX + 1];
            cwd2 = getcwd( buff2, PATH_MAX + 1 );
            printf ("After child finished, working diretory is: %s\n",cwd2);
        }
        freeCmdLines(pCmdLine);
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
    myLink* pair_list;
    pair_list=NULL;
    

    pair* pairToInsert=(struct pair*)malloc(sizeof(struct pair));
    pairToInsert->name="~";
    pairToInsert->value=getenv("HOME");
    pair_list=list_append(pair_list,pairToInsert);

    while (1){
        signal(SIGQUIT, handleSignal); 
        signal(SIGTSTP, handleSignal); /* CTRL+Z */     
        signal(SIGCHLD, handleSignal);
    	fgets(input,2048,stdin);    	
    	if (input[0]=='q' || strncmp("quit",input,4)==0) {
    		break;
    	}
        else if (strncmp("set",input,3)==0){
            /* set x y */
            box = parseCmdLines(input);  
            pair* pairToInsert=(struct pair*)malloc(sizeof(struct pair));
            pairToInsert->name=box->arguments[1];
            pairToInsert->value=box->arguments[2];
            pair_list=list_append(pair_list,pairToInsert);
        }
        else if (strncmp("env",input,3)==0){
            list_print(pair_list);
        }
        else if (strncmp("delete",input,6)==0){
            box = parseCmdLines(input);  
            printf("Going to delete %s\n",box->arguments[1]);
            deletePair(pair_list,box->arguments[1]);
        }
        else if (input[0]!=10){
            box = parseCmdLines(input);  
	        if (strncmp(input,"cd ",3)==0){
	            printf("arguments[1]: %s \n",box->arguments[1]);
		            if (strncmp(box->arguments[1],"~",1)==0){
		            	char* home = handleVal(pair_list,"~");
		            	printf("Home: %s\n", home);
		            	chdir(home);
		            	char* cwd2;
					    char buff2[PATH_MAX + 1];
					    cwd2 = getcwd( buff2, PATH_MAX + 1 );
					    /*printf ("After, working diretory is: %s\n",cwd2);*/
		            }
	            	else changeDir(box->arguments[1]);  
	        } else {
                    /*int i; 
                    for (i = 0; i < box->argCount; i++)
                    {
                        if (strncmp("$",box->arguments[i],1)==0)
                            box->arguments[i]=getMappedVal(pair_list,box->arguments[i]);
                    }*/
                /* Replace $'s with it's value , and send it to execute */
                execute(box); 
            } 
	        
        }
	}
    return 0;
}