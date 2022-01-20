#pragma once

void* safeMalloc(size_t size);
void* safeRealloc(void* ptr, size_t size);
void*  safeAlloc(void* ptr, size_t size);