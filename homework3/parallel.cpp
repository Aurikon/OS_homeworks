#include <iostream>
#include <pthread.h>
#include <vector>
#include <queue>

std::queue<std::pair<void(*)(void*), void*>> functions;  // pair.first is function, pair.second is arguments
	
pthread_mutex_t mutex;
pthread_cond_t cond;

void *routine(void*)
{
	while(true)
	{
		pthread_mutex_lock(&mutex);
		while(functions.empty())
		{
			pthread_cond_wait(&cond, &mutex);
		}
		
		void(*start)(void*) = functions.front().first;
		void* arg = functions.front().second;
		functions.pop();
		
		pthread_mutex_unlock(&mutex);
	
		start(arg);
	}
}


class ParallelScheduler
{
public:

	ParallelScheduler(int N):
		N(N)
	{
		threads.resize(N);
		for(int i = 0; i < N; ++i)
		{
			int status = pthread_create(&threads[i], NULL, routine, NULL);
			if(status != 0)
			{
				std::cerr << "Error thread creating \n Status: " << status;
			}
		}
	}

	~ParallelScheduler()
	{
		for(auto& thread : threads)
		{
			pthread_cancel(thread);
		}
	}
	
	void run(void (*start_routine)(void*), void* args)
	{
		pthread_mutex_lock(&mutex); // block queue for synchronization
		
		functions.push(std::pair<void(*)(void*), void*>(start_routine, args));

		pthread_mutex_unlock(&mutex); 

		pthread_cond_signal(&cond); // signal that queue is ready	
	}

	
	friend void *routine(void*);
private:
	int N; //number of threads
	std::vector<pthread_t> threads;
};


int main()
{
	return 0;
}