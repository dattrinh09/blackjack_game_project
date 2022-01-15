#include <stdio.h>
#include <string.h>

void print_heart_card(char x){
	printf("***********************\n");
	printf("*  %-2c                *\n",x);
	printf("*      ***     ***    *\n");
	printf("*     *****   *****   *\n");
	printf("*    ***************  *\n");
	printf("*   ***************** *\n");
	printf("*    ***************  *\n");
	printf("*      ***********    *\n");
	printf("*        *******      *\n");
	printf("*          ***        *\n");
	printf("*           *         *\n");
	printf("*                     *\n");
	printf("*                     *\n");
	printf("*                     *\n");
	printf("***********************\n");
}

void print_diamond_card(char x){
	printf("***********************\n");
	printf("*  %-2c                *\n",x);
	printf("*           *         *\n");
	printf("*          ***        *\n");
	printf("*        *******      *\n");
	printf("*      ***********    *\n");
	printf("*    ***************  *\n");
	printf("*      ***********    *\n");
	printf("*        *******      *\n");
	printf("*          ***        *\n");
	printf("*           *         *\n");
	printf("*                     *\n");
	printf("*                     *\n");
	printf("*                     *\n");
	printf("***********************\n");
}

void print_club_card(char x){
	printf("***********************\n");
	printf("*  %-2c                *\n",x);
	printf("*          ***        *\n");
	printf("*         *****       *\n");
	printf("*          ***        *\n");
	printf("*     ***   *   ***   *\n");
	printf("*    ***************  *\n");
	printf("*     ***   *   ***   *\n");
	printf("*           *         *\n");
	printf("*          ***        *\n");
	printf("*        *******      *\n");
	printf("*                     *\n");
	printf("*                     *\n");
	printf("*                     *\n");
	printf("***********************\n");
}

void print_spade_card(char x){
	printf("***********************\n");
	printf("*  %-2c                *\n",x);
	printf("*           *         *\n");
	printf("*          ***        *\n");
	printf("*        *******      *\n");
	printf("*      ***********    *\n");
	printf("*    ***************  *\n");
	printf("*     ***   *   ***   *\n");
	printf("*           *         *\n");
	printf("*          ***        *\n");
	printf("*        *******      *\n");
	printf("*                     *\n");
	printf("*                     *\n");
	printf("*                     *\n");
	printf("***********************\n");
}

void main(){
	print_heart_card('A');
	printf("\n");
	print_diamond_card('1');
	printf("\n");
	print_club_card('A');
	printf("\n");
	print_spade_card('A');
}