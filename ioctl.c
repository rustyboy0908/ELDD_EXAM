#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
#define SIZE 200
//IOCTL format MACRO 
#define GETSTAT _IOWR('r','a',int32_t*)
 
int main()
{
        int fd;
        int32_t number;

        struct stats
        {
             int size;
             char buff[SIZE];
             int r_w;
        };

        struct stats *value;
        
        printf("Welcome to User IOCTL program \n");
 
        printf("\nOpening Driver\n");
        fd = open("/dev/myexam", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
        printf("Reading Value from Structure\n");
        ioctl(fd, GETSTAT, (int32_t*) &value);
        printf("Read_Write Value is %d\n", value->r_w);
        printf("Size %d \n",value->size);
 
        printf("Closing Driver\n");
        close(fd);
}