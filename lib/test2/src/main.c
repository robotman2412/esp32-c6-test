
#include <stdio.h>
#include <system.h>

const int col_arr[] = {
	91, 93, 92, 96, 94, 95
};

void type(const char *msg) {
	int col = 0;
	while (*msg) {
		printf("\033[%dm%c", col_arr[++col % 6], *msg);
		msg ++;
	}
	printf("\033[0m");
}
