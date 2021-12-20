#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"login.h"

/**
 * menu user muon dang nhap, dang ki, exit
 * @return 1 -> login, 2 -> dang ki, 3 -> exit 
 */
int menuLogin(){
    int choice ;
    while (1){
        printf("1.Register\n");
        printf("2.Sign in\n");
        printf("3.Exit\n");
        printf("Your choice (1-3):");
        scanf("%d%*c",&choice);
        if (choice>=1 && choice <=3){
            break;
        }
        printf("Vui long chon so tu 1 den 3!\n");
    }
    return choice;
}

void registerFunc(char *userName,char *passWord){
    char *user = malloc(50*sizeof(char));
    char *pass = malloc(50*sizeof(char));
    printf("========Register=========\n");
    printf("Username: ");
    gets(user);
    printf("Password: ");
    gets(pass);
    userName = user;
    passWord = pass;
    printf("%s",userName);
    printf("%s",passWord);
}
int checkLogin(char *userName,char *passWord){
    return 1;
}

void signIn(){
    char *userName = malloc(50*sizeof(char));
    char *passWord = malloc(50*sizeof(char));
    printf("========Register=========\n");
    printf("Username: ");
    gets(userName);
    printf("Password: ");
    gets(passWord);
    
}



void requestToCreateUserInServer(char* userName, char* passWord){
    // TODO 
}