#include "func.h"

//服务器断开，客户端正常退出
int main(int argc,char *argv[])
{
	chdir(main_download);//本地客户端文件所存储位置
	printf("连接中......\n");
	int sfd=socket(AF_INET,SOCK_STREAM,0); //产生套接字
	if(sfd==-1)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in saddr;
	bzero(&saddr,sizeof(struct sockaddr_in));
	saddr.sin_family=AF_INET;  //采用IPV4
	saddr.sin_port=htons(2000); //端口号，转换为网络字节序
	saddr.sin_addr.s_addr=inet_addr("192.168.88.131");//将IP地址转换为网络字节序

	int ret=connect(sfd,(struct sockaddr*)&saddr,sizeof(struct sockaddr));
	if(ret==-1)
	{
		perror("connect");
		return -1;
	}
	printf("连接成功\n");

	char select;
	int i=0;
	while(i!=3)
	{
		printf("1.登陆\n");
		printf("2.注册\n");
		select=getchar();
		//scanf("%c",select);
		setbuf(stdin,NULL);//使stdin输入流由默认缓冲区变成无缓冲区，这样scanf输入完成后，缓冲区东西就消失了
		//flush(stdin);
		if(select=='1')
		{	
			//int flag=1;
			send(sfd,(char*)&select,sizeof(char),0);
			ret=Login(sfd);//登陆
			setbuf(stdin,NULL);
			if(ret==1) break;
		}
		else if(select=='2') {
			send(sfd,(char*)&select,sizeof(char),0);
			ret=Register(sfd);
			setbuf(stdin,NULL);
			if(ret==1) continue;
			else {
				i++;
				continue;
			}
		}
		else printf("\n输错啦,重新选择功能序号，加油你可以的，还剩%d次机会哦\n",2-i);
		if(i==2) //输错三次关闭客户端
		{
			printf("byebye\n");
			close(sfd);
			return 0;
		}
		i++;
	}
	system("clear");
	char buf[100]={0};
	File_Info *file=(File_Info *)calloc(20,sizeof(File_Info ));
	//int num;
	print();//菜单栏
	char func[100]={0};//用于存储输入指令
	while(1)
	{
		bzero(func,sizeof(func));
		printf("[ranchunfu]$> ");
		i=0;
		//此方法是为了输入空格，同时避免使用gets的报错
		while( (func[i]=getchar()) && func[i]!='\n'){i++;}
		func[i]='\0';
	//	printf("func=%s\n",func);
		if(strncmp(func,"cd",2)==0) //cd进入对应目录
		{
			printf("%s\n",func);
			send(sfd,func,sizeof(func),0);
		}
		else if(strcmp(func,"ls")==0) //ls列出相应目录文件
		{
			send(sfd,"ls",2,0);
			bzero(file,sizeof(file));
			ret=Ls(sfd,file);
			if(ret==-1) continue;
		}
		else if(strcmp(func,"lshost")==0)//ls 本地目录文件
		{
			system("clear");
			print();
			ls_host();
			printf("\n");
		}
		else if(strncmp(func,"mkdir",5)==0) 
		{	
			send(sfd,func,sizeof(func),0);
			mkdir_file(sfd,func);
		}
		else if(strncmp(func,"puts",4)==0) //puts 将本地文件上传至服务器
		{
			send(sfd,func,sizeof(func),0);
			printf("准备发送文件\n");
			ret=tran_file(sfd,func); //发送文件
			if(ret==-1) printf("发送文件失败\n");
			else printf("发送文件完成\n");

		}
		else if(strncmp(func,"gets",4)==0)//gets 文件名下载服务器文件到本地
		{
			send(sfd,func,sizeof(func),0);
			printf("准备接收文件\n");
			get_file(sfd,func);
		}
		else if(strncmp(func,"rm",2)==0)//remove 删除服务器上文件
		{
			printf("%s\n",func);
			send(sfd,func,sizeof(func),0);
		}
		else if(strcmp(func,"pwd")==0) //pwd显示目前所在路径
		{
			//	gets(buf);
			bzero(buf,sizeof(buf));
			send(sfd,"pwd",3,0);
			recv(sfd,buf,sizeof(buf),0);
			puts(buf);
		}
		
		else if(strcmp(func,"exit")==0) //exit 退出
		{
		
			send(sfd,"exit",5,0);
			break;
		}
		else printf("    func输入错误\n");
	}
	close(sfd);
}


