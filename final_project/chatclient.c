/*Client */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include "menu.h"
#include "common.h"
#include "login.h"

#define QUIT_STRING "/end"
#define CREATEROOM "/createroom"
#define GETROOM "/getroom"
#define LISTROOM "/listroom"
#define LISTUSERGROUP "/listusergroup"
#define KICKUSER "/kickuser"
#define HELP "/help"
#define TOUSER "/touser"
#define BUFF_SIZE 8192

/* In danh sách phòng */
void showgroups(long lent, char *text)
{
	char *tptr,*a1,*length;
	length=strtok(text,"/");
	int length1=atoi(length);
	tptr = text;
	a1=text+strlen(text)+1;
	// printf("%d",lent);
	// printf("%s\n",a1);
	printf("%18s %19s %19s\n", "Room's name", "Capacity", "Online");
	for (int i=0;i<length1;i++)
	{
		char *name, *capa, *occu, *user;

		name = strtok(NULL,"/");
		// tptr = name + strlen(name) + 1;
		capa = strtok(NULL,"/");
		// tptr = capa + strlen(capa) + 1;
		occu = strtok(NULL,"/");
		// tptr = occu + strlen(occu) + 1;

		printf("%15s %19s %19s\n", name, capa, occu);
	}
}
void showgroups1(long lent, char *text)
{
	char *str;

	// tptr = text;
	// a1=text+strlen(text)+1;
	// printf("%d",lent);
	// printf("%s\n",a1);
	printf("%18s\n","Room's name");
	str = strtok(text,"/");
	while (str!=NULL)
	{
		
		printf("%18s\n", str);
		str=strtok(NULL,"/");
	}
}
// void showUser(long lent, char *text)
// {
// 	char *tptr;

// 	tptr = text;
// 	printf("%18s %19s %19s\n", "Username", "Feeling", "Sock");
// 	while (tptr < text + lent)
// 	{
// 		char *username, *status, *stt, *sock;
// 		int st;

// 		username = tptr;
// 		tptr = username + strlen(username) + 1;
// 		//printf("%s",username);
// 		status = tptr;
// 		tptr = status + strlen(status) + 1;

// 		sock = tptr;
// 		tptr = sock + strlen(sock) + 1;

		// st = atoi(status);

		// switch (st)
		// {
		// case 1:
		// 	strcpy(stt, "Smile");
		// 	break;
		// case 2:
		// 	strcpy(stt, "Sad");
		// 	break;
		// case 3:
		// 	strcpy(stt, "Busy");
		// 	break;
		// case 4:
		// 	strcpy(stt, "Need_Chat");
		// 	break;
		// }
// 		printf("%18s %19s %19s\n", username, stt, sock);
// 	}
// }
void showUser1(char *text)
{
	char *tptr ;
	int st;
	tptr = text;
	printf("%18s\n", "Username");
	char *username,*status;
	username = strtok(text,"/");
	while (username!=NULL)
	{
		
		printf("%18s\n", username);
		username=strtok(NULL,"/");
	}
}

int update(int sock)
{
	Packet *pkt;
	char bufr[MAXPKTLEN];
	char *bufrptr;
	int bufrlen;
	char *status;
	printf("===UPDATE===\n");
	while (getchar() != '\n')
		;
	do
	{
		all_emotion();
		fgets(bufr, MAXPKTLEN, stdin);
	} while (atoi(bufr) > 4 || atoi(bufr) < 1);
	bufr[strlen(bufr) - 1] = '\0';
	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	status = strdup(bufr);

	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, status);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, UPDATE, bufrlen, bufr);
	/* Nhận phản hồi từ server */
	//printf("%s",&bufr);
	pkt = recvpkt(sock);
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	/*Error */
	if (pkt->type == JOIN_REJECTED)
	{
		printf("admin: %s\n", pkt->text);
		free(status);
		return (0);
	}
	else
	{
		//check[sock] = 0;
		printf("%s!\n", pkt->text);
		free(status);
		return 1;
	}
}

int sendListOn(int sock)
{
	Packet *pkt;
	/* Yêu cầu thông tin phòng chat */
	sendpkt(sock, LISTUSERON, 0, NULL);

	/* Nhận phản hồi từ phòng chat */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	if (pkt->type != LISTUSERON)
	{
		fprintf(stderr, "error: unexpected reply from server2\n");
		exit(1);
	}

	/* Hiển thị phòng chat */
	showUser1(pkt->text);
	// showUser(pkt->lent, pkt->text);
	return 1;
}
int kickuser(int sock){
	Packet *pkt;
	char bufr[MAXPKTLEN];
	sendpkt(sock, LIST_USERGR, 0, NULL);

	/* Nhận phản hồi từ phòng chat */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	if (pkt->type != LIST_USERGR)
	{
		fprintf(stderr, "error: unexpected reply from server3\n");
		exit(1);
	}

	/* Hiển thị phòng chat */
	showUser1(pkt->text);
	printf("kick username?\n ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';
	sendpkt(sock, KICK, strlen(bufr)+1, bufr);
	pkt = recvpkt(sock);
	
	printf("%s\n",pkt->text);
}
int sendListUserGr(int sock)
{
	Packet *pkt;
	/* Yêu cầu thông tin phòng chat */
	sendpkt(sock, LIST_USERGR, 0, NULL);

	/* Nhận phản hồi từ phòng chat */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	if (pkt->type != LIST_USERGR)
	{
		fprintf(stderr, "error: unexpected reply from server3\n");
		exit(1);
	}

	/* Hiển thị phòng chat */
	showUser1(pkt->text);
	return 1;
}
/*Xem danh sách phòng */
int sendListGr(int sock)
{
	Packet *pkt;
	/* Yêu cầu thông tin phòng chat */
	sendpkt(sock, LIST_GROUPS, 0, NULL);

	/* Nhận phản hồi từ phòng chat */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	if (pkt->type != LIST_GROUPS)
	{
		fprintf(stderr, "error: unexpected reply from server3\n");
		exit(1);
	}

	/* Hiển thị phòng chat */
	showgroups(pkt->lent, pkt->text);
	return 1;
}

/* Tham gia nhóm chat */
int joinagroup(int sock)
{

	Packet *pkt;
	char bufr[MAXPKTLEN];
	char *bufrptr;
	int bufrlen;
	char *gname;
	char *mname;

	/* Yêu cầu thông tin phòng chat */
	sendpkt(sock, LIST_GROUPS, 0, NULL);

	/* Nhận phản hồi từ phòng chat */
	pkt = recvpkt(sock);
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	if (pkt->type != LIST_GROUPS)
	{
		fprintf(stderr, "error: unexpected reply from server4\n");
		exit(1);
	}

	/* Hiển thị phòng chat */
	showgroups(pkt->lent, pkt->text);
	
	/* Tên phòng chat */
	printf("which group?\n ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	/* Thoát */
	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	gname = strdup(bufr);

	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, gname);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, JOIN_GROUP, bufrlen, bufr);
	
	/* Nhận phản hồi từ server */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}
	if (pkt->type != JOIN_ACCEPTED && pkt->type != JOIN_REJECTED)
	{
		fprintf(stderr, "error: unexpected reply from server5\n");
		exit(1);
	}

	/*Từ chối cho vào phòng */
	if (pkt->type == JOIN_REJECTED)
	{
		printf("admin: %s\n", pkt->text);
		free(gname);
		return (0);
	}
	else /* Tham gia thành công */
	{
		printf("admin: You joined '%s'!\n", gname);
		printf("(Press '/help' to help or '/end' to exit!)\n");
		free(gname);
		return (1);
	}
}

int join11(int sock)
{

	Packet *pkt;
	char bufr[MAXPKTLEN];
	char *bufrptr;
	int bufrlen;
	char *uname;
	char *mname;

	/* Yêu cầu thông tin phòng chat */
	sendpkt(sock, LISTUSERON, 0, NULL);

	/* Nhận phản hồi từ phòng chat */
	pkt = recvpkt(sock);
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	if (pkt->type != LISTUSERON)
	{
		fprintf(stderr, "error: unexpected reply from server6\n");
		exit(1);
	}

	/* Hiển thị phòng chat */
	showUser1(pkt->text);

	/* Tên phòng chat */
	printf("which account?\n ");

	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';
	uname = strdup(bufr);
	// /* Thoát */
	// if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	// {
	// 	close(sock);
	// 	exit(0);
	// }
	// uname = malloc(strlen(bufr) + 1);
	// strcpy(uname, bufr);//strcpy(uname,bufr);

	/* Gửi tin nhắn */
	// bufrptr = bufr;
	// strcpy(bufrptr, uname);
	// bufrptr += strlen(bufrptr) + 1;
	// bufrlen = bufrptr - bufr;
	sendpkt(sock, JOIN_2, strlen(uname)+1, uname);

	/* Nhận phản hồi từ server */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}
	if (pkt->type != JOIN_ACCEPTED && pkt->type != JOIN_REJECTED && pkt->type != DONE)
	{
		fprintf(stderr, "error: unexpected reply from server7\n");
		exit(1);
	}

	/*Từ chối cho vào phòng */
	if (pkt->type == JOIN_REJECTED)
	{
		printf("admin: %s\n", pkt->text);
		// free(uname);
		return (0);
	}
	else 
	if (pkt->type == DONE){
		printf("admin: %s refuse",uname);
		return(0);
	} 
	else/* Tham gia thành công */
	{
		printf("admin: You chat with '%s'!\n", uname);
		// free(uname);
		return (1);
	}
}

int login(int sock, int *check)
{
	Packet *pkt;
	char bufr[MAXNAMELEN];
	char *bufrptr,*bufrptr1;
	int bufrlen;
	char *username, *pass;
	if (check[sock] == 1)
		return 1;
	//username
	printf("===LOG IN===\n");
	while (getchar() != '\n')
		;
	printf("Username: ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	bufrptr1 = strdup(bufr);
	username = strdup(bufr);
	
	//pass
	printf("Password: ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	pass = strdup(bufr);
	int i, j, n1, n2;
	strcat(bufrptr1,"/");
	strcat(bufrptr1, bufr);
	// strcat(bufrptr1, pass);
	printf("%s",bufrptr1);
	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, username);
	// printf("%s",bufrptr);
	bufrptr += strlen(bufrptr) + 1;
	strcpy(bufrptr, pass);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, LOG_IN, bufrlen, bufrptr1);

	/* Nhận phản hồi từ server */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	/*LOG IN sai */
	if (pkt->type == JOIN_REJECTED)
	{
		printf("admin: %s\n", pkt->text);
		free(username);
		free(pass);
		return (0);
	}
	else
	{
		check[sock] = 1;
		printf("%s!\n", pkt->text);
		free(username);
		free(pass);
		return 1;
	}
}

int sendRegister(int sock)
{
	Packet *pkt;
	char bufr[MAXNAMELEN];
	char *bufrptr,*bufrptr1;
	int bufrlen;
	char *username, *pass;
	printf("===Register===\n");
	while (getchar() != '\n')
		;
	printf("Username: ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	bufrptr1 = strdup(bufr);
	username = strdup(bufr);

	//pass
	printf("Password: ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	pass = strdup(bufr);
	strcat(bufrptr1,"/");
	strcat(bufrptr1, bufr);
	// strcat(bufrptr1, pass);
	printf("%s",bufrptr1);
	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, username);
	bufrptr += strlen(bufrptr) + 1;
	strcpy(bufrptr, pass);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, REGISTER, bufrlen, bufrptr1);

	/* Nhận phản hồi từ server */
	pkt = recvpkt(sock);
	if (!pkt)
	{
		fprintf(stderr, "error: server died\n");
		exit(1);
	}
	
	/*Error */
	if (pkt->type == JOIN_REJECTED)
	{
		printf("admin: %s\n", pkt->text);
		free(username);
		free(pass);
		return (0);
	}
	else
	{
		printf("%s!\n", pkt->text);
		free(username);
		free(pass);
		return 1;
	}
}
int sendToUser(int sock){
	char name[MAXPKTLEN];
	char content[MAXPKTLEN],txt[MAXPKTLEN];
	printf("name: ");
	fgets(name, MAXPKTLEN, stdin);
	printf("content: ");
	fgets(content, MAXPKTLEN, stdin);
	// name[strlen(name) - 1] = '\0';
	strcpy(txt,name);
	strcat(txt,"/");
	strcat(txt,content);		
	sendpkt(sock, TO, strlen(txt)+1 ,txt);
	// sendpkt(sock, TO, , "abcs");
	// pkt = recvpkt(sock);
	return 1;
}
int sendCreatRoom(int sock)
{
	Packet *pkt;
	char bufr[MAXNAMELEN];
	char *bufrptr,*bufrptr1;
	int bufrlen;
	char *name;
	char *cap;
	printf("===Creat===\n");
	printf("Room's name: ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	bufrptr1 = strdup(bufr);
	name = strdup(bufr);

	//cap
	printf("Capacity: ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	cap = strdup(bufr);
	strcat(bufrptr1,"/");
	strcat(bufrptr1,cap);
	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, name);
	bufrptr += strlen(bufrptr) + 1;
	strcpy(bufrptr, cap);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, CREAT_ROOM, bufrlen, bufrptr1);

	/* Nhận phản hồi từ server */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		fprintf(stderr, "error: server died\n");
		exit(1);
	}

	/*Error */
	if (pkt->type == UNDONE)
	{
		printf("admin: %s\n", pkt->text);
		free(name);
		free(cap);
		return (0);
	}
	else
	{
		printf("%s!\n", pkt->text);
		if(pkt->type == JOIN_REJECTED) {
			free(name);
			free(cap);
			return 0;
		}
		bufrptr = bufr;
		strcpy(bufrptr,name);
		bufrptr += strlen(bufrptr) + 1;
		bufrlen = bufrptr - bufr;
		sendpkt(sock, JOIN_GROUP, bufrlen, bufr);

		/* Nhận phản hồi từ server */
		pkt = recvpkt(sock);
		if (!pkt)
		{
			printf("error: server died\n");
			exit(1);
		}
		if (pkt->type != JOIN_ACCEPTED && pkt->type != JOIN_REJECTED)
		{
			fprintf(stderr, "error: unexpected reply from server5\n");
			exit(1);
		}

		/*Từ chối cho vào phòng */
		if (pkt->type == JOIN_REJECTED)
		{
			printf("admin: %s\n", pkt->text);
			free(name);
			return (0);
		}
		else /* Tham gia thành công */
		{
			printf("admin: You joined '%s'!\n", name);
			printf("(Press '/help' to help or '/end' to exit!)\n");
			free(name);
			return (1);
		}
		free(name);
		free(cap);
		return 1;
	}
}

int logout(int sock, int *check)
{
	Packet *pkt;
	char bufr[MAXNAMELEN];
	char *bufrptr;
	int bufrlen;
	char *username;
	printf("===LOG OUT===\n");
	printf("Username: ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	username = strdup(bufr);

	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, username);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, LOG_OUT, bufrlen, bufr);

	/* Nhận phản hồi từ server */
	pkt = recvpkt(sock);
	
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	/*Error */
	if (pkt->type == JOIN_REJECTED)
	{
		printf("admin: %s\n", pkt->text);
		free(username);
		return (0);
	}
	else
	{
		check[sock] = 0;
		printf("%s!\n", pkt->text);
		free(username);
		return 1;
	}
}

/* Các chức năng chính */
int main(int argc, char *argv[])
{
	int choiceFunc = 0;
	char bufr1[MAXPKTLEN];
	int sock;
	/*Để kiểm tra trạng thái đăng nhập */
	int check[MAXNAMELEN];
	memset(check, 0, MAXNAMELEN);
	/* Kiểm tra tính hợp lệ của cú pháp  */
	if (argc != 3)
	{
		printf("Wrong syntax!!!\n--> Correct Syntax: ./client AddressIP PortNumber\n");
		return 1;
	}
	/* Kết nối vs máy chủ */
	sock = hooktoserver(argv[2], argv[1]);
	if (sock == -1)
		exit(1);

	fflush(stdout); /* Xóa bộ đệm */

	/* Khởi tạo tập thăm dò */
	fd_set clientfds1, tempfds1;
	fd_set clientfds, tempfds;
	FD_ZERO(&clientfds);
	FD_ZERO(&clientfds1);
	FD_ZERO(&tempfds1);
	FD_ZERO(&tempfds);
	FD_SET(sock, &clientfds); /* Thêm sock vào tập clientfds */
	FD_SET(sock, &clientfds1);
	FD_SET(0, &clientfds); /* �Thêm 0 vào tập clientfds */
	FD_SET(0, &clientfds1);
	/* Vòng lặp */
	char choice[100] = " ";
	while (1)
	{
		/*Menu login/logout */
		menu();
		// char choice=' ';
		strcpy(choice, " ");
		__fpurge(stdin);
		scanf("%s", &choice);
		//printf("%s",Gettype(choice));
		if (strcmp(choice, "3") == 0)
		{
			printf("-->Exit!\n");
			exit(0);
		}
		if (strcmp(choice, "1") == 0)
		{
			sendRegister(sock);
			continue;
		}
		else if (strcmp(choice, "2") == 0)
		{
			if (!login(sock, check))
			{
				continue;
			}
			chatFunction();
			do
			{
				__fpurge(stdin);
				tempfds = clientfds;
				if (select(FD_SETSIZE, &tempfds, NULL, NULL, NULL) == -1)
				{
					perror("select");
					exit(4);
				}

				/* Các bộ trong tempfds kiểm tra xem có phải là bộ socket k? Nếu có, nghĩa là máy chủ gửi tin nhắn
			, còn nếu không thì nhập tin nhắn để gửi đến máy chủ */
				if (FD_ISSET(0, &tempfds))
				{
					fgets(bufr1, MAXPKTLEN, stdin);
					sendpkt(sock, MENU, strlen(bufr1), bufr1);
				}
				/* Xử lí thông tin từ máy chủ */
				if (FD_ISSET(sock, &tempfds))
				{
				    fflush(stdin);
					Packet *pkt1;
					char *tm;
					pkt1 = recvpkt(sock);
					// printf("bang tin,type :%d , text:%s\n", pkt1->type,pkt1->text);
					if (!pkt1)
					{
						/* Máy chủ ngừng hoạt động */
						printf("error: server died\n");
						exit(1);
					}

					/* Hiển thị tin nhắn văn bản */
					if (pkt1->type != MENU && pkt1->type != REQUEST)
					{
						fprintf(stderr, "error: unexpected reply from server\n");
						exit(1);
					}
					else if (pkt1->type == REQUEST)
					{	
							Packet *pkt2;
							int pkt1_len = pkt1->lent;
						    char *uname;
							// uname=strtok(pkt1->text,":");
							char bufr[MAXPKTLEN],tl[MAXPKTLEN];
							// snprintf(uname, pkt1_len + 1, "%s", pkt1->text);
							printf("admin: You chat with '%s' 'y' to chat or 'n' to no \n", pkt1->text);
							/* Tiếp tục trò chuyện */
							fgets(bufr, MAXPKTLEN, stdin);
							bufr[strlen(bufr) - 1] = '\0';
							strcpy(tl,bufr);
							strcat(tl,"/");
							strcat(tl,pkt1->text);
							// printf("%s",tl);
							sendpkt(sock,REQUEST1,strlen(tl)+1,tl);
							pkt2 = recvpkt(sock);
							if (pkt2->type == SUCCESS){
								printf("admin: You chat with '%s' \n", pkt1->text);
								while (1)
								{
									/* Gọi select để theo dõi thông tin bàn phím và máy chủ */
									tempfds = clientfds;
									if (select(FD_SETSIZE, &tempfds, NULL, NULL, NULL) == -1)
									{
										perror("select");
										exit(4);
									}
									fflush(stdout);

									/* Các bộ trong tempfds kiểm tra xem có phải là bộ socket k? Nếu có nghĩa là máy chủ gửi tin nhắn
								, còn nếu không thì nhập tin nhắn để gửi đến may chủ */

									/* Xử lí thông tin từ máy chủ */
									if (FD_ISSET(sock, &tempfds))
									{
										Packet *pkt;
										pkt = recvpkt(sock);
										if (!pkt)
										{
											/* Máy chủ ngừng hoạt động */
											printf("error: server died\n");
											exit(1);
										}

										/* Hiển thị tin nhắn văn bản */
										if (pkt->type != USER_TEXT1 && pkt->type != QUIT)
										{
											fprintf(stderr, "error: unexpected reply from server\n");
											exit(1);
										}
										if (pkt->type == QUIT) break;
										printf("recv: %s", pkt->text);
										freepkt(pkt);
									}
									/* Xử lí đầu vào */
									if (FD_ISSET(0, &tempfds))
									{
										char bufr[MAXPKTLEN];
										fgets(bufr, MAXPKTLEN, stdin);
										if (strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
										{
											/* Thoát khỏi phong chat */
											sendpkt(sock, QUIT, 0, NULL);
											break;
										}

										/*Gửi tin nhắn đến máy chủ */
										sendpkt(sock, USER_TEXT1, strlen(bufr) + 1, bufr);
									}
								}	
							} else {
								// chatFunction();
							}
							chatFunction();
							// break;
					}
					else if (pkt1->type == MENU)
					{
						choiceFunc = atoi(pkt1->text);
						switch (choiceFunc)
						{
						case 0:
						{
							sendListOn(sock);
							break;
						}
						case 1:
							/*Tao phong */
							{
								if(!sendCreatRoom(sock)){
									break;
								};
								while (1)
								{
									/* Gọi select để theo dõi thông tin bàn phím và máy chủ */
									tempfds = clientfds;

									if (select(FD_SETSIZE, &tempfds, NULL, NULL, NULL) == -1)
									{
										perror("select");
										exit(4);
									}

									/* Các bộ trong tempfds kiểm tra xem có phải là bộ socket k? Nếu có nghĩa là máy chủ gửi tin nhắn
							, còn nếu không thì nhập tin nhắn để gửi đến may chủ */

									/* Xử lí thông tin từ máy chủ */
									if (FD_ISSET(sock, &tempfds))
									{

										Packet *pkt;
										pkt = recvpkt(sock);
										
										if (!pkt)
										{
											/* Máy chủ ngừng hoạt động */
											printf("error: server died\n");
											exit(1);
										}
										if (pkt->type == REQUEST)
											{	
												Packet *pkt2;
												int pkt1_len = pkt->lent;
												char *uname;
												// uname=strtok(pkt1->text,":");
												char bufr[MAXPKTLEN],tl[MAXPKTLEN];
												// snprintf(uname, pkt1_len + 1, "%s", pkt1->text);
												printf("admin: You chat with '%s' 'y' to chat or 'n' to no \n", pkt->text);
												/* Tiếp tục trò chuyện */
												fgets(bufr, MAXPKTLEN, stdin);
												bufr[strlen(bufr) - 1] = '\0';
												strcpy(tl,bufr);
												strcat(tl,"/");
												strcat(tl,pkt->text);
												// printf("%s",tl);
												sendpkt(sock,REQUEST1,strlen(tl)+1,tl);
												pkt2 = recvpkt(sock);
												if (pkt2->type == SUCCESS){
													printf("admin: You chat with '%s' \n", pkt->text);
													while (1)
													{
														/* Gọi select để theo dõi thông tin bàn phím và máy chủ */
														tempfds = clientfds;
														if (select(FD_SETSIZE, &tempfds, NULL, NULL, NULL) == -1)
														{
															perror("select");
															exit(4);
														}
														fflush(stdout);

														/* Các bộ trong tempfds kiểm tra xem có phải là bộ socket k? Nếu có nghĩa là máy chủ gửi tin nhắn
													, còn nếu không thì nhập tin nhắn để gửi đến may chủ */

														/* Xử lí thông tin từ máy chủ */
														if (FD_ISSET(sock, &tempfds))
														{
															Packet *pkt3;
															pkt3 = recvpkt(sock);
															if (!pkt3)
															{
																/* Máy chủ ngừng hoạt động */
																printf("error: server died\n");
																exit(1);
															}

															/* Hiển thị tin nhắn văn bản */
															if (pkt3->type != USER_TEXT1 && pkt3->type != QUIT && pkt3->type != USER_TEXT)
															{
																fprintf(stderr, "error: unexpected reply from server\n");
																exit(1);
															}
															if (pkt3->type == QUIT) {
																printf("%s quit\n",pkt->text);
																// sendpkt(sock, QUIT, 0, NULL);
																break;
															} else 
															if (pkt3->type == USER_TEXT1 ){
																printf("recv: %s", pkt3->text);
																freepkt(pkt3);
															}
														
														}
														/* Xử lí đầu vào */
														if (FD_ISSET(0, &tempfds))
														{
															char bufr[MAXPKTLEN];
															fgets(bufr, MAXPKTLEN, stdin);
															if (strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
															{
																/* Thoát khỏi phong chat */
																sendpkt(sock, QUIT, 0, NULL);
																break;
															}

															/*Gửi tin nhắn đến máy chủ */
															sendpkt(sock, USER_TEXT1, strlen(bufr) + 1, bufr);
														}
													}	
												} else {
													// chatFunction();
												}
												// chatFunction();
												// break;
											}

										/* Hiển thị tin nhắn văn bản */
										if (pkt->type != USER_TEXT && pkt->type != REQUEST && pkt->type != KICKU)
										{
											fprintf(stderr, "error: unexpected reply from serve1r\n");
											exit(1);
										}
										if (pkt->type == KICKU){
											sendpkt(sock, LEAVE_GROUP, 0, NULL);
											break;
										}
										if ( pkt->type == USER_TEXT ){
											char *us,*txt;
											us=strtok(pkt->text,"/");
											txt=strtok(NULL,"/");
											printf("%s: %s", us, txt);
											freepkt(pkt);
										}
										
									}
									/* Xử lí đầu vào */
									if (FD_ISSET(0, &tempfds))
									{
										char bufr[MAXPKTLEN];
										fgets(bufr, MAXPKTLEN, stdin);
										if (strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
										{
											/* Thoát khỏi phong chat */
											sendpkt(sock, LEAVE_GROUP, 0, NULL);
											break;
										}
										if (strncmp(bufr, KICKUSER, strlen(KICKUSER)) == 0)
										{
											kickuser(sock);
											// sendListUserGr(sock);
											// break;
											// sendpkt(sock,KICK,0,NULL);
										}else
										if (strncmp(bufr, LISTUSERGROUP, strlen(LISTUSERGROUP)) == 0)
										{
											sendListUserGr(sock);
											// break;
										}else
										if (strncmp(bufr, LISTROOM, strlen(LISTROOM)) == 0)
										{
											sendListGr(sock);
											// break;
										}else
										if (strncmp(bufr, CREATEROOM, strlen(CREATEROOM)) == 0)
										{
											if(!sendCreatRoom(sock)){
												break;
											};
											// break;
										}else
										if (strncmp(bufr, GETROOM, strlen(GETROOM)) == 0)
										{
											if (!joinagroup(sock))
												continue;
											// break;
										}else 
										if (strncmp(bufr, TOUSER, strlen(TOUSER)) == 0)
										{
											if(!sendToUser(sock)){
												break;
											};
										}else 
										if (strncmp(bufr, HELP, strlen(HELP)) == 0)
										{
											menuhelp();
											// break;
										} else {
											sendpkt(sock, USER_TEXT, strlen(bufr) + 1, bufr);
										}
										

										/*Gửi tin nhắn đến máy chủ */
									}
								}
								break;
							}
						case 2: /*Vào phòng */
							/* Tham gia trò chuyện */
							{
								if (!joinagroup(sock)){
									chatFunction();
									continue;
								}

								/* Tiếp tục trò chuyện */
								while (1)
								{
									/* Gọi select để theo dõi thông tin bàn phím và máy chủ */
									tempfds = clientfds;

									if (select(FD_SETSIZE, &tempfds, NULL, NULL, NULL) == -1)
									{
										perror("select");
										exit(4);
									}

									/* Các bộ trong tempfds kiểm tra xem có phải là bộ socket k? Nếu có nghĩa là máy chủ gửi tin nhắn
							, còn nếu không thì nhập tin nhắn để gửi đến may chủ */

									/* Xử lí thông tin từ máy chủ */
									if (FD_ISSET(sock, &tempfds))
									{

										Packet *pkt;
										pkt = recvpkt(sock);
										
										if (!pkt)
										{
											/* Máy chủ ngừng hoạt động */
											printf("error: server died\n");
											exit(1);
										}
										if (pkt->type == REQUEST)
											{	
												Packet *pkt2;
												int pkt1_len = pkt->lent;
												char *uname;
												// uname=strtok(pkt1->text,":");
												char bufr[MAXPKTLEN],tl[MAXPKTLEN];
												// snprintf(uname, pkt1_len + 1, "%s", pkt1->text);
												printf("admin: You chat with '%s' 'y' to chat or 'n' to no \n", pkt->text);
												/* Tiếp tục trò chuyện */
												fgets(bufr, MAXPKTLEN, stdin);
												bufr[strlen(bufr) - 1] = '\0';
												strcpy(tl,bufr);
												strcat(tl,"/");
												strcat(tl,pkt->text);
												// printf("%s",tl);
												sendpkt(sock,REQUEST1,strlen(tl)+1,tl);
												pkt2 = recvpkt(sock);
												if (pkt2->type == SUCCESS){
													printf("admin: You chat with '%s' \n", pkt->text);
													while (1)
													{
														/* Gọi select để theo dõi thông tin bàn phím và máy chủ */
														tempfds = clientfds;
														if (select(FD_SETSIZE, &tempfds, NULL, NULL, NULL) == -1)
														{
															perror("select");
															exit(4);
														}
														fflush(stdout);

														/* Các bộ trong tempfds kiểm tra xem có phải là bộ socket k? Nếu có nghĩa là máy chủ gửi tin nhắn
													, còn nếu không thì nhập tin nhắn để gửi đến may chủ */

														/* Xử lí thông tin từ máy chủ */
														if (FD_ISSET(sock, &tempfds))
														{
															Packet *pkt3;
															pkt3 = recvpkt(sock);
															if (!pkt3)
															{
																/* Máy chủ ngừng hoạt động */
																printf("error: server died\n");
																exit(1);
															}

															/* Hiển thị tin nhắn văn bản */
															if (pkt3->type != USER_TEXT1 && pkt3->type != QUIT && pkt3->type != USER_TEXT)
															{
																fprintf(stderr, "error: unexpected reply from server\n");
																exit(1);
															}
															if (pkt3->type == QUIT) {
																printf("%s quit\n",pkt->text);
																// sendpkt(sock, QUIT, 0, NULL);
																break;
															} else 
															if (pkt3->type == USER_TEXT1 ){
																printf("recv: %s", pkt3->text);
																freepkt(pkt3);
															}
														
														}
														/* Xử lí đầu vào */
														if (FD_ISSET(0, &tempfds))
														{
															char bufr[MAXPKTLEN];
															fgets(bufr, MAXPKTLEN, stdin);
															if (strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
															{
																/* Thoát khỏi phong chat */
																printf("you out chat with %s\n",pkt->text);
																sendpkt(sock, QUIT, 0, NULL);
																break;
															}

															/*Gửi tin nhắn đến máy chủ */
															sendpkt(sock, USER_TEXT1, strlen(bufr) + 1, bufr);
														}
													}	
												} else {
													// chatFunction();
												}
												// chatFunction();
												// break;
											}

										/* Hiển thị tin nhắn văn bản */
										if (pkt->type != USER_TEXT && pkt->type != REQUEST && pkt->type != KICKU)
										{
											fprintf(stderr, "error: unexpected reply from serve1r\n");
											exit(1);
										}
										if (pkt->type == KICKU){
											sendpkt(sock, LEAVE_GROUP, 0, NULL);
											break;
										} 
										if ( pkt->type == USER_TEXT ){
											char *us,*txt;
											us=strtok(pkt->text,"/");
											txt=strtok(NULL,"/");
											printf("%s: %s", us, txt);
											freepkt(pkt);
										}
										
									}
									/* Xử lí đầu vào */
									if (FD_ISSET(0, &tempfds))
									{
										char bufr[MAXPKTLEN];
										fgets(bufr, MAXPKTLEN, stdin);
										if (strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
										{
											/* Thoát khỏi phong chat */
											sendpkt(sock, LEAVE_GROUP, 0, NULL);
											break;
										}
										if (strncmp(bufr, KICKUSER, strlen(KICKUSER)) == 0)
										{
											kickuser(sock);
											// sendListUserGr(sock);
											// break;
											// sendpkt(sock,KICK,0,NULL);
										}else
										if (strncmp(bufr, LISTUSERGROUP, strlen(LISTUSERGROUP)) == 0)
										{
											sendListUserGr(sock);
											// break;
										}else
										if (strncmp(bufr, LISTROOM, strlen(LISTROOM)) == 0)
										{
											sendListGr(sock);
											// break;
										}else
										if (strncmp(bufr, CREATEROOM, strlen(CREATEROOM)) == 0)
										{
											if(!sendCreatRoom(sock)){
												break;
											};
											// break;
										}else
										if (strncmp(bufr, GETROOM, strlen(GETROOM)) == 0)
										{
											if (!joinagroup(sock))
												continue;
											// break;
										}else 
										if (strncmp(bufr, TOUSER, strlen(TOUSER)) == 0)
										{
											if(!sendToUser(sock)){
												break;
											};
										}else 
										if (strncmp(bufr, HELP, strlen(HELP)) == 0)
										{
											menuhelp();
											// break;
										} else {
											sendpkt(sock, USER_TEXT, strlen(bufr) + 1, bufr);
										}
										

										/*Gửi tin nhắn đến máy chủ */
									}
								}
								break;
							}
						case 3: /*Xem danh sách phòng */
						{
							sendListGr(sock);
							break;
						}
						case 4:
						{
							update(sock);
							break;
						}
						case 5:
						{
							if (join11(sock)==1)
							{

							/* Tiếp tục trò chuyện */
								while (1)
								{
									/* Gọi select để theo dõi thông tin bàn phím và máy chủ */
									tempfds = clientfds;
									if (select(FD_SETSIZE, &tempfds, NULL, NULL, NULL) == -1)
									{
										perror("select");
										exit(4);
									}

									/* Các bộ trong tempfds kiểm tra xem có phải là bộ socket k? Nếu có nghĩa là máy chủ gửi tin nhắn
								, còn nếu không thì nhập tin nhắn để gửi đến may chủ */

									/* Xử lí thông tin từ máy chủ */
									if (FD_ISSET(sock, &tempfds))
									{

										Packet *pkt;
										pkt = recvpkt(sock);
										if (!pkt)
										{
											/* Máy chủ ngừng hoạt động */
											printf("error: server died\n");
											exit(1);
										}

										/* Hiển thị tin nhắn văn bản */
										if (pkt->type != USER_TEXT1 && pkt->type != QUIT)
										{
											fprintf(stderr, "error: unexpected reply from serve1r\n");
											exit(1);
										}
										if (pkt->type == QUIT) break;
										printf("recv: %s", pkt->text);
										freepkt(pkt);
									}
									/* Xử lí đầu vào */
									if (FD_ISSET(0, &tempfds))
									{
										char bufr[MAXPKTLEN];
										fgets(bufr, MAXPKTLEN, stdin);
										if (strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
										{
											/* Thoát khỏi phong chat */
											sendpkt(sock, QUIT, 0, NULL);
											break;
										}

										/*Gửi tin nhắn đến máy chủ */
										sendpkt(sock, USER_TEXT1, strlen(bufr) + 1, bufr);
									}
								}
							} else {
								break;
							}
							break;
						}
						case 6:
						{
							logout(sock, check);
							break;
						}
						}
						if(choiceFunc!=6) {chatFunction();}
					}
					freepkt(pkt1);
				}
				/* Xử lí đầu vào */
			} while (choiceFunc != 6);
			choiceFunc = -1;
		}
		else
		{
			//printf("Nhap lai:");
			continue;
		}
	}
}
