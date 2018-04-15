#include "func.h"

int  send_n(int fd,char *buf,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=send(fd,buf+total,len-total,0);
		if(ret==-1)//说明客户端断开
		{
			printf("client close\n");
			return -1;
		}
		total+=ret;
	}
	return 0;
}
int recv_n(int fd,char *buf,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=recv(fd,buf+total,len-total,0);
		total+=ret;

	}
	return 0 ;
}


int epoll_add(int epfd,int fd)
{
	struct epoll_event event;
	bzero(&event,sizeof(event));
	event.events=EPOLLIN; //监控读事件
	event.data.fd=fd;
	int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);
	if(ret==-1)
	{
		perror("epoll_ctl_add");
		return -1;
	}
}

int epoll_del(int epfd,int fd)
{
	struct epoll_event event;
	bzero(&event,sizeof(event));
	event.events=EPOLLIN; //监控读事件
	event.data.fd=fd;
	int ret=epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&event);
	if(ret==-1)
	{
		perror("epoll_ctl_del");
		return -1;
	}
}

//内核控制信息传递
void send_fd(int pfd,int fd,int exit_flag,char *ip,int port)
{
	struct msghdr msg;
	bzero(&msg,sizeof(msg));
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)malloc(len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	//char buf[10]="hello";
	char buf1[10]="world";
	struct iovec iov[3];
	iov[0].iov_base=&exit_flag;
	iov[0].iov_len=4;
	iov[1].iov_base=ip;
	iov[1].iov_len=20;
	iov[2].iov_base=&port;
	iov[2].iov_len=sizeof(port);
	msg.msg_iov=iov;
	msg.msg_iovlen=3;
	int ret=sendmsg(pfd,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return;
	}
}

void recv_fd(int pfd,int *fd,int *exit_flag,char *ip,int *port)
{
	struct msghdr msg;
	bzero(&msg,sizeof(msg));
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)malloc(len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	//char buf[10]="hello";
	char buf1[10]="world";
	struct iovec iov[3];
	iov[0].iov_base=exit_flag;
	iov[0].iov_len=4;
	iov[1].iov_base=ip;
	iov[1].iov_len=20;
	iov[2].iov_base=port;
	iov[2].iov_len=sizeof(port);
	msg.msg_iov=iov;
	msg.msg_iovlen=3;
	int ret=recvmsg(pfd,&msg,0);
	if(-1==ret)
	{
		perror("recvmsg");
		return;
	}
	*fd=*(int*)CMSG_DATA(cmsg);
}







