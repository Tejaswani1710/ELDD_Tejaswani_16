#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int8_t write_buf[50];

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
	printf("Enter the string to write into driver :");
        scanf("  %[^\t\n]s", write_buf);
        printf("Data Writing ...");
        write(fd, write_buf, strlen(write_buf)+1);
        printf("Done!\n");
        close(fd);
}

