#include "_components.h"
#include "iohandle.h"

void* safeMalloc(size_t size) {
	void* ret = malloc(size);
	if (ret == NULL) {
		IOHandle_Quit();
		printf("malloc(%ld) returned NULL\n", size);
		exit(1);
	}
	return ret;
}

void* safeRealloc(void* ptr, size_t size) {
	void* ret = realloc(ptr, size);
	if (ret == NULL) {
		IOHandle_Quit();
		printf("realloc(%p, %ld) returned NULL\n", ptr, size);
		exit(1);
	}
	return ret;
}

void* safeAlloc(void* ptr, size_t size) {
	if (ptr == NULL) {
		ptr = malloc(size);
		if (ptr == NULL) {
			IOHandle_Quit();
			printf("safeAlloc(%p, %ld) returned NULL\n", ptr, size);
			exit(1);
		}
	}
	else {
		ptr = realloc(ptr, size);
		if (ptr == NULL) {
			IOHandle_Quit();
			printf("safeAlloc(%p, %ld) returned NULL\n", ptr, size);
			exit(1);
		}
	}
	return ptr;
}