#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/string.h>

int main() {
	int fd;
	char *buf1 = "Hello, This is advanced system programming class. We just finished final project! It is so hard task for us, but we learned so many things according to this project. Good Luck and good bye! Thank you so much.\0";
	
	if((fd = open("/media/sbull/test.txt",O_RDWR | O_CREAT)) < 0) {
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
