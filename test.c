#include <stdio.h>
#include <string.h>

void main(){
	char str[100];
	printf("Enter string: ");
	scanf("%s", str);

	char strs[5][100];
	for(int i = 0; i < 5; i++){
		memset(strs[i], '\0', 100);
	}

	char *token = strtok(str, "#");

	int k = 0;
	while(token != NULL){
		// printf("%s\n", token);
		strcpy(strs[k], token);
		token = strtok(NULL, "#");
		k++;
	}

	for(int i = 0; i < 5; i++){
		printf("%d-%s-\n",i,strs[i]);
	}
}