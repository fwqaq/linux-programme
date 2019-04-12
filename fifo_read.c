#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
int main(){
	char * filename = "./pipe.fifo";
	int ret = mkfifo(filename,664);
	if(ret == -1){
		if(errno != EEXIST){
			perror("mkfifo error\n");
			return -1;
		}
	}
	printf("打开fifo文件成功\n");
	int fd = open(filename,O_RDONLY);
	if(fd == -1){
		perror("read error\n");
		return -1;
	}
	while(1){
		sleep(1);
		char* buf[1024];
		ret = read(fd,buf,sizeof(buf));
		if(ret == 0){
			printf("写入端已经关闭\n");
			exit(0);
		}
		if(ret == -1){
			perror("read error\n");
			return -1;
		}
		printf("buf：[%s\n]",buf);
	}
	close(fd);
	return 0;
}
