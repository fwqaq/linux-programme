#include "tcpsocket.hpp"
#include <pthread.h>
void* start_thr(void* arg){
	TcpSocket* sock = (TcpSocket*)arg;
	while(1){
		std::string str;
		sock->Recv(str);
		std::cout<<"client said:"<<str<<std::endl;
		str.clear();
		
		std::cout<<"server say:";
		fflush(stdout);
		std::cin>>str;
		sock->Send(str);
	}
	delete sock;
	sock->Close();
	return NULL;
}
int main(int argc,char* argv[]){
	if(argc != 3){
		perror("./pro ip port\n");
		return -1;
	}
	std::string ip = argv[1];
	uint16_t port = atoi(argv[2]);

	TcpSocket sock;
	CHECK(sock.Socket());
	CHECK(sock.Bind(ip,port));
	CHECK(sock.Listen(10));
	while(1){
		struct sockaddr_in cli;
		//这里在堆上开辟空间，是线程共用
		TcpSocket* csock = new TcpSocket();
		if(sock.Accept(*csock,&cli) == false){
			continue;
		}
		std::cout<<"新的客户端IP："<<inet_ntoa(cli.sin_addr);
		std::cout<<"  端口："<<ntohs(cli.sin_port)<<std::endl;
		pthread_t pid;
		pthread_create(&pid,NULL,start_thr,(void*)csock);
		pthread_detach(pid);
	}
	sock.Close();
	return 0;
}
