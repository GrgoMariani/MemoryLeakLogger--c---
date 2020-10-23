#include <thread>
#include <string>

#include "example.h"

#include "MemoryLeakLogger.h"

class HugeClass
{
public:
	int a;
	std::string b;
	float c;
	int d;
	HugeClass() {}
};


int main(int argc, char *argv[])
{
	// start the UDP client on a different thread. It will send the data to port 12000 every 15 seconds
	std::thread thread_1(start_memory_leak_UDP_client, "127.0.0.1", 12000, 15);
	
	// 19 instances are not deleted
	std::string * o;
	for(int i = 0; i<20; i++)
	{
		std::string * n1 = new std::string();
		if (i% 5 ==1) o = n1;
	}
	delete o;

	// 11 instances are not deleted
	HugeClass * n2;
	for(int i=0; i < 12; i++)
		n2 = new HugeClass;
	delete n2;

	// check example_part2.cpp and example_part3.cpp
	example_part2_function();
	example_part3_function();

	// infinite loop
	while(1);

	return 1;
}
