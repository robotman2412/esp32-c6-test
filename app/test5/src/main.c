
#include <stdio.h>
#include <system.h>

const int col_arr[] = {
	91, 93, 92, 96, 94, 95
};

int main(int argc, char **argv, char **envp) {
	int c   = -1;
	int col = 0;
	do {
		c = getchar();
		if (c == -1) {
			delay_ms(10);
		} else if (c == '\n') {
			putchar('\r');
			putchar('\n');
		} else if (c >= 0x21 && c <= 0x7e) {
			printf("\033[%dm%c", col_arr[col++ % 6], c);
			fflush(stdout);
		} else if (c == 0x20) {
			putchar(' ');
		}
	} while (c != 0x04);
	return 0;
}
