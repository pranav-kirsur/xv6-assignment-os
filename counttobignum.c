#include "types.h"
#include "stat.h"
#include "user.h"


int main()
{
    volatile float a=0;
    for(volatile int i=0;i<100000000;i++)
    {
        a = (a + 1.43 * 1.35);
    }
 
    exit();
}