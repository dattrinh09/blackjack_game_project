#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int card_values[52];
int card_suits[52];
int ncard;

int players_hand_values[MAX_PLAYERS][20], dealers_hand_values[MAX_PLAYERS][20];
int players_hand_suits[MAX_PLAYERS][20], dealers_hand_suits[MAX_PLAYERS][20];
int nplayers[MAX_PLAYERS], ndealers[MAX_PLAYERS];

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