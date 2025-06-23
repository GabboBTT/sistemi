#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#define ARCHIVE_DIR "archive"
#define PATH_MAX 4096

/*
    Esercizio 2: Linguaggio C: 10 punti
    (undo dell'esercizio 1) Scrivere un programma che sostituisca tutti i link simbolici presenti nella
    directory corrente che puntano a .../nomefile con i veri file che l'esercizio 1 aveva spostato nella
    directory tre punti. Usare la system call rename per fare la sostituzione in modo atomico (in nessun
    istante il file deve risultare inesistente).
*/

// Controlla se il link simbolico punta a un file nella directory "archive"
int is_symlink_to_archive(const char *path, char *target_buf, size_t bufsize) {
    ssize_t len = readlink(path, target_buf, bufsize - 1); // Legge il link simbolico
    if (len == -1) return 0; 
    target_buf[len] = '\0'; // Termina la stringa
    return (strncmp(target_buf, "archive/", 8) == 0); // Controlla se il link punta a "archive/"
}

int main(){
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    char link_target[PATH_MAX]; // Path del file che i link devono puntare

    while ((entry = readdir(dir)) != NULL) {
        // Continua solo se è un link simbolico
        struct stat sb;
        if (lstat(entry->d_name, &sb) == -1) {
            perror("lstat");
            continue;
        }
        if (!S_ISLNK(sb.st_mode)) continue;


        // Costruisce il percorso del link simbolico della directory corrente
        if (!is_symlink_to_archive(entry->d_name, link_target, sizeof(link_target))) continue;

        // Calcola path src da dove il link simbolico punta
        char src_path[PATH_MAX];
        snprintf(src_path, sizeof(src_path), "%s", link_target);

        // Calcola path destinazione (nomefile) dove spostare il file
        char dst_path[PATH_MAX];
        snprintf(dst_path, sizeof(dst_path), "%s", entry->d_name);

        // Rimuovi il link simbolico
        if (unlink(dst_path) != 0) {
            perror("unlink");
            continue;
        }

        // Rinomina il file spostato nella directory "archive" al suo nome originale
        if (rename(src_path, dst_path) != 0) {
            perror("rename");
            continue;
        }

        printf("Ripristinato: %s\n", dst_path);
    }

    closedir(dir);
    return 0;
}