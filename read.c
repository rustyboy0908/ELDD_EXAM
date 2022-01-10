#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#define DEV_MEM_SIZE 200
int i;

int main()
{
 // file discriptor of opened file from kernel space!
  int fd; 
  char Kbuffer[SIZE];
  char URbuffer[DEV_MEM_SIZE];//kernel buffer
  fd = open("/dev/myexam",O_RDWR);

  if(fd <0)
  {
    perror("Error");
    exit(1);
  }
  read(fd,Kbuffer,sizeof(Kbuffer));
  //printf("DATA received : %s\n",Kint);
  //printf("Ubuffer data: %s\n",Ubuffer);
  printf("DATA received : %s\n",Kbuffer);
  close(fd);
}