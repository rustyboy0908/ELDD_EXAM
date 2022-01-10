#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#define SIZE 200
int i;
int main()
{
 // file discriptor of opened file from kernel space!
  int fd; 
  char Ubuffer[SIZE]="Writing to Device from Application 1!!"; //user space buffer
  

  fd = open("/dev/myexam",O_RDWR);

  if(fd <0)
  {
    perror("Error");
    exit(1);
  }
  //write operation
write(fd,Ubuffer,sizeof(Ubuffer));

close(fd);
return 0;
}