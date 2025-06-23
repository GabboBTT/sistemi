#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <time.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{
 char* file = argv[1];
 char* cfile = argv[2];
 int fd, fdc;
 struct flock lock;

FILE* ffd = fopen (file, "r");

fseek(ffd, 0, SEEK_END);
  
 int res = ftell(ffd);
 
 fseek(ffd, 0, SEEK_SET);
 
 fclose(ffd);

 printf ("opening %s\n", file);

 fd = open (file, O_RDONLY);
 
 
srand(time(NULL));
int r = rand() %10;
printf("sleeping for %d seconds\n", r);
sleep(r);

 fdc = open (cfile, O_WRONLY);
 printf ("locking\n");
 
 memset (&lock, 0, sizeof(lock));
 lock.l_type = F_WRLCK;
 
 fcntl (fdc, F_SETLKW, &lock);

 printf ("locked; hit Enter to unlock... ");
 
 sendfile(fdc, fd, NULL, res );

 printf ("unlocking\n");
 
 lock.l_type = F_UNLCK;
 fcntl (fd, F_SETLKW, &lock);
 
 r = rand() %10;
printf("sleeping for %d seconds\n", r);
 sleep(r);

 close (fd);
 return 0;
}
