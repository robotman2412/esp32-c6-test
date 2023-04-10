
#include <stdio.h>
// #include <string>

struct X {
	X() {
		printf("X()\n");
	}
	~X() {
		printf("~X()\n");
	}
};

X myThingy;

int main(int argc, char **argv, char **envp) {
	for (int i = 0; i < argc; i++) {
		printf("Arg %d: %s\n", i, argv[i]);
	}
	for (int i = 0; envp[i]; i++) {
		printf("Env %d: %s\n", i, envp[i]);
	}
	return 0xcafe;
}
