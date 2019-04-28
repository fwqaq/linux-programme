#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
int main(int argc,char* argv[]){

	if(argc <= 3){
		printf("参数没有输入完整，第一个为iP，第二个为端口号，第三个文件名字");
		return -1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);
	const char* file_name = argv[3];
	int filefd = open(file_name,O_RDONLY|O_CREAT);
	assert(filefd>0);
	struct stat stat_buff;
	fstat(filefd,&stat_buff);

	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&address.sin_addr);
	address.sin_port = htons(port);
	
	int sock = socket(PF_INET,SOCK_STREAM,0);
	assert(sock >= 0);
	
	int ret = bind(sock,(struct sockaddr*)&address,sizeof(address));
	assert(ret != -1);
	ret = listen(sock,6);
	assert(ret != -1);
	struct sockaddr_in client;
	int client_len = sizeof(client);
	int connfd = accept(sock,(struct sockaddr*)&client,&client_len);
	if(connfd < 0){
		printf("errno is:%d",errno);
	}else{
		sendfile(connfd,filefd,NULL,stat_buff.st_size);
		close(connfd);
	}
	close(sock);
	return 0;
}
