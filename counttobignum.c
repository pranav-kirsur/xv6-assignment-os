#include "types.h"
#include "stat.h"
#include "user.h"


int main()
{
    volatile int a=0;
    for(volatile int i=0;i<100000000;i++)
    {
        a++;
    }
 
    exit();
}