# Operating System Lab

### by Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa

#### Submitted under cs341109

# Table of Contents
  - ~~[Project overview](#project-overview)~~ AGB
    - ~~[Machine](#machine)~~ AGB
      - ~~[Memory](#memory)~~ DJK
         - ~~[Paging](#Paging)~~ DJK
	      - ~~[MMU](#mmu)~~ DJK
    - [OS](#os)
      - ~~[UI](#ui)~~ AGB
      - ~~[Scheduler](#scheduler)~~ DJK
      - ~~[Data](#data)~~ AGB
    - ~~[Difficulties Encountered](#difficulties-encountered)~~ DJK
  - ~~[How to Run](#how-to-run)~~ DJK
  - ~~[Time Spent](#time-spent)~~ DJK
  - [Verification](#verification)
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
This project overview has been to explore, understand, and implement basic operating system concepts and components. The portion of this third lab involved the implementation of paging for logical addressing. This entailed an overhaul of some of the components that were previously developed for our simple console operating system.

### Machine
Here is an updated version of the specifications of our operating system:
 - 16-bit words
 - 256-word memory (16 bits) word addressable
 - 512-word disk space (16 bits)
 - 3 general purpose registers (1-3), 1 special: A (0)
 - A Page Table Buffer Register
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
Memory access has been significantly overhauled in this version of the lab. Main memory is now paged, and hardware support for paging has been implented in the form of a Memory Management Unit.

##### Paging
256 words of memory have been divided into 64 4-word frames. Because the logical memory addresses referred to in the operand of our 16 bit instructions can only be 8 bits max, this essentially limits our logical addresses to 256 words or less, i.e. exactly 64 pages.

Page tables are maintained by the operating system for each process. Each process control block contains a pointer to its page table. The operating system uses a global frame table to determine which frames are free to be allocated, and then allocates space dynamically for any referenced logical addresses that are outside the currently allocated virtual address space.

For example, if a process is loaded by the operating system into its first n pages (n being the number required to load the executable code; for this example, let's arbitrarily pick 5 pages), then the existing virtual address space for that process is 5 4-word pages, i.e. 20 words of memory. If the program makes a reference to the logical memory address 25 (the 26th memory word -> frame 6 word 1), the operating system would catch the page fault, allocate an entry in the page table for page 6 using an arbitrarily chosen available frame, and then return that frame number. The page table would then contain entries for pages 0, 1, 2, 3, 4, and 6.


##### MMU
The MMU resides in hardware, and uses a special register called the Page Table Base Register (PTBR) to access the appropriate page table. Because each process control block contains a pointer to its page table (i.e. the "address" to go into the PTBR), the PTBR is set to the appropriate page table with each context switch.

The MMU takes a logical address, performs the page to frame translation using the page table object (keep in mind that page faults are caught and handled by the operating system independent of the MMU), then replaces the page bits with the corresponding frame bits to form the physical address. Page faults are completely transparent to the MMU for the time being; because we only have one process per user, and only two users, ready/blocked queues were a tricky design decision, and the lack thereof makes it difficult to actually signal an interrupt and "block" the program while the page fault is handled. The queue design will be discussed in further detail under OS.

In an actual system, the MMU would also contain its own cache, the TLB, which would significantly improve performance. However, as this is an extremely simplified system, and we are not worried with such low-level details, we implemented only the translation aspect of the MMU.

The primary advantage of this implementation is data protection. All of the processor instructions now deal solely with logical addresses which are translated dynamically at runtime through the MMU. With a dedicated piece of hardware controlling all memory accesses, it becomes impossible for any process to access an area of memory which it has not been allocated.

Typically there would be some form of frame sharing to support file handles and dlls, however, again, we are not concerning ourselves with such complex details. For now, we made the assumption that all processes are self-contained, and they do not share any frame references. The immediate implication of this is that the frame table now essentially acts as a semaphore. A simple bit array indicating where a frame has already been allocated prevents said frame from being allocated elsewhere. Simply put, there is no way that a process could access a frame that has not been allocated to it, as that process would never be returned by the MMU. So in effect, each frame has a "semaphore" that is only unlocked for the process that owns it.


### OS
The biggest re-work of OS was the addition of processes (independent from users). Before this portion of the lab our users and processes were essentially the exact same. With this version of our simulated Operating System we have distinguished the two from eachother. This allows for more modifiability and expandability for upcoming projects. A simple addition of an array of processes to the new user structure would allow a Process table so that users could execute more than one process at a time. This update coupled with the new `run` functionality should allow us a good foundation in the next iteration of the OS.

Once again here is the list of commands that can be entered at the interactive ui console:
 - `run` - run the program in main memory *(user)*
 - `dmp` - dump (output) of main memory *(o/s only)*
 - `nop` - none (no command to execute) *(user or o/s)*
 - `stp` - stop the program *(o/s only)*

The `dmp` function will now dump all the registers, and now the frames and pages of the memory.

>**Note:** The program submitted to the cs.sfasu.edu server was done so with the `DEBUG` flag **defined**.  To disable debug output, simply change line 24 of machine.hpp from `//undef DEBUG` to `#undef DEBUG`, then remake the project with the `make` command.

We will now briefly talk about how components of the OS have changed during this portion of the project:

#### UI
User interface changes have not been incredibly significant since it's implementation in lab 1. The dump command has been vastly expanded to include information on page tables and the frames that are located in main memory. Otherwise, you can fully expect the same UI that was found in the previous iteration of our simulated console OS.

Additionally, the running user processes has been changed. "Run" will now only utilize the rest of the user ticks, if a process is 6 ticks long, and they use only 4, then they would need to type "Run" a second time to finish off the 2 last ticks once they receive the CPU's time again. This allows the user to execute any additional commands they might want to utilize.

#### Organization
Significant restructuring was necessary for this project. We initially attempted to implement a queue class in C, and while we found many examples online, none of them ended up working for our purposes. Thus, the decision was made to migrate to C++.

Our former approach to implementing the users, having an enum for the users which could be incremented with each switch, would clearly no longer work in this program, as the users can be in one of two states: ready, and blocked. One queue was implemented for each of these states. At startup, all users are loaded into the ready queue. Users are only moved to the blocked queue if they request access to memory while the semaphore is already locked, and are only moved back once the `semsignal()` function is called, when the program using memory is done, and unlocks the semaphore.

Finally, while the initial version was simple enough that the scheduler could essentially perform the job of the dispatcher as well, this program grew such that the resulting scheduler was enormous and unwieldy. The decision was made to split this up into two separate functions, the scheduler and the dispatcher.

#### Data
Readability should now be increased. We have the input files hardcoded into disk, thus on load the "data" is already located on the disk waiting for our loader to appropriately bring the data into the frames in main_memory. We use to had a read file method to scan for text input on file, but for efficiency's sake we decided that scanning a file was just extra resources we did not really need to expend if we did not have to.

Once again, Process 1 starts at location 0 and process 2 starts at location 100, as indicated in the previous handout.


### Difficulties Encountered
As stated above, we ran into difficulties implementing the queue structure in C, and ultimately had to convert our program to C++. This was not by any means a difficult task. However the main concern was the version difference between gcc on our personal computers, and gcc on the cs server. In the past, difficulties have been encountered wherein certain statements were not recognized as legal by the cs server due to version and standards differences. We set the standard to c++98, which should be completely supported on the cs server, and we have not had any problems with this particular project.

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
 - Interact with the OS as detailed in the OS section above. The OS loads a program named part2.dat into memory at startup.

## Time Spent
GitHub tracks contributions over time.
##### Project Commits by Day:
![timesheet.png]({{ site.path }}/CSC_341_OS_Lab/images/timesheet.png)

## Verification
![verification.png]({{ site.path }}/CSC_341_OS_Lab/images/verification.png)
