
#include <malloc.h>
#include <string.h>

// Declare main function.
extern int main(int argc, char **argv, char **envp);

// Initialisers list.
typedef void (*init_func_t)();
extern init_func_t __start_init_array[];
extern init_func_t __stop_init_array[];

// On exit callback function.
typedef void (*exit_func_t)(void *arg);

// List of on exit callbacks.
typedef struct {
	exit_func_t func;
	void       *arg;
} exit_entry_t;
static exit_entry_t *exit_entries = NULL;
static size_t num_exit_entries = 0;

// Register exit handlers.
int __cxa_atexit(exit_func_t callback, void *arg, void *dso_handle) {
	num_exit_entries ++;
	void *mem = realloc(exit_entries, num_exit_entries * sizeof(exit_entry_t));
	// // if (!mem) abort();
	// exit_entries = mem;
	// exit_entries[num_exit_entries-1] = (exit_entry_t) {
	// 	callback, arg
	// };
}

// Copy of `envp` from main.
char **_envp;

// Get value of environment variable `__env`, or NULL if it does not exist.
char *getenv(const char *__env) {
	size_t len = strlen(__env);
	for (int i = 0; _envp[i]; i++) {
		if (!strncmp(__env, _envp[i], len) && _envp[i][len] == '=') {
			return _envp[i] + len + 1;
		}
	}
	return NULL;
}

// Dummy definition so that __dso_handle exists.
int __dso_handle;

int _start(int argc, char **argv, char **envp) {
	_envp = envp;
	
	// Run global constructors.
	for (init_func_t *i = __start_init_array; i != __stop_init_array; i++) {
		// (*i)();
		// malloc(0);
	}
	
	int retval = main(argc, argv, envp);
	
	// Run global deconstructors.
	// for (size_t i = 0; i < num_exit_entries; i++) {
		// exit_entries[i].func(exit_entries[i].arg);
	// }
	// free(exit_entries);
	
	puts("Exit time.");
	
	asm volatile(
		"  li a0, 512\n"
		"  ecall"
	);
}
