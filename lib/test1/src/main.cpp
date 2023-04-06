
extern "C" {

extern int variabler;
int variabler;

int callback();

int callback() {
	variabler = 256;
	return 256;
}

}
