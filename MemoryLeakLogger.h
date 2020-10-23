#pragma once

#include <cstddef>

void start_memory_leak_UDP_client(const char* destIP, const int port, const int timeout);

void* operator new (size_t size, const char* filename, int line);
void* operator new[](size_t size, const char* filename, int line);

void operator delete(void * p);
void operator delete[](void * p);

#define new new(__FILE__, __LINE__)
