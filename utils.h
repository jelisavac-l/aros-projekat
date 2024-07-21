#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "extern/printC.h"

void file_assembler(const char* filename, const unsigned char* bytes, size_t size);
unsigned char* file_disassembler(const char* path, size_t* size);

#endif