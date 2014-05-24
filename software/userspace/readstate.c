#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILE "/dev/mem"
#define DEVICE_OFFSET 0xff200000
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

int main(void) {
	volatile uint8_t *user_inputs_mem;
	uint8_t user_inputs;
	int fd, i;

	fd = open(DEVICE_FILE, O_RDONLY);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	user_inputs_mem = mmap(NULL, PAGE_SIZE, PROT_READ, MAP_PRIVATE,
			fd, DEVICE_OFFSET);

	if (user_inputs_mem == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return EXIT_FAILURE;
	}

	user_inputs = *user_inputs_mem;

	for (i = 0; i < 4; i++) {
		int state = (user_inputs >> i) & 0x1;
		if (state)
			printf("Switch %d is up\n", i);
		else
			printf("Switch %d is down\n", i);
	}

	for (i = 0; i < 4; i++) {
		int state = (user_inputs >> (i + 4)) & 0x1;
		if (state)
			printf("Key %d is up\n", i);
		else
			printf("Key %d is down\n", i);
	}

	munmap((void *) user_inputs_mem, PAGE_SIZE);
	close(fd);

	return 0;
}
