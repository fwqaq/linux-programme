#include <iostream>
#include <queue>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
using std::cout;
using std::endl;
//任务类
typedef bool (*task_callback)(int data);
bool deal_data(int data){
	srand(time(NULL));
	unsigned int n = rand()%5+1;
	cout<<"线程id"<<pthread_self()<<
		"处理数据"<<data<<"处理数据了,并且睡了"<<n<<"s"<<endl;
	sleep(n);
	return true;
}
class Task{
public:
	Task(){
	}
	~Task(){
	}
	void Settask(int data,task_callback handle){
		_data = data;
		_handle = handle;
	}
	bool run(){
		return _handle(_data);
	}

	private:
		int _data;//需要处理的数据
		task_callback _handle;
};
//线程池类
class PthreadPool{
public:
	bool ThreadInit(){
		//对我们的数据进行初始化操作
		_max_size = 10;
		_cur_size = 10;
		_quit_flag = false;
		_capacity = 10;
		pthread_mutex_init(&_mutex,NULL);
		pthread_cond_init(&_pro_con,NULL);
		pthread_cond_init(&_con_con,NULL);
		//生产我们的线程
		int i ;
		pthread_t tid;
		int ret = 0;
		for(i = 0;i < _max_size;i++){
			ret = pthread_create(&tid,NULL,thr_start,(void*)this);
			if(ret != 0){
				return false;
			}
			pthread_detach(tid);//加入到我们的线程分离，就不用管释放了
		}
		return true;
	}
	bool PushTask(Task& task){
		if(_quit_flag == true){
			return false;
		}
		//加锁
		Queue_Lock();
		//添加任务，在条件变量不为0的情况下
		while(QueueIsfull()){//队列满了就等待
			ProWait();
		}
		_queue.push(task);
		ConWakeUp();//唤醒我们的消费者
		Queue_Unlock();//解锁的操作
		return true;
	}
	bool ThreadQuit(){
		if(_quit_flag != true){
			_quit_flag = true;
		}
		//此时进行唤醒所有的线程进行删除
		while(_cur_size > 0){
			ConWakeAll();
			sleep(1);
		}
		return true;
	}
	private:
	static void* thr_start(void* arg){
			PthreadPool* pool = (PthreadPool*)arg;
			while(1){
				//判断队列是不是空，时空就ConWait();
				pool->Queue_Lock();
				while(pool->QueueIsEmpty()){
					pool->Con_wait();
				}
				Task task;//添加任务到
				pool->PopTask(&task);
				pool->ProWakeUp();
				pool->Queue_Unlock();//解锁
				task.run();
			}
			return NULL;
		}
		bool QueueIsEmpty(){
			return _queue.empty();
		}
		bool PopTask(Task* task){
			*task = _queue.front();
			_queue.pop();
			return true;
		}
		void ProWakeUp(){//生产者进行唤醒
			pthread_cond_signal(&_pro_con);
		}
		bool QueueIsfull(){//判断队列是不是满了
			return _capacity == _queue.size();
		}
		bool Queue_Lock(){//进行加锁的操作
			return pthread_mutex_lock(&_mutex);
		}
		bool Queue_Unlock(){//进行解锁的操作
			return pthread_mutex_unlock(&_mutex);
		}
		void ProWait(){//生产者进行等待
			pthread_cond_wait(&_pro_con,&_mutex);
		}
		void Con_wait(){//消费者进行等待
			if(_quit_flag == true){
				pthread_mutex_unlock(&_mutex);
				cout<<"thread:"<<pthread_self()<<"已经退出了"<<endl;
				_cur_size--;
				pthread_exit(NULL);
			}
			pthread_cond_wait(&_con_con,&_mutex);
		}
		void ConWakeUp(){//唤醒我们的消费者
			pthread_cond_signal(&_con_con);
		}
		void ConWakeAll(){//唤醒我们所有的
			cout<<"wake up all" <<endl;
			pthread_cond_broadcast(&_con_con);
		}
	private:
		int _max_size;//线程池中的最大的数量
		int _cur_size;//线程当前的
		bool _quit_flag;//线程退出的标志
		int _capacity;//队列中的最大节点数目
		std::queue<Task> _queue;//任务队列
		pthread_mutex_t _mutex;//锁
		pthread_cond_t _pro_con;//生产者的条件变量
		pthread_cond_t _con_con;//消费者的条件变量
};

int main(){
	PthreadPool pool;
	pool.ThreadInit();//初始化
	//添加我们的任务
	Task task[10];//是个任务
	int i = 0;
	for(i = 0;i<10;i++){
		task[i].Settask(i,deal_data);
		pool.PushTask(task[i]);
	}
	pool.ThreadQuit();
	return 0;
}
