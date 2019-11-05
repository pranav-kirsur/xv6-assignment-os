//program that changes priority of a process

#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char* argv[])
{
    int prev_priority = set_priority(atoi(argv[1]), atoi(argv[2]));
    printf(1,"Previous priority was %d\n",prev_priority);
    exit();
}