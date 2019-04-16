#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
int main(int argc,char* argv[]){
	//1.创建套接字
	int fd = socket(AF_LOCAL,SOCK_STREAM,0);
	if(fd == -1){
		perror("socket error");
		exit(0);
	}
	unlink("client.sock");
	//2.绑定client的结构体
	struct sockaddr_un client;
	client.sun_family = AF_LOCAL;
	strcpy(client.sun_path,"client.sock");
	int ret = bind(fd,(struct sockaddr*)&client,sizeof(client));
	if(ret == -1){
		perror("bind error\n");
	}
	//3.建立连接(绑定一个server的结构体)
	struct sockaddr_un serv;
	int serv_len = sizeof(serv);
	serv.sun_family = AF_LOCAL;
	strcpy(serv.sun_path,"server.sock");
	connect(fd,(struct sockaddr*)&serv,serv_len);
	//4.通信
	while(1){
		char buf[1024] = {0};
		fgets(buf,sizeof(buf),stdin);
		send(fd,buf,strlen(buf),0);

		recv(fd,buf,sizeof(buf),0);
		printf("recv data:%s\n",buf);
	}

	return 0;
}
