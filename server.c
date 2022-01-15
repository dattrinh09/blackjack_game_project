#include "linkedlist.h"
#include "common.h"
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

//linked list
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
void updateScore(node head, Account a, int score, int check, char fileName[31]);
void printList(node head);

//cards
int get_suit_id(char suit);
int get_value_id(char value);
int calc_sum(const int hand_values[], int ncards);
void display_state(int hand_values[], int hand_suits[], int ncards);

int card_values[52];
int card_suits[52];
int ncard;

void init_cards()
{
	int i;

	for (i = 0; i < 52; ++i)
	{
		card_values[i] = (i % 13) + 1;
		card_suits[i] = i / 13;
	}

	srand(time(NULL));
	/* srand(1); */
	for (i = 0; i < 52; ++i)
	{
		int cv, cs;
		int j = rand() % 52;

		cv = card_values[i];
		card_values[i] = card_values[j];
		card_values[j] = cv;

		cs = card_suits[i];
		card_suits[i] = card_suits[j];
		card_suits[j] = cs;
	}

	ncard = 0;
}


int checkWin_hit(int p_score){
	if(p_score == 21) return 1;

	if(p_score > 21) return 2;

	return -1;
}

int checkWin_stand(int d_score, int p_score){
	if(d_score == 21) return 2;
	
	if(d_score > 21) return 1;

	if(d_score > p_score) return 2;

	if(d_score < p_score) return 1;

	return 0;
}

int checkWin_double(int d_score, int p_score){	
	if(d_score < 21 && p_score < 21){
		if(d_score < p_score) return 1;
		else if(d_score > p_score) return 2;
		else return 0;
	}else{
		if(p_score == d_score && p_score == 21) return 0;
		if(p_score == 21 || d_score > 21) return 1;
		if(d_score == 21 || p_score > 21) return 2;
	}

	return -1;
}

char update_score(int check, node head, Account a, int score, char fileName[31]){
	char c;
	pthread_mutex_t mptr;

	switch(check){
		case 0:
			c = 'P';
			break;
		case 1:
			c = 'W';
			pthread_mutex_lock(&mptr);
			updateScore(head, a, score, 1, fileName);
			pthread_mutex_unlock(&mptr);
			break;
		case 2:
			c = 'L';
			pthread_mutex_lock(&mptr);
			updateScore(head, a, score, 0, fileName);
			pthread_mutex_unlock(&mptr);
			break;
		default:
			c = 'B';
			break;
		}

	return c;
}

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
			break;
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
						
						if(acc->value.isLogin == 0){
							if(acc->value.status == 1){
								pthread_mutex_lock(&mptr);
								acc->value.isLogin = 1;
								pthread_mutex_unlock(&mptr);
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
			    pthread_mutex_lock(&mptr);
				acc->value.isLogin = 0;
				pthread_mutex_unlock(&mptr);
				printf("User logout\n");
				//curAccList = delByVal(curAccList, acc->value.username);
				strcpy(buf, "LOGOUT");
			}

			if(menuOp == 2){
				int option = atoi(data[2]);

				int dealer_hand_values[7];
				int dealer_hand_suits[7];
				int player_hand_suits[7];
				int player_hand_values[7];

				int score_bet;
				int n_player_cards, n_dealer_cards, card_postion;

				if(option == 0){
					init_cards();
					score_bet = atoi(data[3]);
					printf("Player bet: %d score\n", score_bet);

					pthread_mutex_lock(&mptr);
					dealer_hand_values[0] = card_values[0];
					dealer_hand_suits[0] = card_suits[0];
					dealer_hand_values[1] = card_values[2];
					dealer_hand_suits[1] = card_suits[2];
					player_hand_values[0] = card_values[1];
					player_hand_suits[0] = card_suits[1];
					player_hand_values[1] = card_values[3];
					player_hand_suits[1] = card_suits[3];
					pthread_mutex_unlock(&mptr);

					card_postion = 3;
					n_dealer_cards = 2;
					n_player_cards = 2;
					display_state(dealer_hand_values, dealer_hand_suits, 2);
					display_state(player_hand_values, player_hand_suits, 2);

					int dealer_score = calc_sum(dealer_hand_values, 2);
					int player_score = calc_sum(player_hand_values, 2);

					buf[0] = value_codes[player_hand_values[0]];
					buf[1] = suit_codes[player_hand_suits[0]];
					buf[2] = value_codes[player_hand_values[1]];
					buf[3] = suit_codes[player_hand_suits[1]];
					buf[4] = value_codes[dealer_hand_values[0]];
					buf[5] = suit_codes[dealer_hand_suits[0]];
					buf[6] = value_codes[dealer_hand_values[1]];
					buf[7] = suit_codes[dealer_hand_suits[1]];

					if(player_score == 21){
						buf[8] = 'B';
						pthread_mutex_lock(&mptr);
						score_bet = score_bet * 3 / 2;
						updateScore(head, acc->value, score_bet, 1, "account.txt");
						pthread_mutex_unlock(&mptr);
					}else{
						buf[8] = 'C';
					}

					buf[9] = 0;

					printf("%s-\n", buf);
				}

				if(option == 1){
					card_postion = card_postion + 1;
					player_hand_values[n_player_cards] = card_values[card_postion];
					player_hand_suits[n_player_cards] = card_suits[card_postion];

					buf[0] = value_codes[player_hand_values[n_player_cards]];
					buf[1] = suit_codes[player_hand_suits[n_player_cards]];
					n_player_cards = n_player_cards + 1;

					switch(checkWin_hit(calc_sum(player_hand_values, n_player_cards))){
						case 1: 
							buf[2] = 'W'; 
							pthread_mutex_lock(&mptr);
							updateScore(head, acc->value, score_bet, 1, "account.txt");
							pthread_mutex_unlock(&mptr);
							break;
						case 2: 
							buf[2] = 'L';
							pthread_mutex_lock(&mptr);
							updateScore(head, acc->value, score_bet, 0, "account.txt");
							pthread_mutex_unlock(&mptr); 
							break;

						default: buf[2] = 'C'; break;
					}

					buf[3] = 0;
					display_state(player_hand_values, player_hand_suits, n_player_cards);
				}

				if(option == 2){
					if(calc_sum(dealer_hand_values, n_dealer_cards) >= 17){
						int dealer_score = calc_sum(dealer_hand_values, n_dealer_cards);
						int player_score = calc_sum(player_hand_values, n_player_cards);

						buf[0] = '0';
						buf[1] = update_score(checkWin_stand(dealer_score, player_score), head, acc->value, score_bet, "account.txt");
						buf[2] = 0;
					}else{
						buf[0] = '1';
						int n = 1;
						while(calc_sum(dealer_hand_values, n_dealer_cards) < 17){
							card_postion = card_postion + 1;
							dealer_hand_values[n_dealer_cards] = card_values[card_postion];
							dealer_hand_suits[n_dealer_cards] = card_suits[card_postion];

							buf[n] = value_codes[dealer_hand_values[n_dealer_cards]];
							buf[n + 1] = suit_codes[dealer_hand_suits[n_dealer_cards]];

							n_dealer_cards = n_dealer_cards + 1;
							n = n + 2;
						}

						buf[n] = '#';

						int dealer_score = calc_sum(dealer_hand_values, n_dealer_cards);
						int player_score = calc_sum(player_hand_values, n_player_cards);

						buf[n+1] = update_score(checkWin_stand(dealer_score, player_score), head, acc->value, score_bet, "account.txt");
						buf[n+2] = 0;
					}

					display_state(dealer_hand_values, dealer_hand_suits, n_dealer_cards);
					display_state(player_hand_values, player_hand_suits, n_player_cards);

					printf("%s-\n", buf);
				}

				if(option == 3){
					card_postion = card_postion + 1;
					player_hand_values[n_player_cards] = card_values[card_postion];
					player_hand_suits[n_player_cards] = card_suits[card_postion];

					buf[0] = value_codes[player_hand_values[n_player_cards]];
					buf[1] = suit_codes[player_hand_suits[n_player_cards]];
					n_player_cards = n_player_cards + 1;

					int dealer_score = calc_sum(dealer_hand_values, n_dealer_cards);
					int player_score = calc_sum(player_hand_values, n_player_cards);
					int double_score = score_bet * 2;

					buf[2] = update_score(checkWin_double(dealer_score, player_score), head, acc->value, double_score, "account.txt");	
					buf[3] = 0;
					display_state(dealer_hand_values, dealer_hand_suits, n_dealer_cards);
					display_state(player_hand_values, player_hand_suits, n_player_cards);
				}

				if(option == 5){
					display_state(dealer_hand_values, dealer_hand_suits, n_dealer_cards);
					display_state(player_hand_values, player_hand_suits, n_player_cards);

					int check = calc_sum(dealer_hand_values, 2);

					if(check == 21) {
						strcpy(buf, "BJ");
						pthread_mutex_lock(&mptr);
						updateScore(head, acc->value, score_bet, 1, "account.txt");
						pthread_mutex_unlock(&mptr);
					}else {
						strcpy(buf, "NOT_BJ");
					}
				}
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
