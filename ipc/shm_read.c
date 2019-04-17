#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define KEY 0x11223344
int main(){
	//1.创建共享内存
	int shm_id = shmget(KEY,1024,IPC_CREAT|664);
	if(shm_id == -1){
		perror("shmget error\n");
		exit(1);
	}
	//2.将共享内存映射到虚拟地址空间
	char* shm_addr  = shmat(shm_id,NULL,IPC_CREAT);//0表示可以读写
	if((int)shm_addr == -1){
		perror("shamat error\n");
		exit(0);
	}
	//3.在我们的共享内存中写入数据
	while(1){
		static int num = 0;
		printf("%s",shm_addr);
		sleep(1);
	}
	//4.解除我们的映射
	int ret = shmdt(shm_addr);
	if(ret == -1){
		perror("shmdt error\n");
		exit(0);
	}
	//4.删除我们的首地址
	ret = shmctl(shm_id,IPC_RMID,NULL);//NULL是表示我们不了解之前的
	return 0;
}
