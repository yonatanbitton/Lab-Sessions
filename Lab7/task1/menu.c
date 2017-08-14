#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define STDIN 0 
#define STDOUT 1 
#define STDERR 2

int debugFlag;
int size;
int lengthOfInp;
char fileName[100];
char* dataPtr;  

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


void setFilename(){
	/*printf("Please enter fileName: ");
	*/
	write(STDOUT,"Please enter filename: ",23);
	/*lengthOfInp=read(STDIN,fileName,100); */
	fgets(fileName,100,stdin);
	if (debugFlag==1)
		printf("Debug: Filename set to: %s\n", fileName);
}

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

void fileDisplay(){
	printf("Filename is (DEBUG)%s\n",fileName);
	printf("lengthOfInp is (DEBUG)%d\n",lengthOfInp);
	int location;
	int length;
	int fd;
	char* buf;
	char copy[lengthOfInp];
	strcpy(copy,fileName);
	copy[strlen(copy)]=0;
	printf("copy is:(DEBUG) %s\n",copy);
	FILE* file; 
	if (fileName==NULL) {
		write (STDERR,"error: fileName is null!",24);
		return;
	}
	char* abcPtr = (char*)copy;
	printf("abcPtr: \n%s",abcPtr);
	/*file = fopen (abcPtr, "r+");
	printf("File==null?\n%d",file);*/
	char* aout = "a.out";
  	fd=open(aout,O_RDONLY);
  	printf("FD IS (debug) %d\n", fd);
  	if (fd<0){
  		write (STDERR,"error on file opening",21);
		return;
  	}
  	printf("Please enter location and length\n");
	
	/*scanf("%x %d",&location, &length);*/
	char chars[8];
	char locationChars[4];
	char lengthChars[4];
	fgets (locationChars,4,stdin);
	sscanf (locationChars, "%x", &location);
	fgets (lengthChars,4,stdin);
	sscanf (lengthChars, "%d", &length);

	if (debugFlag==1)
		printf("Debug: location and length are: %x %d\n", location, length);
	buf = (char*) malloc (length*size);
	int bytes=lseek(fd,location,SEEK_SET);
	printf("bytes: %d\n",bytes);
	printf("location: %x\n",location);
	read(fd,buf,size*length);
	close(fd);
	printf("The hexadecimal read units are: %4x\n",buf);
	printf("The decimal read units are: %4d\n",buf);
	free(buf);
}

void loadToMem(){
	/*printf("Filename is (DEBUG)%s\n",fileName);
	printf("lengthOfInp is (DEBUG)%d\n",lengthOfInp);*/
	int location;
	int length;
	int fd;
	char* buf2;
	/*char* buf;
	char copy[lengthOfInp];
	strcpy(copy,fileName);
	copy[strlen(copy)]=0;
	printf("copy is:(DEBUG) %s\n",copy);
	FILE* file;*/ 
	if (fileName==NULL) {
		write (STDERR,"error: fileName is null!",24);
		return;
	}
 	/*file = fopen (abcPtr, "r+");
	printf("File==null?\n%d",file);*/
	char* abc = "abc.txt";
  	fd=open(abc,O_RDONLY);
  	printf("FD IS (debug) %d\n", fd);
  	if (fd<0){
  		write (STDERR,"error on file opening",21);
		return;
  	}
  	printf("Please enter location and length\n");
	
 	char chars[8];
	char locationChars[4];
	char lengthChars[4];
	fgets (locationChars,4,stdin);
	sscanf (locationChars, "%x", &location);
	fgets (lengthChars,4,stdin);
	sscanf (lengthChars, "%d", &length);

	if (dataPtr!=NULL) free(dataPtr);

	
	if (debugFlag==1)
		printf("Debug: location and length are: %x %d\n", location, length);
	dataPtr = (char*) malloc (size);
	if (debugFlag==1)
		printf("Debug: fileName is: %s, dataPtr: %x, location: %x, length: %d \n",abc, dataPtr, location, length);

	int cursor=lseek(fd,location,SEEK_SET);
	printf("cursor: %d\n",cursor);
	printf("location: %x\n",location);
	read(fd,dataPtr,size*length);
	printf("Loaded 10 bytes into %02p\n",dataPtr);
	printf("read: %02x\n",*dataPtr);
	close(fd);
	/*printf("The hexadecimal read units are: %4x\n",buf);
	printf("The decimal read units are: %4d\n",buf);*/
}

 
char* map(char *array, int array_length, char (*f) ()){
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
	char* ans;
	struct fun_desc menu[] ={{"Toggle Debug Mode",&toggleDebug},{"Set File Name",&setFilename},{"Set Unit Size",&setUnitsize},{"File Display",&fileDisplay},{"Load Into Memory",&loadToMem},{"Quit",&quit}};

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
			ans=map(carray, 1, menu[c].fun);
			int j;

			printf("DONE.\n\n");
		}
	}
}