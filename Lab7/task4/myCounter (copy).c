#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int counter(char* str){
	int i;
	int sum=0;
	for (i=0; i<strlen(str); i++){
		if (str[i]>='0' && str[i]<='9')
			sum++;
	}
	return sum;
}

int main(int argc, char* argv[]){
	if (argc<2) { 
		fprintf(stderr, "Please enter string.\n");
		exit(1);
	}
	char* str = argv[1];
	int res=counter(str);
	
	printf("The number of digits in the string is %d\n",res);
	return 0;
}