#include "func.h"

//生成12位的随机字符串
int get_rand_str(char *s)
{
	int num=11;
	char *str="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;\"'<>?";
	int i,lstr=strlen(str);
	char ss[2]={0};
	srand((int)time(0));
	for(i=0;i<num;i++)
	{
		sprintf(ss,"%c",str[(rand()%lstr)]);//rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
		strcat(s,ss);//将随机生成的字符串连接到指定数组后面
	}
	s[11]='\0';
	return 0;
}

int Login(int sfd)
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
	int t,r;
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
		goto end;
	}
	res=mysql_use_result(conn);
	
	char username[20]={0};
	char passwd[20]={0};
	char salt[20]={0};
	char p[100]={0};//密文
  	int result=0;	
	recv(sfd,username,sizeof(username),0);
	int flag=0;
	if(res)
	{
		while((row=mysql_fetch_row(res))!=NULL)//依次取一行
		{
			if(strcmp(username,row[0])==0)
			{
				strcpy(salt,row[1]);
				send(sfd,salt,sizeof(salt),0); //接收随机字符串
				flag=1;
				recv(sfd,p,sizeof(p),0);  //接收密文
				if(strcmp(p,row[2])==0)
				{
					flag=2;
					result=1;
					send(sfd,(char *)&result,sizeof(int),0);
					printf("有个吊毛客户端登录了\n");
				}else{
					result=0;
					send(sfd,(char *)&result,sizeof(int),0);
				}
			}else{
				continue;
			}
		}
		if(flag==0){
			bzero(salt,sizeof(salt));
			strcpy(salt,"error");
			send(sfd,salt,sizeof(salt),0);
		}
	}
end:
	mysql_free_result(res);
	mysql_close(conn);
	return result;
}
//客户端注册
int Register(int sfd)
{
	char username[20]={0};
	char salt[20]={0};
	char p[100]={0};//密文
	get_rand_str(salt);//获取随机字符串
	recv(sfd,username,sizeof(username),0);//接收用户名
	send(sfd,salt,(sizeof(salt)),0);//发送随机字符串
	recv(sfd,p,sizeof(p),0);//接收密文
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="ran123";
	char* database="ranchunfu_1";//要访问的数据库名称
	char insert[300]="insert into Account(Username,salt,passwd)values(";
	char buf[100]={0};
	sprintf(buf,"'%s','%s','%s')",username,salt,p);
	strcat(insert,buf);
	//printf("insert_sql=%s\n",insert);
	conn=mysql_init(NULL);
	//int result=0;
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}else{
		//printf("连接数据库成功,table:Account\n");
	}
	int t=mysql_query(conn,insert);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
		mysql_close(conn);
		int result=0;
		send(sfd,(char*)&result,sizeof(int),0);
		return -1;
	}
	else{
		printf("insert success\n");
		mysql_close(conn);
		int result=1;
		send(sfd,(char*)&result,sizeof(int),0);
		return 1;
	}
	return 0;
}

