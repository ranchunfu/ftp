#include "func.h"

int Ls(int sfd,File_Info *file )
{
	system("clear");
	print();
	int num=0;
	recv(sfd,&num,sizeof(int),0);//先接收个数
	if(num==0)
	{
		printf("该目录下没有文件\n");
		return -1;
	}
	printf("num=%d\n",num);
	recv(sfd,file,sizeof(File_Info)*num,0);//再接收具体信息
	for(int i=0;i<num;i++)
	{
		printf("  %-4c%-20s%16ld\n",file[i].type,file[i].name,file[i].size);
		//  sleep(1);
	}
	return 0;
}


//ls显示本地服务器的文件
int ls_host()
{
	DIR *dir;
	dir=opendir(getcwd(NULL,0));
	if(dir==NULL)
	{
		perror("opendir error ");
		return -1;
	}
	int ret;
	struct dirent *p;
	struct stat buf;
	//int i=0;
	while( (p=readdir(dir))!=NULL ) //每readdir一次，指向下一个文件
	{
		if(strcmp(p->d_name,".")==0||strcmp(p->d_name,"..")==0) continue;
		ret=stat(p->d_name,&buf);//获取每个文件信息，存储在stat结构体中
		if(ret==-1)
		{
			perror("stat");
			return -1;
		}
		if(buf.st_mode/4096==4) printf("  d"); //文件类型，d为目录
		else printf("  -");
		printf("   %-20s",p->d_name); //文件名
		printf("%16ld字节\n",buf.st_size); //文件大小
	}

	return 0;
}

int mkdir_file(int sfd,char *func)
{	
	char fname[100]={0};
	int i=5,j=0;
	while(func[i]!='\0')
	{
		if(func[i]!=' ') fname[j++]=func[i];
		i++;
	}
	//puts(fname);
	if(strcmp(fname,"\0")==0)
	{
		printf("输入错误");
		return 0;
	}
	send(sfd,fname,sizeof(fname),0);


	return 0;
}

//接收文件
int get_file(int sfd,char *func)
{
	char fname[100]={0};
	int i=4,j=0;
	while(func[i]!='\0')
	{
		if(func[i]!=' ') fname[j++]=func[i];
		i++;
	}
	fname[j]='\0';
	puts(fname);

	int len;
	char buf[1000]={0};
	int ret;
	struct stat st;
	ret=stat(fname,&st);
	if(ret==-1)
	{
		off_t i=0;
		send(sfd,&i,sizeof(int),0);//告诉对端本地没有该文件，请求传整体
	}else{
		send(sfd,&st.st_size,sizeof(off_t),0);//告诉对端本地该文件，并发送其大小
	}
	
	//接收文件名
	ret=recv(sfd,&len,sizeof(len),0);
	if(ret==-1)
	{
		printf("server close on fname 1\n");
		goto end;
	}
	if(len==-1)
	{
		printf("没有该文件\n");
		return 0;
	}
	ret=recv(sfd,buf,len,0);
	if(ret==-1)
	{
		printf("server close on fname 2 \n");
		goto end;
	}

	//接收需要的大小
	off_t f_size;
	ret=recv(sfd,&len,sizeof(len),0);
	if(ret==-1)
	{
		printf("server close on size 1\n");
		goto end;
	}
	if(len==0)
	{
		//printf("");
		goto end;
	}
	ret=recv(sfd,&f_size,sizeof(f_size),0);
	if(ret==-1)
	{
		printf("server close on size 2\n");
		goto end;
	}
	//printf("%ld\n",f_size);
	
	int fd=open(buf,O_RDWR|O_CREAT|O_APPEND,0666); //以接收到的文件名为名字创建文件
	if(fd==-1)
	{
		perror("open file");
		return -1;
	}
	struct stat sbuf;
	ret=fstat(fd,&sbuf);
	
	float total=0;
	//float f=f_size*100;
	time_t now,last;
	now=time(NULL);
	last=now;
	//以时间打百分比
	while(1)
	{
		ret=recv_n(sfd,(char*)&len,sizeof(len));
		if(ret==-1)
		{
			printf("%5.2f%s\n",total/f_size*100,"%");
			break;
		}
		else if(len>0)
		{
			total+=len;
			time(&now);
			if(now-last>=1)
			{
				printf("%5.2f%s\r",total/f_size*100,"%");
				fflush(stdout);//若不刷新缓冲区,在高速循环中，不能实时打印,只 有循环结束才打印
				last=now;
			}
			ret=recv_n(sfd,buf,len);
			write(fd,buf,len);
			if(ret==-1)
			{
				printf("%5.2f%s\n",total/f_size*100,"%");
				break;
			}
		}
		else
		{
			printf("            \r");
			printf("%s\n","100%");
			break;
		}
	}
end:
	close(fd);
	return 0;
}


//puts本地文件到服务器端
int tran_file(int sfd,char *func)
{
	char filename[100]={0};
	int i=4,j=0;
	while(func[i]!='\0')
	{
		if(func[i]!=' ') filename[j++]=func[i];
		i++;
	}
	filename[j]='\0';
	puts(filename);

	signal(SIGPIPE,SIG_IGN);
	train d;

	int fd=open(filename,O_RDONLY);
	if(fd==-1)
	{
		perror("tran_file:  open");
		//  close(sfd);
		return -1;
	}

	strcpy(d.buf,filename);
	d.len=strlen(d.buf);
	send(sfd,&d,4+d.len,0); //把文件名发给服务端
	
	//发送MD5码
	char *md5=(char*)malloc(32);
	getmd5(filename,md5);
	printf("md5=%s\n",md5);
	send(sfd,md5,32,0); //发送MD5


	struct stat sbuf;
	int ret=fstat(fd,&sbuf);
	if(ret==-1)
	{
		perror("fstat");
		return -1;;
	}
	
	d.len=sizeof(off_t); //
	memcpy(d.buf,(char*)&sbuf.st_size,d.len);//将文件长度给d.buf
	send_n(sfd,(char*)&d,d.len+4);//发送文件长度给服务端

	float total=0;
	//float f=f_size*100;
	time_t now,last;
	now=time(NULL);
	last=now;
	while((d.len=read(fd,d.buf,sizeof(d.buf)))>0) //发送文件内容
	{
		ret=send_n(sfd,(char*)&d,4+d.len);
		if(ret==-1)
		{
			return -1;
		} 
		else if(d.len>0)
		{
			total+=d.len;
			time(&now);
			if(now-last>=1)
			{
				printf("%5.2f%s\r",total/sbuf.st_size*100,"%");
				fflush(stdout);//若不刷新缓冲,在高速循环中，不能实时打印,只有循环结束才打印
				last=now;
			}
		}
	}
	printf("            \r");
	printf("%s\n","100%");

	d.len=0;
	send(sfd,&d,4+d.len,0);//告诉服务端发送结束
	//close(sfd);
	return 0;
}


void print()
{
	printf("1. cd     进入对应目录\n");
	printf("2. ls     列出相应目录文件\n");
	printf("3. puts   将本地文件上传至服务器\n");
	printf("4. gets   文件名下载服务器文件到本地\n");
	printf("5. rm 删除服务器上文件\n");
	printf("6. pwd    显示目前所在路径\n");
	printf("7. lshost 列出本地客户端目录文件\n");
	printf("8. mkdir  创建新目录文件\n");
	printf("0. exit   退出\n");
	printf("\n\n");
}



