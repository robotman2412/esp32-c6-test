
#include <stdio.h>

extern "C" void type(const char *msg);

int main(int argc, char **argv, char **envp) {
	puts("Typing yes:");
	type("Hello, World!\n");
	puts("OK");
	return 1234;
}
