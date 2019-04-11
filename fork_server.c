#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
void worker(int num){
	pid_t pid;
	while((pid = waitpid(-1,NULL,WNOHANG))>0){
		printf("回收子进程pid=%d",pid);
	}
}
int main(int argc,const char*argv[]){
	if(argc <2){
		printf("please reinput\n");
		exit(1);
	}
	//1.执行socket
	int fld = socket(AF_INET,SOCK_STREAM,0);
	if(fld == -1){
		perror("socket error");
		exit(1);
	}
	//初始化我们的服务器
	int pos =atoi(argv[1]);
	struct sockaddr_in serv;
	int serv_len = sizeof(serv);
	memset(&serv,0,serv_len);
	serv.sin_family = AF_INET;//ip4
	serv.sin_port = htons(pos);//设置端口
	serv.sin_addr.s_addr = htonl(INADDR_ANY);//监视本机所有的ip地址
	//绑定我们的ip和端口
	int ret = bind(fld,(struct sockaddr*)&serv,serv_len);
	//设置监听
	ret = listen(fld,36);//同时监听的最大个数
	//接收消息的sockadd
	struct sockaddr_in client;
	int client_len = sizeof(client);
	//利用信号回收子进程
	struct sigaction act;
	act.sa_handler = worker;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGCHLD,&act,NULL);
	while(1){
		int cfd = accept(fld,(struct sockaddr*)&client,&client_len);
		//子进程通信 父进程回收
		while(cfd == -1&&errno == EINTR){
			cfd = accept(fld,(struct sockaddr*)&client,&client_len);
		}
		printf("conecction successful\n");
		pid_t pid = fork();
		if(pid == 0){
			//子进程
			close(fld);
			char ip[64];
			while(1){
				char buf[256];
				printf("client IP = %s,port:%d\n",inet_ntop(AF_INET,&client.sin_addr.s_addr,ip,sizeof(ip)),ntohs(client.sin_port));
				int data_len = read(cfd,buf,sizeof(buf));
				if(data_len == -1){
					printf("read error\n");
					exit(1);
				}
				if(data_len == 0){
					printf("客户端已经断开连接\n");
					close(cfd);
					break;
				}else{
					printf("recv data:%s\n",buf);
					write(cfd,buf,data_len);
				}
			}
			return 0;
		}else if(pid > 0){
			//父进程，回收子进程
			close(cfd);
		}
	}
	close(fld);
	return 0;
}
