#include "bitops.h"
#include <limits.h>

unsigned int SetBit32(unsigned int value, int pos) {
    if (pos < 0 || pos >= 32) return value;
    return value | (1u << pos);
}

unsigned int ClearBit32(unsigned int value, int pos) {
    if (pos < 0 || pos >= 32) return value;
    return value & ~(1u << pos);
}

unsigned int ToggleBit32(unsigned int value, int pos) {
    if (pos < 0 || pos >= 32) return value;
    return value ^ (1u << pos);
}

int GetBit32(unsigned int value, int pos) {
    if (pos < 0 || pos >= 32) return 0;
    return (value >> pos) & 1u;
}

int CountBits32(unsigned int value) {
    int c = 0;
    while (value) { c += value & 1u; value >>= 1; }
    return c;
}

unsigned int ShiftLeft32(unsigned int value, int positions) {
    if (positions < 0 || positions >= 32) return value;
    return value << positions;
}

unsigned int ShiftRight32(unsigned int value, int positions) {
    if (positions < 0 || positions >= 32) return value;
    return value >> positions;
}

void PrintBinary32(unsigned int value) {
    for (int i = 31; i >= 0; --i) {
        putchar((value >> i) & 1u ? '1' : '0');
        if (i % 8 == 0 && i != 0) putchar(' ');
    }
    putchar('\n');
}

void PrintHex32(unsigned int value) {
    printf("0x%08X\n", value);
}

/* 64-bit helpers */

uint64_t SetBit64(uint64_t value, int pos) {
    if (pos < 0 || pos >= 64) return value;
    return value | (1ULL << pos);
}

uint64_t ClearBit64(uint64_t value, int pos) {
    if (pos < 0 || pos >= 64) return value;
    return value & ~(1ULL << pos);
}

uint64_t ToggleBit64(uint64_t value, int pos) {
    if (pos < 0 || pos >= 64) return value;
    return value ^ (1ULL << pos);
}

int GetBit64(uint64_t value, int pos) {
    if (pos < 0 || pos >= 64) return 0;
    return (int)((value >> pos) & 1ULL);
}

int CountBits64(uint64_t value) {
    int c = 0;
    while (value) { c += value & 1ULL; value >>= 1ULL; }
    return c;
}

uint64_t ShiftLeft64(uint64_t value, int positions) {
    if (positions < 0 || positions >= 64) return value;
    return value << positions;
}

uint64_t ShiftRight64(uint64_t value, int positions) {
    if (positions < 0 || positions >= 64) return value;
    return value >> positions;
}

void PrintBinary64(uint64_t value) {
    for (int i = 63; i >= 0; --i) {
        putchar((value >> i) & 1ULL ? '1' : '0');
        if (i % 8 == 0 && i != 0) putchar(' ');
    }
    putchar('\n');
}
