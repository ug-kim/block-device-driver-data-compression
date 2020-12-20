#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/string.h>

int main() {
	int fd;
	char *buf1 = "Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello\0";
	
	if((fd = open("/media/sbull/test_3.txt",O_RDWR | O_CREAT)) < 0) {
	    perror("open error");
		exit(1);
	}
	
	if(write(fd, buf1, strlen(buf1)) < 0) {
		perror("write error");
		exit(1);
	}
    
    fsync(fd);
    close(fd);

	printf("Input: %s\n", buf1);


	return 0;
}
