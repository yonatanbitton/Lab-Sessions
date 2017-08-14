#define _GNU_SOUR
#define SYS_WRITE 4
#define SYS_GETDENTS 141
#define SYS_OPEN 5
#define STDOUT 1
#define STDERR 2
#define __NR_exit                 1
#define __NR_fork                 2
#define __NR_read                 3
#define __NR_write                4
#define __NR_open                 5
#define __NR_close                6
#define __NR_creat                8
#define BUF_SIZE 8192
#include "util.h"
extern int system_call();


void debugPrint(int sysCallNumber, int sysCallReturnCode, int length){
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
    else if (length==-1){
      char msg2[] = "System call return code: None";
      system_call(__NR_write,STDERR,msg2,strlen(msg2));
      system_call(__NR_write,STDERR,en,1);
    }
    else {
      char msg2[] = "Name length: ";
      system_call(__NR_write,STDERR,msg2,strlen(msg2));
      buf = itoa (length);
      system_call(__NR_write,STDERR,buf,strlen(buf)); 
      system_call(__NR_write,STDERR,en,1);
    }
}

struct linux_dirent {
 
   long           d_ino;
   int          d_off;  
   unsigned short d_reclen;
   char           d_name[];
};

int main(int argc, char *argv[])
{
   int fd, nread;
   char buf[BUF_SIZE];
   struct linux_dirent *d;
   int bpos;
   char d_type;
   char ent[]="\n";
   int j=1;
   int suffixLocation= -1;
   int i;
   int debug=0;
   for (i=1; i<argc; i++){
          if (argv[i][0]=='-' && argv[i][1]=='d')
             debug=1;
   }
   for (j=1; j<argc; j++){                
      if ((strcmp(argv[j],"-s")==0)) {
            j+=1;
            suffixLocation=j;           
      }
   }
   fd=system_call(SYS_OPEN,".", 0 , 0);
   if (fd == -1){ 
        char errorMsg[] = "open";
        int return_num1=system_call(SYS_WRITE,STDOUT,errorMsg,4);
        if (debug==1) 
              debugPrint(__NR_write,return_num1,-1);                      
        return(0x55); /* 00x55 hexa */
   }

   for (;;) {
       nread=system_call(SYS_GETDENTS,fd, buf, BUF_SIZE);

       if (nread == -1)
       { 
          char errorMsg[] = "getdents";
          int return_num2=system_call(SYS_WRITE,STDOUT,errorMsg,7);
          if (debug==1)
               debugPrint(__NR_write,return_num2,-1);
          return(0x55); /* 00x55 hexa */
       }
       if (nread == 0)
           break;

       for (bpos = 0; bpos < nread;) {
           d = (struct linux_dirent *) (buf + bpos);
           d_type = *(buf + bpos + d->d_reclen - 1);
            if (argc>2 && suffixLocation!=-1){
                if (d_type!=4 && d->d_name[strlen(d->d_name)-1]==(*argv[suffixLocation])){                          
                   system_call(SYS_WRITE,STDOUT,d->d_name,strlen(d->d_name));
                   system_call(SYS_WRITE,STDOUT,ent,1);
                   if (debug==1) 
                         debugPrint(__NR_write,666,strlen(d->d_name));                       
                }
           } 
           else 
           {
           	   if (d_type!=4){
                 	 system_call(SYS_WRITE,STDOUT,d->d_name,strlen(d->d_name));
           		     system_call(SYS_WRITE,STDOUT,ent,1);
                   if (debug==1) 
                        debugPrint(__NR_write,666,strlen(d->d_name));
           	   }                   
           }
           bpos += d->d_reclen;
           
       }
   }

   return 0;
}
