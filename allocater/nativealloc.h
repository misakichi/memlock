#pragma once
#include <stdint.h>

void* nativeAlloc(void* lpAddress, size_t dwSize);
bool nativeFree(void* lpAddress, size_t dwSize);

void nativeSetup(void* hProc);