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

int main(int argc, char const *argv[])
{
    /*cambiamo dir corrente in /proc*/
    chdir("/proc");
    /*la apriamo*/
    DIR *dir = opendir(".");
    struct dirent *entry;
    /*sleep ritorna 0 se si chiude correttamente*/
    while (!sleep(1))
    {
        int count = 0;
        entry = NULL;
        while ((entry = readdir(dir)) != NULL)
        {
            struct stat info;
            stat(entry->d_name, &info);
            /*vede se e' una directory il cui nome e' un numero*/
            if (S_ISDIR(info.st_mode) && strspn(entry->d_name, "0123456789") == (strlen(entry->d_name)))
            {
                count++;
            }
        }
        printf("Current active process %d\n", count);
        /*riposiziona il dir stream all'inizio*/
        rewinddir(dir);
    }
    
    return 0;
}
