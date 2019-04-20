#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
//读回调函数
void read_cb(struct bufferevent*bev,void* ctx){
	char buf[1024] = {0};
	bufferevent_read(bev,buf,sizeof(buf));
	printf("读取到的数据:%s\n",buf);
	//并且将返回一个数据
	char* ptr = "我收到了数据\n";
	bufferevent_write(bev,ptr,strlen(ptr));
	printf("已经发送了数据了\n");
}
//写回调函数
void write_cb(struct bufferevent*bev,void* ctx){
	//只有有写的动作的时候才会触发写事件，在此函数中写bufferevent_write的时候不能触发写事件
	printf("写事件触发了，并且已经发送了数据\n");
}
//事件回调函数
void event_cb(struct bufferevent*bev,short events,void* ctx){
	if(events & BEV_EVENT_EOF){
		printf("connection close\n");
	}else if(events & BEV_EVENT_ERROR){
		printf("some other error\n");
	}
	//回收bufferevent资源
	bufferevent_free(bev);
}
//连接完成之后通信操作回调函数
void listen_cb(struct evconnlistener* listen,evutil_socket_t fd,
					struct sockaddr* addr,int len,void* ptr){
	//注册我们的带缓冲去event事件
	struct event_base *base = (struct event_base*)ptr;
	struct bufferevent* ev = bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
	//给读写去设置我们的读写回调
	bufferevent_setcb(ev,read_cb,write_cb,event_cb,NULL);
	bufferevent_enable(ev,EV_READ);
}

int main(){
	//创建事件
	struct event_base*base = event_base_new();
	//初始化服务器中的ip和端口
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8888);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	//创建套接字
	//绑定
	//监听
	//等待并接受连接请求
	struct evconnlistener* listen = evconnlistener_new_bind(
		base,listen_cb,base,LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
				-1,(struct sockaddr*)&serv,sizeof(serv)
			);
	//开始事件循环
	event_base_dispatch(base);
	//回收资源
	evconnlistener_free(listen);
	event_base_free(base);//回收事件资源
	return 0;
}
