---
layout: index
---

<!--
Quick overview of .md (markdown) syntax:
*text* = italics
**text** = bold
***text*** = bold italics
~~text~~ = strikethrough
#text = h1
##text = h2
###text = h3
... h5, h6
 - text = bulleted list
 1. text = numbered list
 - [ ] = checkbox list
> text = block quote
`text` = inline code snippet
```
text
``` = large code snippet
[text](link) = hyperlink

Markdown supports HTML code and comments as well.
-->

# Operating System Lab

### by Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa

#### CSC 341, Spring 2016

##### Submitted under cs341109

## Table of contents
  - [Project overview](#project-overview)
    - [Machine](#machine)
    - [OS](#os)
    - [Difficulties Encountered](#difficulties-encountered)
  - [Verification](#verification)
  - [Project Listing](#project-listing)
    - [machine.h](#listing_machine_h)
    - [machine.c](#listing_machine_c)
    - [os.h](#listing_os_h)
    - [os.c](#listing_os_c)
    - [part1.dat](#listing_part1_dat)
    - [part1x.dat](#listing_part1x_dat)
    - [makefile](#listing_makefile)

## Project overview
During this course of this class we are going to be exploring and creating operating system concepts and components. In this very first lab we are going to implement a machine language interpreter (or Hardware Simulator) for the architecture defined in our handout. This will be our basis for the *Hardware System*.

### Machine
First we will go over the Architectural  design of the machine as indicated on the handout:
* 16-bit words
* 256-word memory (16 bits) word addressable
* 3 general purpose registers (1-3), 1 special: A (0)
* 8-bit program counter (PC)
* condition code flags : equals or zero-(010), positive-(001), negative-(100)
* Machine instruction cycle - fetch, decode, opfetch, execute, writeback

This was the basis of the design of our machine. The 16 bit words are held in the <b>short int</b> data type, which in c can only hold 16 bits. The 256 word memory is simply an array of size 256, the indexes obviously refer to the address location of instructions.<br></br>To hold the Registers we designed a structure that represented all possible machine registers states (including condition codes). Since we knew in advance that users were going to have different register states that would need to be saved and loaded, this just made sense.</br></br>The machine instructions and operation codes are as follows:

![alt text](https://i.imgur.com/eAVF44a.png)

The above outlines the basic operations the machine had to be able to perform.

### OS

### Difficulties Encountered

## Verification
