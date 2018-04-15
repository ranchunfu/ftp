#include "func.h"
//创建子进程
void make_child(pData p,int num)
{
	int i;
	pid_t pid;
	int fds[2];
	for(i=0;i<num;i++)
	{
		socketpair(AF_LOCAL,SOCK_STREAM,0,fds);//创建全双工管道
		pid=fork();
		if(pid==0) //子进程
		{
			close(fds[0]);  //子进程关闭写端
			child_handle(fds[1]);
		}
		close(fds[1]);
		p[i].pid=pid;  
		p[i].fd=fds[0];
		printf("pid=%d,fds[0]=%d\n",pid,fds[0]);
	}
}

void child_handle(int fd)
{
	int new_fd;
	char unbusy='o';
	int exit_flag;
	int ret;
	char buf[100]={0};
	//char pwd[100];
	char ip[20]={0};
	int port=0;
	while(1)
	{
		chdir(main_pwd); //修改该进程目录到网盘目录
		bzero(ip,sizeof(ip));
		bzero(&port,sizeof(int));
		recv_fd(fd,&new_fd,&exit_flag,ip,&port); //接受客户端的描述符、ip、端口
		//printf("ip=%s,port=%d\n",ip,port);
		if(exit_flag)    //当接收到主进程发送的退出标记时，退出
		{
			printf("I am child,I will exit\n");
			exit(0);
		}
		printf("I will send file to customer %d\n",new_fd);
 	//	登陆模块		
		char flag='0';
		while(1)
		{
			ret=recv(new_fd,(char*)&flag,sizeof(int),0);
			if(ret<=0)
			{
				perror("recv on login");		
				goto end;
			}
			if(flag=='1') 
			{
				ret=Login(new_fd);
				if(ret==1) break;//登录成功
			}
			if(flag=='2'){
			   	ret=Register(new_fd);
				if(ret==1)
					break;
			}
		}

		while(1)
		{
			bzero(buf,sizeof(buf));
			ret=recv(new_fd,buf,sizeof(buf),0);
			if(ret<=0)
			{
				Record("exit\0",ip,port);
				goto end;
			}
			//puts(buf);
			Record(buf,ip,port); //log记录
			if(strncmp(buf,"cd",2)==0) //比较前2个字符
			{
				ret=Cd(buf);
				if(ret==-1) continue;
			}
			else if(strcmp(buf,"ls")==0)
			{
				File_Info *file=(File_Info *)calloc(20,sizeof(File_Info ));
				int num;
				Ls(getcwd(NULL,0),file,&num);
				send(new_fd,&num,sizeof(int),0);//先发送个数
				send(new_fd,file,sizeof(File_Info)*num,0);//再发送具体信息
				free(file);
			}

			else if(strncmp(buf,"puts",4)==0)
			{	
				printf("reading recv file\n");
				Puts(new_fd);
				printf("接收文件成功\n");
			}
			else if(strncmp(buf,"mkdir",5)==0)
			{
				char fname[20]={0};
				recv(new_fd,fname,sizeof(fname),0);
				mkdir(fname,0777);
			}
			else if(strncmp(buf,"gets",4)==0)
			{
				Gets(buf,new_fd);	
			}

			else if(strncmp(buf,"rm",2)==0)
			{
				Remove(buf);
			}
			else if(strcmp(buf,"pwd")==0)
			{
				bzero(buf,sizeof(buf));
				strcpy(buf,getcwd(NULL,0));
				send(new_fd,buf,sizeof(buf),0); //给客户端发送路径
			}
			else if(strcmp(buf,"exit")==0) 
			{
				goto end;
			}
			else continue;
		}
end:
		write(fd,&unbusy,sizeof(char)); //告诉父进程子进程非忙碌
		printf("someone %d go out\n",new_fd);
		close(new_fd);
	}
}


