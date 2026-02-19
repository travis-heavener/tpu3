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

// TPU specifications
#define MAX_MEMORY_ALLOC 1024 * 1024 * 256 // 256 MiB

#endif
