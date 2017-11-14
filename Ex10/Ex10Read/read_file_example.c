#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	printf("Reading from file example \n");
	int res;
	char buf[100];
	int fd = open("/dev/myresource", O_RDWR);

	res = read(fd, buf, 100);
	printf("%i\n", res);
	buf[res] = '\0';

	printf("Read: %s\n", buf);

	close(fd);
	return EXIT_SUCCESS;
}
