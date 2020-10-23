#include "example.h"

#include "MemoryLeakLogger.h"


class SomethingOther
{
    int x;
    float y;
public:
    SomethingOther() : x(0), y(0) {}
};

class Something
{
    int i1;
    float i2;
    SomethingOther * i3;
public:
    Something()
    {
        i3 = new SomethingOther();
    }
    // The deconstructor should have taken care of deleting i3
};


void example_part3_function()
{
    // Create something, but something other is still in memory
    auto something = new Something();
    delete something;
    
    // Create one instance of something[20] which creates 20 something other
    Something * somethings = new Something[20];
}
