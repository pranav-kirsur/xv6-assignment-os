#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char **argv)
{
    volatile int k, n, id;
    volatile double x = 0, z;

    n = atoi(argv[1]); 

    x = 0;
    id = 0;
    for (k = 0; k < n; k++)
    {
        id = fork();
        if (id < 0)
        {
            printf(1, "Fork failed\n");
        }
        else if (id > 0)
        { 
            //in parent
            printf(1, "Parent %d creating child  %d with priority %d\n", getpid(), id, 60-k);
            set_priority(id, 60-k);

        }
        else
        { 
            //in child
            printf(1, "Child %d running\n", getpid());
            break;
        }
    }
    volatile double a, b;
    a = 3.14;
    b = 36.29;
    for (z = 0; z < 9000000.0; z += 0.1)
    {
        x = x + a * b; 
    }
    exit();
}