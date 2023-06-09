
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <system.h>
#include <display.h>
#include <gpio.h>

#include <pax_gfx.h>

uint8_t imagerom[104*64/8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x7E, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x1D, 0xDF, 0xE0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFA, 0xAF, 0xF0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0xE1, 0x1F, 0xF8, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0xC0, 0x0E, 0x38, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x80, 0xA0, 0x1C, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6E, 0x04, 0x00, 0x3F, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x80, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x10, 0x23, 0xFF, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x02, 0x21, 0xF1, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x08, 0x43, 0xFD, 0xE0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x3C, 0x10, 0xDB, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x8D, 0x68, 0xEF, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x05, 0xF4, 0x74, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x9C, 0xF0, 0x78, 0xF8, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x1F, 0xC0, 0x78, 0x78, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x5C, 0x02, 0x6C, 0x38, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x1C, 0x03, 0x70, 0x1C, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x40, 0x03, 0x30, 0x1E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x90, 0x07, 0xF0, 0x0E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x10, 0x0F, 0xD0, 0x0F, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x60, 0x3F, 0xC0, 0x07, 0x00,
	0x00, 0x1F, 0x20, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x7F, 0x40, 0x47, 0x80,
	0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x65, 0x61, 0xFF, 0x90, 0x0F, 0x00,
	0x00, 0x34, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x26, 0x87, 0xFE, 0x08, 0x03, 0x80,
	0x00, 0x7A, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x1F, 0xFF, 0x0C, 0x57, 0x00,
	0x00, 0xFE, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xFF, 0xFE, 0x06, 0x03, 0x80,
	0x00, 0xDF, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xBD, 0x07, 0x13, 0x80,
	0x01, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFE, 0x5C, 0x7F, 0x83, 0x80,
	0x01, 0x90, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x07, 0xEF, 0x34, 0xFF, 0xC9, 0x80,
	0x01, 0x89, 0x76, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xD5, 0xFF, 0xE3, 0xC0,
	0x03, 0xEE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xF1, 0xAB, 0xC0, 0xD1, 0xC0,
	0x01, 0xEF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFB, 0xF7, 0x00, 0xFB, 0xC0,
	0x03, 0x47, 0x6C, 0x00, 0x00, 0x3E, 0x00, 0x37, 0xEF, 0xF6, 0x00, 0xFB, 0xC0,
	0x03, 0x86, 0x24, 0x0E, 0x00, 0x7B, 0x80, 0x3F, 0x8F, 0xFE, 0x00, 0xFF, 0xC0,
	0x07, 0xC6, 0x3C, 0x3F, 0x80, 0xEB, 0x83, 0xBF, 0x87, 0xFC, 0x00, 0x7F, 0xE0,
	0x06, 0x06, 0x1C, 0x3D, 0x80, 0x5F, 0xE3, 0xFE, 0x43, 0x8E, 0x00, 0x7F, 0xE0,
	0x0F, 0x0E, 0x0C, 0x79, 0x00, 0x3F, 0xE2, 0xFD, 0x03, 0xCE, 0x00, 0x5F, 0xE0,
	0x0F, 0x0E, 0x04, 0xF1, 0x00, 0x07, 0xFC, 0xF8, 0x91, 0xC3, 0x04, 0x5F, 0xE0,
	0x1F, 0xBC, 0x04, 0x78, 0x00, 0x01, 0xFF, 0xF5, 0x01, 0xC3, 0x10, 0x0F, 0x70,
	0x1A, 0x30, 0x00, 0x78, 0x00, 0xB5, 0xCF, 0xF2, 0x21, 0xE3, 0xC1, 0x0D, 0xE0,
	0x38, 0x78, 0x00, 0x78, 0x0F, 0xFF, 0xF3, 0xC4, 0x88, 0xF1, 0xC8, 0x1F, 0xE0,
	0x34, 0xFF, 0x80, 0x3C, 0x3F, 0xFF, 0xF9, 0xC9, 0x3E, 0xF8, 0xC4, 0x1F, 0xF0,
	0x38, 0xFF, 0xEF, 0x7E, 0x6F, 0xFF, 0xF8, 0xA8, 0xFF, 0xFC, 0x20, 0x1F, 0xF0,
	0x30, 0x6F, 0xFF, 0xEE, 0x0F, 0xFF, 0xFE, 0x05, 0xFF, 0xFD, 0x00, 0x3D, 0xF0,
	0x10, 0x14, 0xBD, 0x7E, 0x0F, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0x48, 0x7F, 0xF8,
	0x1E, 0x7F, 0xF7, 0xFE, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x7F, 0xFC,
	0x0F, 0xFF, 0xFF, 0xFC, 0x00, 0x01, 0x07, 0xFF, 0xFF, 0xFF, 0x05, 0xFF, 0xFC,
	0x01, 0xE0, 0xBC, 0x10, 0x00, 0x00, 0x00, 0xDA, 0x0D, 0xFF, 0x9F, 0xF9, 0xF8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0xFF, 0xE0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF4, 0x00, 0x00,
	0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x01, 0xC0, 0x00, 0x00,
	0x0F, 0xF0, 0x38, 0x70, 0x01, 0xC0, 0x7E, 0x1F, 0xF1, 0xF8, 0x70, 0x38, 0x18,
	0x3F, 0xF0, 0x78, 0x7C, 0x07, 0xC3, 0xFC, 0x7F, 0xEF, 0xF8, 0xF8, 0x3C, 0x38,
	0x1E, 0x78, 0xFC, 0xFE, 0x0F, 0xC3, 0xF8, 0x3F, 0x8F, 0xE1, 0xF0, 0x7C, 0xFC,
	0x17, 0xF1, 0xFC, 0x67, 0x1F, 0x03, 0x80, 0x07, 0x0E, 0x03, 0xF8, 0x7D, 0xF8,
	0x07, 0xE1, 0xDC, 0x63, 0xBC, 0x63, 0xF8, 0x07, 0x0F, 0xE3, 0xB8, 0xFF, 0xF8,
	0x0F, 0xE1, 0xDE, 0x63, 0x78, 0xF7, 0xF8, 0x0E, 0x0F, 0xE3, 0xBC, 0xFF, 0xB8,
	0x07, 0x3B, 0xFF, 0xE7, 0xF0, 0xFF, 0xE0, 0x0E, 0x1F, 0x87, 0xFF, 0xEF, 0x38,
	0x0E, 0x7B, 0xFE, 0x7F, 0x73, 0xFF, 0x08, 0x0C, 0x1C, 0x27, 0xFD, 0xC6, 0x38,
	0x0F, 0xFF, 0x2E, 0xFC, 0x7F, 0xF7, 0xFD, 0x0C, 0x1F, 0xFE, 0x5D, 0xC0, 0x70,
	0x1F, 0xE7, 0x1C, 0xF8, 0x7F, 0xC7, 0xFB, 0x8E, 0x1F, 0xEE, 0x3D, 0xC0, 0x30,
	0x0F, 0x42, 0x0C, 0x60, 0x1E, 0x03, 0xF3, 0x0E, 0x07, 0xCC, 0x18, 0x80, 0x20,
};
uint8_t framebuffer[128*64/8];

uint8_t bytereverse(uint8_t in) {
	bool bif[8];
	// Unpacc.
	bif[7] = in & 1;
	bif[6] = in & 2;
	bif[5] = in & 4;
	bif[4] = in & 8;
	bif[3] = in & 16;
	bif[2] = in & 32;
	bif[1] = in & 64;
	bif[0] = in & 128;
	// Repacc.
	return bif[7]*128 + bif[6]*64 + bif[5]*32 + bif[4]*16 + bif[3]*8 + bif[2]*4 + bif[1]*2 + bif[0];
}

void heart(pax_buf_t *buf, float x, float y, float radius) {
	pax_push_2d(buf);
	pax_apply_2d(buf, matrix_2d_translate(x, y));
	pax_apply_2d(buf, matrix_2d_scale(radius, radius));
	pax_apply_2d(buf, matrix_2d_translate(0, -0.65));
	pax_apply_2d(buf, matrix_2d_rotate(M_PI/4));
	pax_draw_rect(buf, 0xffffffff, 0, 0, -1, 1);
	pax_draw_arc(buf, 0xffffffff, 0, 0.5, 0.5, -M_PI/2, M_PI/2);
	pax_draw_arc(buf, 0xffffffff, -0.5, 0, 0.5, 0, M_PI);
	pax_pop_2d(buf);
}

int dither_thresh(int x, int y) {
	if (x & 1) {
		return (y & 1)
			? 102
			: 153;
	} else {
		return (y & 1)
			? 204
			: 51;
	}
}

void dither(pax_buf_t *from, pax_buf_t *to) {
	for (int y = 0; y < pax_buf_get_height(from); y++) {
		for (int x = 0; x < pax_buf_get_width(from); x++) {
			int grey = pax_get_pixel(from, x, y) & 255;
			bool bit = grey >= dither_thresh(x, y);
			pax_set_pixel_raw(to, bit, x, y);
		}
	}
}

int main(int argc, char **argv) {
	pax_buf_t buf;
	pax_buf_init(&buf, framebuffer, 64, 128, PAX_BUF_1_GREY);
	pax_buf_set_orientation(&buf, PAX_O_FLIP_V_ROT_CCW);
	
	// I was too lazy to reimport the image.
	for (size_t i = 0; i < sizeof(imagerom); i++) {
		imagerom[i] = bytereverse(imagerom[i]);
	}
	pax_buf_t img;
	pax_buf_init(&img, imagerom, 104, 64, PAX_BUF_1_GREY);
	
	io_set_mode(21, IO_MODE_INPUT);
	io_set_mode(22, IO_MODE_INPUT);
	io_set_pull(21, IO_PULLUP);
	io_set_pull(22, IO_PULLUP);
	io_set_mode(15, IO_MODE_OUTPUT);
	
	// Badge.team
	pax_background(&buf, 0);
	pax_draw_image(&buf, &img, 12, 0);
	display_write(1, framebuffer, sizeof(framebuffer));
	delay_ms(1500);
	
	pax_background(&buf, 0);
	display_write(1, framebuffer, sizeof(framebuffer));
	delay_ms(500);
	
	// Presents.
	int64_t start = uptime_ms();
	while (uptime_ms() < start + 3000) {
		int y;
		int64_t now = uptime_ms() - start;
		if (now < 500) {
			y = 82 + now * (48-82) / 500;
		} else if (now < 2500) {
			now -= 500;
			y = 48 + now * (16-48) / 2000;
		} else {
			now -= 2500;
			y = 16 + now * (-18-16) / 500;
		}
		
		pax_background(&buf, 0);
		pax_center_text(&buf, 0xffffffff, pax_font_sky, 18, 64, y-9, "PRESENTS");
		display_write(1, framebuffer, sizeof(framebuffer));
	}
	
	pax_background(&buf, 0);
	display_write(1, framebuffer, sizeof(framebuffer));
	delay_ms(500);
	
	// Computer graphics.
	float r0 = 15, r1 = 20;
	start = uptime_ms();
	while (uptime_ms() < start + 3000) {
		int y;
		int64_t now = uptime_ms() - start;
		if (now < 500) {
			y = 64 + now * (16-64) / 500;
		} else if (now < 2500) {
			now -= 500;
			y = 16 + now * (-16-16) / 2000;
			now += 500;
		} else {
			now -= 2500;
			y = -16 + now * (-64+16) / 500;
			now += 2500;
		}
		pax_background(&buf, 0);
		
		float a = now % 4000 / 2000.0f;
		float a0, a1;
		if (a < 1) {
			a0 = 0;
			a1 = -a*(2*M_PI);
		} else {
			a0 = -(a-1)*(2*M_PI);
			a1 = -(2*M_PI);
		}
		pax_push_2d(&buf);
		pax_apply_2d(&buf, matrix_2d_translate(0, y));
		pax_draw_round_hollow_arc(&buf, 0xffffffff, 64, 32, r0, r1, a0+M_PI/2, a1+M_PI/2);
		// heart(&buf, 108, 19, 20);
		
		pax_push_2d(&buf);
		pax_apply_2d(&buf, matrix_2d_translate(64, 32));
		pax_apply_2d(&buf, matrix_2d_rotate(now % 9000 / 9000.0f * 2 * M_PI));
		pax_draw_rect(&buf, 0xffffffff, -5, -5, 10, 10);
		pax_pop_2d(&buf);
		
		pax_center_text(&buf, 0xffffffff, pax_font_sky, 9, 64, 55, "Computer graphics");
		pax_pop_2d(&buf);
		
		display_write(1, framebuffer, sizeof(framebuffer));
	}
	
	pax_background(&buf, 0);
	display_write(1, framebuffer, sizeof(framebuffer));
	delay_ms(500);
	
	// But!
	start = uptime_ms();
	while (uptime_ms() < start + 3000) {
		float scale;
		int64_t now = uptime_ms() - start;
		if (now < 500) {
			scale = now * 1.2 / 500;
		} else if (now < 600) {
			scale = 1.2 + (now - 500) * (1.0-1.2) / 100;
		} else if (now < 2500) {
			scale = 1.0;
		} else {
			scale = 1.0 + (now - 2500) * (0-1.0) / 500;
		}
		pax_background(&buf, 0);
		
		pax_push_2d(&buf);
		pax_apply_2d(&buf, matrix_2d_translate(64, 32));
		pax_apply_2d(&buf, matrix_2d_scale(scale, scale));
		pax_center_text(&buf, 0xffffffff, pax_font_sky, 18, 0, 0-18, "But!");
		pax_center_text(&buf, 0xffffffff, pax_font_sky, 9, 0, 0, "This is actually\nloaded to RAM!");
		pax_pop_2d(&buf);
		
		display_write(1, framebuffer, sizeof(framebuffer));
	}
	
	pax_background(&buf, 0);
	display_write(1, framebuffer, sizeof(framebuffer));
	delay_ms(500);
	
	pax_buf_destroy(&buf);
	return 0;
}
