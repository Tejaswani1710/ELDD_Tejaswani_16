#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int8_t read_buf[50];

int main()
{
        int fd;
        char option;
        fd = open("/dev/my_device", O_RDWR);
        if(fd > 0) 
	{
                printf("Device File Opened successfully in the path /dev/driver2_device...\n");
        }
        else 
	{
                printf("Cannot open device file...\n");
                return 0;
        }
	printf("Data Reading ...");
        read(fd, read_buf, 1024);
        printf("Done!\n\n");
        printf("Data = %s\n\n", read_buf);
close(fd);
}
