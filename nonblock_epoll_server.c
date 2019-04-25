#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
int main(int argc,char *argv[]){
	if(argc < 2){
		printf("请输入端口号\n");
		exit(-1);
	}
	//创建套接字
	int lfd = socket(AF_INET,SOCK_STREAM,0);
	//初始化我们的端口和IP地址
	struct sockaddr_in server;
	int server_len = sizeof(server);
	int port = atoi(argv[1]);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	//绑定端口
	int ret = bind(lfd,(struct sockaddr*)&server,server_len);
	if(ret == -1){
		perror("bind error\n");
	}
	//设置监听
	ret = listen(lfd,32);
	//创建epoll头节点
	int epfd = epoll_create(2000);
	if(epfd == -1){
		perror("epoll_create error\n");
	}
	//客户端的初始化
	struct sockaddr_in client;
	int client_len = sizeof(client);
	//加入我们的监听
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = lfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&event);
	//能保存的数量
	struct epoll_event all[2000];
	printf("start except....\n");
	while(1){
		//内核帮助做出检测
		ret = epoll_wait(epfd,all,sizeof(all)/sizeof(all[0]),-1);
		int i;
		for(i = 0;i<ret;i++){
			if(all[i].data.fd == lfd){
				//此时就时有新的连接请求了
				int cfd = accept(lfd,(struct sockaddr*)&client,&client_len);
				//将cfd设置为非阻塞
				int flags = fcntl(cfd,F_GETFL);
				flags|=O_NONBLOCK;
				fcntl(cfd,F_SETFL,flags);
				//添加到树结构中
				struct epoll_event  temp;
				temp.events = EPOLLIN;
				temp.data.fd = cfd;
				ret = epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&temp);
				if(ret == -1){
					perror("epoll_ctl error\n");
				}
				//输入端口号和ip地址
				char ip[64];
				printf("new connect IP=%s,PORT=%d",
						inet_ntop(AF_INET,&client.sin_addr.s_addr,ip,sizeof(ip)),
						ntohs(client.sin_port));
			}else{
				//开始进行数据的传送,我们值接受客户端发送的数据
				if(!all[i].events & EPOLLIN){
					continue;
				}
				char buf[5] = {0};
				while((ret = recv(all[i].data.fd,buf,sizeof(buf),0))>0){
					//使用write写入到显示屏上
					write(STDOUT_FILENO,buf,ret);
					//将数据写回到客户端
					send(all[i].data.fd,buf,ret,0);
				}
				if(ret == 0){
					printf("客户端断开连接\n");
					close(all[i].data.fd);
					epoll_ctl(epfd,EPOLL_CTL_DEL,all[i].data.fd,NULL);
					break;
				}else if(ret == -1){
					if(errno == EAGAIN){
						printf("数据的缓冲区已经读完\n");
					}else {
						perror("recv error");
						exit(-1);
					}
				}
#if 0
				if(ret == -1){
					perror("recv error\n");
					exit(-1);
				}else if(ret == 0){
					printf("客户端已经断开了连接\n");
					close(all[i].data.fd);
					//在epoll树结构中删除掉这个节点
					epoll_ctl(epfd,EPOLL_CTL_DEL,all[i].data.fd,NULL);
					break;
				}
				//进行通信
				printf("recv data:%s\n",buf);
				printf("发送数据：%s",buf);
				send(all[i].data.fd,buf,strlen(buf),0);
#endif
			}
		}
	}
	close(lfd);
	return 0;
}
