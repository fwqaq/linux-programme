#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
/*
 *两个管道文件描述符之间的复制数据
*/
int main(int argc,char* argv[]){
 	
	if(argc != 2){
		printf("参数的数量不对的,\n");
		return 1;
	}
	int filefd = open(argv[1],O_CREAT | O_WRONLY | O_TRUNC, 0666);
	assert(filefd > 0);

	int pipefd_stdout[2];
	int ret = pipe(pipefd_stdout);
	assert(ret != -1);

	int pipefd_file[2];
	ret = pipe(pipefd_file);
	assert(ret != -1);

	/*将标准输入内容输入管道pipe_stdout*/
	ret = splice(STDIN_FILENO,NULL,pipefd_stdout[1],NULL,10240);
	assert(ret != -1);

	/*将管道pipefd_stdout输出复制到管道pipefd_file的输入端*/
	ret = tee(pipefd_stdout[0],pipefd_file[1],10240);
	assert(ret != -1);

	/*将管道pipefd_file的输出定向到文件描述符filefd上，从而将标准输入的内容写入文件*/
	ret = splice(pipefd_file[0],NULL,filefd,NULL,10240);
	assert(ret != -1);

	/*将管道pipefd_stdout的输出定向到标准输出，其内容和写入文件的内容完全一致*/
	ret = splice(pipefd_stdout[0],NULL,STDOUT_FILENO,NULL,10240);
	assert(ret != -1);

	close(filefd);
	close(pipefd_stdout[0]);
	close(pipefd_stdout[1]);
	close(pipefd_file[0]);
	close(pipefd_file[1]);
	return 0;
}
