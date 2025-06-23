#include <stdio.h>      // per printf, perror, fprintf
#include <stdlib.h>     // per getenv, exit
#include <string.h>     // per strcmp, snprintf
#include <unistd.h>     // per unlink, rename, access
#include <sys/stat.h>   // per mkdir, stat
#include <sys/types.h>  // per struct stat, DIR
#include <dirent.h>     // per DIR, opendir, readdir
#include <errno.h>      // per errno
#include <time.h>       // per time()

#define MAX_PATH 4096   // dimensione massima per i percorsi

// Mostra messaggio se si usa male il comando, per esempio facendo safrem -L -P, che non e' possibile
void usage() {
    fprintf(stderr, "Uso: saferm [-L | -P | -R file | file ...]\n");
    exit(EXIT_FAILURE);            // Dopo aver scritto l'errore, esce 
}

// Crea la directory .trash nella home se non esiste
void ensure_trash_exists(const char *trash_path) {
    struct stat st;   // Variabile struct che uso per salvare info(dimensione, etc...) sulla directory .trash nel caso esista
    /*
        stat in POSIX controlla se esiste o no una determinata directory:
            - trash_path --> percorso da controllare
            - &st --> se trova la directory, salva info sulla directory in st
        Se ritorna -1, non trovata
    */
    if (stat(trash_path, &st) == -1) {
        mkdir(trash_path, 0700);  // crea la directory solo se non esiste e gli fornisco l'accesso completo all'utente con 0700
    }
}

// Elenca i file presenti nel cestino .trash
void list_trash(const char *trash_path) {
    DIR *dir = opendir(trash_path);
    struct dirent *entry;

    // dir con problemi, allora problemi con la dir esco con errore
    if (!dir) {
        perror("Errore apertura .trash");
        exit(EXIT_FAILURE);
    }

    // Legge ogni file in dir fino a che non e' vuoto e stampo il nome
    while ((entry = readdir(dir)) != NULL) {
        // Esclude i file nascosti come "." e ".."
        if (entry->d_name[0] != '.')
            printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

// Svuota completamente il contenuto del cestino
void purge_trash(const char *trash_path) {
    DIR *dir = opendir(trash_path);
    struct dirent *entry;
    char filepath[MAX_PATH];

    if (!dir) {
        perror("Errore apertura .trash");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            // Costruisce il path mettendo il path per la cartella trash + il file appena letto da readdir, 
            // si usa snprintf perche' si parla di stringhe dinamiche, che cambiano ad ogni iter
            snprintf(filepath, MAX_PATH, "%s/%s", trash_path, entry->d_name);
            unlink(filepath);  // rimuove il file
        }
    }

    closedir(dir);
}

/* Sposta il file nel cestino, se esiste gia un file con lo stesso nome, bisogna:
    - Rinominare il file esistende aggiungendo la data (timestamp)
    - Solo dopo, spostare il nuovo file nel cestino col suo nome originale
*/ 
void move_to_trash(const char *trash_path, const char *filename) {
    char new_path[MAX_PATH];
    snprintf(new_path, MAX_PATH, "%s/%s", trash_path, filename);

    // Se il file esiste gia nel cestino
    if (access(new_path, F_OK) == 0) {   // access controlla in questo caso avendo F_OK, se esiste il file
        char backup_path[MAX_PATH];
        time_t now = time(NULL);  // Ottieni timestamp attuale
        // Prende il path di prima e aggiunge il timestamp attuale
        snprintf(backup_path, MAX_PATH, "%s/%s.%ld", trash_path, filename, now);

        // Rinominare il file gia nel cestino con timestamp
        if (rename(new_path, backup_path) == -1) {
            perror("Errore nel rinominare il file esistente nel cestino");
            exit(EXIT_FAILURE);
        }
    }

    // Ora possiamo spostare il nuovo file normalmente
    if (rename(filename, new_path) == -1) {
        perror("Errore spostamento nel cestino");
        exit(EXIT_FAILURE);
    }
}

// Ripristina un file dal cestino nella directory corrente
void restore_file(const char *trash_path, const char *filename) {
    char from[MAX_PATH];
    snprintf(from, MAX_PATH, "%s/%s", trash_path, filename);

    // Sposta il file dal cestino alla directory corrente
    if (rename(from, filename) == -1) {
        perror("Errore ripristino file");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) usage();  // almeno un argomento necessario

    char trash_path[MAX_PATH];
    const char *home = getenv("HOME");  // recupera la directory home dell'utente
    snprintf(trash_path, MAX_PATH, "%s/.trash", home);  // costruisce il path di .trash

    ensure_trash_exists(trash_path);  // assicura che .trash esista

    // Gestione opzione -L → lista il cestino
    if (strcmp(argv[1], "-L") == 0) {
        if (argc != 2) usage();  // opzione esclusiva
        list_trash(trash_path);
    }
    // Gestione opzione -P → svuota il cestino
    else if (strcmp(argv[1], "-P") == 0) {
        if (argc != 2) usage();  // opzione esclusiva
        purge_trash(trash_path);
    }
    // Gestione opzione -R file → ripristina file dal cestino
    else if (strcmp(argv[1], "-R") == 0) {
        if (argc != 3) usage();  // richiede un file
        restore_file(trash_path, argv[2]);
    }
    // Nessuna opzione: sposta i file nel cestino
    else {
        for (int i = 1; i < argc; ++i) {
            move_to_trash(trash_path, argv[i]);
        }
    }

    return 0;
}
