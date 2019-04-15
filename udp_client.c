#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
int main(int argc,char* argv[]){
	if(argc<2){
		printf("请输入端口号\n");
		exit(1);
	}
	int lfd = socket(AF_INET,SOCK_DGRAM,0);
	if(lfd == -1){
		perror("socket error\n");
		exit(-1);
	}
	//定义一个服务器的端口号和ip
	int port = atoi(argv[1]);
	struct sockaddr_in ser;
	int ser_len = sizeof(ser);
	ser.sin_port = htons(port);//设置端口
	ser.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&ser.sin_addr.s_addr);
	//通信
	while(1){
		char buf[1024] ={0};
		printf("输入传输的数据：");
		scanf("%s",buf);
		sendto(lfd,buf,strlen(buf),0,(struct sockaddr*)&ser,ser_len);
		int ret = recvfrom(lfd,buf,sizeof(buf),0,NULL,NULL);//不用管我们回收回来的数据
		printf("我们回收回来的数据是:%s\n",buf);
	}
	close(lfd);
	return 0;
}
