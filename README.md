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

```c
    //FCFS scheduling
    // Loop over process table looking for process to run.
    acquire(&ptable.lock);

    struct proc *first_process = 0;

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->state != RUNNABLE)
        continue;

      if (!first_process)
      {
        first_process = p;
      }
      else
      {
        if (p->ctime < first_process->ctime)
        {
          first_process = p;
        }
      }
    }
    if (first_process)
    {
      p = first_process;
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);
```

# Priority Based scheduler

It chooses the process with minimum `priority` from the `struct proc`. All processes are given a default priority of 60 when allocated.

```c
//Priority based scheduling
    // Loop over process table looking for process to run.
    acquire(&ptable.lock);

    struct proc *highest_priority_process = 0;

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->state != RUNNABLE)
        continue;

      if (!highest_priority_process)
      {
        highest_priority_process = p;
      }
      else
      {
        if (p->priority < highest_priority_process->priority)
        {
          highest_priority_process = p;
        }
      }
    }
    if (highest_priority_process)
    {
      p = highest_priority_process;
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

```

The syscall `set_priority` allows changing the priority of a process. It simply modifies the process table entry for the given process that the scheduler then uses while scheduling.

```c
//set_priority syscall
int set_priority(int pid, int new_priority)
{
  int old_priority = -1;
  //acquire process table lock
  acquire(&ptable.lock);

  //scan through process table
  struct proc *p;
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->pid == pid)
    {
      //store old priority and change the priority
      old_priority = p->priority;
      p->priority = new_priority;
    }
  }
  //release process table lock
  release(&ptable.lock);

  //output old priority
  return old_priority;
}
```

# Multilevel Feedback Queue scheduler

For each process we maintain `queue` and `current_ticks_in_current_slice` that maintain the queue it is in and the number of ticks it has consumed in the current time slice. 

When processes are created, we add them to queue 0 initially. We move the processes into lower queues as specified when they have consumed the time slice allocated to them in the queue.

```c
  p->queue = 0;
  p->ticks_in_current_slice = 0;
```

When a process voluntarily gives up CPU and calls `sleep` we keep it in the same queue.

```c
  //if voluntarily released cpu then reset time slice
  p->ticks_in_current_slice = 0;
```

There is no explicit queue as such but implicit queueing is maintained through the process table.

When scheduling, we first see the first queue and check if any process is ready to execute.

After that we go to the lower queues and check.

```c
    struct proc *process_to_run = 0;
    // Run processes in order of priority
    for (int priority = 0; priority < 5; priority++)
    {
      for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {
        if (p->state != RUNNABLE)
          continue;
        if (p->queue == priority)
        {
          process_to_run = p;
          goto down;
        }
      }
    }
  down:
    // If process found
    if (process_to_run)
    {
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      p = process_to_run;
      p->num_run++;
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
```

If any process consumes its time slice, we move it to a lower queue.

```c
    //See which processes have expired their time slices
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->state != RUNNABLE)
        continue;
      if (p->ticks_in_current_slice >= max_ticks_in_queue[p->queue])
      {
        //cprintf("Process %d hit %d ticks\n",p->pid,p->ticks_in_current_slice);
        if (p->queue != 4)
        {
          //demote priority
          p->queue++;
          p->ticks_in_current_slice = 0;

          //cprintf("Process %d priority demoted to %d\n",p->pid,p->queue);
        }
      }
    }

```

We also implement aging by maintaining a time of last execution for each process and promoting processes which haven't been executed in a while.

```c
    //implements aging
    //promote processes with longer wait times
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->state != RUNNABLE)
        continue;
      // if waiting too long
      if(ticks - p->last_executed > 1000)
      {
        if(p->queue!=0)
        {
          p->queue--;
          p->ticks_in_current_slice = 0;
        }

      }
    }
```

A process could potentially take advantage of this scheduling policy by giving up CPU just before time slice is over, so that it is not demoted to a lower queue and gets a fresh time slice.

# Compiling the code
Run

```bash
$ make qemu SCHEDULER=<scheduler>
```

where `SCHEDULER` may be FCFS, PBS, DEFAULT(Round Robin), or MLFQ