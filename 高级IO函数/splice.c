#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>

int main(int argc,char* argv[]){

	if(argc <= 2){
		printf("参数不全，第一个时ip,第二个时端口\n");
		return -1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in address;
	//绑定我们的ip和端口
	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET,ip,&address.sin_addr);
	
	int sock = socket(AF_INET,SOCK_STREAM,0);
	assert(sock >= 0);
	int ret = bind(sock,(struct sockaddr*)&address,sizeof(address));
	assert(ret != -1);
	ret = listen(sock,5);
	assert(ret != -1);
	
	struct sockaddr_in client;
	int client_len = sizeof(client);
	int connfd = accept(sock,(struct sockaddr*)&client,&client_len);
	if(connfd < 0){
		printf("error num:%d",errno);
	}
	else{
		int pipefd[2];
		ret = pipe(pipefd);
		assert(ret != -1);
		ret = splice(connfd,NULL,pipefd[1],NULL,10240);
		assert(ret != -1);
		ret = splice(pipefd[0],NULL,connfd,NULL,10240);
		assert(ret != -1);
		close(connfd);
	}
	close(sock);
	return 0;
}
