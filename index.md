---
layout: index
---

# Operating System Lab

### by Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa

#### Submitted under cs341109

# Table of Contents
  - [Project overview](#project-overview)
    - [Machine](#machine)
    - [OS](#os)
      - [UI](#ui)
      - [Memory](#memory)
      - [Organization](#organization)
      - [Data](#data)
    - [Difficulties Encountered](#difficulties-encountered)
  - [How to Run](#how-to-run)
  - [Time Spent](#time-spent)
  - [Verification](#verification)
  - [Project Listing](#project-listing)
    - [Execution Listing](#out_txt)
    - [machine.hpp](#listing_machine_hpp)
    - [machine.cpp](#listing_machine_cpp)
    - [os.hpp](#listing_os_hpp)
    - [os.cpp](#listing_os_cpp)
    - [part2.dat](#listing_part2_dat)
    - [part2x.dat](#listing_part2x_dat)
    - [makefile](#listing_makefile)

## Project Overview
During this course of this class we are going to be exploring and creating operating system concepts and components. This second part of our project involves thinking about concurrency, mutal exclusion, synchronization, enforcement, deadlock and starvation avoidance. In order to develop and explore these operating system issues, we are to implement a new way of handling access to memory locations.

### Machine
The following is a brief review of the virtual system we have built:
 - 16-bit words
 - 256-word memory (16 bits) word addressable
 - 3 general purpose registers (1-3), 1 special: A (0)
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

### OS
The OS saw some siginifcant overhaul in this portion of the project. We had to rework our scheduling of users to accomidate the addition of a Semaphore or locking feature. Additionally the OS saw the addition of data structures to accomodate the new features.

The OS is still running the simplistic round-robin scheduler. The main difference, however, is in the use of a dispatcher, which determines through `semwait` and `semsignal` functions whether the next process should be allowed to run, or blocked.

Once again here is the list of commands that can be entered at the interactive ui console:
 - `run` - run the program in main memory *(user)*
 - `dmp` - dump (output) of main memory *(o/s only)*
 - `nop` - none (no command to execute) *(user or o/s)*
 - `stp` - stop the program *(o/s only)*

The `dmp` function will now dump all the registers, the queues for the semaphore (the ready queue and the blocked queue), and the status of the semaphore. Additionally we have a debug mode that will print the registers of the current user after every instruction executed, as well as the "assembly" printout of the instruction that was just executed.

>**Note:** The program submitted to the cs.sfasu.edu server was done so with the `DEBUG` flag **defined**.  To disable debug output, simply change line 24 of machine.hpp from `//undef DEBUG` to `#undef DEBUG`, then remake the project with the `make` command.

We will now briefly talk about how components of the OS have changed during this portion of the project:

#### UI
User interface has not changed significantly between these versions of the operating system. The primary difference is that the dispatcher writes to the console to inform users when their processes have been blocked, and when they resume execution. Beyond this, the UI should be effectively the same as that of Lab 1.

#### Memory
In terms of the implementation, memory is exactly the same in Lab 2 as in Lab 1. The primary difference is that there is a global variable, semaphore, which, when locked, redirects all new requests to access memory to the blocked queue, to be recalled to the ready queue once the semaphore is unlocked.

#### Organization
Significant restructuring was necessary for this project. We initially attempted to implement a queue class in C, and while we found many examples online, none of them ended up working for our purposes. Thus, the decision was made to migrate to C++.

Our former approach to implementing the users, having an enum for the users which could be incremented with each switch, would clearly no longer work in this program, as the users can be in one of two states: ready, and blocked. One queue was implemented for each of these states. At startup, all users are loaded into the ready queue. Users are only moved to the blocked queue if they request access to memory while the semaphore is already locked, and are only moved back once the `semsignal()` function is called, when the program using memory is done, and unlocks the semaphore.

Finally, while the initial version was simple enough that the scheduler could essentially perform the job of the dispatcher as well, this program grew such that the resulting scheduler was enormous and unwieldy. The decision was made to split this up into two separate functions, the scheduler and the dispatcher.

#### Data
One of the advantages C++ brought with it was the ability to quickly and easily convert to hex in our output. Thus, the `dmp` function now reports in hexadecimal as opposed to binary, allowing us to fit 50% more content along the width of the screen. We expect this will greatly improve readability, and reduce paper use in our printouts.

In addition, our inputs changed slightly as well. This time around, our input was one file with two programs, separated by a single * on its own line. The first program was to be loaded into user 1's memory (starting at location 0), while the second program was to be loaded into user 2's memory (starting at location 100). The readfile method did have to be rewritten from the ground up to accomodate C++'s use of streams, however this did not take long.


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
