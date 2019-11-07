# Report for Assignment 5
# Enhancing xv6 OS


# waitx syscall

For implementing waitx syscall in the `struct proc`, three values `ctime`, `rtime`, `etime` were maintained.
The value `ctime` was initialised in `allocproc` and the value `rtime` was incremented after every clock tick.
`etime` was set at the end of the process.

The syscall `waitx` simply obtains the required values from the `struct proc`. Rest of the implementation is identical to `wait`.

# getpinfo syscall

Similar to waitx, we maintain the required info in the `struct proc` and obtain the info from process table whenever this syscall is called.


# FCFS scheduler

This simply searches for the process that wants to run that has minimum `ctime` and allocates this process to the CPU.

# Priority Based scheduler

It chooses the process with minimum `priority` from the `struct proc`. All processes are given a default priority of 60 when allocated.

The syscall `set_priority` allows changing the priority of a process. It simply modifies the process table entry for the given process that the scheduler then uses while scheduling.

# Multilevel Feedback Queue scheduler

For each process we maintain `queue` and `current_ticks_in_current_slice` that maintain the queue it is in and the number of ticks it has consumed in the current time slice. 

When processes are created, we add them to queue 0 initially. We move the processes into lower queues as specified when they have consumed the time slice allocated to them in the queue.

When a process voluntarily gives up CPU and calls `sleep` we keep it in the same queue.

There is no explicit queue as such but implicit queueing is maintained through the process table.

When scheduling, we first see the first queue and check if any process is ready to execute.

After that we go to the lower queues and check.

If any process consumes its time slice, we move it to a lower queue.

We also implement aging by maintaining a time of last execution for each process and promoting processes which haven't been executed in a while.

A process could potentially take advantage of this scheduling policy by giving up CPU just before time slice is over, so that it is not demoted to a lower queue and gets a fresh time slice.