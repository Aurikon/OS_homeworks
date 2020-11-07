#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <fstream>

int main()
{

	while(true)
	{
		int status;
		std::string name{};
		std::cin >> name;

		//exit programm
		if(name == "q")
		{
			break;    
		}
		
		char* name_c = new char[50];
		std::strcpy(name_c, name.c_str());
		
		std::string line{};
		char c;

		char* argv[50];
		for(int i = 0; i < 50; ++i)
		{
			argv[i] = new char[50];
		}

		//copy from string to char* argv[];
		std::strcpy(argv[0], name.c_str());
		int i = 1;
		while(true)
		{
			std::cin.get(c);
			if(c == '\n')
			{
				break;	
			}
			std::cin.putback(c);
			std::cin>>line;
			std::strcpy(argv[i++], line.c_str());
		}
		argv[i] = NULL;
				
		pid_t pid;
		pid = fork();
		
		if(pid == -1) //error
		{
			perror("fork");
			exit(1);
		} 
		else if(pid) //in parent process
		{
			wait(&status);
		}
		else //in child process
		{

		
			//get child pid for redirect to path
			pid_t child_pid = getpid();
			std::string pid_string = std::to_string(child_pid);
			//Save original cin, cout, cerr
			std::streambuf* cerrbuf = std::cerr.rdbuf();
			std::streambuf* coutbuf = std::cout.rdbuf();
			std::streambuf* cinbuf = std::cin.rdbuf();

			std::string err_path = "/opt/silentshell/" + pid_string + "/err.std";
			std::string out_path = "/opt/silentshell/" + pid_string + "/out.std";
			std::string in_path = "/opt/silentshell/" + pid_string + "/in.std";
			
			std::ofstream err(err_path.c_str());
			std::ofstream out(out_path.c_str());
			std::ifstream in(in_path.c_str());

			std::cerr.rdbuf(err.rdbuf());
			std::cout.rdbuf(out.rdbuf());
			std::cin.rdbuf(in.rdbuf());

			execvp(name_c, argv);
			wait(&status);

			//restore back
			std::cerr.rdbuf(cerrbuf);
			std::cout.rdbuf(coutbuf);
			std::cin.rdbuf(cinbuf);
			
		}
		
		std::cout << "\n";
	}
	
	return 0;
}