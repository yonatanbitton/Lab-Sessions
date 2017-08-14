
#define STDOUT 1
#define STDERR 2
#define __NR_exit                 1
#define __NR_fork                 2
#define __NR_read                 3
#define __NR_write                4
#define __NR_open                 5
#define __NR_close                6
#define __NR_creat                8
#include "util.h"
extern int system_call();

void debugPrint(int sysCallNumber, int sysCallReturnCode){
	char en[1]="\n";
	char* buf;
	char msg1[] = "System call number: ";
    system_call(__NR_write,STDERR,msg1,strlen(msg1));
    char* sysNumBuf = itoa(sysCallNumber);
    system_call(__NR_write,STDERR,sysNumBuf,strlen(sysNumBuf));
    system_call(__NR_write,STDERR,en,1);
    if (sysCallReturnCode!=666){
	    char msg2[] = "System call return code: ";
	    system_call(__NR_write,STDERR,msg2,strlen(msg2));
	    buf = itoa(sysCallReturnCode);
	    system_call(__NR_write,STDERR,buf,strlen(buf));
	    system_call(__NR_write,STDERR,en,1);
    }
    else {
	    char msg2[] = "System call return code: None";
	    system_call(__NR_write,STDERR,msg2,strlen(msg2));
        system_call(__NR_write,STDERR,en,1);
    }
}

int main(int argc, char* argv[]){


int i;
int inputFile=0;
int input_identifier=0;
int output_identifier=STDOUT;
int debug=0;
char en[1]="\n";

for (i=1; i<argc; i++){

        if (argv[i][0]=='-' && argv[i][1]=='d'){
                debug=1;
            }
}

for (i=1; i<argc; i++){
                
        if (argv[i][0]=='-' && argv[i][1]=='i') {
            input_identifier=system_call(__NR_open,argv[++i],2);
            /*input_identifier=open(argv[++i],2);*/
            inputFile=1;
            if (debug==1) 
            	debugPrint(__NR_open,input_identifier);
                    
        }
        
        /*if ((strcmp(argv[i],"-o")==0)) {*/
        if (argv[i][0]=='-' && argv[i][1]=='o'){
            output_identifier=system_call(__NR_open,argv[++i],2 | 64, 0777);
            /*output_identifier=creat(argv[++i],0666);*/
            if (debug==1)
            	debugPrint(__NR_open,output_identifier);               

        }     
}
if (debug==1 && inputFile==0){
        system_call(__NR_write,STDERR,"stdin\n",strlen("stdin\n"));

}
    
if (debug==1 && output_identifier==STDOUT){
        system_call(__NR_write,STDERR,"stdout\n",strlen("stdout\n"));
}
        

if (input_identifier<0){
    char errorMsg[] = "Problem opening file\n";
    system_call(__NR_write,STDOUT,errorMsg,strlen(errorMsg));
    if (debug==1) 
    	debugPrint(__NR_write,666);                   
    return 1;
}


char cBuf[1];
int c;

while ((c=system_call(__NR_read,input_identifier,cBuf,1)!=0)){

    if (cBuf[0]>='A' && cBuf[0]<='Z') {
        cBuf[0]+=32;
    }
    system_call(__NR_write,output_identifier,cBuf,1);
    
}
    if (inputFile==1) {
        int back2=system_call(__NR_write,output_identifier,en,1);
        if (debug==1) 
        	debugPrint(__NR_write,back2);
    }

return 0; 
}