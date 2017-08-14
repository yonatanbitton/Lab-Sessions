#include <stdlib.h>
#include <string.h>
#include <stdio.h>

 
int main(int argc, char* argv[]){
 
int increment=0;
int i;
FILE* input=stdin;
FILE* output=stdout;
int toFile=0;

for (i=1; i<argc; i++){
        if (argv[i][0]=='+' && argv[i][1]!='o' && argv[i][1]!='i') increment=argv[i][1]-'0';
            else if (argv[i][0]=='-' && argv[i][1]!='o' && argv[i][1]!='i') { 
                    increment=argv[i][1]-'0'; increment=-increment;
                }
                
        if ((strcmp(argv[i],"-i")==0)) {
            input=fopen(argv[++i],"r");
        }
        if ((strcmp(argv[i],"-o")==0)) toFile=1; 
}

if (toFile==1) {
        char outputFile[30]; 
        printf("Enter output file: ");
        fgets(outputFile, 30, stdin);
        outputFile[strlen(outputFile)-1]='\0';
        output=fopen(outputFile, "w+");
    }

int c;
while ((c=getc(input))!=EOF){
    if (c>='A' && c<='Z') 
        c+=32;
    if (c>='a' && c<='z') {
        c+=increment;
        if (c>'z') c-=26;
        if (c<'a') c+=26;
    }
    fputc(c,output);
}

 if (input!=stdin) fclose(input);
 if (output!=stdout) fclose(output);

 return 0;   
}