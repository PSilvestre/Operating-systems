#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define HOW_MANY 50
#define MAX 100

int main(){
	int ramdisk = open("/dev/my_c_ramdisk", O_RDWR);
	int ones = open("/dev/my_ones", O_RDONLY);

	char contents[MAX];
	
	printf("Enter a string to write to disk: ");
	fgets(contents, MAX, stdin);
	lseek(ramdisk, 0, SEEK_SET);
	write(ramdisk, contents, strlen(contents));
	lseek(ramdisk, 0, SEEK_SET);
	read(ramdisk, contents, strlen(contents));

	printf("test string: %s\n", contents);

	sleep(5);
	
	lseek(ramdisk, 0, SEEK_SET);
	read(ones, contents, HOW_MANY);
	lseek(ramdisk, 0, SEEK_SET);
	write(ramdisk, contents, HOW_MANY);
	lseek(ramdisk, 0, SEEK_SET);
	read(ramdisk, contents, HOW_MANY);

	printf("using my_ones to copy %d ones into ramdisk: %s\n", HOW_MANY, contents);

	return 0;

}


