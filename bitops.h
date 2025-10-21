#ifndef BITOPS_H
#define BITOPS_H

#include <stdint.h>
#include <stdio.h>

// 32-bit helpers (for Phase 1)
unsigned int SetBit32(unsigned int value, int pos);
unsigned int ClearBit32(unsigned int value, int pos);
unsigned int ToggleBit32(unsigned int value, int pos);
int GetBit32(unsigned int value, int pos);
int CountBits32(unsigned int value);
unsigned int ShiftLeft32(unsigned int value, int positions);
unsigned int ShiftRight32(unsigned int value, int positions);
void PrintBinary32(unsigned int value);
void PrintHex32(unsigned int value);

// 64-bit helpers (for bitboards)
uint64_t SetBit64(uint64_t value, int pos);
uint64_t ClearBit64(uint64_t value, int pos);
uint64_t ToggleBit64(uint64_t value, int pos);
int GetBit64(uint64_t value, int pos);
int CountBits64(uint64_t value);
uint64_t ShiftLeft64(uint64_t value, int positions);
uint64_t ShiftRight64(uint64_t value, int positions);
void PrintBinary64(uint64_t value);

#endif // BITOPS_H
