#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/string.h>

int main() {
	int fd;
	char *buf1 = "Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?Hello! Nice to meet you! Nice to meet you, too! What's your name? My name is kyeonghyeon. How are you? I'm fine thank you and you?\0";

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
