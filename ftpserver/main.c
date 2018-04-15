#include "func.h"

//接受到信号，信号处理函数值往管道输入值，while循环中epoll发现管道可读,进行退出机制
int fds[2];
void sigfunc2(int sig)
{
	char flag='e';
	write(fds[1],&flag,sizeof(flag));
}

int main(int argc,char *argv[])
{
	pipe(fds);
	signal(SIGUSR1,sigfunc2);	
	//signal(SIGQUIT,sigfunc2);	

	int num=5;
	pData p=(pData)calloc(num,sizeof(Data));
	make_child(p,num);  //产生子进程
	
	int server=socket(AF_INET,SOCK_STREAM,0); //产生套接字
	if(server==-1)
	{
		perror("socket");
		return -1;
	}
	int ret;	
	int reuse=1;
	ret=setsockopt(server,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));//设置服务端断开可重连接
	if(-1==ret)
	{
		perror("setsockopt");
		return -1;
	}
	struct sockaddr_in saddr;
	bzero(&saddr,sizeof(saddr));
	saddr.sin_family=AF_INET;  //采用IPV4
	saddr.sin_port=htons(2000); //端口号，转换为网络字节序
	saddr.sin_addr.s_addr=inet_addr("192.168.88.131");//将IP地址转换为网络字节序
	ret=bind(server,(struct sockaddr*)&saddr,sizeof(saddr));//绑定端口和ip地址
	if(ret==-1)
	{
		perror("bind");
		return -1;
	}

	int epfd=epoll_create(1);
	//epoll_add(epfd,0); //监控标准输入
	epoll_add(epfd,server); //监控socket描述符
	for(int i=0;i<num;i++)
	{
		epoll_add(epfd,p[i].fd);  //监控每个子进程
	}
	epoll_add(epfd,fds[0]);
	ret=listen(server,10); //激活，使服务器这个端口和ip处于监听状态
	if(ret==-1)
	{
		perror("listen");
		return -1;
	}
	struct epoll_event *event=(struct epoll_event*)calloc(num+1,sizeof(struct epoll_event));
	int i,j,count;	
	int new_fd;
	char buf[100]={0};
	char flag;
	while(1)
	{
		bzero(event,sizeof(struct epoll_event)*(num+1));
		count=epoll_wait(epfd,event,num+1,-1);
		for(i=0;i<count;i++)
		{
			if(server==event[i].data.fd)
			{
				struct sockaddr_in clientaddr;
				int addrlen=sizeof(struct sockaddr);
				bzero(&clientaddr,sizeof(struct sockaddr));
				new_fd=accept(server,(struct sockaddr*)&clientaddr,&addrlen);
				char ip[100]={0};
				strcpy(ip,inet_ntoa(clientaddr.sin_addr));
				int port=ntohs(clientaddr.sin_port);
				Record("accept\0",ip,port);
				//找到非忙碌子进程，把new_fd传递给它
				for(j=0;j<num;j++)
				{
					if(p[j].busy==0)
					{
						//printf("server:ip=%s,port=%d\n",ip,port);
						send_fd(p[j].fd,new_fd,0,ip,port);//把客户端new_fd,ip,port发给子进程
						break;
					}
				}
				close(new_fd); //引用计数-1，只有子进程引用该描述符
				p[j].busy=1; //找到子进程置为忙碌状态
				printf("p[j].pid=%d is busy\n",p[j].pid);
			}
			if(fds[0]==event[i].data.fd)//退出
			{
				epoll_del(epfd,server); //解注册sfd
				close(server);
				for(j=0;j<num;j++)
				{
					send_fd(p[j].fd,0,1,NULL,0);//告诉每一次子进程要退出了,1为退出标记
				}
				for(j=0;j<num;j++)
				{
					wait(NULL);
				}
				exit(0);
			}
			for(j=0;j<num;j++)//发现对应子进程的描述符可读，设置为非忙碌
			{
				if(p[j].fd==event[i].data.fd)
				{
					read(p[j].fd,&flag,sizeof(char));
					printf("p[j].pid=%d is not busy,flag=%c\n",p[j].pid,flag);
					p[j].busy=0;
				}
			}
		}
	}
	close(server);
}




