
#include <malloc.h>
#include <system.h>

#include <list_malloc.h>

void *malloc(size_t len) {
	return list_malloc(len);
}

void *realloc(void *mem, size_t len) {
	return list_realloc(mem, len);
}

void free(void *mem) {
	return list_free(mem);
}
