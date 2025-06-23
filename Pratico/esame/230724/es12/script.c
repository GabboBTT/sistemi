#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define ARCHIVE_DIR "archive"
#define PATH_MAX 4096

/*
Esercizio 1: Linguaggio C (obbligatorio) 20 punti
Scrivere un programma che crei nella directory corrente (se non esiste già) una sottodirectory di
nome ... (tre punti).
Tutti i file (regolari) presenti nella directory devono essere spostati nella sottodirectory ... (tre punti) e
ogni file deve essere sostituito nella dir corrente con un link simbolico (relativo, non assoluto) alla
nuova locazione. Usare la system call rename per fare la sostituzione in modo atomico (in nessun
istante il file deve risultare inesistente).
*/

// Controlla se è un file e non un link
int is_regular_file(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

int main () {
    DIR *dir = opendir(".");   // Apro la directory attuale
    if (!dir) {
        perror("opendir");
        return 1;
    }

    // Creo la sottodirectory se non esiste
    struct stat st;
    if (stat(ARCHIVE_DIR, &st) == -1) {      // Controllo se esiste la dir richiesta
        if (mkdir(ARCHIVE_DIR, 0755) == -1) {  // Se non esiste la creo e le do il permessi
            perror("mkdir");
            return 1;
        }
    }
    
    struct dirent *entry; // Funzione associata a opendir, per avere info base sulla dir
    // Itero sulla directory ogni file
    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;

        // Salta "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Ignoro se è già un link simbolico o non un file
        if (!is_regular_file(name))  
            continue;

        // Costruisce path di destinazione
        char dest_path[PATH_MAX];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", ARCHIVE_DIR, name);

        // Sposta il file con rename() in modo atomico
        if (rename(name, dest_path) == -1) {
            perror("rename");
            continue;
        }

        // Crea il symlink relativo nella directory corrente
        char rel_path[PATH_MAX];
        snprintf(rel_path, sizeof(rel_path), "%s/%s", ARCHIVE_DIR, name);
        if (symlink(rel_path, name) == -1) {
            perror("symlink");
        }
    }

    closedir(dir);
    return 0;
}