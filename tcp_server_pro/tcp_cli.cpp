#include "tcpsocket.hpp"

int main(int argc,char* argv[]){
	if(argc != 3){
		perror("./tcp_cli ip port\n");
		return -1;
	}
	std::string ip = argv[1];
	uint16_t port = atoi(argv[2]);

	TcpSocket sock;
	CHECK(sock.Socket());
	CHECK(sock.Connect(ip,port));
	while(1){
		std::string buf;
		std::cout<<"client say：";
		fflush(stdout);
		std::cin>>buf;
		sock.Send(buf);

		buf.clear();
		sock.Recv(buf);
		std::cout<<"server said："<<buf<<std::endl;
	}
	sock.Close();
	return 0;
}
