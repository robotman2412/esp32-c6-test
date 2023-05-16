
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <display.h>

#include <pax_gfx.h>

uint8_t framebuffer[128*64/8];

void test_a_draw(pax_buf_t *buf) {
	pax_background(buf, 0xff000000);
	pax_push_2d(buf);
	float w = pax_buf_get_width(buf);
	float h = pax_buf_get_height(buf);
	// float fac = min(w, h);
	// pax_apply_2d(buf, matrix_2d_scale(fac, fac));
	// pax_apply_2d(buf, matrix_2d_translate(w/2, h/2));
	// pax_draw_circle(buf, 0xffffffff, 0.5, 0.5, 0.5);
	pax_draw_rect(buf, 0xffffffff, 5, 5, 20, 20);
	pax_pop_2d(buf);
}

int main(int argc, char **argv);
int main(int argc, char **argv) {
	printf("main @ %p\n", main);
	
	pax_buf_t buf;
	printf("pax_buf_init\n");
	pax_buf_init(&buf, framebuffer, 64, 128, PAX_BUF_1_GREY);
	printf("test_a_draw\n");
	test_a_draw(&buf);
	
	printf("display_write\n");
	display_write(1, framebuffer, sizeof(framebuffer));
	
	printf("pax_buf_destroy\n");
	pax_buf_destroy(&buf);
	
	printf("exit\n");
	return 0;
}
