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


#define main_pwd "/home/ranchunfu/ftp/ftpserver/BaiduNetdisk"  //网盘目录

typedef struct{ //用于ls显示
	char type;
	char name[30];
	long size;
}File_Info;

typedef struct {
	pid_t pid;
	int fd;//父子进程使用的全双工管道
	short busy;//1代表忙碌，0代表非忙碌
}Data,*pData;

//小火车，用于文件传送
typedef struct { 
	int len;//控制数据
	char buf[1000];
}train;

void make_child(pData p,int num);
void child_handle(int fd);

void send_fd(int pfd,int  fd,int  ,char *,int);
void recv_fd(int pfd,int *fd,int *,char *,int *);

int tran_file(int,char *);

int send_n(int ,char *,int);
int recv_n(int ,char *,int);

int epoll_add(int epfd,int fd);
int epoll_del(int epfd,int fd);

int Cd(char *);//进入相对应的目录
int Ls(char *,File_Info *,int*);
int Puts(int);
int Gets(char * ,int);
int Remove(char *);
char* Pwd();

int Record(char *operate,char *ip,int port);//记录操作

int get_rand_str(char *s);//生成12位的随机字符串
int Login(int);   //客户端登陆
int Register(int);//客户端注册

