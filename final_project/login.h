#ifndef __LOGIN_H__
#define __LOGIN_H__
#define BUFF_SIZE 8192

/* 1 node la 1 account -> check authen */
typedef struct Node{
	char username[30];
	char pass[30];
	int status; // check trang thai lock
	int state; // check online off
	int sock;
	int ID;
	char status2[30]; // du tinh lam cai trang thai emoji
	struct Node *next;
} node;
/* la 1 user info trong room */
typedef struct Node1{
	char name[30];
	char cap[10];
	int online;
	char admin[30];
	struct Node1 *next;
} node1;

node *head; // first node user
node1 *head1; // first node room
void addNode(char* user, char* password, char* status2, int status);
void addNodeRoom(char* name, char *cap, char *admin);
node *checkExist(char* user);
node1 *checkExistRoom(char* name);
int checkPass(char* user, char* pass);
int checkStatus(char* user);
int lockAccount(char* user);
void writeFile();
void writeRoomFile(int a);
void printlist();
#endif