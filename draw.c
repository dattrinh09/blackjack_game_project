#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* drawSuits(char* type)
{
    if(strcmp(type, "spades") == 0) //printf("♠️\n"); 
        return "♠️\0";
    if(strcmp(type, "hearts") == 0) //printf("♥️\n");
        return "♥️\0";
    if(strcmp(type, "diamonds") == 0) //printf("♦️\n");
        return "♦️\0";
    if(strcmp(type, "clubs") == 0) //printf("♣️\n");
        return "♣️\0";
    
}

void main(){
    char str[100];
    memset(str, '\0', sizeof(str));
    printf("Enter suits: ");
    scanf("%s", str);

    char suit[100];
    memset(suit, '\0', sizeof(str));
    strcpy(suit, drawSuits(str));
    printf("%s\n", suit);
}

