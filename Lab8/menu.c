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

char* typeToStr(int sh_type){
	/* Legal values for sh_type (section type).  */
 
	switch(sh_type) {
		case 0:  
	   return "NULL";
		
	   case 1:
	      return "PROGBITS";	
	  
	   case 2:
	      return "SYMTAB";	
	 
	   case 3:
	      return "STRTAB";
		
	   case 4:
	      return "RELA";	
	  
	   case 5:
	      return "HASH";	
	 
	   case 6:
	      return "DYNAMIC";
		
	   case 7:
	      return "NOTE";	
	  
	   case 8:
	      return "NOBITS";	
	  
	   case 9:
	      return "REL";
		
	   case 10:
	      return "SHLIB";	
	  
	   case 11:
	      return "DYNSYM";	

	   case 14:
	      return "INIT_ARRAY";	
	 
	   case 15:
	      return "FINI_ARRAY";
		
	   case 0x6ffffffe:
	      return "VERNEED";	
	  
	   case 0x6fffffff:
	      return "VERSYM";	
	 
	   case 0xfff1:
	      return "UNDEF";	

  	defult: 
  	return 0; 
	} 
}

void printType(int sh_type){
	/* Legal values for sh_type (section type).  */
 
 	/*printf("Received sh_type: %d \n",sh_type);*/
	switch(sh_type) {
		case 0:  
	 	 printf("R_386_NONE ");
		 break; 

	   case 1:
	      printf("R_386_32 ");	
	  	  break; 

	   case 2:
	      printf("R_386_PC32 ");
	      break; 

	   case 3:
	      printf("R_386_32 ");
		  break; 

	   case 6:
	      printf("R_386_GLOB_DAT ");
		  break; 

	   case 7:
	      printf("R_386_JMP_SLOT ");
	      break; 
	 
		defult: 
		   printf("Unrecognized ");
	}
}
 

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

void examineELF() {
   char* fileName = malloc (80);
   int debugBefore = debugFlag;
   printf("Please enter fileName\n");
   fgets(fileName,80,stdin);
   debugFlag = debugBefore;
   fileName[strlen(fileName)-1]=0;   
   currentFD=open(fileName, O_RDWR);
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
   printf("first three bytes: 			      %c %c %c\n",header->e_ident[EI_MAG1],header->e_ident[EI_MAG2], header->e_ident[EI_MAG3]);
   if (header->e_ident[EI_MAG1]!='E' || header->e_ident[EI_MAG2]!='L' || header->e_ident[EI_MAG3]!='F') {
   	printf("I refuse to continue. It's not an ELF header :O\n");
    currentFD=-1;
   	quit(1);
   }
   printf("data encoding scheme (e_ident[EI_DATA]):          %x\n", header->e_ident[EI_DATA]);
   printf("entry point is:				    %x\n",header->e_entry);
   printf("section header offset (e_shoff): 	       %d\n",header->e_shoff); /* change */
   printf("number of section header entries (e_shnum):      %d\n",header->e_shnum);
   printf("section header entry size (e_shentsize): 	 %d\n",header->e_shentsize); 
   printf("program header offset (e_phoff): 	         %d\n",header->e_phoff);
   printf("number of program header entries (e_phnum):	  %d\n",header->e_phnum);
   printf("size of program header entries (e_phentsize):    %d\n",header->e_phentsize);
}  


void printSecNames(){


   if (currentFD==-1) {  
     	  perror("Printing Failed");
     	  return;
   } 
   Elf32_Shdr* sections = (Elf32_Shdr *)((char *)map_start + header->e_shoff); /* shoff is the offset of the section header table of the file */
   Elf32_Shdr* sh_strtab = &sections[header->e_shstrndx]; /* shstrndx is the index of the string table of the section header's names */
   /* So now sh_strtab points to the section that holds the names array */
   const char * const sh_strtab_p = map_start + sh_strtab->sh_offset;
   int i;
   printf("[Nr] Name Addr Off Size Type\n");
   for (i=0;i< header->e_shnum; i++){ /* e_shnum is the number of section headers in the table */
   		printf("[%d] %s ",i, sh_strtab_p + sections[i].sh_name); 
   		/* sh_strtab_p is the names array, and the sections[i].sh_name is the index */
   		printf("%08x ",sections[i].sh_addr);
   		printf("%06x ",sections[i].sh_offset);  		
   		printf("%06x ",sections[i].sh_size);  
   		int sh_type = sections[i].sh_type;
   		char* sh_type_str = typeToStr(sh_type);
   		if (sh_type_str!=0) printf("%s",sh_type_str);  
   			else printf("%x",sections[i].sh_type); 
   		printf("\n");
   }
   if (debugFlag==1){
   		printf("Debug print: index to section that holds the string table (shstrndx): %d \n",header->e_shstrndx);
   		printf("Debug print: Offset of string table (sh_strtab->sh_offset): %x \n",sh_strtab->sh_offset);

   }
}   

char* getName(int index){
   Elf32_Shdr* sections = (Elf32_Shdr *)((char *)map_start + header->e_shoff); /* shoff is the offset of the section header table of the file */
   Elf32_Shdr* sh_strtab = &sections[header->e_shstrndx]; /* shstrndx is the index of the string table of the section header's names */
   /* So now sh_strtab points to the section that holds the names array */
   const char * const sh_strtab_p = map_start + sh_strtab->sh_offset;
	/* e_shnum is the number of section headers in the table */
	return (sh_strtab_p + sections[index].sh_name); 
	/* sh_strtab_p is the names array, and the sections[i].sh_name is the index */
}

void printSymbols(){

   if (currentFD==-1) {  
     	  perror("Printing Failed");
     	  return;
   } 
   Elf32_Shdr* sections = (Elf32_Shdr *)((char *)map_start + header->e_shoff); /* shoff is the offset of the section header table of the file */
   Elf32_Sym* symtab;

   int i;
	for (i = 0; i < header->e_shnum; i++){
    	if (sections[i].sh_type==SHT_SYMTAB) {
    		/*printf("indexes are: %d\n",i );
    		printf("this sections size is: %x\n",sections[i].sh_size);*/
        	symtab = (Elf32_Sym *)((char *)map_start + sections[i].sh_offset);
       		/*printf("(sizeof(Elf32_Sym)): %x\n", (sizeof(Elf32_Sym)));*/
       		int entries=sections[i].sh_size/(sizeof(Elf32_Sym));
       		/*printf("dec entries: %d , hex entries: %x \n",entries,entries);*/
       		 int j;
       		 int off=0;
       		 int placeOfStrTab;
       		 printf(" Num: Value  Ndx Section_Name Symbol_Name\n");
       		 for (j = 0; j < header->e_shnum; j++)
    			if ( sections[j].sh_type==SHT_STRTAB) {
    				off = sections[j].sh_offset;
    				placeOfStrTab=j;
    			}


		     Elf32_Shdr* strtab = &sections[j]; /* shstrndx is the index of the string table of the section header's names */

		     const char * const strtab_p = map_start + off;


       		 for(j=0;j<entries;j++){
       		 	printf("[%d] %08x ",j,symtab[j].st_value); 
       		 	if (symtab[j].st_shndx==0)
       		 		printf("UND ");
       		 	else if (symtab[j].st_shndx==0xfff1)
       		 		printf("ABS ");
       		 	else printf("%d ", symtab[j].st_shndx);
       		 	if (i==32) printf("SYMTAB ");
       		 		else if (i==4) printf("DYNSYM ");
       		 	/*printf(" symtab[j].st_name%d ",symtab[j].st_name );*/
       		 	printf("%s", strtab_p + symtab[j].st_name);
        		printf("\n");
       		 }


        }
        else if (sections[i].sh_type==SHT_DYNSYM) {
    		/*printf("indexes are: %d\n",i );
    		printf("this sections size is: %x\n",sections[i].sh_size);*/
        	symtab = (Elf32_Sym *)((char *)map_start + sections[i].sh_offset);
       		/*printf("(sizeof(Elf32_Sym)): %x\n", (sizeof(Elf32_Sym)));*/
       		int entries=sections[i].sh_size/(sizeof(Elf32_Sym));
       		/*printf("dec entries: %d , hex entries: %x \n",entries,entries);*/
       		 int j;
       		 int off=0;
       		 int placeOfStrTab;
       		 printf(" Num: Value  Ndx Section_Name Symbol_Name\n");
       		 for (j = 0; j < header->e_shnum; j++)
    			if ( sections[j].sh_type==SHT_STRTAB) {
    				off = sections[j].sh_offset;
    				placeOfStrTab=j;
    				break; /* Stops at first String Table (DYNSTR) */
    			}


		     Elf32_Shdr* dyn_strtab = &sections[placeOfStrTab]; /* shstrndx is the index of the string table of the section header's names */

		     const char * const dyn_strtab_p = map_start + off;


       		 for(j=0;j<entries;j++){
       		 	printf("[%d] %08x ",j,symtab[j].st_value); 
       		 	if (symtab[j].st_shndx==0)
       		 		printf("UND ");
       		 	else if (symtab[j].st_shndx==0xfff1)
       		 		printf("ABS ");
       		 	else printf("%d ", symtab[j].st_shndx);
       		 	if (i==32) printf("SYMTAB ");
       		 		else if (i==4) printf("DYNSYM ");
       		 	/*printf(" symtab[j].st_name%d ",symtab[j].st_name );*/
       		 	printf("%s", dyn_strtab_p + symtab[j].st_name);
        		printf("\n");
       		 }


        }
    }
   if (debugFlag==1){
   		printf("Debug print: index to section that holds the string table (shstrndx):");

   }
}

void linkCheck(){
   int hasStart=0;
   if (currentFD==-1) {  
     	  perror("currentFD is -1. Error.");
     	  return;
   } 
   Elf32_Shdr* sections = (Elf32_Shdr *)((char *)map_start + header->e_shoff); /* shoff is the offset of the section header table of the file */
   Elf32_Sym* symtab;

   int i;
   for (i = 0; i < header->e_shnum; i++){
    	if (sections[i].sh_type==SHT_SYMTAB) {
        	symtab = (Elf32_Sym *)((char *)map_start + sections[i].sh_offset);
       		int entries=sections[i].sh_size/(sizeof(Elf32_Sym));
       		 int j;
       		 int off=0;
       		 int placeOfStrTab;
      		 for (j = 0; j < header->e_shnum; j++)
    			if ( sections[j].sh_type==SHT_STRTAB) {
    				off = sections[j].sh_offset;
    				placeOfStrTab=j;
    			}


		     Elf32_Shdr* strtab = &sections[placeOfStrTab]; /* shstrndx is the index of the string table of the section header's names */

		     const char * const strtab_p = map_start + off;


       		 for(j=0;j<entries;j++){
       		 	char* tmpName = strtab_p + symtab[j].st_name;
       		 	if (strcmp(tmpName,"_start")==0)
       		 		hasStart=1;
       		 }
        }
	}
	if (hasStart==1) printf("_start check: PASSED\n");
		else printf("_start check: FAILED\n");
}


void printIndexAtSymTab(int index){

   Elf32_Shdr* sections = (Elf32_Shdr *)((char *)map_start + header->e_shoff); /* shoff is the offset of the section header table of the file */
   Elf32_Sym* dynSymbol;
   int i;
	for (i = 0; i < header->e_shnum; i++){
        	if (sections[i].sh_type==SHT_DYNSYM) {

        	 dynSymbol = (Elf32_Sym *)((char *)map_start + sections[i].sh_offset);
        	 int j;
       		 int off=0;
       		 int placeOfStrTab;
       		 for (j = 0; j < header->e_shnum; j++)
    			if ( sections[j].sh_type==SHT_STRTAB) {
    				off = sections[j].sh_offset;
    				placeOfStrTab=j;
    				break; /* Stops at first String Table (DYNSTR) */
    			}
		     Elf32_Shdr* dyn_strtab = &sections[placeOfStrTab]; /* shstrndx is the index of the string table of the section header's names */
		     const char * const dyn_strtab_p = map_start + off;
       		 	printf("%s ", dyn_strtab_p + dynSymbol[index].st_name);    
        }
    }
}


void relocTables(){
   if (currentFD==-1) {  
     	  perror("Printing Failed");
     	  return;
   } 
   Elf32_Shdr* sections = (Elf32_Shdr *)((char *)map_start + header->e_shoff); /* shoff is the offset of the section header table of the file */
   Elf32_Rel* relTab;
   Elf32_Rela* relAAAtab;
   int i;
	for (i = 0; i < header->e_shnum; i++){
    	if (sections[i].sh_type==SHT_REL || sections[i].sh_type==SHT_RELA) {
    		
        	relTab = (Elf32_Rel *)((char *)map_start + sections[i].sh_offset);
        	int entries=sections[i].sh_size/(sizeof(Elf32_Rel));
       		 int j;
       		 int off=0;
       		 int placeOfDynSymTab;
       		 printf("  Offset     Info    Type      Sym.Value  Sym. Name\n");
       		 for (j = 0; j < header->e_shnum; j++)	
    			if ( sections[j].sh_type==SHT_DYNSYM) {
    				off = sections[j].sh_offset;
    				placeOfDynSymTab=j;
    				break; /* To get to dynstr */
    			}

		     Elf32_Shdr* dynSymTab = &sections[placeOfDynSymTab]; /* shstrndx is the index of the string table of the section header's names */
		     const char * const dynSymTab_p = map_start + off;
       		 for(j=0;j<entries;j++){
         		int strOffset = ELF32_R_SYM(relTab[j].r_info); /* now strOffset holds the correct index of the name in the dyn Symbol Table array */
        		printf("%08x ",relTab[j].r_offset);
        		printf("%08x ",relTab[j].r_info);
        		int tmpType = ELF32_R_TYPE(relTab[j].r_info);
        		printType(tmpType);
        		printIndexAtSymTab(strOffset);
        		printf("\n");
       		 }
       		 printf("\n");

        }
    }
}

void relocSemantics(){
   if (currentFD==-1) {  
     	  perror("Printing Failed");
     	  return;
   } 
   Elf32_Shdr* sections = (Elf32_Shdr *)((char *)map_start + header->e_shoff); /* shoff is the offset of the section header table of the file */
   Elf32_Rel* relTab;
   Elf32_Rela* relAAAtab;
   int i;
	for (i = 0; i < header->e_shnum; i++){
    	if (sections[i].sh_type==SHT_REL || sections[i].sh_type==SHT_RELA) {
    		
        	relTab = (Elf32_Rel *)((char *)map_start + sections[i].sh_offset);
        	int entries=sections[i].sh_size/(sizeof(Elf32_Rel));
       		 int j;
       		 int off=0;
       		 int placeOfDynSymTab;
       		 char* name = getName(i);
       		 printf("Relocation section '%s' at offset 0x%x contains %d entries: \n" ,name,sections[i].sh_offset,entries);
       		 printf("  Offset     Info    Type      Sym.Value  Sym. Name\n");
       		 for (j = 0; j < header->e_shnum; j++)	
    			if ( sections[j].sh_type==SHT_DYNSYM) {
    				off = sections[j].sh_offset;
    				placeOfDynSymTab=j;
    				break; /* To get to dynstr */
    			}

		     Elf32_Shdr* dynSymTab = &sections[placeOfDynSymTab]; /* shstrndx is the index of the string table of the section header's names */
		     const char * const dynSymTab_p = map_start + off;
       		 for(j=0;j<entries;j++){
         		int strOffset = ELF32_R_SYM(relTab[j].r_info); /* now strOffset holds the correct index of the name in the dyn Symbol Table array */
        		printf("%08x ",relTab[j].r_offset);
        		printf("%08x ",relTab[j].r_info);
        		int tmpType = ELF32_R_TYPE(relTab[j].r_info);
        		printType(tmpType);
        		printIndexAtSymTab(strOffset);
        		printf("\n");
       		 }
       		 printf("\n");

        }
    }
}

/* Iterates all functions and activates them (void) */
char* forEach(char *array, int array_length, char (*f) ()){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  int i;
  for (i=0; i<array_length; i++){
  	char ans=f();
  	if (ans==0) break;
  	mapped_array[i]=ans;
  }
  return mapped_array;
}

/* unmaps and close all used files */
void quit(){
	munmap(map_start,fileSize);
	close(currentFD);
	printf("quiting...\n");
	exit(0);
}

struct fun_desc{
  char *name;
  char (*fun)();
};


int main(int argc, char **argv){
  printf("sizeof (Elf32_Shdr) :%d  %x \n" ,sizeof(Elf32_Shdr),sizeof(Elf32_Shdr));
	debugFlag=0;
	char carray[5]="";
	int i;
	int c;
	int range;
	struct fun_desc menu[] ={{"Toggle Debug Mode",&toggleDebug},
							 {"Examine ELF File",&examineELF},
							 {"Print Section Names",&printSecNames},
							 {"Print Symbols",&printSymbols},
							 {"Link Check",&linkCheck},
							 {"Relocation Tables - Raw",&relocTables},
							 {"Relocation Tables - Semantics",&relocSemantics},
							 {"Quit",&quit}};

	long size=sizeof(menu)/sizeof(struct fun_desc);
	 

	while(1){
		
 		printf("Please choose a function:\n");
		for (i=0; i<size; i++){
			printf("%d) %s\n",i,menu[i].name);
		}
		printf("Option: ");
		c=getc(stdin)-'0';
		range=size-1;
		if (c<0 || c>range) { 
			printf("Not within bound\n");
			quit(0);
		}
		else {
			printf("Within bounds\n");
			getc(stdin);
			forEach(carray, 1, menu[c].fun);
			printf("DONE.\n\n");
		}
	}
}