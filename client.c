#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h> 
#include <netdb.h>	
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio_ext.h>

typedef struct _Data{
	char value[100];
}Data;

typedef struct _Account{
	char username[31];
	char password[31];
	int status;
	int score;
}Account;

Data inputString(char label[31]){
	char str[100] = "";
	char c;
	int i, check;
	while(10){
		printf("%s", label);
		i = 0;
		check = 1;
		__fpurge(stdin);
		while(10){
			c = getchar();
			if(c == '\n' || c == EOF) break;
			if(c == ' ') check = 0;
			str[i] = c;
			i++;
		}
		if(check == 0){
			printf("Input is incorrect\n");
			memset(str, '\0', 31*sizeof(char));
		}else break;
	}
	str[i] = '\0';
	Data a;
	strcpy(a.value, str);
	return a;
}

int checkIPIsValid(char *ipAddr){
	char str[255];
	strcpy(str, ipAddr);
	int count = 0;

	if(str[0] == '.' || str[strlen(str)-1] == '.') return 0;

	for(int i = 0; i < strlen(str); i++){
		if(str[i] == '.'){
			if(str[i+1] == '.') return 0;
			count++;
		}
	}
	if(count != 3) return 0;

	char *token = strtok(str, ".");

	while(token != NULL) {
		if(atoi(token) < 0 || atoi(token) > 255) return 0;
		token = strtok(NULL, ".");
	}

	return 1;
}

int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("Please enter IP address and port number\n");
		return 0;
	}

	if(argc != 3){
		printf("Error parameters! Please try again\n");
		return 0;
	}

	if(checkIPIsValid(argv[1]) == 0){
		printf("IP address is invalid\n");
		return 0;
	}

	char *ipAddress = argv[1];
	int portNumber = atoi(argv[2]);
	int sockfd;
	struct sockaddr_in serveraddr;
	char sendline[100]; 
	char recvline[100];
	int n;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Problem in creating the socket");
		return 0;
	}

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipAddress);
	serveraddr.sin_port = htons(portNumber);

	if(connect(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0){
		perror("Problem in connecting to the server");
		return 0;
	}

	Account curAcc;
	int isLogin = 0;

	while(10){
		if(isLogin == 0){
			printf("----------------------------------------\n");
			printf("Wellcome to Blackjack Game\n");
			printf("1. Sign Up\n");
			printf("2. Login\n");
			printf("3. Quit\n");
			printf("----------------------------------------\n");

			Data menuOp = inputString("\nEnter menu option: ");
			if(strcmp(menuOp.value, "1") == 0){
				printf("\nSign Up:\n");
				Data username = inputString("Username: ");
				Data password = inputString("Password: ");
				
				strcpy(sendline, "0#1#");
				strcat(sendline, username.value);
				strcat(sendline, "#");
				strcat(sendline, password.value);

				send(sockfd, sendline, strlen(sendline), 0);

				n = recv(sockfd, recvline, 100, 0);
				recvline[n] = '\0';

				if(strcmp(recvline, "SUCCESS") == 0){
					printf("Create account sccessfull!\n");
					strcpy(menuOp.value, "2");
				}else{
					printf("This account is current!\nCan not create!\n");
				}
			}

			if(strcmp(menuOp.value, "2") == 0){
				printf("\nLogin:\n");
				Data username = inputString("Enter username: ");
				strcpy(sendline, "0#2#1#");
				strcat(sendline, username.value);
				send(sockfd, sendline, strlen(sendline), 0);

				n = recv(sockfd, recvline, 100, 0);
				recvline[n] = '\0';

				if(strcmp(recvline, "IS_CUR") == 0){
					while(10){
						Data password = inputString("Enter password: ");
						strcpy(sendline, "0#2#2#");
						strcat(sendline, password.value);
						send(sockfd, sendline, strlen(sendline), 0);

						n = recv(sockfd, recvline, 100, 0);
						recvline[n] = '\0';

						if(strcmp(recvline, "IS_LOGIN") == 0){
							printf("This account is current login in another client!\n");
							break;
						}

						if(strcmp(recvline, "IS_BLOCKED") == 0){
							printf("This account is blocked!\n");
							break;
						}

						if(strcmp(recvline, "BLOCK_ACC") == 0){
							printf("You input password wrong 3 time!\n.This account is blocked!\n");
							break;
						}

						if(strcmp(recvline, "NOT_SUCCESS") == 0){
							printf("Password is incorrect\n");
						}else{
							strcpy(curAcc.username, username.value);
							curAcc.score = atoi(recvline);
							printf("Score: %d\n", curAcc.score);
							isLogin = 1;
							break;
						}
					}
				}else{
					printf("Account not find!\n");
				}
			}
		}else{
			printf("----------------------------------------\n");
			printf("Wellcome to Blackjack Game\n");
			printf("Hello %s\n", curAcc.username);
			printf("1. User Profile\n");
			printf("2. Single Play\n");
			printf("3. Party Play\n");
			printf("4. Sign Out\n");
			printf("----------------------------------------\n");

			Data menuOp = inputString("\nEnter menu option: ");

			if(strcmp(menuOp.value, "1") == 0){
				printf("User Profile\n");
				printf("Name: %s\n", curAcc.username);
				printf("Score: %d\n", curAcc.score);
			}

			if(strcmp(menuOp.value, "4") == 0){
				strcpy(sendline, "1#4");
				send(sockfd, sendline, strlen(sendline), 0);

				n = recv(sockfd, recvline, 100, 0);
				recvline[n] = '\0';

				if(strcmp(sendline, "LOGOUT") == 0){
					memset(curAcc.username, '\0', 31);
					curAcc.score = 0;
					isLogin = 0;
					printf("Goodbye %s\n", curAcc.username);
				}
			}
		}
	}

	return 0;
}