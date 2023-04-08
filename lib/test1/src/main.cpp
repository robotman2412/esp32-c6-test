
extern "C" {

extern int variabler;
int variabler;

int callback(int *ptrptr);

int callback(int *ptrptr) {
	variabler = 256;
	*ptrptr = 96;
	return 256;
}

}
