
// Simple printer.
void print(const char *msg) {
	if (!msg) return;
	while (*msg) {
		*(volatile char *) 0x60000000 = *msg;
		msg ++;
	}
}

// This is the entrypoint after the stack has been set up and the init functions have been run.
// Main is not allowed to return, so declare it noreturn.
void main() __attribute__((noreturn));
void main() {
	print("Hello, World!\r\n");
	print("This is a C runtime!\r\n");
	while(1);
}
