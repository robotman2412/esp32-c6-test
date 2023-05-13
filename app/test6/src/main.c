
#include <stdio.h>

int functor(int);

int main(int argc, char **argv) {
	int a = 512;
	printf("functor(%d) = %d\n", a, functor(a));
	return 0;
}
