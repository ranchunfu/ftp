#include "func.h"

//log日志，记录客户端的操作

int Record(char *operate,char *ip,int port)
{
	char log[100]={0};//写入log.txt的内容
	time_t t=time(0); //初始化日历时间
	char datetime[20];
	strftime(datetime,sizeof(datetime),"%Y/%m/%d %X",localtime(&t));//将时间格式化
	strcat(log,datetime);//将时间写入记录
	strcat(log," \0");
	char buf[100]={0};
	sprintf(buf,"IP:%s,Port=%d ",ip,port);
	strcat(log,buf);
	strcat(log,"  ");
	strcat(log,operate);

//	if(strncmp(operate,"cd",2)==0)
//	{
//		strcat(log,operate);
//	}
//	else if(strcmp(operate,"ls")==0)
//	{
//		strcat(log,operate);
//	}
//	else if(strncmp(operate,"puts",4)==0)
//	{
//		strcat(log,operate);
//	}
//	else if(strncmp(operate,"gets",4)==0)
//	{
//		strcat(log,operate);
//	}
//	else if(strncmp(operate,"remove",6)==0)
//	{
//		strcat(log,operate);
//	}
//	else if(strcmp(operate,"pwd")==0)
//	{
//		strcat(log,operate);
//	}
//	else if(strcmp(operate,"0")==0)
//	{
//		strcat(log,operate);
//	}

 	strcat(log,"\n\0");
	int fd=open("/home/ranchunfu/ftp/ftpserver/log.txt",O_RDWR|O_CREAT|O_APPEND,0666);
	int ret=write(fd,log,sizeof(log));
	if(ret==-1)
	{
		perror("record write");
		return -1;
	}
//	printf("存入log成功\n");
	close(fd);
}


