#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

/*
    Esercizio 1: Linguaggio C (obbligatorio) 20 punti
    Usando inotify scrivere un programma inotirun che ha come parametro il pathname di una
    directory vuota che chiameremo D. Quando vengono inseriti file in D questi vengono eseguiti (uno
    alla volta) e cancellati. I file in D hanno il seguente formato:
    * il pathname dell'eseguibile
    * una riga per ogni elemento di argv.
    Es:
    /bin/ls
    ls
    -l
    /tmp
*/

void esegui_e_cancella(const char *dir, const char *file) {
    char path[4096];
    snprintf(path, sizeof(path), "%s/%s", dir, file);

    FILE *f = fopen(path, "r");
    if (!f) return;

    char *argv[128];
    char line[4096];
    int argc = 0;

    while (fgets(line, sizeof(line), f)) {
        // Rimuovi newline
        line[strcspn(line, "\n")] = 0;

        // Riga vuota → esegui comando corrente
        if (strlen(line) == 0 && argc > 0) {
            argv[argc] = NULL;

            pid_t pid = fork();
            if (pid == 0) {
                execv(argv[0], argv);
                perror("execv");
                exit(1);
            } else if (pid > 0) {
                waitpid(pid, NULL, 0);
            }

            // Libera e resetta
            for (int i = 0; i < argc; ++i) free(argv[i]);
            argc = 0;
        } else if (strlen(line) > 0) {
            argv[argc++] = strdup(line);
        }
    }

    // Esegue l'ultimo comando se non terminava con una riga vuota
    if (argc > 0) {
        argv[argc] = NULL;
        pid_t pid = fork();
        if (pid == 0) {
            execv(argv[0], argv);
            perror("execv");
            exit(1);
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
        }
        for (int i = 0; i < argc; ++i) free(argv[i]);
    }

    fclose(f);
    unlink(path);
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <directory>\n", argv[0]);
        exit(1);
    }
    const char *dir = argv[1];

    // Inizializza inotify
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(1);
    }

    /* 
        Aggiungi la directory da monitorare, con gli eventi che ci interessano, ovvero:
            - IN_CLOSE_WRITE: file chiuso dopo scrittura
            - IN_MOVED_TO: file spostato nella directory
            - IN_CREATE: file creato nella directory
    */
    int wd = inotify_add_watch(fd, dir, IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE);
    if (wd < 0) {
        perror("inotify_add_watch");
        exit(1);
    }

    // Buffer per gli eventi
    char buf[BUF_LEN];
    while (1) {   // Ciclo infinito per monitorare gli eventi
        int len = read(fd, buf, BUF_LEN);   // Legge gli eventi da inotify
        if (len < 0) {
            perror("read");
            break;
        }
        int i = 0;
        // Il while scorre gli eventi letti e li gestisce
        while (i < len) {  
            // Cast del buffer a struct inotify_event, ovvero la struttura che contiene le informazioni sugli event
            struct inotify_event *event = (struct inotify_event *)&buf[i];

            // Se l'evento ha un nome e il tipo di evento è uno di quelli che ci interessano
            if (event->len && (event->mask & (IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE))) {
                esegui_e_cancella(dir, event->name);
            }
            i += EVENT_SIZE + event->len; // Avanza l'indice per il prossimo evento
        }
    }
    close(fd);
    return 0;
}