/*
 *实现udp服务器端
 */
#include "udpsocket.hpp"
#define CHECK(T) if(!T) {return -1;}
int main(int argc,char* argv[]){
	//将我们的ip地址通过参数进行传递
	if(argc < 3){
		perror("./a.out ip port\n");
		return -1;
	}
	std::string ip = argv[1];//ip地址的信息
	uint16_t port = atoi(argv[2]);//port端口信息
	//创建封装的类
	UdpSocket sock;
	//创建我们的套接字
	CHECK(sock.Socket());
	//绑定
	CHECK(sock.Bind(ip,port));
	while(1){
		//接受数据
		std::string str;
		struct sockaddr_in client_addr;
		CHECK(sock.Recv(str,&client_addr));
		std::cout<<"client said:"<<str<<std::endl;
		std::string answer;
		std::cout<<"server say:";
		fflush(stdout);
		std::cin>>answer;
		CHECK(sock.Send(answer,&client_addr));
	}
	sock.Close();
	return 0;
}
