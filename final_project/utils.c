#include <stdio.h>

/* reset color */
void resetColor(){
	printf("\033[0m");
}

/* in chu vang */
void printfYelloww(char * str){
	printf("\033[0;33m"); 
	printf("%s",str);
	resetColor();
}

/* in chu do */
void printfRed(char *str){
	printf("\033[0;31m"); 
	printf("%s",str);
	resetColor(); 
}

/* in chu Green */
void printfGreen(char * str){
	printf("\033[0;32m"); 
	printf("%s",str);
	resetColor();
}