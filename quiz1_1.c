#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/string.h>

void itoa(int num ,char *str){
	int i = 0;
	int radix = 10;
	int deg=1;
	int cnt = 0;

	while(1){
		if ( (num/deg) > 0)
			cnt++;
		else
			break;
		deg *= radix;
	}
	deg /= radix;
	for(i=0; i < cnt; i++) {
		*(str+i) = num/deg + '0';
		num -= ((num/deg) * deg);
		deg /= radix;
	}
	*(str+i) = '!';
	*(str+i+1) = '\0';
}

int main() {
	int fd;
	char *buf;
	buf = (char*)malloc(100);
	if((fd = open("/media/sbull/dgist.txt",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}
	for(int i = 0; i< 1000; i++){
		itoa(i ,buf);
		if(write(fd, buf, 100) < 0){
			perror("write error");
			exit(1);
		}
		printf("buf %d : %s\n", i, buf);
	}

	fsync(fd);
	close(fd);
	
	if((fd = open("/media/sbull/dgist.txt", O_RDWR)) < 0) {
		perror("open error");
		exit(1);
	}
    char *buf2;
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
