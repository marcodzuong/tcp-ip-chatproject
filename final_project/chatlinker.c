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

struct bin_tree {
int data;
struct bin_tree * right, * left;
};

typedef struct bin_tree node;

int startserver(char *port){
  int     sd;      
  int     myport;  
  const char *  myname; 
  sd = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
  server_address.sin_port = htons(atoi(port));

  bind(sd, (struct sockaddr *) &server_address, sizeof(server_address));
  listen(sd, 20);

  char hostname[MAXNAMELEN];

  if (gethostname(hostname, sizeof hostname) != 0)
  	perror("gethostname");

  struct hostent* h;
	h = gethostbyname(hostname);

  int len = sizeof(struct sockaddr);

  getsockname(sd, (struct sockaddr *) &server_address, &len);

  myport = ntohs(server_address.sin_port);
  if(myport!=atoi(port)){
    printf("error: server already exist!\n");
    exit(0);
  }
   printf("admin: started at port: '%d', sd = %d\n",myport, sd);
  return(sd);
}

int hooktoserver(char* port, char* addr){
  int sd;     
	sd = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent *hostinfo;
	struct sockaddr_in address; 
	address.sin_addr.s_addr = inet_addr(addr);
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(port));

	if (connect(sd, (struct sockaddr *) &address, sizeof(address)) < 0){
		perror("connecting");
		exit(1);
	}
	printf("admin: connected to server at port: '%s'\n",
	port);
	return(sd);
}

int readn(int sd, char *buf, int n){
  int     toberead;
  char *  ptr;

  toberead = n; 
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

static int stringCompare(const void *p1, const void *p2)
{
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

Packet *recvpkt(int sd){
  Packet *pkt;
  pkt = (Packet *) calloc(1, sizeof(Packet));
  if (!pkt) {
    fprintf(stderr, "error : unable to calloc\n");
    return(NULL);
  }

  if (!readn(sd, (char *) &pkt->type, sizeof(pkt->type))) {
    free(pkt);
    return(NULL);
  }

  if (!readn(sd, (char *) &pkt->lent, sizeof(pkt->lent))) {
    free(pkt);
    return(NULL);
  }
  pkt->lent = ntohl(pkt->lent);

  if (pkt->lent > 0) {
    pkt->text = (char *) malloc(pkt->lent);
    if (!pkt) {
      fprintf(stderr, "error : unable to malloc\n");
      return(NULL);
    }

    if (!readn(sd, pkt->text, pkt->lent)) {
      freepkt(pkt);
      return(NULL);
    }
  }
  return(pkt);
}

void printStrings(char *arr[], size_t size)
{
    for (size_t i = 0; i < size; i++)
        printf("%10s | ", arr[i]);
    printf("\n");
}

int sendpkt(int sd, char typ, long len, char *buf){
  char tmp[8];
  long siz;

  bcopy(&typ, tmp, sizeof(typ));
  siz = htonl(len); 
  bcopy((char *) &siz, tmp+sizeof(typ), sizeof(len));
  write(sd, tmp, sizeof(typ) + sizeof(len));

  if (len > 0)
    write(sd, buf, len); 
  return(1);
}

void freepkt(Packet *pkt){
  free(pkt->text);
  free(pkt);
}

void insert(node ** tree, int val)
{
    node *temp = NULL;
    if(!(*tree))
    {
        temp = (node *)malloc(sizeof(node));
        temp->left = temp->right = NULL;
        temp->data = val;
        *tree = temp;
        return;
    }

    if(val < (*tree)->data)
    {
        insert(&(*tree)->left, val);
    }
    else if(val > (*tree)->data)
    {
        insert(&(*tree)->right, val);
    }

}

void print_preorder(node * tree)
{
    if (tree)
    {
        printf("%d\n",tree->data);
        print_preorder(tree->left);
        print_preorder(tree->right);
    }

}

void print_inorder(node * tree)
{
    if (tree)
    {
        print_inorder(tree->left);
        printf("%d\n",tree->data);
        print_inorder(tree->right);
    }
}

void print_postorder(node * tree)
{
    if (tree)
    {
        print_postorder(tree->left);
        print_postorder(tree->right);
        printf("%d\n",tree->data);
    }
}

void deltree(node * tree)
{
    if (tree)
    {
        deltree(tree->left);
        deltree(tree->right);
        free(tree);
    }
}

node* search(node ** tree, int val)
{
    if(!(*tree))
    {
        return NULL;
    }

    if(val < (*tree)->data)
    {
        search(&((*tree)->left), val);
    }
    else if(val > (*tree)->data)
    {
        search(&((*tree)->right), val);
    }
    else if(val == (*tree)->data)
    {
        return *tree;
    }
}