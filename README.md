
# Operating System Lab

### by Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa

#### Submitted under cs341109

# Table of Contents
  - [Project overview](#project-overview)
    - [Requirements](#requirements)
    - [Machine](#machine)
      - [Memory](#memory)
        - [Paging](#Paging)
        - [MMU](#mmu)
    - [OS](#os)
      - [UI](#ui)
      - [Data](#data)
    - [Difficulties Encountered](#difficulties-encountered)
  - [How to Run](#how-to-run)
  - [Time Spent](#time-spent)
  - [Verification](#verification)
  - [References](#references)
  - [Project Listing](#project-listing)
    - [Execution Listing](#out_txt)
    - [machine.hpp](#listing_machine_hpp)
    - [machine.cpp](#listing_machine_cpp)
    - [os.hpp](#listing_os_hpp)
    - [os.cpp](#listing_os_cpp)
    - [utils.hpp](#listing_utils_hpp)
    - [utils.cpp](#listing_utils_cpp)
    - [makefile](#listing_makefile)

## Project Overview
This project overview has been to explore, understand, and implement basic operating system concepts and components. The portion of this third lab involved the implementation of virtual memory, specifically paging. This required a significant overhaul of some of the components developed for previous labs.

### Requirements
The following is a list of requirements of the changes necessary for the update to the simulated operating system that was specified in the project handout.

 - **OS Design** - Changes will include the introduction of a simple priority scheme for the scheduler.
 - **UI** - The UI should still collect request in RR as usual, but the requests should schedule according to the priorities given below rather than being executed immediately. If the O/S has a command it should be scheduled at priority 1. If a nop is encountered on the O/S UI "turn", the priority 1 queue should be checked for user "elevated" requests. If the priority 1 queue is empty, the priority 2 queue should control the next operations. If a request doesn't finish, then it should be placed in a shadow queue at the same priority and the next request processed. Once the original queues are empty, the shadow queues should be copied to the original queues and UI should be called to collect the next set of requests. **Add a "User" time command to display the current time.**
 - **Scheduler** - Two priority levels, 1 and 2, with the lower number being the higher priority level. Round robin scheduling is done within each priority level. The initialization routine should be modified to include queue data structures for priority levels. O/S commands should be scheduled and handled.
 - **Organization** - Modify the UI routine to collect the requests and place them in the appropriate queues. The UI routine should then call the scheduler to process the requests as indicated by priority. The scheduler should process the request as indicated subject to the time quantum and if completed remove the request from the queue, otherwise place it on the shadow queue. If a request has had zero execution time so far, it should be elevated to the priority 1 queue. The scheduler routine should create manipulate a data structure that includes the user, the priority, and the time spent in execution so far.

### Machine
Here is an updated version of the specifications of our operating system:
 - 16-bit words
 - 256-word memory (16 bits) word addressable
 - ***<sup>New!</sup>*** 512-word disk space (16 bits)
 - 3 general purpose registers (1-3), 1 special: A (0)
 - ***<sup>New!</sup>*** A Page Table Base Register (implemented as a pointer to a PageTable object)
 - 8-bit program counter (PC)
 - condition code flags : equals or zero-(010), positive-(001), negative-(100)
 - Machine instruction cycle - fetch, decode, opfetch, execute, writeback

Furthermore, the following is a list of the instructions that can be executed on our hardware, along with their opcodes and mnemonics. These instructions will serve as the foundation for any programs running on the machine.

| <u>Instruction</u> | <u>Opcode</u> | <u>Mnemonic</u> |
|--------------------|---------------|-----------------|
|Load                |0000           |LOD              |
|Store               |0001           |STO              |
|Add                 |0010           |ADD              |
|Subtract            |0011           |SUB              |
|Add Register        |0100           |ADR              |
|Subtract Register   |0101           |SUR              |
|And                 |0110           |AND              |
|Or                  |0111           |IOR              |
|Not                 |1000           |NOT              |
|Jump                |1001           |JMP              |
|Jump ==             |1010           |JEQ              |
|Jump >              |1011           |JGT              |
|Jump <              |1100           |JLT              |
|Compare             |1101           |CMP              |
|Clear               |1110           |CLR              |
|Halt                |1111           |HLT              |

#### Memory
Memory access has been significantly overhauled in this version of the lab. Main memory is now paged, and hardware support for paging has been implemented in the form of a Memory Management Unit.

##### Paging
256 words of memory have been divided into 64 4-word frames. Because the logical memory addresses referred to in the operand of our 16 bit instructions can only be 8 bits max, this essentially limits our logical addresses to 256 words or less, i.e. exactly 64 pages.

Page tables are maintained by the operating system for each process. Each process control block contains a pointer to its page table. The operating system uses a global frame table to determine which frames are free to be allocated, and then allocates space dynamically for any referenced logical addresses that are outside the currently allocated virtual address space.

For example, if a process is loaded by the operating system into its first n pages (n being the number required to load the executable code; for this example, let's arbitrarily pick 5 pages), then the existing virtual address space for that process is 5 4-word pages, i.e. 20 words of memory. If the program makes a reference to the logical memory address 25 (the 26th memory word -> frame 6 word 1), the operating system would catch the page fault, allocate an entry in the page table for page 6 using an arbitrarily chosen available frame, and then return that frame number. The page table would then contain entries for pages 0, 1, 2, 3, 4, and 6.

##### MMU
The MMU resides in hardware, and uses a special register called the Page Table Base Register (PTBR) to access the appropriate page table. Because each process control block contains a pointer to its page table (i.e. the "address" to go into the PTBR), the PTBR is set to the appropriate page table with each context switch.

The MMU takes a logical address, performs the page to frame translation using the page table object (keep in mind that page faults are caught and handled by the operating system independent of the MMU), then replaces the page bits with the corresponding frame bits to form the physical address. Page faults are completely transparent to the MMU for the time being; because we only have one process per user, and only two users, ready/blocked queues were a tricky design decision, and the lack thereof makes it difficult to actually signal an interrupt and "block" the program while the page fault is handled. The queue design will be discussed in further detail under OS.

In an actual system, the MMU would also contain its own cache, the TLB, which would significantly improve performance. However, as this is an extremely simplified system, and we are not worried with such low-level details, we implemented only the translation aspect of the MMU.

The primary advantage of this implementation is data protection. All of the processor instructions now deal solely with logical addresses which are translated dynamically at runtime through the MMU. With a dedicated piece of hardware controlling all memory accesses, it becomes impossible for any process to access an area of memory which it has not been allocated. A real system may contain some method of accessing physical addresses directly, provided that the currently running process has appropriate privileges, but as we are only handling user-level processes, we decided prohibiting physical addressing entirely would be simpler, and would achieve the same effect.

Typically there would be some form of frame sharing to support file handles and dlls, however, again, we are not concerning ourselves with such complex details. For now, we made the assumption that all processes are self-contained, and they do not share any frame references. The immediate implication of this is that the frame table now essentially acts as a per-frame semaphore. A simple bit array indicating whether a frame has already been allocated prevents said frame from being allocated elsewhere. Simply put, there is no way that a process could access a frame that has not been allocated to it, as that frame would never be returned by the MMU to that process. So in effect, each frame has a "semaphore" that is only unlocked for the process that owns it.

### OS
The biggest re-work of OS was the addition of processes (independent from users). Before this portion of the lab our users and processes were essentially inextricable. With this version of our simulated Operating System we have distinguished the two from each other. This allows much greater flexibility for upcoming projects. A simple addition of an array of processes to the new user structure would allow a Process table so that users could execute more than one process at a time. This update coupled with the new `run` functionality should allow us a good foundation in the next iteration of the OS.

Once again, here is the list of commands that can be entered at the interactive ui console:
 - `run` - run the program in main memory *(user)*
 - `dmp` - dump (output) of main memory *(o/s only)*
 - `nop` - none (no command to execute) *(user or o/s)*
 - `stp` - stop the program *(o/s only)*

The `dmp` function will now dump all machine registers, main memory by frames (one frame per line), as well as each user's page table side-by-side with the global frame table.

>**Note:** The program submitted to the cs.sfasu.edu server was done so with the `DEBUG` flag **defined**.  To disable debug output, simply change line 24 of machine.hpp from `//undef DEBUG` to `#undef DEBUG`, then remake the project with the `make` command.

#### UI
User interface changes have not been incredibly significant since it's implementation in lab 1. The dump command has been vastly expanded to include information on page tables and the frames that are located in main memory. Otherwise, you can fully expect the same UI that was found in the previous iteration of our simulated console OS.

Additionally, the running of user processes has been changed. `run` will now only use the rest of the user ticks; when a running process is preempted, it *no longer* continues its execution automatically with its users next turn. This is to allow the user the choice of either continuing execution by typing `run`, or potentially performing some kind of debug command such as `dmp` (although at this time users are unable to use the dmp command). Because of this design decision, ready and blocked queues would be pointless. Consider the following example:

* User 1 enters `run`. The process is initialized and placed in the ready queue.
* The head of the ready queue is loaded into machine registers, and that process runs until the user's allotted time is up.
* Said process is then preempted; its status is saved from the machine registers back into the PCB, and the process is pushed to the back of the Ready queue.
* User 2 is prompted for input, and enters `run`. User 2's process is initialized and placed in the ready queue (at the back; FIFO).
* The head of the ready queue is loaded into machine registers, and that process runs until the user's allotted time is up. Except that the process now running during user 2's turn is actually user 1's process.

As you can clearly see, this discrepancy is a result of the scheduler allotting time *per user* rather than *per process*. Keeping the user interface in-sync with the ready queue would require one of two solutions:

1. The ready queue is maintained as a queue of users (we've tried this approach in the past, and have been informed that it is incorrect).
2. Each user is to keep its own ready and blocked queues, which are swapped in and out of the OS ready and blocked queues. This would allow each user to run multiple processes, and each of the user's processes would still be able to swap according to it's fair place in the queue, while keeping each user's processes contiguous in the queue, and still allowing the user interface to remain in-sync with the currently running process.

We decided after much deliberation that this was far too convoluted a solution. Although technically we no longer have ready and blocked queues, we felt that our implementation was a decent workaround considering the scheduler is specified to deal with the user interface to determine a user's "turn". Furthermore, we did take the time to make a very clear distinction between a user and a process. The Process struct acts as the process control block, with a process ID, status word, and registers, while the User struct contains a user id and a pointer to a Process. This could be expanded in the future to point to a queue, list, or other container of Processes, but for now we were given the assumption of 1 user : 1 process.

#### Data
Readability should now be increased. We have the input files hardcoded into disk, thus on load the "data" is already located on the disk waiting for our loader to appropriately bring the data into the frames in main_memory. We used to have a read file method to scan for text input on an actual file, but for efficiency's sake we decided that scanning a file was just extra resources we did not really need to expend if we did not have to.

Once again, Process 1 starts at location 0 and process 2 starts at location 100, as indicated in the previous handout, only this time the locations refer to disk rather than main memory.

### Difficulties Encountered
As is becoming increasingly more typical with each project, we had a great deal of difficulty in forcing the CS Server to compile our code, as it does not seem to support any standards implemented in the past decade. Workarounds were implemented, however bulky, in order to get the code compiling and running as expected on the CS Server. We may consider touching up these workarounds later if we have the time for refactoring before the next project.

However the most difficult aspect of this project was in its design. We spent a great deal of time deliberating exactly what changes needed to be made and how they should be implemented before we ever even started programming. Several get-togethers were planned to code up the changes, only to be spent reading through various textbooks and online forums. By far this has been the most difficult project for us to get a handle on, not so much in terms of what needs to change, but what is the most effective way to implement those changes.

## How to Run
This will be a quick overview of how to run the program:
 - Build the project:
 ```
 make
 ```
 - Run the program:
 ```
 ./os
 ```
 - Interact with the OS as detailed in the OS section above. Note that the system must enter the `run` command in order to invoke the loader, which brings the users' executables from disk into main memory.

## Time Spent
As stated previously, the vast majority of our time was spent in research and discussion. Thus, github's commit tracking has very little to show for how productive we actually were. Furthermore, after getting caught up in the design, we neglected to accurately maintain a record of hours spent.

As a rough estimate, it is safe to say that we began our design about two weeks prior to the due date, initially meeting 2-3 times per week, along with intermittent online chats, and as the deadline grew ever closer, we met more frequently and for much longer periods. We did initially underestimate how much time we would be spending in the design phase, which is what ultimately led to our making design decisions as late as two days before the due date.

In the future, we will keep the complexity of the design in mind as we make our estimates, and be more diligent about keeping accurate records of our hours.

## Verification

The implementation of the scheduler in this lab was done through the use of 4 queues, 2 ready queues and 2 shadow queues. The “processes” that are in the lower number queue get priority (while still in round robin fashion). To verify that these queues are working as specified in the project outline we will examine two outputs of our program, **one with a Verbose Debug turned ON and the other with Verbose Debug turned off, WHICH IS HOW IT WILL BE SUBMITTED**. Both outputs will be added to the appendices, but I will be pulling only the relevant parts of output as to which is necessary to prove correctness. This is due to the immense amount of debug information supplied in verbose output. If you need to see the exact output, with or without verbose debug turned on, it is available in the appendices.

When the program is first ran, the UI is added to the ready queue for higher priority in the initialization process. Here is the status of the queues before the scheduler is called:

```
RQ1: ui ->
RQ2:
SQ1:
SQ2:
```

As you can see UI is the only process in the ready queue. We then immeadiately do a dump once the ui is called so that you can see it is no longer in the ready queue but is in fact running:

```
SYS > dmp
#######################################################################
#                   Dumping scheduler queues                          #
#######################################################################
Current process: UI
RQ1:
RQ2:
SQ1:
SQ2:
```

Next we NOP on user 1's turn of the UI so that his process is not added to the queue yet. This is so that we can show the priority elevation is working properly later. Then on User 2's turn of UI we type run to add his process to the queue for the scheduler to handle. This is initially added to RQ2, but since the scheduler does an initial scan to determine if any processes in RQ2 need to promoted it is put into RQ1, thus the next state of the queues is as follows:

```
RQ1: 2 ->
RQ2:
SQ1: ui ->
SQ2:
```

After execution of Process 2 we demote it back to the lower priority as it no longer qualifies for high priority as it has now had processor time. This is shown with in the following state of the queues:

```
RQ1:
RQ2:
SQ1: ui ->
SQ2: 2 ->
```

As you can see there are no more processes in either of the ready queues, thus we move the processes in the shadow queues back into the ready queues, and UI is called again:

```
##########################################################################
#                           Dumping scheduler queues                     #
##########################################################################
RQ1: ui ->
RQ2: 2 ->
SQ1:
SQ2:
##########################################################################
#                               Switching to UI                          #
##########################################################################
SYS > nop
USER1 > run
Adding P1 to RQ2
USER1 > nop
USER2 > nop
```

As you can see above this time USER 1 types run to add his process to the ready queue, once again it is initially added to the RQ2, but the scheduler will do a initial scan and find that it needs to be promoted to higher priority, thus the next queue output will be the following:

```
RQ1: 1 ->
RQ2: 2 ->
SQ1: ui ->
SQ2:
```

The UI is moved to shadow queue after it is finished, and even though process 2 was scheduled first, process 1 will get priority and run before it due to the fact that it has not had and processor time yet. After execution Process 1 has had processor time, and no longer qualifies for priority so it is put into SQ2:

```
RQ1:
RQ2: 2 ->
SQ1: ui ->
SQ2: 1 ->
```

Process 2 finishes it's process with it's next execution and is then removed for the queues:

```
RQ1:
RQ2:
SQ1: ui ->
SQ2: 1 ->
```

Then once again the shadow queues are moved back Into the ready queues and the UI is called. This time I type STP on the System and end execution. The final dump of the queues is as follows:

```
SYS > stp
########################################################################
#                           Dumping scheduler queues                   #
########################################################################
Current process: UI
RQ1:
RQ2: 1 ->
SQ1:
SQ2:
```

As you can see, even though the simulated OS is closing, process one is still in the ready queue as it did not finish execution.


## References

Silberschatz, A., & Galvin, P. B. (1998). Operating system concepts. Reading, MA: Addison Wesley Longman.

Stack Overflow. (2013, July 21). Retrieved March 29, 2016, from http://stackoverflow.com/a/17777130
