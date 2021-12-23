#include <stdio.h>
#include "utils.h"



void printfMenu(){
	printfYelloww("\n\n=======USER MANAGERMENT PROGRAM======\n\n");
	printfGreen("1. Register\n");
	printfGreen("2. Sign in\n");
	printfGreen("3. Exit\n");
	printfRed("Your choice (1-3):  ");
}

void printfAllEmotion(){
	printfYelloww("\n\n=======Status========\n\n");
	printfGreen("1. Smile\n");
	printfGreen("2. Sad\n");
	printfGreen("3. Busy\n");
	printfGreen("4. Need_Chat\n");
	printfRed("Your choice (1-4):  ");
}

void printfChatMenuFunction(){
	printfYelloww("\n\n===========CHATROOM FUNCTION===========\n\n");
	printfGreen("0. List member Online\n");
	printfGreen("1. Create room\n");
	printfGreen("2. Get in room\n");
	printfGreen("3. List of rooms\n");
	printfGreen("4. Update status\n");
	printfGreen("5. Chat 1vs1\n");
	printfGreen("6. Sign Out\n");
	printfRed("Your choice (1-6):\n");
}

void printfMenuHelp(){
	printf("/createroom : Create room\n");
	printf("/getroom : Get in room\n");
	printf("/listroom : List of rooms\n");
	printf("/listusergroup : List user of  rooms\n");
	printf("/touser : chat private to user\n");
	printf("/kickuser : kick user ( admin )\n");
}