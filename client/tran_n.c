#include "func.h"

int send_n(int fd,char *buf,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=send(fd,buf+total,len-total,0);
		total+=ret;
	}
	return 0 ;
}

int recv_n(int fd,char *buf,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=recv(fd,buf+total,len-total,0);
		if(ret==0) //服务端结束发送
		{
			printf("server close on recv\n");
			return -1;
		}
		total+=ret;
	}
	return 0;
}
