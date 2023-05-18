
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <system.h>
#include <display.h>
#include <gpio.h>

#include <pax_gfx.h>

uint8_t framebuffer[128*64/8];

int main(int argc, char **argv);
int main(int argc, char **argv) {
	pax_buf_t buf;
	pax_buf_init(&buf, framebuffer, 64, 128, PAX_BUF_1_GREY);
	pax_buf_set_orientation(&buf, PAX_O_FLIP_V_ROT_CCW);
	
	io_set_mode(21, IO_MODE_INPUT);
	io_set_mode(22, IO_MODE_INPUT);
	io_set_pull(21, IO_PULLUP);
	io_set_pull(22, IO_PULLUP);
	io_set_mode(15, IO_MODE_OUTPUT);
	
	float r0 = 15, r1 = 20;
	while (1) {
		int64_t now = uptime_ms();
		pax_background(&buf, 0);
		
		float a = now % 2000 / 1000.0f;
		float a0, a1;
		if (a < 1) {
			a0 = 0;
			a1 = -a*(2*M_PI);
		} else {
			a0 = -(a-1)*(2*M_PI);
			a1 = -(2*M_PI);
		}
		pax_draw_round_hollow_arc(&buf, 0xffffffff, 64, 32, r0, r1, a0+M_PI/2, a1+M_PI/2);
		
		pax_push_2d(&buf);
		pax_apply_2d(&buf, matrix_2d_translate(64, 32));
		pax_apply_2d(&buf, matrix_2d_rotate(now % 9000 / 9000.0f * 2 * M_PI));
		pax_draw_rect(&buf, 0xffffffff, -5, -5, 10, 10);
		pax_pop_2d(&buf);
		
		if (!io_read(21)) {
			pax_center_text(&buf, 0xffffffff, pax_font_sky, 9, 64, 55, "hI tHER!");
		} else {
			pax_center_text(&buf, 0xffffffff, pax_font_sky, 9, 64, 55, "Hi Ther.");
		}
		io_write(15, io_read(22));
		
		display_write(1, framebuffer, sizeof(framebuffer));
	}
	
	pax_buf_destroy(&buf);
	return 0;
}
