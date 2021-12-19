#include "linkedlist.h"
#include <pthread.h>
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h> 
#include <netdb.h>	
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio_ext.h>
#include <ctype.h>

node head = NULL;
node curAccList = NULL;

node createNode(Account a);
node addTail(node head, Account a);
int search(node head, char username[31]);
node delFisrt(node head);
node delLast(node head);
node delAt(node head, int position);
node delByVal(node head, char username[31]);
node delList(node head);
node checkAccIsCur(node head, char username[31]);
node getDataFromFile(char *fileName);
void storeFile(node head, char fileName[31]);
void changeStatusAcc(node head, Account a, int status, char fileName[31]);
void changePassword(node head, Account a, char newPass[31], char fileName[31]);
void printList(node head);

void *client_handler(void *arg){
	int clientfd;
	int sendBytes, recvBytes;
	char buf[100];
	pthread_mutex_t mptr;

	pthread_detach(pthread_self());
	clientfd = (int) arg;

	int count = 0;
	node acc = NULL;
	while(10){
		recvBytes = recv(clientfd, buf, 100, 0);
		if(recvBytes < 0){
			perror("Can not recvice data from client");
			return 0;
		}
		buf[recvBytes] = '\0';
		printf("[Data from client]: %s-\n", buf);

		char data[5][31];
		for(int i = 0; i < 5; i++){
			memset(data[i], '\0', 31);
		}

		char *token = strtok(buf, "#");
		int k = 0;
		while(token != NULL){
			strcpy(data[k], token);
			token = strtok(NULL, "#");
			k++;
		}

		//Is Login:
		int isLogin = atoi(data[0]);
		printf("isLogin: %d\n", isLogin);
		//#1:Menu option:
		int menuOp = atoi(data[1]);


		//Login Option: Code: 1

		if(isLogin == 0){
			if(menuOp == 1){
				char username[31];
				strcpy(username, data[2]);
				char password[31];
				strcpy(password, data[3]);
				node checkAcc = checkAccIsCur(head, username);
				if(checkAcc == NULL){
					Account newAcc;
					strcpy(newAcc.username, username);
					strcpy(newAcc.password, password);
					newAcc.status = 1;
					newAcc.score = 2000; 
					head = addTail(head, newAcc);

					pthread_mutex_lock(&mptr);
					storeFile(head, "account.txt");
					pthread_mutex_unlock(&mptr);
					strcpy(buf, "SUCCESS");
				}else{
					strcpy(buf, "NOT_SUCCESS");
				}
			}

			if(menuOp ==  2){
				int flag = atoi(data[2]);
				
				if(flag == 1){
					char username[31];
					strcpy(username, data[3]);
					acc = checkAccIsCur(head, username);
					if(acc != NULL){
						strcpy(buf, "IS_CUR");
					}else{
						strcpy(buf, "NOT_CUR");
					}
					count = 0;
				}

				if(flag == 2){
					char password[31];
					strcpy(password, data[3]);
					if(strcmp(password, acc->value.password) == 0){
						node curAcc = checkAccIsCur(curAccList, acc->value.username);
						if(curAcc == NULL){
							if(acc->value.status == 1){
								curAccList = addTail(curAccList, acc->value);
								snprintf(buf, 31, "%d", acc->value.score);
							}else{
								strcpy(buf, "IS_BLOCKED");
							}
						}else{
							strcpy(buf, "IS_LOGIN");
						}
					}else{
						count ++;
						strcpy(buf, "NOT_SUCCESS");
						if(count > 2){
							pthread_mutex_lock(&mptr);
							changeStatusAcc(head, acc->value, 0, "account.txt");
							pthread_mutex_unlock(&mptr);
							strcpy(buf, "BLOCK_ACC");
							acc = NULL;
						}
					}
				}
			}
		}else{
			if(menuOp == 4){
				curAccList = delByVal(curAccList, acc->value.username);
				printf("1\n");
				strcpy(buf, "LOGOUT");
			}
		}
		
		send(clientfd, buf, strlen(buf), 0);
	}
	close(clientfd);
}


int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("Please enter port number\n");
		return 0;
	}

	if(argc != 2){
		printf("Error input! Please try again\n");
		return 0;
	}

	//Server working
	int portNumber = atoi(argv[1]);
	int listendfd, connfd;
	pthread_t tid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listendfd = socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(portNumber);

	bind(listendfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

	listen(listendfd, 3);

	head = getDataFromFile("account.txt");
	
	while(10){
		clilen = sizeof(cliaddr);
		connfd = accept(listendfd, (struct sockaddr*) &cliaddr, &clilen);
		if(connfd < 0){
			if(errno == EINTR)
				continue;
			else{
				perror("Error: No accept");
				exit(0);
			}
		}

		//Create new thread
		printf("Creating one thread...............\n");
		pthread_create(&tid, NULL, &client_handler, (void *)connfd);
	}
	close(listendfd);
	return 0;
}
