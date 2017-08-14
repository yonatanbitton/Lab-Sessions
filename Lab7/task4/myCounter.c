
int digit_cnt(char* str){
	int i;
	int sum=0;
	int j=0;
	while(str[j]!=0)
		j++;

	for (i=0; i<j/*strlen(str)*/; i++){
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