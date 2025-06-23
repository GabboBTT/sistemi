#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#define MAX_PATH 4096
/*
    Esercizio 2: Linguaggio C: 10 punti Scrivere un programma che presi come parametri i pathname di
    un file f e di una directory d stampi l'elenco dei link simbolici che puntano a f presenti nel sottoalbero
    del file system generato dalla directory d.
*/

void search_symlinks(const char *target_file, const char *dir_path, const char *abs_target) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Errore apertura directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Salta "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Costruisce il percorso assoluto del file corrente
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (lstat(full_path, &st) == -1) {
            perror("Errore stat file");
            continue;
        }

        // Se è un link simbolico
        if (S_ISLNK(st.st_mode)) {
            char resolved[MAX_PATH];
            ssize_t len = readlink(full_path, resolved, sizeof(resolved) - 1);
            if (len != -1) {
                resolved[len] = '\0'; // Termina la stringa

                char abs_resolved[MAX_PATH];
                if (realpath(resolved, abs_resolved)) {
                    if (strcmp(abs_resolved, abs_target) == 0) {
                        printf("Link simbolico trovato: %s -> %s\n", full_path, abs_resolved);
                    }
                }
            }
        } else if (S_ISDIR(st.st_mode)) {
            // Se è una directory, chiama ricorsivamente su di essa
            search_symlinks(target_file, full_path, abs_target);
        }
    }
    closedir(dir); // Chiude la directory
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *target_file = argv[1];
    const char *start_dir = argv[2];

    char abs_target[MAX_PATH];
    // Ottiene il percorso assoluto del file target
    if (realpath(target_file, abs_target) == NULL) {
        perror("Errore nel risolvere il percorso assoluto del file target");
        return EXIT_FAILURE;
    }

    // Chiama la funzione per cercare file con stessa dimensione ma inode diverso
    search_symlinks(target_file, start_dir, abs_target);

    return EXIT_SUCCESS;
}