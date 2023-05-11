
#include <stdio.h>
#include <stdlib.h>
#include <system.h>

#define N_MEMS 4

void list_alloc_debug();

int main(int argc, char **argv, char **envp) {
	char *mems[N_MEMS];
	
	list_alloc_debug();
	
	for (size_t i = 0; i < N_MEMS; i++) {
		size_t len = i*7%64+10;
		mems[i] = malloc(len);
		printf("malloc(%zu) = %p\n", len, mems[i]);
		list_alloc_debug();
	}
	
	for (size_t i = 0; i < N_MEMS; i++) {
		printf("free(%p)\n", mems[i]);
		free(mems[i]);
		list_alloc_debug();
	}
	
	return 0;
}
