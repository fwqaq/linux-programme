#include "tcpsocket.hpp"
#include <signal.h>
#include <sys/wait.h>

void sigbc(int num){
	while(waitpid(-1,NULL,WNOHANG)>0);
}

int main(int argc,char* argv[]){
	if(argc != 3){
		perror("tcp_ser ip port\n");
		return -1;
	}
	std::string ip = argv[1];
	uint16_t port = atoi(argv[2]);
	signal(SIGCHLD,sigbc);
	TcpSocket sock;
	CHECK(sock.Socket());
	CHECK(sock.Bind(ip,port));
	CHECK(sock.Listen(10));
	while(1){
		//创建一个通信
		TcpSocket com;
		struct sockaddr_in cli;
		if(sock.Accept(com,&cli) == false){
			continue;
		}
		std::cout<<"新的连接Ip:"<<inet_ntoa(cli.sin_addr);
		std::cout<<"端口port:"<<ntohs(cli.sin_port)<<std::endl;
		int pid = fork();
		if(pid == 0){
			while(1){
				std::string buf;
				com.Recv(buf);
				std::cout<<"client said："<<buf<<std::endl;
				buf.clear();
				std::cout<<"server say：";
				std::cin>>buf;
				fflush(stdout);
				com.Send(buf);
			}
		}
		com.Close();
	}
	sock.Close();
	return 0;
}

