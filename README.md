# Operating System Lab

### by Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa

## Project Overview
This purpose of this project has been to explore, understand, and implement basic operating system concepts and components. Concepts explored are (in chronological order):

- Operating System Overview
- Concurrency
- Virtual Memory
- Process Scheduling
- File Management

### Machine

For this project, we implemented a simple virtual machine with the following specifications:

 - 16-bit words
 - 256-word memory (16 bits) word addressable
 - 512-word disk space (16 bits)
 - 3 general purpose registers (1-3), 1 special: A (0)
 - A Page Table Base Register (implemented as a pointer to a PageTable object)
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

##### Paging
256 words of memory have been divided into 64 4-word frames. Because the logical memory addresses referred to in the operand of our 16 bit instructions can only be 8 bits max, this essentially limits our logical addresses to 256 words or less, i.e. exactly 64 pages.

Page tables are maintained by the operating system for each process. Each process control block contains a pointer to its page table. The operating system uses a global frame table to determine which frames are free to be allocated, and then allocates space dynamically for any referenced logical addresses that are outside the currently allocated virtual address space.

For example, if a process is loaded by the operating system into its first n pages (n being the number required to load the executable code; for this example, let's arbitrarily pick 5 pages), then the existing virtual address space for that process is 5 4-word pages, i.e. 20 words of memory. If the program makes a reference to the logical memory address 25 (the 26th memory word -> frame 6 word 1), the operating system would catch the page fault, allocate an entry in the page table for page 6 using an arbitrarily chosen available frame, and then return that frame number. The page table would then contain entries for pages 0, 1, 2, 3, 4, and 6.

##### Memory Management Unit
The MMU resides in hardware, and uses a special register called the Page Table Base Register (PTBR) to access the appropriate page table. Because each process control block contains a pointer to its page table (i.e. the "address" to go into the PTBR), the PTBR is set to the appropriate page table with each context switch.

The MMU takes a logical address, performs the page to frame translation using the page table object (keep in mind that page faults are caught and handled by the operating system independent of the MMU), then replaces the page bits with the corresponding frame bits to form the physical address. Page faults are completely transparent to the MMU for the time being; the OS is not implemented in a low-level form that runs on the "hardware," and therfore it is unnecessary to "swap out" the faulted process, "swap in" the interrupt handler, etc. Thus, the operating system handles page faults "on the fly" without any form of interrupt being generated. The MMU takes no part int his process; as far as it is concerned, the page table entry existed all along.

In an actual system, the MMU would also contain its own cache, the TLB, which would significantly improve performance. However, as this is an extremely simplified system, and we are not worried with such low-level details, we implemented only the translation aspect of the MMU.

The primary advantage of this implementation is data protection. All of the processor instructions deal solely with logical addresses which are translated dynamically at runtime through the MMU. With a dedicated piece of hardware controlling all memory accesses, it becomes impossible for any process to access an area of memory which it has not been allocated. A real system may contain some method of accessing physical addresses directly, provided that the currently running process has appropriate privileges, but as we are only handling user-level processes, we decided prohibiting physical addressing entirely would be simpler, and would achieve the same effect.

Typically there would be some form of frame sharing to support file handles and dlls, however, again, we are not concerning ourselves with such complex details. For now, we made the assumption that all processes are self-contained, and they do not share any frame references. The immediate implication of this is that the frame table now essentially acts as a per-frame semaphore. A simple bit array indicating whether a frame has already been allocated prevents said frame from being allocated elsewhere. Simply put, there is no way that a process could access a frame that has not been allocated to it, as that frame would never be returned by the MMU to that process. So in effect, each frame has a "semaphore" that is only unlocked for the process that owns it.

### OS

#### UI

The operating system consists of a simple command-line user interface, a system user, and two less-privileged users.

The following is a list of commands that can be entered at the interactive ui console:
 - `run` - run the specified program (takes a filename) *(user)*
 - `dmp` - dump (output) of main memory, page tables, disk, etc. *(o/s only)*
 - `nop` - none (no command to execute) *(user or o/s)*
 - `stp` - stop the system *(o/s only)*
 - `time` - print the current amount of execution time given to a process (for `sys`, this is the system uptime)
 - `stats` - print statistics about running processes *(o/s only)*

#### Scheduler

The scheduler is round-robin, with two priorities: 1 (system) and 2 (user). User processes that have had no execution time thus far are elevated to priority 1 until they have been given time to run. The scheduler collects requests in a set of queues from the UI, and when the UI has finished collecting requests, the requests are run in FIFO order grouped by priority. Each process is allowed a time quantum of 3 "ticks" of the processor.

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
 - Interact with the OS as detailed in the UI section above. Note that the system must enter the `run` command in order to invoke the loader, which brings the users' executables from disk into main memory.
