/*--------------------------------------------------------------------*/
/* Thông tin cổng máy chủ */
#define PORTLINK ".chatport"

/* Giới hạn bộ nhớ */
#define MAXNAMELEN 256
#define MAXPKTLEN  2048

/*Định nghĩa 1 số biến*/
#define LIST_GROUPS    0
#define JOIN_GROUP     1
#define LEAVE_GROUP    2
#define USER_TEXT      3
#define JOIN_REJECTED  4
#define JOIN_ACCEPTED  5
#define LOG_IN         6
#define LOG_OUT        7
#define REGISTER       8
#define SUCCESS        9
#define CREAT_ROOM     10
#define UNDONE         11
#define LISTUSERON     12
#define JOIN_2         13
#define UPDATE         14
#define REQUEST        15
#define QUIT           16
#define REFRESH        17
#define MENU           18
#define USER_TEXT1     19
#define LIST_USERGR    20
#define TO             21
#define REQUEST1       22
#define AWAIT          23
#define DONE           24
#define KICK           25
#define KICKU          26
/* Cấu truct _packet */
typedef struct _packet {

  /* Loại gói */
  char      type;

  /* Độ dài nội dung gói */
  long      lent;

  /* Nội dung gói */
  char *    text;

} Packet;

extern int startserver();
extern int hooktoserver();

extern Packet *recvpkt(int sd);
extern int sendpkt(int sd, char typ, long len, char *buf);
extern void freepkt(Packet *msg);
/*--------------------------------------------------------------------*/
