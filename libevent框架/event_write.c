#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <event2/event.h>
void cb_write(evutil_socket_t fd,short what,void* arg){
	char buf[1024];
	static int num = 1;
	sprintf(buf,"今天真的热的我发慌~~~~%d",num++);
	write(fd,buf,strlen(buf));
	printf("data send:%s\n",buf);
	printf("read event:%s\n",what & EV_WRITE ? "yes":"no");
	printf("\n");
	sleep(1);
}
//读管道
int main(){
	int fd = open("myfifo",O_WRONLY|O_NONBLOCK);
	if(fd == -1){
		perror("open error\n");
	}
	//1.创建一个事件框架
	struct event_base* base = NULL;
	base = event_base_new();
	if(base == NULL){
		perror("event_new error\n");
	}
	//2.创建事件
	struct event* ev = event_new(base,fd,EV_WRITE|EV_PERSIST,cb_write,NULL);
	//3.添加事件
	event_add(ev,NULL);
	//4.事件循环
	event_base_dispatch(base);
	//5.释放资源
	event_base_free(base);//删除框架
	event_free(ev);//删除事件
	close(fd);
	return 0;
}
