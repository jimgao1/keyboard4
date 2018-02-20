#include "usb.h"
#include "tlc5940.h"
#include "util.h"

#include <stdlib.h>

// Escape, C, X, Z
uint8_t key_codes[4] = {0x69, 0x6a, 0x6b, 0x6c};

inline uint16_t sq(uint16_t in) {
	return in * in;
}

// This function must be defined, if util.h is used
void util_key_update(uint8_t key, uint8_t state) {
	srand(util_micros());
	if(state) {
		usb_keyboard_key_down(key_codes[key]);
		//tlc5940_set_rgb(key, sq(random() & 63), sq(random() & 63), sq(random() & 63));
	} else {
		usb_keyboard_key_up(key_codes[key]);
		//tlc5940_set_rgb(key, 0, 0, 0);
	}
	tlc5940_update();
}

// This function must be defined
void usb_cdc_recv(uint8_t *data, uint8_t len) {
	printf("len=%d ", len);
	for (int i = 0; i < len; i++) printf("%d ", *(data + i));
	if (len >= 5) {
		if (data[0] == 0x01) {
			tlc5940_set_rgb(data[1], data[2] << 4, data[3] << 4, data[4] << 4);
			tlc5940_update();
		}
	} else if (len >= 3 && data[0] == 2) {
		key_codes[data[1]] = data[2];
	}
	
	tlc5940_update();
//	usb_cdc_send(data, len);
}

// This function must be defined
void usb_keyboard_update_led(uint8_t led) {
//	printf("LED change: %d\n", led);
}

int main(void) {
	// Initialize things
	usb_init();
	tlc5940_init();
	util_init();

	// Map stdout to USB serial
	FILE f = usb_cdc_stdout_create_stream();
	__iob[1] = &f;

	for (int key = 0; key < 4; key++)
		tlc5940_set_rgb(key, 0, 0, 0);

	while(1) {
		// Poll for key state changes 
		util_key_poll();
		usb_run();
	}
}

