#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define CHECK(T) if((T) == false) {return -1;}
class TcpSocket{
	public:
		bool Close(){
			int ret = close(_sock);
			if(ret < 0){
				perror("close error\n");
				return false;
			}
			_sock = -1;
			return true;
		}
		TcpSocket():_sock(-1){
		};
		~TcpSocket(){
		};
		bool Socket(){
			int ret = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			if(ret < 0){
				perror("socket error\n");
				return false;
			}
			_sock = ret;
			return true;
		}
		bool Bind(std::string& ip,uint16_t port){
			struct sockaddr_in  ser_addr;
			ser_addr.sin_family = AF_INET;
			ser_addr.sin_port = htons(port);
			ser_addr.sin_addr.s_addr = inet_addr(ip.c_str());
			socklen_t len = sizeof(ser_addr);
			int ret = bind(_sock,(struct sockaddr*)&ser_addr,len);
			if(ret < 0){
				perror("bind error\n");
				return false;
			}
			return true;
		}
		bool Listen(int backlog = 10){
			int ret = listen(_sock,backlog);
			if(ret < 0){
				perror("listen error\n");
				return false;
			}
			return true;
		}
		bool Connect(std::string& ip,uint16_t port){
			//给定服务器端的地址
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = inet_addr(ip.c_str());
			socklen_t len = sizeof(addr);
			int ret = connect(_sock,(struct sockaddr*)&addr,len);
			if(ret < 0){
				perror("connect error\n");
				return false;
			}
			return true;
		}
		bool Accept(TcpSocket& csock,struct sockaddr_in *cli = NULL){
			struct sockaddr_in _addr;
			socklen_t len = sizeof(_addr);
			int newfd = accept(_sock,(struct sockaddr*)&_addr,&len);
			if(newfd < 0){
				perror("accept error\n");
				return false;
			}
			//需要将获取的客户端的信息拷贝出去
			memcpy(cli,&_addr,len);
			csock.setSock(newfd);
			return true;

		}
		void setSock(int sock){
			_sock = sock;
		}
		bool Recv(std::string& str){
			char temp[1024] = {0};
			//设置成0默认是阻塞的
			int ret = recv(_sock,temp,1024,0);
			//返回-1出错，返回0客户端关闭，其余返回接受到的数据大小
			if(ret < 0){
				perror("recv error\n");
				return false;
			}
			if(ret == 0){
				std::cout << "客户端已经断开了连接" << std::endl;
				return false;
			}
			str.assign(temp,ret);
			return true;
		}
		bool Send(std::string& str){
			int ret = send(_sock,str.c_str(),str.size(),0);
			if(ret < 0){
				perror("send error\n");
				return false;
			}
			return true;
		}
	private:
		int  _sock;
};

