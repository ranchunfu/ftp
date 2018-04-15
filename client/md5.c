#include<openssl/md5.h>
#include "func.h"

int getmd5(char *fname,char *output)
{
	//popoen("md5sum file","r");
	int fd=open(fname,O_RDWR|O_CREAT);
	unsigned long ret;
	char data[1024]={0};
	unsigned char outmd[16]={0};
	MD5_CTX ctx;
	MD5_Init(&ctx);
	while(1)
	{
		ret=read(fd,data,sizeof(data));
		MD5_Update(&ctx,data,ret);
		if(ret==0||ret<sizeof(data))
			break;
	}
	MD5_Final(outmd,&ctx);
	char *out=NULL;
	out=(char*)malloc(35);
	//char *md5=(char*)calloc(32,sizeof(char));
	for(int i=0;i<16;i++)
	{
		sprintf(out+(i*2),"%02x",outmd[i]);
	}
	out[32]='\0';
	strcpy(output,out);
	//printf("output=%s\n",output);
	//printf("out=%s\n",out);
	free(out);
	return 0;
}

