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
char* fileName;
char name[];
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
	fgets(name,100,stdin);
	int j;
	fileName = malloc (strlen(name));
	for (j=0; j<strlen(name); j++)
		fileName[j]=name[j];
	printf("strlen of name is %d %d\n",strlen(name),strlen(fileName));
	printf("name is : %s\n",name);
	printf("fileName is : %s\n",fileName);
	fileName[strlen(name)-1]=0;
	printf("And now!!!!%s\n",fileName);
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

void PrintHex(char* buffer, int length){
	int j;
	char reverse[length];
	/*for (j=0; j<length; j++)		
		printf("%02X ", buffer[j] & 0x00FF);

	 
	printf("Reverse: \n");*/
	for (j=0; j<length; j=j+2){
		printf("%02X ", (unsigned char) buffer[j+1]);
		printf("%02X ", (unsigned char) buffer[j]);

	}		

	/*printf("buffer[0]%x\n",buffer[0]);*/
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
  	fd=open(fileName,O_RDONLY);
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
	printf("PrintHexa\n");
	PrintHex(buf,size*length);
	printf("PrintDecimal\n");
	PrintDec(buf,size*length);
	printf("The decimal read units are: %4d\n",buf);
	free(buf);
}

void loadToMem(){
	int location;
	int length;
	int fd;
	char* buf2;
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
		printf("Debug: fileName is: %s, dataPtr: %x, location: %x, length: %d \n",fileName, dataPtr, location, length);

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


void saveToFile(){ 
	int sourceAdd;
	int targetLocation, newLength;
	/*                    file to read from, where to write, how much  */
	printf("Please enter source address, target-location, length\n");
	scanf("%x %x %d",&sourceAdd, &targetLocation, &newLength);
	if (sourceAdd==0)
		sourceAdd=dataPtr;
	/*char* abc = "abc.txt";*/
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

  	int cursor=lseek(fd,targetLocation,SEEK_SET);
  	printf("cursor goes to: %d\n", cursor);
  	printf("What's in the address? -> :\n");
  	/*printf("%s\n",sourceAdd);*/
  	write(fd,sourceAdd,newLength);
}
void fileModify(){
	int location;
	int val;
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
  	printf("Please enter location and val\n");
 	char chars[8];
	char locationChars[4];
	char* valPtr;
	char valChars[80];
	/*fgets (valChars,80,stdin);
	sscanf (valChars, "%x", &val);*/

	fgets (locationChars,4,stdin);
	sscanf (locationChars, "%x", &location);

	fgets(valChars,100,stdin);

	int j;
	valPtr = malloc (strlen(valChars));
	for (j=0; j<strlen(valChars); j++)
		valPtr[j]=valChars[j];
	printf("strlen of valPtr is %d %d\n",strlen(valChars),strlen(valPtr));
	printf("valPtr is : %s\n",valPtr);
	printf("valChars is : %s\n",valChars);
	valPtr[strlen(valPtr)-1]=0;
	printf("valPtr is : %s\n",valPtr);


	/*read(fd,valChars,80);*/
	int fileSize = lseek(fd, 0, SEEK_END)+1;
	lseek(fd, 0,SEEK_SET);
	printf("Debug: the fileSize is ! %d\n",fileSize);

  	if (location>fileSize){
  		write (STDERR,"error on file opening",21);
		return;
  	}
	
	if (debugFlag==1)
		printf("Debug: location and val are: %x %d\n", location, val);
	
	if (debugFlag==1)
		printf("Debug: fileName is: %s, dataPtr: %x, location: %x, val: %d \n",fileName, dataPtr, location, val);

	int cursor=lseek(fd,location,SEEK_SET);
	printf("cursor: %d\n",cursor);
	printf("location: %x\n",location);
	int ret=write(fd,valPtr,size);
	printf("ret is: %d\n",ret);
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
			ans=map(carray, 1, menu[c].fun);
			int j;

			printf("DONE.\n\n");
		}
	}
}