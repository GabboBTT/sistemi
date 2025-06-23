#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>

const char *search_name;

void process_file(const char *path, struct stat *st) {
    // Ignora se non è un file regolare (es: directory, link simbolico, ecc.)
    if (!S_ISREG(st->st_mode)) return;   
    // Ignora se il file NON ha il permesso di esecuzione per l’utente
    if (!(st->st_mode & S_IXUSR)) return;  

    // Estrae il nome del file dal percorso completo (basename)
    const char *base = strrchr(path, '/'); // cerca l'ultimo '/' --> ovvero se ho cane/gatto/topo, cerca il / prima di topo
    base = base ? base + 1 : path;         // se '/' trovato, prendi il nome dopo (ovvero topo e ignora il /); altrimenti usa l'intero path

    // Confronta il nome del file con il nome da cercare
    if (strcmp(base, search_name) != 0) return;

    // Prova ad aprire il file in sola lettura
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open"); // stampa errore se non si può aprire
        return;
    }

    // Legge i primi 4 byte del file per identificare il formato
    unsigned char buf[4];
    ssize_t r = read(fd, buf, sizeof(buf));
    close(fd);
    if (r < 2) return; // se meno di 2 byte letti, non possiamo fare controlli

    // Stampa il percorso del file
    printf("%s: ", path);

    // Se inizia con "#!", è uno script (es. bash, python, ecc.)
    if (buf[0] == '#' && buf[1] == '!') {
        printf("script\n");

    // Se inizia con 0x7f 'E' 'L' 'F', è un eseguibile ELF Linux
    } else if (r >= 4 && buf[0] == 0x7f && buf[1] == 'E' && buf[2] == 'L' && buf[3] == 'F') {
        printf("ELF executable\n");

    // Altrimenti non riconosciamo il formato (ma resta eseguibile)
    } else {
        printf("eseguibile (formato non riconosciuto)\n");
    }
}

void recurse(const char *dirpath) {
    DIR *D = opendir(dirpath);
    if (!D) {
        perror("opendir");
        return;
    }

    struct dirent *d;
    while ((d = readdir(D)) != NULL) {
        // Scarto le directory "." e ".." per evitare loop infiniti
        if (strcmp(d->d_name, ".")==0 || strcmp(d->d_name, "..")==0)
            continue;
        
        // Creo il path assoluto al file o directory corrente
        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, d->d_name);

        // Recupero informazioni sul file o directory
        struct stat st;
        if (lstat(fullpath, &st) < 0) {
            perror(fullpath);
            continue;
        }

        // Se è una directory, chiamo ricorsivamente la funzione
        if (S_ISDIR(st.st_mode)) {
            recurse(fullpath);
        
        // Se è un file normale, allora passo alla funzione di elaborazione
        } else {
            process_file(fullpath, &st);
        }     
    }
    
    closedir(D);
}

int main(int argc, char *argv[]) {
    if (argc < 2 ) {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        return 1;
    }

    search_name = argv[1];
    recurse(".");
    return EXIT_SUCCESS;
}