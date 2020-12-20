#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/string.h>

int main() {
	int fd;
	char *buf1 = "Hello, This is advanced system programming class. We just finished final project! It is so hard task for us, but we learned so many things according to this project. Good Luck and good bye! Thank you so much.\0";

	char *buf2;
	buf2 = (char*)malloc(strlen(buf1));

	if((fd = open("/media/sbull/test.txt",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}
	
	if(read(fd, buf2, strlen(buf1)) < 0) {
		perror("read error");
		exit(1);
	}
    
	printf("Output: %s\n", buf2);
	close(fd);

	return 0;
}
