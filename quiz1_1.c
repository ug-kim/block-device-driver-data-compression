#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/string.h>

int main() {
	int fd;
	char *buf2;

	if((fd = open("/dev/sbulla",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}
    
	buf2 = (char*)malloc(100);
	printf("Before output : %s\n", buf2);
	if(read(fd, buf2, 100) < 0) {
		perror("read error");
		exit(1);
	}

//	printf("buf2 : %s\n", buf2);
	printf("Output: %s\n", buf2);

	close(fd);
	return 0;
}
