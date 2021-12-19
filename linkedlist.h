#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Account{
	char username[31];
	char password[31];
	int status;
	int score;
}Account;

struct AccountList{
	Account value;
	struct AccountList *next;
};

typedef struct AccountList *node;

node createNode(Account a){
	node temp;
	temp = (node)malloc(sizeof(struct AccountList));
	temp->next = NULL;
	strcpy(temp->value.username, a.username);
	strcpy(temp->value.password, a.password);
	temp->value.status = a.status;
	temp->value.score = a.score;

	return temp;
}

node addTail(node head, Account a){
	node temp, p;
	temp = createNode(a);
	if(head == NULL){
		head = temp;
	}
	else{
		p = head;
		while(p->next != NULL){
			p = p->next;
		}

		p->next = temp;
	}

	return head;
}

int search(node head, char username[31]){
	int position = 0;
	node p = head;
	while(p != NULL){
		if(strcmp(p->value.username, username) == 0){
			return position;
		}

		position++;
	}

	return -1;
}

node delFisrt(node head){
	node p = head;
	head = p->next;
	p = NULL;
	
	return head;
}

node delLast(node head){
	if(head == NULL || head->next == NULL){
		return delFisrt(head);
	}

	node p = head;
	while(p->next->next != NULL){
		p = p->next;
	}
	p->next = p->next->next;

	return  head;
}

node delAt(node head, int position){
	if(position == 0 || head == NULL || head->next == NULL){
		head = delFisrt(head);
	}else{
		int k = 1;
		node p = head;
		while(p->next->next != NULL && k != position){
			p = p->next;
			k++;
		}

		if(k == position){
			p->next = p->next->next;
		}
	}

	return head;
}

node delByVal(node head, char username[31]){
	int position = search(head, username);
	while(position != -1){
		delAt(head, position);
		position = search(head, username);
	}

	return head;
}

node delList(node head){
	node p = head;
	while(p != NULL){
		head = delFisrt(head);
		p = head;
	}

	return head;
}

node checkAccIsCur(node head, char username[31]){
	node p = head;
	while(p != NULL){
		if(strcmp(p->value.username, username) == 0)
			return p;
		p = p->next;
	}

	return NULL;
}

void printList(node head){
	printf("\n");
	node p = head;
	while(p != NULL){
		printf("- %s %s %d %d\n",p->value.username, p->value.password, p->value.status, p->value.score);
		p = p->next;
	}
}

node getDataFromFile(char fileName[31]){
	Account a;
	node head;
	head = NULL;
	FILE *ptr;
	ptr = fopen(fileName,"r");
	while(feof(ptr) == 0){
		fscanf(ptr,"%s%s%d%d", a.username, a.password, &a.status, &a.score);
		head = addTail(head, a);
	}
	fclose(ptr);

	return head;
}

void storeFile(node head, char fileName[31]){
	FILE *ptr;
	ptr = fopen(fileName,"w");
	node p = head;
	while(p->next != NULL){
		fprintf(ptr,"%s %s %d %d\n",p->value.username, p->value.password, p->value.status, p->value.score);
		p = p->next;
	}
	fprintf(ptr,"%s %s %d %d",p->value.username, p->value.password, p->value.status, p->value.score);
	fclose(ptr);
}

void changeStatusAcc(node head, Account a, int status, char fileName[31]){
	node p = head;
	while(p != NULL){
		if((strcmp(p->value.username, a.username) == 0) 
			&& (strcmp(p->value.password, a.password) == 0))
			{
				p->value.status = status;
				storeFile(head, fileName);
			}
		p = p->next;
	}	
}

void changePassword(node head, Account a, char newPass[31], char fileName[31]){
	node p = head;
	while(p != NULL){
		if((strcmp(p->value.username, a.username) == 0))
			{
				strcpy(p->value.password, newPass);
				storeFile(head, fileName);
			}
		p = p->next;
	}	
}