# MemoryLeakLogger - C++

A completely trivial memory leak logger. Extremely easy to use and compile. Although there are some better options available for use this one requires almost no modification to your code.

## Description

A separate thread checks the current status of not yet deleted pointers and sends them over the UDP socket (as a client). The information is basic and covers the filename and the line of the pointers which are still in the memory, the number of instances and how much space they are occupying.

## Try It

Download the project and type `make` to build it. Run the code with `./build/example` and start a UDP server with `python UDP_Server/udp_server.py`.

## Additional info

This project only overloads the `new` and `delete` operators so it will not catch any C `malloc` allocations.

It is designed to work with `MinGW` and `g++`. `clang` should also work. It can also be compiled and run on linux systems using the same makefile.


## Usage

To use it simply include the `MemoryLeakLogger.h` header into the source file you wish to examine, add a `MemoryLeakLogger.cpp` to your sources and start the UDP client thread in your main function.

So your main file should look something like this
```C++
#include <thread>
#include "MemoryLeakLogger.h"

...

int main()
{
    std::thread thread_1(start_memory_leak_UDP_client, "127.0.0.1", 12000, 15);
    ...
    return 1;
}
```

Any other cpp file you want to monitor should only include the header.

```C++
#include "MemoryLeakLogger.h"
...
```

To catch the output you only need to start the UDP server connection.

## UDP Server

There is a really basic `udp_server.py` code available which will capture all of the packets and print them out to the standard output.

Run it with `python udp_server.py` or create your own.

## Example

Included is an example included which also comes with its own makefile. To compile simply use `make` in the base folder and you should find the executable as `./build/example.exe`.

This example code links `example_part1.cpp`, `example_part2.cpp` and `example_part3.cpp` into one executable.

Don't forget to run the UDP server where you can catch the output as well.