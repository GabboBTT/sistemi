#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_PATH 4096

/* 
  - Se symbolic_mode = 1 allora cerca i link simbolici, altrimenti cerca i file con lo stesso inode
  - Se target_inode = 0 allora cerca i link simbolici, altrimenti cerca i
*/
void search_links(const char *target_file, const char *dir_path, int symbolic_mode, ino_t target_inode){ 
    DIR *dir = opendir(dir_path); // Apre la dir e gli associa il puntatore
    if (!dir) return;
    
    // Puntatore a una struttura che rappresenta un file o directory dentro la cartella
    struct dirent *entry;

    // Scorre tutti i file e cartelle all'interno della directory aperta precedentemente
    while ((entry = readdir(dir)) != NULL) {
        // Salta "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;    
        
        // Costruisce il path assoluto del file corrente: path = dir_path + "/" + nome_file
        char full_path[MAX_PATH]; 
        snprintf(full_path, MAX_PATH, "%s/%s", dir_path, entry->d_name);
        
        // Usa stat per ottenere info sul file
        struct stat st;
        
        /* Caso in cui cerchiamo link simbolici */
        if (symbolic_mode){
            // Controlla se il file corrente e' un link simbolico, se lo e, allora lo stampa
            if (lstat(full_path, &st) == 0 && S_ISLNK(st.st_mode)) {
                char resolved[MAX_PATH]; // Buffer per il percorso risolto
                // Legge il link simbolico e lo mette nel buffer resolved
                ssize_t len = readlink(full_path, resolved, sizeof(resolved) - 1);

                // Se la lettura è valida 
                if (len != -1) {
                    resolved[len] = '\0'; // Termina la stringa

                    char abs_target[MAX_PATH];
                    realpath(target_file, abs_target);

                    char resolved_full[MAX_PATH];
                    // Costruisce il percorso assoluto corretto del target risolto
                    if (snprintf(resolved_full, MAX_PATH, "%s/%s", dir_path, resolved) >= MAX_PATH) {
                        fprintf(stderr, "Percorso troppo lungo, ignorato: %s/%s\n", dir_path, resolved);
                        continue;
                    }

                    char abs_resolved[MAX_PATH];
                    if (realpath(resolved_full, abs_resolved)) {
                        if (strcmp(abs_resolved, abs_target) == 0) {
                            printf("Link simbolico trovato: %s -> %s\n", full_path, abs_resolved);
                        }
                    }
                }
            }
        }

        /* Caso in cui cerchiami link fisici */
        else {
            if (lstat(full_path, &st) == 0 && S_ISREG(st.st_mode) && !S_ISLNK(st.st_mode)) {
                if (st.st_ino == target_inode) {
                    printf("Link fisico: %s\n", full_path);
                }
            }
        }

        // Se il file corrente e' una directory, chiama ricorsivamente la stessa funzione per esplorarl
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            // Se e' una directory, chiama ricorsivamente la stessa funzione per esplorarla
            search_links(target_file, full_path, symbolic_mode, target_inode);
        }
    }

    closedir(dir); // Chiude la directory
}

int main(int argc, char *argv[]) { 

    // Controllo che ci siano gli aromenti necessari, se no gli scrivo cosa deve fare ed esco
    if (argc != 4) {
        fprintf(stderr, "Uso: %s [-s|-l] <file> <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *option = argv[1];
    const char *target_file = argv[2];
    const char *start_dir = argv[3];

    // Verifico info sul file e se non esiste, allora errore di permessi probabile
    struct stat st;
    if (stat(target_file, &st) != 0) {
        perror("Errore stat file");
        return EXIT_FAILURE;
    }

    if (strcmp(option, "-s") == 0) {
        search_links(target_file, start_dir, 1, 0);
    } else if (strcmp(option, "-l") == 0) {
        search_links(target_file, start_dir, 0, st.st_ino);
    } else {
        fprintf(stderr, "Opzione non valida. Usa -s o -l.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}