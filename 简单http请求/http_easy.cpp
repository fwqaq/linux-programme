#include "tcpsocket.hpp"
#include <iostream>
#include <sstream>
int main(int argc,char* argv[]){
	if(argc != 3){
		std::cout<<"./http_server ip port"<<std::endl;
		return -1;
	}
	std::string ip = argv[1];
	uint16_t port = atoi(argv[2]);
	TcpSocket sock;
	//1.创建套接字
	CHECK_RET(sock.Socket());
	//2.绑定连接
	CHECK_RET(sock.Bind(ip,port));
	//设置监听
	CHECK_RET(sock.Listen(10));
	//开始通信
	while(1){
		//首先创建一个sockaddr_in
		struct sockaddr_in client;
		TcpSocket cli;
		if(sock.Accept(cli,&client) == false){
			continue;
		}
		//接受连接请求
		std::string buf;
		cli.Recv(buf);
		std::cout<<"接受到的数据["<<buf<<"]"<<std::endl;
		//发送一个简单的页面请求
		//使用一个数据流来格式化数据
		std::string body;
		body = "<html><body><h1>hello world</h1></body></html>";
		std::stringstream ss;
		ss << "http/1.1 200 OK\r\n";//响应行
		ss << "Content_Length: "<<body.size()<<"\r\n";
		ss << "Content_Type: text/html"<<"\r\n";
		ss << "Location: http://www.baidu.com\r\n";
		ss<<"\r\n";
		std::string header = ss.str();
		cli.Send(header);
		cli.Send(body);
		cli.Close();
	}
	sock.Close();
	return 0;
}
