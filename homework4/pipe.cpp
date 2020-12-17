#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <cstdlib>
#include <ctime>

int main()
{
	unsigned int N; // vector size
	unsigned int M; // process count
	int indexPipe[2]; // pipe gives index to child process
	int results[2]; // pipe returns result from child process
	std::cin >> N >> M;

	std::vector<int> randNumbers(N);
	
	std::srand(std::time(NULL));
	for(auto& elem : randNumbers)
	{
		elem = std::rand();
	}

	int sum = 0;


	// calculating index
	int i = 0;
	int step = N / M;
	int j = step + N % M;
	int twoIndexToPipe[2]{i, j};
	
	for (int k = 0; k < M; ++k)
	{
		int pipeStatus = pipe(indexPipe);
		if(pipeStatus < 0)
		{
			std::cout << "Can't create pipe \n";
			exit(-1);
		}
		pipeStatus = pipe(results);
		if(pipeStatus < 0)
		{
			std::cout << "Can't create pipe \n";
			exit(-1);
		}

	
		int process = fork();
		if(process == -1) // process create error
		{
			std::cout << "Can't create process " << getpid() << "\n";
			exit(-1);
		}
		else if(process == 0) //  child process
		{
			// get index, calculating result
			int twoIndex[2];
			
			close(indexPipe[1]);
			close(results[0]);
			
			std::size_t size;
			size = read(indexPipe[0], &twoIndex, 2*sizeof(int));
			if(size == -1)
			{
				std::cout << "Can't read from indexPipe\n";
				exit(-1);
			}
			
			int result = 0;
			for(int index = twoIndex[0]; index < twoIndex[1]; ++index)
			{
				result += randNumbers[index];
			}
			
			std::cout << "Process " << getpid() << " result: " << result << "\n";;
			// return result from child to parent
			
			size = write(results[1], &result, sizeof(int));
			if(size == -1)
			{
				std::cout << "Can't write to results\n";
				exit(-1);
			}
			
			kill(getpid(), SIGKILL);
		}
		else // parent process
		{
			close(indexPipe[0]);
			close(results[1]);

			
			std::size_t size;
			size = write(indexPipe[1], &twoIndexToPipe, 2*sizeof(int));
			if(size == -1)
			{
				std::cout << "Can't write to results\n";
				exit(-1);
			}
			
			int result;
			size = read(results[0], &result, sizeof(int));
			if(size == -1)
			{
				std::cout << "Can't read from results\n";
				exit(-1);
			}
			sum += result;
			
			twoIndexToPipe[0] = twoIndexToPipe[1];
			twoIndexToPipe[1] = twoIndexToPipe[0] + step;
		}
	}
	int status;
	wait(&status);
	
	std::cout << "Sum is: " << sum << "\n";
	return 0;
}