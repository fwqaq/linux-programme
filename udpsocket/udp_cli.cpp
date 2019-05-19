/*
 udp客户端程序实现
 */
#include "udpsocket.hpp"

#define CHECK(T) if(!T) {return -1;}
int main(int argc,char* argv[]){
	if(argc < 3){
		perror("./a.out ip port\n");
		return -1;
	}
	std::string ip = argv[1];
	uint16_t port = atoi(argv[2]);

	UdpSocket sock;
	//创建套接字
	CHECK(sock.Socket());
	//客户端不用创建连接，绑定的是绑定的服务器端的地址信息
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	ser_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	
	while(1){
		//发送数据
		std::string str;
		std::cout<<"client say:";
		fflush(stdout);
		std::cin>>str;
		CHECK(sock.Send(str,&ser_addr));
		std::string answer;
		CHECK(sock.Recv(answer,&ser_addr));
		std::cout<<"server said:"<<answer<<std::endl;
	}
	sock.Close();
	return 0;
}
