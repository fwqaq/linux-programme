#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
int main(){
	char* filename = "pipe.fifo";
	int ret =  mkfifo(filename,664);
	if(ret == -1){
		if(errno != EEXIST){
			perror("mkfifo error\n");
			return -1;
		}
	}
	//创建fifo成功
	int fd = open(filename,O_WRONLY);
	if(fd == -1){
		perror("open error\n");
		return -1;
	}
	printf("打开文件成功\n");
	while(1){
		char buf[1024];
		scanf("%s",buf);
		write(fd,buf,strlen(buf));
	}
	close(fd);
	return 0;
}
