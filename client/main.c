#include<stdio.h>
#include<stdlib.h>
#include"login.h"

int main (){
    int choice = loginFunc();
    char *userName;
    char *passWord;
    switch (choice){
    case 1:
        registerFunc(userName,passWord);
        break;
    case 2:
        signIn();
        break;
    
    default:
        break;
    }
    return 1;
}