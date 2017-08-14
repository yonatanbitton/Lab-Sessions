#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
	
#define STDIN 0 
#define STDOUT 1 
#define STDERR 2

int debugFlag;
int size=4;
int lengthOfInp;
char* fileName;
char name[];
char* dataPtr;  

/* Helper functions for printing */
void PrintHex(char* buffer, int length){
	int j;
 	for (j=0; j<length; j=j+2){
		printf("%02X ", (unsigned char) buffer[j+1]);
		printf("%02X ", (unsigned char) buffer[j]);

	}		

 	printf("\n");
}

void PrintDec(char* buffer, int length){
	int j;
    for (j=0; j<length; j++)		
		printf("%d", (unsigned char)(buffer[j]-'0'));
	printf("\n");
	for (j=0; j<length; j=j+2){
		printf("%d", (unsigned char) buffer[j+1]);
		printf("%d",(unsigned char) buffer[j]);
	printf("\n");
	}		


	printf("\n");
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


/* Set global char* fileName */
void setFilename(){
	
	fileName = malloc (strlen(name));
	int debugBefore = debugFlag;
	write(STDOUT,"Please enter filename: ",23);
	fgets(fileName,100,stdin);
	int j;
	debugFlag = debugBefore;
	printf("Debug flag restore to %d after malloc on setFilename.\n",debugFlag);
 	fileName[strlen(fileName)-1]=0;
 	if (debugFlag==1)
		printf("Debug: Filename set to: %s with last char %d yes.\n", fileName, fileName[strlen(fileName)-1]);
}

/* Set global int unitSize */
void setUnitsize(){
	char numChar; 
	write(STDOUT,"Please enter size units: ",25);
	/*read(STDIN,numChar,1);*/
	numChar=getc(stdin);
	getc(stdin);
	if (numChar=='1' || numChar=='2' || numChar == '4') {
		size=numChar-'0';
		if (debugFlag==1)
			printf("Debug: set size to: %d\n", size);
	}
	else write (STDERR,"error on file size units",24);
}

/* Input: <length> <location> */
/* Purpose: Displays <length> number of file data in a given <location> */
void fileDisplay(){
	int location;
	int length;
	int fd;
	char* buf;
	char copy[lengthOfInp];
	strcpy(copy,fileName);
	copy[strlen(copy)]=0;
 	if (fileName==NULL) {
		write (STDERR,"error: fileName is null!",24);
		return;
	}
	 
  	fd=open(fileName,O_RDONLY);
  	printf("FD IS (debug) %d\n", fd);
  	if (fd<0){
  		write (STDERR,"error on file opening",21);
		return;
  	}
  	printf("Please enter location and length\n");	
	char* locLenBuf = malloc(100);
	fgets (locLenBuf,100,stdin);
	sscanf(locLenBuf,"%x %d",&location,&length);
  
	if (debugFlag==1)
		printf("Debug: location and length are: %x %d\n", location, length);
	buf = (char*) malloc (length*size);
	int cursor=lseek(fd,location,SEEK_SET);
	printf("cursor on file: %d\n",cursor);
	printf("location: %x\n",location);
	int readFromSys = read(fd,buf,size*length);
	close(fd);
	printf("PrintHexa\n");
	PrintHex(buf,size*length);
	printf("PrintDecimal\n");
	PrintDec(buf,size*length);
	printf("The decimal read units are: %4d\n",readFromSys);
	free(buf);
}

/* Input: <location> <length> */
/* Purpose: Takes data of size <length> from the fileName in <location>, and store it to (global) dataPtr */
void loadToMem(){
	int location;
	int length;
	int fd;
 	if (fileName==NULL) {
		write (STDERR,"error: fileName is null!",24);
		return;
	}
  	fd=open(fileName,O_RDONLY);
  	printf("FD IS (debug) %d\n", fd);
  	if (fd<0){
  		write (STDERR,"error on file opening",21);
		return;
  	}
  	printf("Please enter location and length\n");
 	char* locLenBuf = malloc(100);
	fgets (locLenBuf,100,stdin);
	sscanf(locLenBuf,"%x %d",&location,&length);


	if (dataPtr!=NULL) free(dataPtr);

	
	if (debugFlag==1)
		printf("Debug: location and length are: %x %d\n", location, length);
	dataPtr = (char*) malloc (size);
	if (debugFlag==1)
		printf("Debug: fileName is: %s, dataPtr: %x, location: %x, length: %d \n",fileName, dataPtr, location, length);

	int cursor=lseek(fd,location,SEEK_SET);
	printf("cursor: %d\n",cursor);
	printf("location: %x\n",location);
	read(fd,dataPtr,size*length);
	printf("Loaded 10 bytes into %02p\n",dataPtr);
	printf("read: %02x\n",*dataPtr);
	printf("The data in memory is: %s \n",dataPtr);
	close(fd);
	free (locLenBuf);
	/*printf("The hexadecimal read units are: %4x\n",buf);
	printf("The decimal read units are: %4d\n",buf);*/
}


/* Input: <source address> <target location> <length> */
/* Purpose: Takes data that is stored at <source address> and store it in <target location> offset at fileName by <length> */
void saveToFile(){ 
	int sourceAdd;
	int targetLocation, newLength;
	/*                    file to read from, where to write, how much  */
	printf("Please enter source address, target location, length\n");

	scanf("%x %x %d",&sourceAdd, &targetLocation, &newLength);
	if (sourceAdd==0)
		sourceAdd=dataPtr;
  	int fd=open(fileName,O_RDWR);
  	printf("FD IS (debug) %d\n", fd);
  	if (fd<0){
  		write (STDERR,"error on file opening",21);
		return;
  	}
	int fileSize = lseek(fd, 0, SEEK_END)+1;
	lseek(fd, 0,SEEK_SET);
	printf("Debug: the fileSize is ! %d\n",fileSize);

  	if (targetLocation>fileSize){
  		write (STDERR,"error on file opening",21);
		return;
  	}

  	char* sourceAddPtr = sourceAdd;

  	int cursor=lseek(fd,targetLocation,SEEK_SET);
  	printf("cursor goes to: %x\n", cursor);
  	printf("What's in the address? -> :\n");
  	 
  	int ret=write(fd,sourceAdd,newLength);

  	printf("Return value is: %d\n",ret);
  	close(fd);
}

/* Input: <location> <val> */
/* Purpose: Edit the fileName <location> offset, with the <val> value, of size <= (global) size bytes. */
void fileModify(){
	int location;
	int val;
	int fd;
	if (fileName==NULL) {
		write (STDERR,"error: fileName is null!",24);
		return;
	}

  	fd=open(fileName,O_RDWR);
  	printf("FD IS (debug) %d\n", fd);
  	if (fd<0){
  		write (STDERR,"error on file opening",21);
		return;
  	}

  	printf("Please enter location and val\n");
  	char* locValBuf = malloc(100);
	fgets (locValBuf,100,stdin);
	sscanf(locValBuf,"%x %x",&location,&val);
	printf("Location and val read: %x %x \n",location, val);
 	 

	int fileSize = lseek(fd, 0, SEEK_END)+1;
	lseek(fd, 0,SEEK_SET);
	printf("Debug: the fileSize is ! %d\n",fileSize);

  	if (location>fileSize){
  		write (STDERR,"error on file size\n",21);
		return;
  	}
	
	if (debugFlag==1)
		printf("Debug: location and val are: %x %d\n", location, val);

	int cursor=lseek(fd,location,SEEK_SET);
	printf("cursor: %d\n",cursor);
	printf("location: %x\n",location);
 
	unsigned int* tmp = (void*) val;
 
	int ret=write(fd,&tmp,size); /* Make sure size=4 ! */
	printf("ret is: %d\n",ret); 
	close(fd);
	free (locValBuf);
 	
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

void quit(){
	printf("quiting...\n");
	exit(0);
}

struct fun_desc{
  char *name;
  char (*fun)();
};

 
int main(int argc, char **argv){
	debugFlag=0;
	char carray[5]="";
	int i;
	int c;
	int range;
	struct fun_desc menu[] ={{"Toggle Debug Mode",&toggleDebug},{"Set File Name",&setFilename},{"Set Unit Size",&setUnitsize},
	{"File Display",&fileDisplay},{"Load Into Memory",&loadToMem},{"Save Into File",&saveToFile},{"File Modify",&fileModify},{"Quit",&quit}};

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