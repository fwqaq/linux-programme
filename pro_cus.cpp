#include <iostream>
#include <queue>
#include <stdlib.h>
#include <unistd.h>
class Blockqueue{
	public:
		//构造函数，初始化我们的锁和队列的大小
		Blockqueue(int cap = 10):_capcity(cap){
			pthread_mutex_init(&_mutex,NULL);
			pthread_cond_init(&_cond_prodoct,NULL);
			pthread_cond_init(&_cond_consumer,NULL);
		}
		~Blockqueue(){//析构函数，销毁我们的锁和条件变量
			pthread_mutex_destroy(&_mutex);
			pthread_cond_destroy(&_cond_prodoct);
			pthread_cond_destroy(&_cond_consumer);
		}
		//提供公共的接口。出栈和入栈
		bool QueuePush(int data){
			//加锁
			QueueLock();
			while(QueueIsfull()){//当队列满了生产者等待等待
				ProductorWait();
			}
			//进行入队列的操作
			_queue.push(data);
			//唤醒我们的消费者
			ConsumerWakeup();
			//解锁
			QueueUnlock();
			return true;
		}
		bool QueuePop(int* data){
			QueueLock();//加锁
			while(QueueIsempty()){//当队列是空的就等待
				ConsumerWait();
			}
			//进行出队列的操作
			*data = _queue.front();
			_queue.pop();
			//唤醒我们的生产者
			ProductorWakeup();
			QueueUnlock();
			return true;
		}
	private:
		//实现上面的小函数
		void QueueLock(){
			pthread_mutex_lock(&_mutex);
		}
		void QueueUnlock(){
			pthread_mutex_unlock(&_mutex);
		}
		void ProductorWait(){
			pthread_cond_wait(&_cond_prodoct,&_mutex);
		}
		void ConsumerWait(){
			pthread_cond_wait(&_cond_consumer,&_mutex);
		}
		void ProductorWakeup(){
			pthread_cond_signal(&_cond_prodoct);
		}
		void ConsumerWakeup(){
			pthread_cond_signal(&_cond_consumer);
		}
		bool QueueIsfull(){
			return (_queue.size() == _capcity);
		}
		bool QueueIsempty(){
			return _queue.empty();
		}
	private:
		//一个队列
		std::queue<int> _queue;
		int _capcity;//容量
		pthread_mutex_t _mutex;//一个锁
		pthread_cond_t _cond_prodoct;
		pthread_cond_t _cond_consumer;

};
void* thr_product(void* arg){
	Blockqueue* p = (Blockqueue*)arg;
	int i = 0;
	while(1){
		p->QueuePush(i++);
		std::cout<<"生产者生产数据:"<<i<<std::endl;
	}
	return NULL;
}
void* thr_consumer(void* arg){
	Blockqueue* p = (Blockqueue*)arg;
	int data;
	while(1){
		p->QueuePop(&data);
		std::cout<<"消费者使用数据:"<<data<<std::endl;
	}
	return NULL;
}
int main(){
	pthread_t ptid[4],ctid[4];
	//创建四个生产者和四个消费者线程
	//创建一个对垒
	Blockqueue q;
	int i = 0;
	int ret;
	for(i = 0;i < 4; i++){
		ret = pthread_create(&ptid[i],NULL,thr_product,(void*)&q);
		if(ret < 0){
			std::cout<<"创建线程错误"<<std::endl;
			return 0;
		}
	}
	for(i = 0;i < 4; i++){
		ret = pthread_create(&ctid[i],NULL,thr_consumer,(void*)&q);
		if(ret < 0){
			std::cout<<"创建线程错误"<<std::endl;
			return 0;
		}
	}
	//线程回收
	for(int i = 0;i < 4;i++){
		pthread_join(ptid[i],NULL);
	}
	for(int i = 0;i < 4;i++){
		pthread_join(ctid[i],NULL);
	}
	
	return 0;
}
