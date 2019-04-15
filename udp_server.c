#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <arpa/inet.h>
int main(int argc,char* argv[]){
	if(argc<2){
		printf("请输入端口号\n");
		exit(-1);
	}
	int port = atoi(argv[1]);
	//1.创建套接字
	int lfd = socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in server;
	int ser_len = sizeof(server);
	//2.端口初始化
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	//3绑定端口和ip
	int ret = bind(lfd,(struct sockaddr*)&server,ser_len);
	if(ret == -1){
		perror("bind error\n");
		exit(-1);
	}
	//通信
	struct sockaddr_in client;
	int client_len = sizeof(client);
	while(1){
		char buf[1024]={0};
		ret = recvfrom(lfd,buf,sizeof(buf),0,(struct sockaddr*)&client,&client_len);
		//打印出来ip信息和端口信息
		char ip[64];
		printf("client IP=%s,PORT=%d\n",
				inet_ntop(AF_INET,&client.sin_addr.s_addr,ip,sizeof(ip)),
				ntohs(client.sin_port));
		if(ret == -1){
			perror("recvfrom error\n");
			exit(0);
		}
		else if(ret == 0){
			printf("客户端断开连接\n");
			exit(0);
		}
		else{
			printf("recv data:%s\n",buf);
			sendto(lfd,buf,strlen(buf),0,(struct sockaddr*)&client,client_len);
		}
	}
	close(lfd);
	return 0;
}
