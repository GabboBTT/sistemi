#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 
#include <stdlib.h>
#include <errno.h>  
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <grp.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <sys/prctl.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/eventfd.h>
#include <sys/inotify.h>
#include <sys/signalfd.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <grp.h>
#include <string.h>

/*per provare a mander SIGUSR1 fare kill -n SIGUSR1 pid con pid uguale a quello che viene stampa*/

/*variabili globali*/
char path[PATH_MAX] = {0};          //path del file
int current = 0;                    //current 0 = sta stampando su terminale, current 1 sta stampando su file
int saved_stout;                    //salva il fd del terminale

void handler(int signo) {
    /*se sta stampando su terminale e arriva un SIGUSR1*/
    if (current == 0)
    {
        /*salva il fd creandone uno nuovo*/
        saved_stout = dup(STDOUT_FILENO);
        int fd = open(path, O_WRONLY | O_APPEND);
        /*STDOUT diventa il file*/
        dup2(fd, STDOUT_FILENO);
        /*setta append mode per il nuovo STDOUT*/
        fcntl(STDOUT_FILENO, F_SETFD, O_WRONLY | O_APPEND);
        close(fd);
        /*ora sta scrivendo su file*/
        current = 1;
    }
    /*se sta scrivendo su file*/
    else
    {
        /*chiudi il file*/
        close(STDOUT_FILENO);
        /*rimetti STDOUT su terminale*/
        dup2(saved_stout, STDOUT_FILENO);
        close(saved_stout);
        /*ora sta stampando su terminale*/
        current = 0;
    }
}

void redirectsetup(char *s)
{
    strcpy(path, s);
    /*imposta l'handler per SIGUSR1*/
    signal(SIGUSR1, handler);
    printf("Pid: %d\n", getpid());
}

int main(int argc, char *argv[]) {
    redirectsetup(argv[1]);
    while (1)
    {
    time_t now = time(NULL);
    printf("%s", ctime(&now));
    sleep(1);
    }
}
