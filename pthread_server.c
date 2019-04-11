#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
typedef struct Pthread_server{
	int fd;
	struct sockaddr_in addr;
	pthread_t pid;
}Pthread_server;
void* worker(void* arg){
	Pthread_server* ps =(Pthread_server*)arg;
	//通信
	char ip[32];
	printf("客户端ip:%s,port:%d\n",
			inet_ntop(AF_INET,&ps->addr.sin_addr.s_addr,ip,sizeof(ip)),
			ntohs(ps->addr.sin_port));
	char buf[256];
	while(1){
		int ret = read(ps->fd,buf,sizeof(buf));
		if(ret == -1){
			perror("read error\n");
			pthread_exit(NULL);
		}
		else if(ret == 0){
			printf("客户端已经断开连接\n");
			close(ps->fd);
			break;
		}
		printf("recv data:%s",buf);
		write(ps->fd,buf,sizeof(buf));
	}
	return NULL;
}
int main(int argc,char* argv[]){
	if(argc<2){
		printf("input error\n");
		exit(1);
	}
	//1.创建套接字
	int lfd = socket(AF_INET,SOCK_STREAM,0);
	if(lfd==-1){
		perror("创建socket失败\n");
		exit(1);
	}
	//绑定套接字
	int post = atoi(argv[1]);
	struct sockaddr_in server;
	int server_len = sizeof(server);
	memset(&server,0,server_len);
	server.sin_family = AF_INET;
	server.sin_port =htons(post);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = bind(lfd,(struct sockaddr*)&server,server_len);
	if(ret == -1){
		perror("bind error\n");
	}
	//设置监听
	ret = listen(lfd,32);
	if(ret == -1){
		perror("listen error\n");
	}
	printf("start accept\n");
	Pthread_server ps[256];
	int i = 0;
	for(i =0;i<sizeof(ps)/sizeof(ps[0]);i++){
		ps[i].fd = -1;
	}
	int ps_len = sizeof(Pthread_server);
	while(1){
		//主线程,接受到accept就创建一个线程
		//寻找最小的i进行赋值
		for(i = 0;i<sizeof(ps)/sizeof(ps[0]);i++){
			if(ps[i].fd == -1){
				break;
			}
		}
		if(i == 256){
			break;
		}
		int ret = accept(lfd,(struct sockaddr*)&ps[i].addr,&ps_len);
		if(ret == -1){
			perror("accept error");
		}
		ps[i].fd = ret;
		//创建线程
		pthread_create(&ps[i].pid,NULL,worker,&ps[i]);
		//设置线程分离
		pthread_detach(ps[i].pid);

	}
	return 0;
}
