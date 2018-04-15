#include "func.h"

int Cd(char* buf)//进入相对应的目录
{
	//printf("cd:  %s\n",buf);
	char func[100]={0};
	int i=2,j=0;
	while(buf[i]!='\0')
	{
		if(buf[i]!=' ') func[j++]=buf[i];
		i++;
	}
	func[j]='\0';
	//printf("func=%s\n",func);
	if(strcmp(main_pwd,getcwd(NULL,0))==0)
	{
		if(strcmp(func,"..")==0)
		{
			printf("cd error\n");
			return -1;
		}
	}
	chdir(func);

	return 0;
}

int Ls(char *path,File_Info *fileinfo ,int *num)
{
	DIR *dir;
	dir=opendir(path);
	if(dir==NULL)
	{
		perror("opendir error ");
		return -1;
	}
	int ret;
	struct dirent *p;
	struct stat buf;
	int i=0;
	while( (p=readdir(dir))!=NULL ) //每readdir一次，指向下一个文件
	{
		if(strcmp(p->d_name,".")==0||strcmp(p->d_name,"..")==0) continue;
		ret=stat(p->d_name,&buf);//获取每个文件信息，存储在stat结构体中
		if(ret==-1)
		{
			perror("stat");
			return -1;
		}
		if(buf.st_mode/4096==4) fileinfo[i].type='d'; //文件类型，d为目录
		else fileinfo[i].type='-';	
		strcpy(fileinfo[i].name,p->d_name);
		fileinfo[i].size=buf.st_size;
		i++;
		//printf(" %20s",p->d_name); //文件名
		//printf(" %10ld字节\n",buf.st_size); //文件大小
	}
	*num=i;
	//printf("ls发送成功\n");
	return 0;
}

int Puts(int sfd)
{

	int len;
	char buf[1000]={0};
	int ret;
	//接收文件名
	ret=recv(sfd,&len,sizeof(len),0);
	if(ret==-1)
	{
		printf("server close\n");
		goto end;
	}
	ret=recv(sfd,buf,len,0);
	if(ret==-1)
	{
		printf("server close\n");
		goto end;
	}
	//puts(buf);
	
	//接收MD5码
	char *md5=NULL;
	md5=(char *)malloc(32*sizeof(char));
	recv(sfd,md5,32,0);  //接收MD5

//	Connect_Mysql();
//	if(strcmp(md5,row[ md5 ])==0)
//	{	
//		row[link]++;
//	}
//
//

	//接收文件大小
	off_t f_size;
	ret=recv(sfd,&len,sizeof(len),0);
	if(ret==-1)
	{
		printf("server close\n");
		goto end;
	}
	ret=recv(sfd,&f_size,sizeof(f_size),0);
	if(ret==-1)
	{
		printf("server close\n");
		goto end;
	}
	printf("%ld\n",f_size);
	
	
	int fd=open(buf,O_RDWR|O_CREAT,0666); //以接收到的文件名为名字创建文件

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
				fflush(stdout);//若不刷新缓冲,高速循环中，不能实时打印,只有循环结束才打印
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
//给客户端发送文件
int Gets(char *buf,int new_fd)
{	
	//printf("正在发送文件\n");
	char fname[100]={0};
	int i=4,j=0;
	while(buf[i]!='\0')
	{
		if(buf[i]!=' ') fname[j++]=buf[i];
		i++;
	}
	//fname[j]='\0';
	printf("fname=%s\n",fname);
	int ret=tran_file(new_fd,fname); //发送文件
	if(ret==-1) printf("发送文件失败\n");
	else printf("发送文件完成\n");
	return 0;
}

int Remove(char *buf)
{
	char fname[100]={0};
	int i=2,j=0;
	while(buf[i]!='\0')
	{
		if(buf[i]!=' ') fname[j++]=buf[i];
		i++;
	}
	//puts(fname);
	int ret=remove(fname);
	if(ret==-1) printf("删除失败\n");
	else printf("删除成功\n");
	return 0;
}

char* Pwd()
{
	return getcwd(NULL,0);
}



int Connect_Mysql(char *md5)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="ran123";
	char* database="ranchunfu_1";//要访问的数据库名称
	char query[300]="select * from Account";
	//puts(query);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}else{
	//	printf("连接数据库成功,table:Account\n");
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
		mysql_close(conn);
		return -1;
	}
	res=mysql_use_result(conn);
	if(res)
	{
		while( (row=mysql_fetch_row(res))!=NULL)
		{
			
		}
	}
}


