#include "func.h"

int Login(int sfd)
{
	char username[20]={0};
	//char passwd[20]={0};
	char *passwd;
	char salt[20]={0};
	char p[100]={0};//密文
	int result;
	int ret=0;
	printf("账号：");
	scanf("%s",username);
	printf("密码：");
	passwd=getpass("");
	//scanf("%s",passwd);
	setbuf(stdin,NULL);//清空缓冲区
	ret=send(sfd,username,sizeof(username),0);//发送账号
	ret=recv(sfd,salt,sizeof(salt),0); //接收随机字符串
	if(strcmp(salt,"error")==0)
	{
		printf("账户名错误\n");
		return -1;
	}
	strcpy(p,crypt(passwd,salt));   //密文
	send(sfd,p,sizeof(p),0);  //发送密文
	recv(sfd,(char *)&result,sizeof(int),0);//得到结果
	if(result==1) return 1;
	else {
		printf("密码错误\n");
		return -1;
	}
}

int Register(int sfd)
{
	char username[20]={0};
	char passwd[20]={0};
	char salt[20]={0};
	char p[100]={0};//密文
	printf("账号：");
	scanf("%s",username);
	printf("密码：");
	scanf("%s",passwd);
	send(sfd,username,sizeof(username),0);//发送账号
	recv(sfd,salt,sizeof(salt),0);//接收随机字符串
	strcpy(p,crypt(passwd,salt));   //密文
	send(sfd,p,sizeof(p),0);  //发送密文
	int result;
	recv(sfd,(char*)&result,sizeof(int),0);
	if(result==1){
		printf("注册成功啦！！come on \n\n");
		return 1;
	}
	else printf("注册失败，用户名已经被注册啦骚年\n\n");
//	printf("username=%s\n",username);
//	printf("salt=%s\n",salt);
//	printf("p=%s\n",p);
	return 0;
}

