#include <algorithm>
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__)
#include <WinSock2.h>
#include <Ws2tcpip.h>
#endif
#if defined(__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <chrono>
#include <thread>
#include <map>
#include <tuple>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <ctime>

#include "MemoryLeakLogger.h"

class MemoryLeakLogger
{
	std::map<std::tuple<std::string, int>, std::tuple<int, size_t>> file_line_to_hits_size;
	std::map<unsigned long long, std::tuple<std::string, int, size_t>> address_to_file_line_size;
public:
	MemoryLeakLogger() : file_line_to_hits_size(), address_to_file_line_size() {}

	void AddOccurence(unsigned long long address, std::string filename, int line, size_t size)
	{
		auto file_line_pair = std::tuple<std::string, int>(filename, line);
		if ( !file_line_to_hits_size.count(file_line_pair))
		{
			auto occurences_size_pair = std::tuple<int, size_t>(1, size);
			file_line_to_hits_size[file_line_pair] = occurences_size_pair;
		}
		else
		{
			auto old_value = file_line_to_hits_size[file_line_pair];
			auto occurences_size_pair = std::tuple<int, size_t>(std::get<0>(old_value)+1, std::get<1>(old_value)+size);
			file_line_to_hits_size[file_line_pair] = occurences_size_pair;
		}
		auto file_line_size_tuple = std::tuple<std::string, int, size_t>(filename, line, size);
		address_to_file_line_size[address] = file_line_size_tuple;
	}

	void RemoveOccurence(unsigned long long address)
	{
		if (address_to_file_line_size.count(address) == 0)
			return;
		auto file_line_size_tuple = address_to_file_line_size[address];
		auto file_line_pair = std::tuple<std::string, int>(std::get<0>(file_line_size_tuple), std::get<1>(file_line_size_tuple));
		auto size = std::get<2>(file_line_size_tuple);
		auto old_value = file_line_to_hits_size[file_line_pair];
		auto hits = std::get<0>(old_value)-1;
		if(!hits)
		{
			file_line_to_hits_size.erase(file_line_pair);
			return;
		}
		auto new_value = std::tuple<int, size_t>(hits, std::get<1>(old_value)-size);
		file_line_to_hits_size[file_line_pair] = new_value;
	}

	std::vector<std::tuple<std::string, int, int, size_t>> GetOrderedOccurences()
	{
		auto sorting_function = [](std::tuple<std::string, int, int, size_t>& i, std::tuple<std::string, int, int, size_t>& j)
		{
			return std::get<2>(i) < std::get<2>(j);
		};
		auto result = std::vector<std::tuple<std::string, int, int, size_t>>();
		for (auto& item : file_line_to_hits_size)
		{
			result.push_back(std::tuple<std::string, int, int, size_t>(std::get<0>(item.first), std::get<1>(item.first), std::get<0>(item.second), std::get<1>(item.second)));
		}
		std::sort(result.begin(), result.end(), sorting_function);
		return result;
	}

	std::string ToString()
	{
		std::time_t curr_time = std::time(0);
		std::string result(std::ctime(&curr_time));
		auto occurences = GetOrderedOccurences();
		for (auto& occurence : occurences)
		{
			result.append(std::get<0>(occurence));
			result.append(":");
			result.append(std::to_string(std::get<1>(occurence)));
			result.append("\t\t ACTIVE OCCURENCES: ");
			result.append(std::to_string(std::get<2>(occurence)));
			result.append("\t\t OCCUPATING: ");
			result.append(std::to_string(std::get<3>(occurence)));
			result.append(" Bytes\n");
		}
		return result;
	}
	
} memoryLeakLogger;


void start_memory_leak_UDP_client(const char* destIP, const int port, const int timeout)
{
	const char* pkt = "----------------------------------------------------------------------\n";
	const char* srcIP = "127.0.0.1";
	sockaddr_in dest;
	sockaddr_in local;

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__)
	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKET s;
#endif
#if defined(__linux__)
	int s;
#endif

	local.sin_family = AF_INET;
	inet_pton(AF_INET, srcIP, &local.sin_addr.s_addr);
	local.sin_port = htons(0);

	dest.sin_family = AF_INET;
	inet_pton(AF_INET, destIP, &dest.sin_addr.s_addr);
	dest.sin_port = htons(port);

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind(s, (sockaddr *)&local, sizeof(local));

	while (true)
	{
		try
		{
			std::string st = memoryLeakLogger.ToString();
			size_t index = 0;
			while (index < st.length())
			{
				const std::string& a = st.substr(index, 4096);
				sendto(s, a.c_str(), a.length(), 0, (sockaddr *)&dest, sizeof(dest));
				index += 4096;
			}
			sendto(s, pkt, strlen(pkt), 0, (sockaddr *)&dest, sizeof(dest));
			std::this_thread::sleep_for(std::chrono::seconds(timeout));
		}
		catch(std::exception& e)
		{
			std::time_t curr_time = std::time(0);
			std::cerr << std::ctime(&curr_time) << " " << __FILE__ << ":" << __LINE__ << " Something happened : "<< e.what() << std::endl;
		}
	}
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__)
	closesocket(s);
	WSACleanup();
#endif
#if defined(__linux__)
	close(s);
#endif
}


#define new new

void* operator new (size_t size, const char* filename, int line)
{
	void* ptr = new char[size];
	std::string fname = std::string(filename);
	memoryLeakLogger.AddOccurence((unsigned long long)ptr, fname, line, size);
	return ptr;
}

void* operator new[] (size_t size, const char* filename, int line) 
{
	void* ptr = new char[size];
	std::string fname = std::string(filename);
	memoryLeakLogger.AddOccurence((unsigned long long)ptr, fname, line, size);
	return ptr;
}


void operator delete(void * p)
{
	memoryLeakLogger.RemoveOccurence((unsigned long long)p);
	free(p);
}

void operator delete[](void * p)
{
	memoryLeakLogger.RemoveOccurence((unsigned long long)p);
	free(p);
}
