
// typedef void*(*MessagePrinter)(const char*);

// static MessagePrinter printer;

extern "C" int __plt[1];
int __plt[1] = { 12 };

// extern "C" {

// // Required by obscure ABI things.
// int __cxa_guard_acquire(void *) { return 0; }
// void __cxa_guard_release(void *) {}
// void __cxa_guard_abort(void *) {}

// // Wrapper callback in testing, see 'test.s'.
// int wrapper();

// // Data read from loader code.
// extern const char mydata[];
// const char mydata[] = "This is, a MESSAGE.";

// // Test to make sure functions work.
// void functor(const char **strings, int count, MessagePrinter printer) {
// 	for (int i = 0; i < count; i++) {
// 		printer(strings[i]);
// 	}
// }

// int _start(MessagePrinter _printer) {
// 	const char *strings[] = {
// 		"First line",
// 		"Second line",
// 		"Third line",
// 	};
// 	functor(strings, 3, _printer);
// 	// Test to make sure addressing works.
// 	return (int) &mydata;
// }

// }

__attribute__((constructor))
void corooror() {
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
}


const char mydata[] = "HSD";


extern "C" int wrapper();

int main() {
	return wrapper();
}
