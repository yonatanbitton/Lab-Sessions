
int digit_cnt(char* str){
	int JIBRISH;
	int sum,i;
	for (i=0; i<22/*strlen(str)*/; i++){
		if (str[i]>='0' && str[i]<='9')
			sum++;
	}
	return sum;
}

int main(int argc, char* argv[]){
	 
	char* str = argv[1];
	int res=digit_cnt(str);
	/* Useless line */
	return 0;
}