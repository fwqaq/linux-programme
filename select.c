#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
int main(int argc,char* argv[]){
	if(argc<2){
		printf("没有输入端口号\n");
		return -1;
	}
	struct sockaddr_in server;
	int server_len = sizeof(server);
	int port = atoi(argv[1]);
	//创建套接字
	int lfd = socket(AF_INET,SOCK_STREAM,0);
	//设置端口号
	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	//绑定ip和端口号
	int ret = bind(lfd,(struct sockaddr*)&server,server_len);
	if(ret == -1){
		perror("bind error\n");
		exit(-1);
	}
	//设置监听的最大个数
	ret = listen(lfd,32);
	printf("start accept\n");
	//设置客户端的socket
	struct sockaddr_in client;
	int client_len = sizeof(client);

	//最大的文件描述符
	int maxfd = lfd;//刚开始只有我们一个文件描述符
	fd_set reads,tmp;//这里设置两个，tmp用于内核进行改变
	//初始化文件描述符表
	FD_ZERO(&reads);
	FD_SET(lfd,&reads);

	while(1){
		//委托内核进行IO检测
		tmp = reads;
		ret = select(maxfd+1,&tmp,NULL,NULL,NULL);
		if(ret == -1){
			perror("select error\n");
			exit(1);
		}
		//客户端发起新的连接
		if(FD_ISSET(lfd,&tmp)){
			//接受连接请求   accept不阻塞
			int cfd = accept(lfd,(struct sockaddr*)&client,&client_len);
			if(cfd == -1){
				perror("accept error\n");
				exit(1);
			}
			char ip[64];
			printf("new client IP=%s,  port=%d\n",
					inet_ntop(AF_INET,&client.sin_addr.s_addr,ip,sizeof(ip)),ntohs(client.sin_port));
			FD_SET(cfd,&reads);
			maxfd = maxfd > cfd ? maxfd : cfd;
		}
		int i = lfd+1;	
		for(i = lfd+1;i<=maxfd;++i){
			if(FD_ISSET(i,&tmp)){
				char buf[1024] = {0};
				int len = recv(i,buf,sizeof(buf),0);
				if(len == -1){
					perror("recv error\n");
					exit(0);
				}
				if(len == 0){
					printf("客户端已经关闭了\n");
					close(i);
					FD_CLR(i,&reads);
				}else{
					printf("recv data[%s]",buf);
					send(i,buf,strlen(buf),0);
				}
			}
		}
	}
	close(lfd);
	return 0;

}
