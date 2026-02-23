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

// Instruction helpers

// Extract modifier bits from control byte (byte after instruction)
#define IMOD(controlByte) (0b111 & controlByte)

// Extract sign bit from control byte (byte after instruction)
#define ISIGN(controlByte) ((0b1000 & controlByte) >> 3)

// Extract address mode bit from control byte (byte after instruction)
#define IADDRMODE(controlByte) ((0b10000 & controlByte) >> 4)

/**************************************/
/********* TPU specifications *********/
/**************************************/
#define MAX_MEMORY_ALLOC 0x1000'0000 // 256 MiB

/**************************************/
/********** Kernel Addresses **********/
/**************************************/
// See docs/TPU.md for more information on the TPU's memory layout

// The address of the start of the kernel image
#define IMAGE_START_ADDR    0x0001'0500

// The maximum size of a TPU image
#define MAX_IMAGE_SIZE      0x0002'0000

// The address of the first syscall in the syscall table
#define SYSCALL_TABLE_FIRST 0x0000'0100

// The length of the syscall table in memory
#define SYSCALL_TABLE_SIZE  0x400

// The address of the start of the kernel stack space
#define KERNEL_STACK_LOWER  0x0000'0500

// The size of the kernel's stack
#define KERNEL_STACK_SIZE   0x1'0000

// The start of user space
#define USER_SPACE_START    0x0004'0000

#endif
