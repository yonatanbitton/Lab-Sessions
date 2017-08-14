 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define buffer_size 100

char encrypt(char c){
	if (c>='A' && c<='Z') c+=('a'-'A');
	return c;
} /* Gets a char c and returns its encrypted form by adding 3 to its value. 
          If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c){
	if (c>='a' && c<='z') c-=('a'-'A');
	return c;
} /* Gets a char c and returns its decrypted form by reducing 3 to its value. 
            If c is not between 0x20 and 0x7E it is returned unchanged */
char xprt(char c){
	printf("0x%X\n",c);
	return c;
} /* xprt prints the value of c in a hexadecimal representation followed by a 
           new line, and returns c unchanged. */
char cprt(char c){
	if (c>=' ' && c<='~') printf("%c\n",c);
		else printf(".");
	return c;
} /* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed 
                    by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns 
                    the value of c unchanged. */
char my_get(char c){
	int p;
	if ((p=fgetc(stdin))=='\n') return 0;
	return p;
} /* Ignores c, reads and returns a character from stdin using fgetc. */

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char quit(char c){
	exit(c);
}

typedef struct fun_desc {
  char *name;
  char (*fun)(char);
}fun_desc;
 
char* map(char *array, int array_length, char (*f) (char)){
  if (f==quit) quit(0);
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  int i;
  for (i=0; i<array_length; i++){
  	if ((mapped_array[i]=f(array[i]))==0) break;
  }
  return mapped_array;
}
 
int main(int argc, char **argv){
	char carray[buffer_size]="";
	fun_desc menu[] = { { "Censor", &censor },{ "Encrypt", &encrypt },{"Decrypt", &decrypt},
	{"Print hex", &xprt},{"Print string",&cprt},{"Get string",&my_get},{"Quit",&quit},{NULL, NULL}};

	int size = sizeof(menu)/sizeof(fun_desc)-1;
	while (1){
		int	i,c;
		printf("Please choose a function:\n");
		for (i=0; i<size; i++){
			printf("%d) %s\n",i,menu[i].name);
		}
		printf("Option: ");
		c = (fgetc(stdin) - '0');
		if (c<0 || c>size) {
			printf("Not within bounds\n\n");
			quit(0);
		} 
		printf("Within bounds\n");
		fgetc(stdin);
		char* ans = map(carray, buffer_size, menu[c].fun);
		for (i=0; i<buffer_size; i++){
			carray[i]=ans[i];
		}
		free(ans);
		printf("DONE.\n\n");
	}
	return 0;
}