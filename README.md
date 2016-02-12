# Operating System Lab

### by Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa

#### Submitted under cs341109

# Table of Contents
  - [Project overview](#project-overview)
    - [Machine](#machine)
    - [OS](#os)
      - [Scheduling Users](#scheduling-users)
    - [Difficulties Encountered](#difficulties-encountered)
  - [How to Run](#how-to-run)
  - [Time Spent](#time-spent)
  - [Verification](#verification)
  - [Project Listing](#project-listing)
    - [Execution Listing](#out_txt)
    - [machine.h](#listing_machine_h)
    - [machine.c](#listing_machine_c)
    - [os.h](#listing_os_h)
    - [os.c](#listing_os_c)
    - [part1.dat](#listing_part1_dat)
    - [part1x.dat](#listing_part1x_dat)
    - [makefile](#listing_makefile)

## Project Overview
During this course of this class we are going to be exploring and creating operating system concepts and components. In this first lab we are going to implement a machine language interpreter for the given architecture. This will serve as the foundation for later projects, which will highlight more specific concepts in OS design.

### Machine
First we will go over the Architectural  design of the machine as indicated on the handout:
* 16-bit words
* 256-word memory (16 bits) word addressable
* 3 general purpose registers (1-3), 1 special: A (0)
* 8-bit program counter (PC)
* condition code flags : equals or zero-(010), positive-(001), negative-(100)
* Machine instruction cycle - fetch, decode, opfetch, execute, writeback

This was the basis of our machine's design. The 16-bit words are held as short ints, which in c hold 16 bits. The 256 words of memory are simply an array of 256 short ints, with the indexes obviously referring to addresses.

The registers are held in a structure which contains several short int values, one for each of the four main registers, one for the instruction register, one for the program counter, and one for the condition flag register. The last two are set to use only 8 and 3 bits, respectively, in an attempt to more accurately represent the physical hardware we are simulating.

The following is a list of the instructions that can be executed on our hardware, along with their opcodes and mnemonics. These instructions will serve as the foundation for any programs running on the machine.

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
The Operating System design currently is very basic; it is essentially a ver simplistic round-robin scheduler. It also currently has a simple interactive command-line interface. The users type in the commands that they want executed (the typing of a command counts as a single tick against the user). The following commands currently in place are:
 - `run` - run the program in main memory *(user)*
 - `dmp` - dump (output) of main memory *(o/s only)*
 - `nop` - none (no command to execute) *(user or o/s)*
 - `stp` - stop the program *(o/s only)*

When the `DEBUG` flag is defined in machine.h, the registers will be dumped on every tick. For this reason the `dmp` function does not currently dump registers. This will likely change in later iterations of the OS.

>**Note:** The program submitted to the cs.sfasu.edu server was done so with the `DEBUG` flag **undefined**. The output shown in the out.txt listing **will not** match the output given from the server, as debug output has been **disabled** for the final version we've handed in. To enable debug output, simply remove the `#undef DEBUG` statement from the top of machine.h and remake.

>The out.txt file was generated with debugging enabled simply to give an idea of what said debug output looks like, without making anyone go to the trouble of modifying the source code.

#### Scheduling Users
We specified a `struct` which would hold one of each kind of register, then declared one in machine.c which would hold the machine registers (representative of the actual hardware). The OS declares a `registers` value for itself, as well as one for each user. These register sets can be thought of as caches, to save/load user state with each user switch.

We then defined an enum in os.h with `u1`, `u2`, and `sys`, to be representative of all the users on the system. The OS declares a variable of this type to keep track of the current user, and uses the `nextUser()` method to increment the value on switch.

Finally, the OS keeps an integer value for the clock, which is incremented each time an instruction executes (either prompting for input on the CLI or by running another line of the program in memory). On switch, the variable switchTime is set to three values higher than the current clock. The scheduler then allows the next user time on the processor while `clock < switchTime`. If the user specifies `NOP`, the program breaks from the loop and the switch is called early.

### Difficulties Encountered
The biggest difficulty that we ran into during this project was having getting a precise idea of what each op code was supposed to do, and how it would affect registers. Even after discussing it with our instructor and each other we still found topics that were not immediately clear (e.g., should the jump command be able to use immediate addressing?).

Also, we found toward the beginning of writing the machine language interpreter that we were repeating a good bit of code (mostly switch statements) trying to get/set register values. The `getRegCode()` function gave us the specified register, at which point various switch statements were used to work with the specified register.

Then we realized that if we created a pointer, and assigned it the address of the specified register, we could write each opcode function such that it would operate on the pointer; all the code determining which register to use, and at which address it is located, was outsourced to the `getRegister()` function, which takes an integer representing the register number, and returns the address to that register.

Finally, we used the version control website Github to help simplify collaboration on this project. The initial setup took some time, and learning the command-line interface did prove something of a challenge. But that was essentially just the only other difficulty we ran into.

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
 - Interact with the OS as detailed in the OS section above. The OS loads a program named part1.dat into memory at startup.

>**Note:** Currently we are reading the program directly into memory form the "part1.dat" file that is in the directory. Both user 1 and user 2 are defaulted to this program, although each user has their own instance of it in memory. The simple loader we use does take into consideration the size of the program, in words, allowing for programs of variable length, but it does very little error handling beyond that. Programs must be under 128 words, as beyond this the second user's instance will overflow out of memory. This possibility is **not** checked.

## Time Spent
GitHub tracks contributions over time.
##### Project Commits by Day:
![alt text](https://i.imgur.com/ylnJUZh.png)

## Verification

#### Verifying program load:
To start, we'll verify that the program loads into memory correctly. The program in question is listed in part1.dat. The assembly equivalent of this program is:

Operation | Addressing Mode | Register | Operand
-|-|-|-
`LOD`|`1`|`0`|`10`
`STO`|`0`|`0`|`6`
`LOD`|`1`|`1`|`5`
`ADR`|`0`|`1`|`0`
`STO`|`0`|`0`|`7`
`HLT`|`0`|`0`|``

Which can also be represented in decimal form as:
```
2058
4102
2309
16640
4103
61440
```

The loader should load this into memory starting at location 0, then again starting at the location after the first program ends. There should be one empty memory space between, as the loader reads the newline at the end and returns 0 for that space. Thus, our memory should look like this:

Address | Value (Decimal)
-|-
0x0|2058
0x1|4102
0x2|2309
0x3|16640
0x4|4103
0x5|61440
0x6|0
0x7|2058
0x8|4102
0x9|2309
0xA|16640
0xB|4103
0xC|61440
0xD|0
0xE|0
0xF|0

We can see from the first `dmp` in out.txt that this is indeed how are memory has been loaded.

#### Running u1
The program that is loaded essentially does the following:
 - Load the immediate value 10 into the default register (rA)
 - Store the default register into memory address 6
 - Load the immediate value 5 into r1
 - Add the immediate value 0 to r1
 - Store the default register into memory address 7
 - Halt
Basically, the end result of this program is that locations 6 and 7 are overwritten with the value 10, and this is what we should expect to see in our output. The second `dmp` verifies that the program has run as expected.

#### Running u2
Once u1 has finished running, the value in address 7 has changed to 10. This address is still u2's starting location. Thus, u2's program now starts with decimal 10, which would be binary 0000000000001010, which loads the value in memory address 10 into the default register (rA). The program then proceeds as expected, its only side effect being that the value in rA is written to addresses 6 and 7.

The value in address 10 is decimal 16640. Our third `dmp` shows that addresses 6 and 7 were correctly overwritten with this value.

#### Scheduler
We can see throughout the output listing that the scheduler runs as expected. Each user gets three ticks, including entering input as one tick, or if a `nop` is received the user's turn is skipped.

Additionally, if a user is currently executing a program, their turn is skipped. For example, when u1 is running its program, the scheduler executes its statements, then switches to u2. From the CLI (ignoring debug output) it would appear that the scheduler has skipped u1 entirely, though we can see from debug output that this is not the case, and u1 did still get its 3 turns.
