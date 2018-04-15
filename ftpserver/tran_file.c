#include "func.h"
int tran_file(int new_fd,char *filename)
{
	signal(SIGPIPE,SIG_IGN);
	train d;
	off_t size=0;
	recv(new_fd,&size,sizeof(off_t),0);//接收对端该文件已有的大小
	printf("client have size=%ld\n",size);
	int fd=open(filename,O_RDWR,0777);
	if(fd==-1)
	{
		perror("open");
		int i=-1;
		send(new_fd,&i,sizeof(int),0); //告诉对端没有该文件
		//	close(new_fd);
		return -1;
	}

	strcpy(d.buf,filename);
	d.len=strlen(d.buf);
	send(new_fd,&d,4+d.len,0); //把文件名发给客户端

	struct stat sbuf;
	int ret=fstat(fd,&sbuf);
	if(ret==-1)
	{
		perror("fstat");
		return -1;;
	}

	long flag=1024*1024*100;
	printf("sbuf.st_size=%ld\n",sbuf.st_size);
	if(sbuf.st_size<flag)
	{	
		lseek(fd,size,SEEK_SET);//偏移到客户端文件已有字节的位置
		d.len=sizeof(off_t); //
		sbuf.st_size=sbuf.st_size-size;
		memcpy(d.buf,(char*)&sbuf.st_size,d.len);//将需要传送的文件长度给d.buf
		send_n(new_fd,(char*)&d,d.len+4);//发送文件长度给客户端
		while((d.len=read(fd,d.buf,sizeof(d.buf)))>0) //发送文件内容
		{
			ret=send_n(new_fd,(char*)&d,4+d.len);
			if(ret==-1)
			{
				printf("tran file error on while\n");
				return -1;
			}
		}
	}
	else  //文件大于100M，用mmap映射 (该模块暂时没有错误了！！！！！！)
	{
		printf("start use mmap\n");
		off_t need_send=sbuf.st_size-size;
		printf("need_send=%ld\n",need_send);
		char *p=(char *)calloc(need_send+1,sizeof(char));
		p=(char*)mmap(NULL,sbuf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
		if(p==(char*)-1||p==NULL)
		{
			perror("mmap");
			goto end;
		}
		int i;
		//由于mmap中的偏移只能是一页大小的整数倍，所以此处需要进行手动偏移
		for(i=0;i<size;i++) p++;
		d.len=sizeof(off_t); //
		
		memcpy(d.buf,(char*)&sbuf.st_size,d.len);//将文件长度给d.buf
		send_n(new_fd,(char*)&d,d.len+4);//发送文件长度给客户端
		long count=0;
		d.len=1;
		while(1) //发送文件内容
		{ 	
			if( (need_send-count*1000) > 1000)
			{
				memcpy(d.buf,p,1000);
				d.len=1000;	
			}else {
				 memcpy(d.buf,p,need_send-count*1000);
				 d.len=need_send-count*1000;
				 send_n(new_fd,(char*)&d,4+d.len);
				 break;
			}
			for(i=0;i<1000;i++)  p++;
			ret=send_n(new_fd,(char*)&d,4+d.len);
			if(ret==-1)
			{
				return -1;
			}
			count++;
		}
		printf("count=%ld\n",count);
		munmap(p,sbuf.st_size);//结束映射
	}
end:
	d.len=0;
	send(new_fd,&d,4+d.len,0);//告诉客户端发送结束
	close(fd);
	//close(new_fd);
	return 0;
}
