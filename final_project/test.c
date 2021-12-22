
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
#define BUFF_SIZE 8192

/* In danh sách phòng */
void showgroups(long lent, char *text)
{
	char *tptr;

	tptr = text;
	printf("%18s %19s %19s\n", "Room's name", "Capacity", "Online");
	while (tptr < text + lent)
	{
		char *name, *capa, *occu, *user;

		name = tptr;
		tptr = name + strlen(name) + 1;
		capa = tptr;
		tptr = capa + strlen(capa) + 1;
		occu = tptr;
		tptr = occu + strlen(occu) + 1;

		printf("%15s %19s %19s\n", name, capa, occu);
	}
}

void showUser(long lent, char *text)
{
	char *tptr;

	tptr = text;
	printf("%18s %19s %19s\n", "Username", "Feeling", "Sock");
	while (tptr < text + lent)
	{
		char *username, *status, *stt, *sock;
		int st;

		username = tptr;
		tptr = username + strlen(username) + 1;
		//printf("%s",username);
		status = tptr;
		tptr = status + strlen(status) + 1;

		sock = tptr;
		tptr = sock + strlen(sock) + 1;

		st = atoi(status);

		switch (st)
		{
		case 1:
			strcpy(stt, "Smile");
			break;
		case 2:
			strcpy(stt, "Sad");
			break;
		case 3:
			strcpy(stt, "Busy");
			break;
		case 4:
			strcpy(stt, "Need_Chat");
			break;
		}
		printf("%18s %19s %19s\n", username, stt, sock);
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
	sendpkt(sock, DONE, 0, NULL);

	/* Nhận phản hồi từ phòng chat */
	pkt = recvpkt(sock);
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	if (pkt->type != DONE)
	{
		fprintf(stderr, "error: unexpected reply from server2\n");
		exit(1);
	}

	/* Hiển thị phòng chat */
	showUser(pkt->lent, pkt->text);
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
	while (getchar() != '\n')
		;
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
	sendpkt(sock, DONE, 0, NULL);

	/* Nhận phản hồi từ phòng chat */
	pkt = recvpkt(sock);
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}

	if (pkt->type != DONE)
	{
		fprintf(stderr, "error: unexpected reply from server6\n");
		exit(1);
	}

	/* Hiển thị phòng chat */
	showUser(pkt->lent, pkt->text);

	/* Tên phòng chat */
	while (getchar() != '\n')
		;
	printf("which account?\n ");

	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	/* Thoát */
	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
	uname = malloc(strlen(bufr) + 1);
	strcpy(uname, bufr);//strcpy(uname,bufr);

	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, uname);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, JOIN_2, bufrlen, bufr);

	/* Nhận phản hồi từ server */
	pkt = recvpkt(sock);
	if (!pkt)
	{
		printf("error: server died\n");
		exit(1);
	}
	if (pkt->type != JOIN_ACCEPTED && pkt->type != JOIN_REJECTED)
	{
		fprintf(stderr, "error: unexpected reply from server7\n");
		exit(1);
	}

	/*Từ chối cho vào phòng */
	if (pkt->type == JOIN_REJECTED)
	{
		printf("admin: %s\n", pkt->text);
		free(uname);
		return (0);
	}
	else /* Tham gia thành công */
	{
		printf("admin: You chat with '%s'!\n", uname);
		free(uname);
		return (1);
	}
}

int login(int sock, int *check)
{
	Packet *pkt;
	char bufr[MAXNAMELEN];
	char *bufrptr;
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

	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, username);
	bufrptr += strlen(bufrptr) + 1;
	strcpy(bufrptr, pass);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, LOG_IN, bufrlen, bufr);

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
	char *bufrptr;
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

	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, username);
	bufrptr += strlen(bufrptr) + 1;
	strcpy(bufrptr, pass);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, REGISTER, bufrlen, bufr);

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

int sendCreatRoom(int sock)
{
	Packet *pkt;
	char bufr[MAXNAMELEN];
	char *bufrptr;
	int bufrlen;
	char *name;
	char *cap;
	printf("===Creat===\n");
	while (getchar() != '\n')
		;
	printf("Room's name: ");
	fgets(bufr, MAXPKTLEN, stdin);
	bufr[strlen(bufr) - 1] = '\0';

	if (strcmp(bufr, "") == 0 || strncmp(bufr, QUIT_STRING, strlen(QUIT_STRING)) == 0)
	{
		close(sock);
		exit(0);
	}
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

	/* Gửi tin nhắn */
	bufrptr = bufr;
	strcpy(bufrptr, name);
	bufrptr += strlen(bufrptr) + 1;
	strcpy(bufrptr, cap);
	bufrptr += strlen(bufrptr) + 1;
	bufrlen = bufrptr - bufr;
	sendpkt(sock, CREAT_ROOM, bufrlen, bufr);

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
					__fpurge(stdout);
					sendpkt(sock, MENU, strlen(bufr1), bufr1);
				}
				/* Xử lí thông tin từ máy chủ */
				if (FD_ISSET(sock, &tempfds))
				{
				    fflush(stdin);
					Packet *pkt1;
					char *tm;
					pkt1 = recvpkt(sock);
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
							int pkt1_len = pkt1->lent;
						    char uname[pkt1_len];
							snprintf(uname, pkt1_len + 1, "%s", pkt1->text);
							printf("admin: You chat with '%s' \n", uname);
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
							break;
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
								sendCreatRoom(sock);
								break;
							}
						case 2: /*Vào phòng */
							/* Tham gia trò chuyện */
							{
								if (!joinagroup(sock))
									continue;

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
										if (pkt->type != USER_TEXT)
										{
											fprintf(stderr, "error: unexpected reply from serve1r\n");
											exit(1);
										}
										printf("%s: %s", pkt->text, pkt->text + strlen(pkt->text) + 1);
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
											sendpkt(sock, LEAVE_GROUP, 0, NULL);
											break;
										}

										/*Gửi tin nhắn đến máy chủ */
										sendpkt(sock, USER_TEXT, strlen(bufr) + 1, bufr);
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
							if (!join11(sock))
								continue;

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
							break;
						}
						case 6:
						{
							logout(sock, check);
							break;
						}
						}
						if(choiceFunc!=6) {chatFunction();free(choiceFunc);}
					}
					freepkt(pkt1);
				}
				/* Xử lí đầu vào */
			} while (choiceFunc != 6);
		}
		else
		{
			//printf("Nhap lai:");
			continue;
		}
	}
}
