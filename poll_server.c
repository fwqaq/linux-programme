#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <poll.h>
#include <ctype.h>
int main(int argc,const char* argv[]){

	if(argc<2){
		printf("请输入端口号\n");
		exit(1);
	}
	struct sockaddr_in server,client;
	int server_len,client_len;
	server_len = sizeof(server);
	client_len = sizeof(server);
	memset(&server,0,sizeof(server));
	//创建套接字
	int lfd = socket(AF_INET,SOCK_STREAM,0);
	//设置端口号
	int port = atoi(argv[1]);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	//绑定端口
	int ret = bind(lfd,(struct sockaddr*)&server,server_len);
	if(ret == -1){
		perror("bind error\n");
		exit(0);
	}
	//建立绑定的最大端口数量
	ret = listen(lfd,32);
	if(ret == -1){
		perror("listen error\n");
		exit(0);
	}
	printf("start accept\n");
	//设置po'l'l结构体
	struct pollfd allfd[1024];
	int max_index = 0;
	//对结构体进行初始化
	int i;
	for(i = 0;i<1024;i++){
		allfd[i].fd = -1;
	}
	//加入监听的文件描述符
	allfd[0].fd = lfd;
	allfd[0].events = POLLIN;
	while(1){
		i = 0;
		ret = poll(allfd,max_index+1,-1);//请求内核帮助监听文件描述符
		if(ret == -1){
			perror("poll error\n");
			exit(1);
		}
		//判断是不是由连接请求
		if(allfd[0].revents & POLLIN){
			int cfd = accept(lfd,(struct sockaddr*)&client,&client_len);
			//接受连接请求了
			if(cfd == -1){
				perror("accept error\n");
			}
			printf("已经接受来连接请求了\n");
			for(i = 0;i<1024;i++){
				if(allfd[i].fd==-1){
					allfd[i].fd = cfd;
					break;//将客户端的文件描述符添加到我们的数组中去
				}
			}
			max_index = max_index > i ? max_index : i;
		}
		//遍历数组进行检查书否有客户端通信
		for(i = 1;i <= max_index;i++){
			if(allfd[i].fd == -1){
				continue;
			}
			if(allfd[i].revents & POLLIN){
				printf("这里有错吗\n");
				char buf[1024] = {0};
				ret = recv(allfd[i].fd,buf,sizeof(buf),0);
				if(ret == -1){
					perror("read error\n");
					exit(-1);
				}
				if(ret == 0){
					close(allfd[i].fd);
					allfd[i].fd = -1;
					printf("客户端已经断开了连接\n");
					exit(0);
				}
				else{
					printf("这里有错吗\n");
					printf("recv data[%s]\n",buf);
					send(allfd[i].fd,buf,strlen(buf),0);
				}
			}
		}
	}
	close(lfd);
	return 0;
}
