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

typedef struct entry_str
{
    int suffix;
    char *name;
} entry_str;

/*funzione ausiliaria per l'ordinamento*/
int comp (const void * elem1, const void * elem2) 
{
    entry_str a = (*(entry_str*) elem1);
    entry_str b = (*(entry_str*) elem2);
    if (a.suffix > b.suffix) return  1;
    if (a.suffix < b.suffix) return -1;
    return 0;
}

int main(int argc, char *argv[])
{
    entry_str vect[1000];
    int current = 0;
    /*cambiamo la directory corrente e la apriamo*/
    chdir(argv[1]);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        struct stat info;
        stat(entry->d_name, &info);
        /*se e' un file regolare*/
        if (S_ISREG(info.st_mode))
        {
            char ch = '.';
            /*contiene un puntatore all'ultima occorrenza del punto*/
            char *dot = strrchr(entry->d_name, ch);
            /*vediamo se conteneva un punto e se la parte dopo e' fatta da tutti numeri*/
            if (dot != NULL && strspn(dot + 1, "0123456789") == strlen(dot + 1))
            {
                if (current < 1000)
                {
                    /*allochiamo memoria sufficiente*/
                    vect[current].name = malloc(strlen(entry->d_name) + 1);
                    strcpy(vect[current].name, entry->d_name);
                    vect[current].suffix = atoi(dot + 1);
                    current++;
                }
            }   
        }      
    }
    /*se ne abbiamo trovato almeno 1*/
    if (current > 0)
    {
        /*ordina il vettore*/
        qsort(vect, current, sizeof(entry_str), comp);
        for (int i = 0; i < current; i++)
        {
            printf("%s\n", vect[i].name);
        }
        
    }
    /*non c'erano file con estensione numerica*/
    else
    {
        printf("Nessun file con sufisso numerico trovato in questa cartella\n");
    }
    return 0;
}
