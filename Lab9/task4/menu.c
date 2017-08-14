#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#define STDIN 0 
#define STDOUT 1 
#define STDERR 2

void *map_start; /* will point to the start of the memory mapped file */
int currentFD;
Elf32_Ehdr *header; 
int debugFlag=0;
int fileSize; 


/* Activate\De-activate debug flag */
void toggleDebug() {
   if (debugFlag==0){
   		debugFlag=1;
   		printf("Debug flag now on\n");
   }
   else {
   		debugFlag=0;
   		printf("Debug flag now off\n");
   }
   
}

void examineELF(int argc, char** argv) {
   char* fileName = malloc (80);
   int debugBefore = debugFlag;
   if (argc<2) {
   	  perror("no args :( ");
   	  currentFD=-1;
	  exit(-4);
   }
   debugFlag = debugBefore;
   currentFD=open(argv[1], O_RDWR);
   if(currentFD<0){
   	  perror("open failed");
   	  currentFD=-1;
	  exit(-4);
   }
   int fileSize = lseek(currentFD, 0, SEEK_END)+1;
   lseek(currentFD, 0,SEEK_SET);
   printf("Debug: the fileSize is %d\n",fileSize);
   if ( (map_start = mmap(0, fileSize, PROT_READ | PROT_WRITE , MAP_SHARED, currentFD, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      close(currentFD);
      currentFD=-1;
      exit(-4);
   }
   header = (Elf32_Ehdr *) map_start;
   if (debugFlag==1){
       printf("data encoding scheme (e_ident[EI_DATA]):          %x\n", header->e_ident[EI_DATA]);
	   printf("entry point is:				    	%x\n",header->e_entry);
	   printf("section header offset (e_shoff): 	        %d\n",header->e_shoff); /* change */
	   printf("number of section header entries (e_shnum):      %d\n",header->e_shnum);
	   printf("section header entry size (e_shentsize): 	 %d\n",header->e_shentsize); 
	   printf("program header offset (e_phoff): 	          %d\n",header->e_phoff);
	   printf("number of program header entries (e_phnum):	  %d\n",header->e_phnum);
	   printf("size of program header entries (e_phentsize):     %d\n",header->e_phentsize);
   }
   printProgramHeaders();
}  


void printProgramHeaders(){
   int i;
   if (currentFD==-1) {  
     	  perror("Printing Failed");
     	  return;
   } 
   Elf32_Phdr* programTable = (Elf32_Phdr *)((char *)map_start + header->e_phoff); /* phoff is the offset of the program header table of the file */
     
   printf("Type     Offset         VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align\n");
   for (i=0;i<header->e_phnum; i++){ /* e_shnum is the number of section headers in the table */
   		printf("%x 	",programTable[i].p_type);
   		printf("0x%06x 	",programTable[i].p_offset);
   		printf("0x%06x 	",programTable[i].p_vaddr);
   		printf("0x%06x 	",programTable[i].p_paddr);
   		printf("0x%06x 	",programTable[i].p_filesz);
   		printf("0x%06x 	",programTable[i].p_memsz);
   		printf("0x%06x 	",programTable[i].p_flags);
   		printf("0x%06x 	",programTable[i].p_align);
   		printf("\n");
   }
   
}   


/* unmaps and close all used files */
void quit(){
	munmap(map_start,fileSize);
	close(currentFD);
	printf("quiting...\n");
	exit(0);
}
 


int main(int argc, char **argv){
	examineELF(argc,argv);
   printf("sizeof Elf32_Phdr is: %x %d\n",sizeof(Elf32_Phdr),sizeof(Elf32_Phdr) );
}