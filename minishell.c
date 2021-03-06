#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int main(){
	//1.等待用户输入
	while(1){
		printf("[fw@localhost$ root] ");
		char str[1024] = {0};//设置最大的长度
		if(scanf("%[^\n]%*c",str)!=1){
			getchar();
			continue;
		}
		//选出>或者>>两种符号
		int fd = 1;
		int dup_flag = 0;
		char* dup_file;
		char* ff = str;
		while(*ff!='\0'){
			if(*ff == '>'){
				dup_flag = 1;
				*ff = '\0';
				ff++;
				if(*ff == '>'){
					dup_flag = 2;
					*ff = '\0';
					ff++;
				}
				while(*ff==' '){
					ff++;
				}
				dup_file = ff;
			}
			ff++;
		}
		
		//2.解析输入数据,用数组存储起来，
		char* s[32]={0};
		char* start = str;
		char* end = str;
		int i = 0;
		while(*end !='\0'){
			if(*end!=' '){
				s[i++] = end;
				while(*end!=' '&&*end!='\0'){
					end++;
				}
			}
			*end = '\0';
			end++;
		}
		s[i]= NULL;
		if(strcmp(s[0],"cd") == 0){
			//   int chdir(const char *path);
			chdir(s[1]);
			continue;
		}
		//3.创建子进程，在子进程中进行数据读
		//4.等待子进程退出
		pid_t pid;
		pid = fork();
		if(pid<0){
			continue;
		}
		else if(pid == 0){//创建一个子进程执行逻辑
			//找到文件描述符
			if(dup_flag == 1){
				fd = open(dup_file,O_WRONLY|O_CREAT|O_TRUNC);
			}
			if(dup_flag == 2){
				fd = open(dup_file,O_WRONLY|O_CREAT|O_APPEND);
			}
			dup2(fd,1);
			//    int execvp(const char *file, char *const argv[]);
			if(execvp(s[0],s)<0){
				perror("");
			}
			exit(0);
		}
		wait(NULL);//父进程等待子进程的推出，防止产生僵尸进程
	}
	return 0;
}
