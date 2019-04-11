#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int beginnum = 1000;
typedef struct _Product{
	int num;
	struct _Product* next;
}Product;
Product* Head = NULL;
void* thr_Product(void* arg){
	//向链表中添加数据
	while(1){
		Product* prod = malloc(sizeof(Product));
		prod->num = beginnum++;
		printf("---%s--- self=%lu --- %d\n",__FUNCTION__,pthread_self(),prod->num);
		pthread_mutex_lock(&mutex);//加锁
		//插入链表，头插
		prod->next = Head;
		Head = prod;
		pthread_mutex_unlock(&mutex);//解锁
		//发起通知
		pthread_cond_signal(&cond);
		sleep(rand()%4);
	}
}
void* thr_customer(void* arg){
	Product* prod = NULL;
	while(1){
		pthread_mutex_lock(&mutex);
		if(Head == NULL){
			pthread_cond_wait(&cond,&mutex);//在此等待，之前需要加上锁
		}
		prod = Head;
		Head = Head->next;//去除头节点
		printf("---%s--- self=%lu --- %d\n",__FUNCTION__,pthread_self(),prod->num);
		pthread_mutex_unlock(&mutex);//解锁
		sleep(rand()%4);
		free(prod);
	}
	return NULL;
}
int main(){
	pthread_t tid[2];
	pthread_create(&tid[0],NULL,thr_Product,NULL);
	pthread_create(&tid[1],NULL,thr_customer,NULL);

	pthread_join(tid[0],NULL);
	pthread_join(tid[1],NULL);

	//销毁锁和条件变量
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return 0;
}

