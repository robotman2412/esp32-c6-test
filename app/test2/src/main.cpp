
#include <stddef.h>

// typedef void*(*MessagePrinter)(const char*);

// static MessagePrinter printer;


__attribute__((constructor))
void corooror() {
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
}


const char mydata[] = "HSD";


extern "C" int callback();
extern "C" int variabler;

const int device = 0;

int main() {
	// return callback() + variabler;
	*(int*)&device = 1;
	return (int) callback;
}
