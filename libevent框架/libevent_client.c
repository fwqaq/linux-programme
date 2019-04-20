#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
//读回调函数
void read_cb(struct bufferevent* bev,void*arg){
	//读取数据
	char buf[1024] = {0};
	bufferevent_read(bev,buf,sizeof(buf));
	printf("recv data: %s\n",buf);
}
//写回调函数
void write_cb(struct bufferevent*bev,void* arg){
	printf("已经发送了数据\n");
}
//事件回调
void event_cb(struct bufferevent* bev,short events,void* arg){
	if(events & BEV_EVENT_EOF){
		printf("connection closed\n");
	}else if(events & BEV_EVENT_ERROR){
		printf("some error\n");
	}else if(events & BEV_EVENT_CONNECTED){
		printf("客户端已经连接\n");
		return;
	}
	//回收资源
	bufferevent_free(bev);
}
void fn_cb(int fd,short what,void* arg){
	//读取的数据发送到我们的服务器端
	char buf[1024] = {0};
	read(fd,buf,sizeof(buf));//读取到数据
	//发送数据。但是需要服务器端的ip和端口，传递参数
	struct bufferevent* bev = (struct bufferevent*)arg;
	bufferevent_write(bev,buf,strlen(buf));
}
int main(){
	//创建事件处理框架
	struct event_base* base = event_base_new();
	//创建套接字
	int fd = socket(AF_INET,SOCK_STREAM,0);
	struct bufferevent* bev = bufferevent_socket_new(base,
			fd,	BEV_OPT_CLOSE_ON_FREE);
	//建立连接
	//客户端连接服务器connection
	struct sockaddr_in serv;
	memset(&serv,0,sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8888);
	inet_pton(AF_INET,"127.0.0.1",&serv.sin_addr.s_addr);
	bufferevent_socket_connect(bev,(struct sockaddr*)&serv,sizeof(serv));
	//给缓冲区设置回调函数
	bufferevent_setcb(bev,read_cb,write_cb,event_cb,NULL);
	//创建一个新事件并且将键盘上的输入发送给服务器端
	struct event*ev =  event_new(base,STDIN_FILENO,EV_READ|EV_PERSIST,fn_cb,bev);
	event_add(ev,NULL);
	//循环监听
	event_base_dispatch(base);
	//释放资源
	event_base_free(base);
	return 0;
}
