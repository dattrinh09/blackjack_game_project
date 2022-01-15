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
#include "common.h"

//Common
int get_suit_id(char suit);
int get_value_id(char value);
int calc_sum(const int hand_values[], int ncards);
void display_state(int hand_values[], int hand_suits[], int ncards);

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

		if(str[0] == '\0') check = 0;

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

int checkScore(char *str, int score){
	if((strcmp(str, "50") == 0) || 
	   (strcmp(str, "100") == 0) || 
	   (strcmp(str, "150") == 0) || 
	   (strcmp(str, "200") == 0) ||
	   (strcmp(str, "250") == 0)){

	   	int n = atoi(str);
	    if(n <= score) return 1;
	    else printf("Score bet is greate than your current score!\n");		
	}

	return 0; 
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

int printResult(char c, int cur_score, int score_bet){
	printf("\n");
	switch(c){
		case 'W':
			printf("You win!\n");
			cur_score = cur_score + score_bet;
			break;
		case 'L':
			printf("You lose!\n");
			cur_score = cur_score - score_bet;
			break;
		case 'P':
			printf("Push!\n\n");
			break;
		default:
			printf("Two busted!\n");
			break;
		}

	return cur_score;
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

			if(strcmp(menuOp.value, "2") == 0){
				printf("Well come to game\n");
				Data option = inputString("Start new game y(yes) n(no): ");

				if(strcmp(option.value, "y") == 0){
					Data score_str;
					while(10){
						memset(score_str.value, '\0', 100);
						score_str = inputString("Enter score you want bet (50 - 100 - 150 - 200 - 250): ");
						if(checkScore(score_str.value, curAcc.score) == 1)
							break;
						else printf("You input wrong score! Try again\n");
					}
					strcpy(sendline, "1#2#0#");
					strcat(sendline, score_str.value);
					int score_bet = atoi(score_str.value);
					send(sockfd, sendline, strlen(sendline), 0);
					printf("Watting.........\n");

					n = recv(sockfd, recvline, 100, 0);
					recvline[n] = '\0';
					int dealer_hand_values[7];
					int dealer_hand_suits[7];
					int player_hand_suits[7];
					int player_hand_values[7];

					player_hand_values[0] = get_value_id(recvline[0]);
					player_hand_suits[0] = get_suit_id(recvline[1]);
					player_hand_values[1] = get_value_id(recvline[2]);
					player_hand_suits[1] = get_suit_id(recvline[3]);
					dealer_hand_values[0] = get_value_id(recvline[4]);
					dealer_hand_suits[0] = get_suit_id(recvline[5]);
					dealer_hand_values[1] = get_value_id(recvline[6]);
					dealer_hand_suits[1] = get_suit_id(recvline[7]);

					int n_player_cards = 2;
					int n_dealer_cards = 2;

					if(recvline[8] == 'C'){
						if(recvline[4] == 'A'){
							printf("Dealer hands:\n");
							display_state(dealer_hand_values, dealer_hand_suits, 1);
							printf("Your hands:\n");
							display_state(player_hand_values, player_hand_suits, n_player_cards);
							Data ins_op = inputString("Do you want insurance y(yes) or n(no): ");
							if(strcmp(ins_op.value, "y") == 0){
								strcpy(sendline, "1#2#5");
								send(sockfd, sendline, strlen(sendline), 0);
								printf("Watting.........\n");

								n = recv(sockfd, recvline, 100, 0);
								recvline[n] = '\0';

								printf("Dealer hands:\n");
								display_state(dealer_hand_values, dealer_hand_suits, n_dealer_cards);

								if(strcmp(recvline, "BJ") == 0){
									printf("Dealer is Blackjack - You win!\n");
									curAcc.score = curAcc.score + score_bet;
								}
								else printf("Dealer not Blackjack - You lose!\n");

								printf("\nCurrent Score: %d\n",curAcc.score);
								printf("----------------------------------------\n");
							}else{
								printf("Continue........\n");
							}
						}

						while(10){
							printf("Dealer hands:\n");
							display_state(dealer_hand_values, dealer_hand_suits, 1);
							printf("Your hands:\n");
							display_state(player_hand_values, player_hand_suits, n_player_cards);

							printf("\n1. Hit\n");
							printf("2. Stand\n");
							printf("3. Double\n");
							printf("4. Surrender\n");

							Data play = inputString("Enter your option: ");

							if(strcmp(play.value, "3") == 0){
								strcpy(sendline, "1#2#3");
								send(sockfd, sendline, strlen(sendline), 0);
								printf("Watting.........\n");

								n = recv(sockfd, recvline, 100, 0);
								recvline[n] = '\0';

								player_hand_values[n_player_cards] = get_value_id(recvline[0]);
								player_hand_suits[n_player_cards] = get_suit_id(recvline[1]);
								n_player_cards = n_player_cards + 1;

								printf("Dealer hands:\n");
								display_state(dealer_hand_values, dealer_hand_suits, n_dealer_cards);
								printf("Your hands:\n");
								display_state(player_hand_values, player_hand_suits, n_player_cards);

								curAcc.score = printResult(recvline[2], curAcc.score, score_bet * 2);

								printf("\nCurrent Score: %d\n",curAcc.score);
								printf("----------------------------------------\n");

								break;
							}

							if(strcmp(play.value, "1") == 0){
								strcpy(sendline, "1#2#1");
								send(sockfd, sendline, strlen(sendline), 0);
								printf("Watting.........\n");

								n = recv(sockfd, recvline, 100, 0);
								recvline[n] = '\0';


								player_hand_values[n_player_cards] = get_value_id(recvline[0]);
								player_hand_suits[n_player_cards] = get_suit_id(recvline[1]);
								n_player_cards = n_player_cards + 1;

								if(recvline[2] == 'C'){
									printf("Continue.........\n");
								}else{
									printf("Dealer hands:\n");
									display_state(dealer_hand_values, dealer_hand_suits, n_dealer_cards);
									printf("Your hands:\n");
									display_state(player_hand_values, player_hand_suits, n_player_cards);

									curAcc.score = printResult(recvline[2], curAcc.score, score_bet);

									printf("\nCurrent Score: %d\n",curAcc.score);
									printf("----------------------------------------\n");

									break;
								}
							}

							if(strcmp(play.value, "2") == 0){
								strcpy(sendline, "1#2#2");
								send(sockfd, sendline, strlen(sendline), 0);
								printf("Watting.........\n");

								n = recv(sockfd, recvline, 100, 0);
								recvline[n] = '\0';

								if(recvline[0] == '0'){
									printf("Dealer hands:\n");
									display_state(dealer_hand_values, dealer_hand_suits, n_dealer_cards);
									printf("Your hands:\n");
									display_state(player_hand_values, player_hand_suits, n_player_cards);

									curAcc.score = printResult(recvline[1], curAcc.score, score_bet);

									printf("\nCurrent Score: %d\n",curAcc.score);
									printf("----------------------------------------\n");
								}else{
									int j = 1;
									while(recvline[j] != '#'){
										dealer_hand_values[n_dealer_cards] = get_value_id(recvline[j]);
										dealer_hand_suits[n_dealer_cards] = get_suit_id(recvline[j+1]);
										n_dealer_cards = n_dealer_cards + 1;
										j = j + 2;
									}

									printf("Dealer hands:\n");
									display_state(dealer_hand_values, dealer_hand_suits, n_dealer_cards);
									printf("Your hands:\n");
									display_state(player_hand_values, player_hand_suits, n_player_cards);

									curAcc.score = printResult(recvline[j+1], curAcc.score, score_bet);

									printf("\nCurrent Score: %d\n",curAcc.score);
									printf("----------------------------------------\n");
								}

								break;
							}
						}

					}else{
						printf("Dealer hands:\n");
						display_state(dealer_hand_values, dealer_hand_suits, 2);
						printf("Your hands:\n");
						display_state(player_hand_values, player_hand_suits, 2);

						printf("Black Jack - You win!\n\n");
						curAcc.score = curAcc.score + score_bet * 3 / 2; 
						printf("\nCurrent Score: %d\n",curAcc.score);
						printf("----------------------------------------\n");
					}
				}
			}

			if(strcmp(menuOp.value, "4") == 0){
				strcpy(sendline, "1#4");
				send(sockfd, sendline, strlen(sendline), 0);

				n = recv(sockfd, recvline, 100, 0);
				recvline[n] = '\0';

				
				if(strcmp(recvline, "LOGOUT") == 0){
					printf("Goodbye %s\n", curAcc.username);
					memset(curAcc.username, '\0', 31);
					curAcc.score = 0;
					isLogin = 0;
				}
			}
		}
	}

	return 0;
}