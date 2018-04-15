#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <mysql/mysql.h>

#include <sys/uio.h>
#include <shadow.h>
#include <crypt.h>

#include <openssl/md5.h>

#define main_download  "/home/ranchunfu/ftp/client/download"

typedef struct{ //用于ls显示
	char type;
	char name[30];
	long size;
}File_Info;

 //小火车，用于文件传送
typedef struct {
	int len;//控制数据
	char buf[1000];
}train;

int get_file(int ,char *);
int mkdir_file(int,char*);

int send_n(int fd,char *buf,int len);
int recv_n(int fd,char *buf,int len);

int Ls(int,File_Info *);
int ls_host();

int tran_file(int,char*);

void print();

int Login(int);
int Register(int);

int getmd5(char*,char*);

