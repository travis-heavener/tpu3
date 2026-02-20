#ifndef __DEFINES_HPP
#define __DEFINES_HPP

/**
 * This file includes all the major macrodefs for the TPU's source code.
 */

// The bits for each flag in the FLAGS register
#define FLAG_CARRY      0
#define FLAG_PARITY     2
#define FLAG_ZERO       6
#define FLAG_SIGN       7
#define FLAG_OVERFLOW   11

// Addressing modes
#define ADDR_MODE_REL   0
#define ADDR_MODE_ABS   1

/**************************************/
/********* TPU specifications *********/
/**************************************/
#define MAX_MEMORY_ALLOC 1024 * 1024 * 256 // 256 MiB

/**************************************/
/********** Kernel Addresses **********/
/**************************************/
// See docs/TPU.md for more information on the TPU's memory layout

// The address of the first syscall in the syscall table
#define SYSCALL_TABLE_FIRST 0x0000'0100

// The length of the syscall table in memory
#define SYSCALL_TABLE_SIZE  0x400

#endif
