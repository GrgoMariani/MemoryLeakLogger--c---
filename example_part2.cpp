#include "example.h"

#include "MemoryLeakLogger.h"



void example_part2_function()
{
    // one instance not deleted of size 52*4 = 208 bytes
    int * ints = new int[52];

    // one instance not deleted of size 100*1 = 100 bytes
    char * chars = new char[100];
}
