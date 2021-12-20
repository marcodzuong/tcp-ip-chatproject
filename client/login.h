#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int menuLogin();
void registerFunc(char *userName,char *passWord);
void requestToCreateUserInServer(char* userName, char* passWord);
int checkLogin(char *userName,char *passWord);
void signIn();