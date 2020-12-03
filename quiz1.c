#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/string.h>

int main() {
	int fd;
	char *buf1 = "Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?";
	char *buf2;

	if((fd = open("/dev/sbulla",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}
	
	if(write(fd, buf1, strlen(buf1)) < 0) {
		perror("write error");
		exit(1);
	}
    
    fsync(fd);
    close(fd);
	if((fd = open("/dev/sbulla",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}
    
	buf2 = (char*)malloc(strlen(buf1));
	
	if(read(fd, buf2, strlen(buf1)) < 0) {
		perror("read error");
		exit(1);
	}
	
//	printf("buf2 : %s\n", buf2);
	printf("Input: %s\n", buf1);
	printf("Output: %s\n", buf2);

	close(fd);

	return 0;
}
