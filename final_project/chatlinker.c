//Chức năng kết nối máy chủ và máy khách

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"

/*
 Chuẩn bị máy chủ cho các yêu cầu của khách hàng,
   Trả lại chính xác mô tả tập tin ổ cắm
   Lỗi trả về -1
*/
int startserver(char *port)
{
  int     sd;      /* socket mô tả */
  int     myport;  /* Cổng máy chủ */
  const char *  myname;  /* Tên đầy đủ của localhosst  */

  /*
	Gọi hàm socket để tạo mô tả socket TCP
  */
  sd = socket(PF_INET, SOCK_STREAM, 0);

  /*
   Gọi bind để gán địa chỉ cục bộ cho socket 
  */

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
  /*Hàm htonl được sử dụng để chuyển đổi INADDR_ANY sang thứ tự byte mạng  */
  server_address.sin_port = htons(atoi(port));

  bind(sd, (struct sockaddr *) &server_address, sizeof(server_address));

  /*Gọi listen để đặt sd  của socket phía máy chủ sang trạng thái nghe thụ động và đặt độ dài của hàng đợi chấp nhận thành 20 */
  listen(sd, 20);

  /*
    Gọi getsockname, gethostname và gethostbyname để xác định tên máy chủ cục bộ và số cổng máy chủ.
  */

  char hostname[MAXNAMELEN];

  if (gethostname(hostname, sizeof hostname) != 0)
  	perror("gethostname");

  struct hostent* h;
	h = gethostbyname(hostname);

  int len = sizeof(struct sockaddr);

  getsockname(sd, (struct sockaddr *) &server_address, &len);

  // myname = h->h_name;
  myport = ntohs(server_address.sin_port);
  if(myport!=atoi(port)){
    printf("error: server already exist!\n");
    exit(0);
  }

  /*Sẵn sàng chấp nhận yêu cầu của khách hàng */
  // printf("admin: started server on '%s' at '%d'\n",
	//  myname, myport);
   printf("admin: started at port: '%d'\n",
	 myport);
  return(sd);
}

/*
  Kết nối đến máy chủ, trả về socket đúng.
  Thất bại trả về  -1
*/
int hooktoserver(char* port, char* addr)
{
	int sd;                 

	/*
	Gọi socket để tạo kết nối TCP
	*/

	sd = socket(AF_INET, SOCK_STREAM, 0);
	
	/*
Gọi gethostbyname () và connect () để kết nối với cổng 'servport' của 'servhost'
	*/
	struct hostent *hostinfo;
	struct sockaddr_in address; // thong tin dia chi server

	// hostinfo = gethostbyname(servhost); /*Tên máy chủ */
	address.sin_addr.s_addr = inet_addr(addr);
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(port));

	if (connect(sd, (struct sockaddr *) &address, sizeof(address)) < 0)
	{
		perror("connecting");
		exit(1);
	}

	/* Kết nối thành công */
	printf("admin: connected to server at port: '%s'\n",
	port);
	return(sd);
}

/* Đọc thông tin về một ổ cắm từ kernel */
int readn(int sd, char *buf, int n)
{
  int     toberead;
  char *  ptr;

  toberead = n; // so byte can ghi
  ptr = buf;
  while (toberead > 0) {
    int byteread;

    byteread = read(sd, ptr, toberead);
    if (byteread <= 0) {
      if (byteread == -1)
	      perror("read");
      return(0);
    }

    toberead -= byteread;
    ptr += byteread;
  }
  return(1);
}

/* Nhận gói */
Packet *recvpkt(int sd)
{
  Packet *pkt;

  /* Bộ nhớ đc phân bố rông */
  pkt = (Packet *) calloc(1, sizeof(Packet));
  if (!pkt) {
    fprintf(stderr, "error : unable to calloc\n");
    return(NULL);
  }

  /* Đọc loại tin nhắn */
  if (!readn(sd, (char *) &pkt->type, sizeof(pkt->type))) {
    free(pkt);
    return(NULL);
  }

  /* Đọc độ dài tin nhắn */
  if (!readn(sd, (char *) &pkt->lent, sizeof(pkt->lent))) {
    free(pkt);
    return(NULL);
  }
  pkt->lent = ntohl(pkt->lent);

  /* Phân bổ không gian cho nội dung tin nhắn */
  if (pkt->lent > 0) {
    pkt->text = (char *) malloc(pkt->lent);
    if (!pkt) {
      fprintf(stderr, "error : unable to malloc\n");
      return(NULL);
    }

    /*Đọc tin nhắn văn bản */
    if (!readn(sd, pkt->text, pkt->lent)) {
      freepkt(pkt);
      return(NULL);
    }
  }
  return(pkt);
}

/* Gửi gói */
int sendpkt(int sd, char typ, long len, char *buf)
{
  char tmp[8];
  long siz;

  /* Viết loại và độ dài của gói vào socket */
  bcopy(&typ, tmp, sizeof(typ));
  siz = htonl(len); // chuyển từ byte máy chủ sag byte mạng
  bcopy((char *) &siz, tmp+sizeof(typ), sizeof(len));
  write(sd, tmp, sizeof(typ) + sizeof(len));

  /*Viêt tin nhắn văn bản vào socket*/
  if (len > 0)
    write(sd, buf, len); //buff: nội dung văn bản 
  return(1);
}

/*Giải phóng không gian bộ nhớ bị chiếm dụng bởi các gói */
void freepkt(Packet *pkt)
{
  free(pkt->text);
  free(pkt);
}
