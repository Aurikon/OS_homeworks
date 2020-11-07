#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>

#define BUFF_SIZE 1024

// When I wrote this code, only God and I understood this... Now, God only knows

void CopyRange(int sourceFD,int destFD, int range)
{
	char buff[BUFF_SIZE];
	int copied;
	while(range > copied)
	{
		int toCopy = range - copied;
		int readed = read(sourceFD, buff, toCopy > BUFF_SIZE ? BUFF_SIZE : toCopy);
		if(readed == 0)
		{
			break;
		}

		while(readed > 0)
		{
			int written = write(destFD, buff, readed);
			readed -= written;
			copied += written;
		}
	}
}

int CopyWithHoles(int sourceFD,int destFD)
{
	int currOffset = lseek(sourceFD, 0, SEEK_CUR);
	if(currOffset == -1)
	{
		return -1;
	}
	int nextOffset = lseek(sourceFD, currOffset, SEEK_DATA);
	if(nextOffset == -1)
	{
		return -1;
	}
	if(currOffset == nextOffset)
	{
			// Start with data
	}
	else
	{
		// Start with hole
		ftruncate(destFD, nextOffset);
		return 1;
	}

	while(true)
	{
		nextOffset = lseek(sourceFD, currOffset, SEEK_HOLE);
		if(nextOffset == -1)
		{
			CopyRange(sourceFD, destFD, lseek(sourceFD, 0, SEEK_END));
		}
		currOffset = lseek(sourceFD, currOffset, SEEK_SET);
		CopyRange(sourceFD, destFD, nextOffset - currOffset);
		currOffset = nextOffset;

		nextOffset = lseek(sourceFD, currOffset, SEEK_DATA);

		// making hole

		int tmp = ftruncate(destFD, nextOffset);
		nextOffset = lseek(destFD, nextOffset, SEEK_SET);

		currOffset = nextOffset;
	}

	nextOffset = lseek(sourceFD, 0, SEEK_END);
	if(currOffset != nextOffset)
	{
		ftruncate(destFD, nextOffset);
	}

	return 1;
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		std::cout << "Wrong number of arguments \n";
		return 0; 
	}

	int sourceFD = open(argv[1], O_RDONLY);
	if(sourceFD == -1)
	{
		std::cout << "Something goes wrong. Can't open file \n";
		return errno;
	}

	int sourceLogicalSize = lseek(sourceFD, 0, SEEK_END);
	std::cout << argv[1] << " logical size is: " << sourceLogicalSize << "\n";

	int destFD = open(argv[2], O_WRONLY | O_CREAT);
	if(destFD == -1)
	{
		std::cout << "Something goes wrong. Can't open file \n";
		return errno;
	}

	CopyWithHoles(sourceFD, destFD);

	return 0;
}
