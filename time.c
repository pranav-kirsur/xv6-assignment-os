//timer program that uses waitx syscall

#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char* argv[])
{
    //fork
    int pid = fork();
    if(pid == 0)
    {
        //printf(1,"Executing %s\n",argv[1]);
        //execute the given program
        exec(argv[1],argv+1);
    }
    else
    {
        //in parent
        int waittime,runtime;
        int status;
        status = waitx(&waittime,&runtime);
        //print to stdout
        printf(1,"Status is %d, Wait time is %d, Running time is %d",status,waittime,runtime);
    }
    exit();
    
}