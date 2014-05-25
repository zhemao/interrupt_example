#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define SYSFS_FILE "/sys/bus/platform/drivers/fpga_uinput/fpga_uinput"
#define NUM_SWITCHES 4
#define NUM_KEYS 4

void print_state_change(uint8_t cur_state, uint8_t last_state)
{
	uint8_t changed = cur_state ^ last_state;
	int i;

	for (i = 0; i < NUM_SWITCHES; i++) {
		if (!((changed >> i) & 1))
			continue;
		if ((cur_state >> i) & 1)
			printf("switch %d flipped up\n", i);
		else
			printf("switch %d flipped down\n", i);
	}

	for (i = 0; i < NUM_KEYS; i++) {
		int shift = NUM_SWITCHES + i;

		if (!((changed >> shift) & 1))
			continue;
		if ((cur_state >> shift) & 1)
			printf("key %d released\n", i);
		else
			printf("key %d pushed\n", i);
	}
}

int main(void) {
	FILE *f;
	uint8_t last_state = 0xf0;
	int ret;

	for (;;) {
		uint8_t cur_state;
		f = fopen(SYSFS_FILE, "r");
		if (f == NULL) {
			perror("fopen");
			return EXIT_FAILURE;
		}
		ret = fread(&cur_state, 1, 1, f);
		fclose(f);
		if (ret != 1) {
			if (errno == EAGAIN)
				continue;
			return EXIT_FAILURE;
		}
		print_state_change(cur_state, last_state);
		last_state = cur_state;
	}

	return 0;
}
