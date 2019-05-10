#include <iostream>
#include <vector>
#include <semaphore.h>
#include <pthread.h>
class Blockqueue{
	public:
	Blockqueue(int cap = 10):_queue(10),_capacity(cap),
	_read_step(0),_write_step(0)
	{
	//对我们三个信号量进行初始化
	//int sem_init(sem_t *sem, int pshared, unsigned int
	//value);
		sem_init(&_sem_data,0,0);
		sem_init(&_sem_idle,0,cap);
		sem_init(&_sem_lock,0,1);
	}
	~Blockqueue(){
		sem_destroy(&_sem_data);
		sem_destroy(&_sem_idle);
		sem_destroy(&_sem_lock);
	}

	//提供公共的接口
	bool QueuePush_back(int data){
		//生产者等待
		ProductWait();
		//加锁
		QueueLock();
		//生产数据
		_queue[_write_step] = data;
		_write_step = (_write_step+1)% _capacity;
		//解锁
		QueueUnlock();
		//唤醒生产者
		ConsumerWakeup();
		return true;
	}
	bool QueuePop(int* data){
		//消费者等地啊
		ConsumerWait();
		//加锁
		QueueLock();
		//消费数据
		*data = _queue[_read_step];
		_read_step = (_read_step+1)%_capacity;
		//解锁
		QueueUnlock();
		//唤醒我们的生产者
		ProductWakeup();
		return true;
	}
	private:
		void QueueLock(){//加锁
			sem_wait(&_sem_lock);
		}
		void QueueUnlock(){
			sem_post(&_sem_lock);
		}
		void ProductWakeup(){
			sem_post(&_sem_idle);
		}
		void ProductWait(){
			sem_wait(&_sem_idle);
		}
		//此时对于消费者来说的话我们是对于数据资源来说的
		void ConsumerWait(){
			sem_wait(&_sem_data);
		}
		void ConsumerWakeup(){
			sem_post(&_sem_data);
		}
	private:
		//一个队列
		std::vector<int> _queue;
		int _capacity;//容量
		int _read_step;//读的位置
		int _write_step;//写的位置
		sem_t _sem_data;//数据资源空间
		sem_t _sem_idle;//空闲资源空间
		sem_t _sem_lock;//实现互斥的信号量

};
void* thr_consumer(void* arg){
	Blockqueue* b = (Blockqueue*)arg;
	int data;
	while(1){
		b->QueuePop(&data);
		std::cout<<"消费者消费数据"<<data<<std::endl;
	}
	return NULL;
}
void* thr_productor(void* arg){
	Blockqueue* b = (Blockqueue*)arg;
	int i = 0;
	while(1){
		std::cout<<"生产者生产了数据"<<i<<std::endl;
		b->QueuePush_back(i++);
	}
	return NULL;
}
int main(){
	pthread_t ctid[4],ptid[4];
	//创建四个生产者和四个消费者
	Blockqueue b;
	int ret = 0;
	int i = 0;
	for(i = 0;i < 4; i++){
		ret = pthread_create(&ctid[i],NULL,thr_consumer,(void*)&b);
		if(ret < 0){
			std::cout<<"线程创建出现问题"<<std::endl;
			return 0;
		}
	}
	for(i = 0;i < 4; i++){
		ret = pthread_create(&ptid[i],NULL,thr_productor,(void*)&b);
		if(ret < 0){
			std::cout<<"线程创建出现问题"<<std::endl;
			return 0;
		}
	}
	for(int i = 0;i < 4;i++){
		pthread_join(ctid[i],NULL);
	}
	for(int i = 0;i < 4;i++){
		pthread_join(ptid[i],NULL);
	}
	return 0;
}
