       #include <sys/signalfd.h>
       #include <signal.h>
       #include <unistd.h>
       #include <stdlib.h>
       #include <stdio.h>
       #include <time.h> 
       #include <string.h>

       #define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

       int
       main(int argc, char *argv[])
       {
           sigset_t mask;
           int sfd;
           int fd;
           struct signalfd_siginfo fdsi;
           ssize_t s;

	printf("%d\n", getpid());
           sigemptyset(&mask);
           sigaddset(&mask, SIGUSR1);
           sigaddset(&mask, SIGUSR2);

           /* Block signals so that they aren't handled
              according to their default dispositions. */

           if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
               perror("sigprocmask");

           sfd = signalfd(-1, &mask, 0);
           if (sfd == -1)
               perror("signalfd");

           while (1) {
               s = read(sfd, &fdsi, sizeof(fdsi));
               if (s != sizeof(fdsi))
                   handle_error("read");

               if (fdsi.ssi_signo == SIGUSR1) {
               
               	time_t mytime = time(NULL);
    			char * time_str = ctime(&mytime);
    			time_str[strlen(time_str)-1] = '\0';
    			
    			char str[10];
    			sprintf(str, "%d", fdsi.ssi_pid);
    			
    			char cont[100] = "USR1 ";
    			strcat(cont,str);
    			strcat(cont," ");
    			strcat (cont, time_str);
    			strcat(cont,"\n");
    			
    			FILE* fd;
    			
    			fd = fopen("./testo.txt", "ab");
    			fwrite(cont,1,strlen(cont),fd);
    			fclose(fd); 
               
                   printf("Got SIGINT\n");
               } else if (fdsi.ssi_signo == SIGUSR2) {
                   printf("Got SIGQUIT\n");
                   exit(EXIT_SUCCESS);
               } else {
                   printf("Read unexpected signal\n");
               }
           }
       }
