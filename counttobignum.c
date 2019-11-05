#include "types.h"
#include "stat.h"
#include "user.h"


int main()
{
    volatile float a=0;
    volatile float b=1.43;
    volatile float c=1.35;
    for(volatile int i=0;i<100000000;i++)
    {
        a = (a + b * c);
    }
 
    exit();
}