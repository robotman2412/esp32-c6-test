
typedef void*(*PLTF)(const char*);

static PLTF lookup;

int __plt[1] = { 12 };

extern "C" {

int __cxa_guard_acquire(void *) { return 0; }
void __cxa_guard_release(void *) {}
void __cxa_guard_abort(void *) {}

int wrapper();

extern const char mydata[];
const char mydata[] = "This is, a MESSAGE.";

void functor(const char **strings, int count, PLTF lookup) {
	for (int i = 0; i < count; i++) {
		lookup(strings[i]);
	}
}

int _start(PLTF _lookup) {
	// const char *strings[] = {
	// 	"First line",
	// 	"Second line",
	// 	"Third line",
	// };
	// functor(strings, 3, _lookup);
	return (int) &mydata;
}

}
