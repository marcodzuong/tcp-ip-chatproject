#include <stdio.h>
void menu(){
	printf("\nUSER MANAGERMENT PROGRAM\n");
	printf("-----------------------------------\n");
	printf("1. Register\n");
	printf("2. Sign in\n");
	//printf("3. Sign out\n");
	printf("3. Exit\n");
	printf("Your choice (1-3):  ");
}

void all_emotion(){
	printf("\nStatus\n");
	printf("-----------------------------------\n");
	printf("1. Smile\n");
	printf("2. Sad\n");
	printf("3. Busy\n");
	printf("4. Need_Chat\n");
	printf("Your choice (1-4):  ");
}

void chatFunction(){
	printf("\nCHATROOM FUNCTION\n");
	printf("-----------------------------------\n");
	printf("0. List member Online\n");
	printf("1. Create room\n");
	printf("2. Get in room\n");
	printf("3. List of rooms\n");
	printf("4. Update status\n");
	printf("5. Chat 1vs1\n");
	printf("6. Sign Out\n");
	printf("Your choice (1-6):\n");
	//printf("a");
}

void menuhelp(){
	printf("/createroom : Create room\n");
	printf("/getroom : Get in room\n");
	printf("/listroom : List of rooms\n");
	printf("/listusergroup : List user of  rooms\n");
	printf("/touser : chat private to user\n");
	printf("/kickuser : kick user ( admin )\n");
}