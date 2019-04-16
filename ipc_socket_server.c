#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/un.h>
int main(int argc,char* argv[]){
	//1.创建套接字
	int fd = socket(AF_LOCAL,SOCK_STREAM,0);
	if(fd == -1){
		perror("socket error\n");
		exit(1);
	}
	//2.绑定端口-----结构体
	struct sockaddr_un serv;
	serv.sun_family = AF_LOCAL;
	strcpy(serv.sun_path,"server.sock");
	int ret = bind(fd,(struct sockaddr*)&serv,sizeof(serv));
	if(ret == -1){
		perror("bind error\n");
		exit(0);
	}
	//3.设置监听
	ret = listen(fd,32);//设置监听描述符，最大为128
	if(ret == -1){
		perror("listen error\n");
	}
	//4.等待接受请求
	struct sockaddr_un client;
	int cli_len = sizeof(client);
	int cfd = accept(fd,(struct sockaddr*)&client,&cli_len);
	if(cfd == -1){
		perror("accept error");
	}
	//5.通信
	while(1){
		char buf[1024] = {0};
		int recvlen = recv(cfd,buf,sizeof(buf),0);
		if(recvlen == -1){
			perror("recv error\n");
			exit(0);
		}else if(recvlen == 0){
			printf("client disconnect\n");
			break;
		}else{
			printf("recv data:%s",buf);
			send(cfd,buf,strlen(buf),0);
		}
	}
	close(fd);
	close(cfd);
	return 0;
}
