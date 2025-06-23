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
    char path[4096];  // Buffer per il percorso completo del file
    snprintf(path, sizeof(path), "%s/%s", dir, file);  // Costruisce il percorso completo

    FILE *f = fopen(path, "r");   // Apre il file con permission di lettura
    if (!f) return;

    char *argv[128];   // Array per gli argomenti del comando
    char line[4096];   // Buffer per leggere le linee dal file
    int argc = 0;      // Contatore per gli argomenti

    // Legge il file riga per riga e popola argv
    while (fgets(line, sizeof(line), f) && argc < 127) {
        line[strcspn(line, "\n")] = 0; // rimuove newline
        argv[argc++] = strdup(line);  // Duplica la stringa per evitare problemi di memoria
    }
    argv[argc] = NULL; // Termina l'array di argomenti con NULL
    fclose(f);

    if (argc > 0) {
        pid_t pid = fork();   // Crea un nuovo processo per eseguire il comando

        // Se pid è 0, siamo nel processo figlio
        if (pid == 0) {
            execv(argv[0], argv);  // Esegue il comando specificato nel file
            perror("execv");    // Se execv fallisce, stampa l'errore
            exit(1);

        // Se pid è maggiore di 0, siamo nel processo padre
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);  // Attende che il processo figlio termini
        }
    }

    // Libera la memoria allocata per gli argomenti
    for (int i = 0; i < argc; ++i) free(argv[i]);
    // Cancella il file dopo l'esecuzione
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
