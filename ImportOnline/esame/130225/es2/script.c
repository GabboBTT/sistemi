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

/*
    Confronta un file target con tutti i file regolari presenti nella directory (e sotto-directory)
    - Se `prefix_mode` è attivo, confronta solo i primi `prefix_len` byte
    - Altrimenti confronta l'intero contenuto dei file
*/
void compare_files(const char *target_file, const char *dir_path, int prefix_mode, size_t prefix_len) {
    FILE *f1 = fopen(target_file, "rb");
    if (!f1) {
        perror("Errore apertura file target");
        return;
    }

    char buffer_target[MAX_PATH];
    size_t size_target;

    // Attivo quindi leggiamo solo i primi `prefix_len` byte
    if (prefix_len){
        size_target = fread(buffer_target, 1, prefix_len, f1);
    
    // Leggiamo tutto il file
    } else {
        fseek(f1, 0, SEEK_END);  // Sposta il puntatore alla fine del file
        size_target = ftell(f1); // Ottiene la dimensione del file
        fseek(f1, 0, SEEK_SET);  // Riporta il puntatore all'inizio del file
        fread(buffer_target, 1, size_target, f1);  // Legge tutto il file
    }

    fclose(f1);

    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Errore apertura directory");
        return;
    }

        struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignora "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Costruisce il percorso assoluto del file o directory
        char path[MAX_PATH];
        snprintf(path, MAX_PATH, "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(path, &st) == -1) continue;

        // Se è una directory, chiama ricorsivamente su di essa
        if (S_ISDIR(st.st_mode)) {
            compare_files(target_file, path, prefix_mode, prefix_len);
        }

        // Se è un file regolare, lo confronto con il file target
        else if (S_ISREG(st.st_mode)) {
            FILE *f2 = fopen(path, "rb");
            if (!f2) continue;

            char buffer_other[MAX_PATH];
            size_t size_other;

            if (prefix_mode) {
                // Modalità prefisso: leggo solo `prefix_len` byte
                size_other = fread(buffer_other, 1, prefix_len, f2);

                // Se hanno stessa dimensione e contenuto iniziale uguale, stampo match
                if (size_other == size_target && memcmp(buffer_target, buffer_other, size_other) == 0) {
                    printf("File con prefisso uguale: %s\n", path);
                }
            } else {
                fseek(f2, 0, SEEK_END);  // Sposta il puntatore alla fine del file
                size_other = ftell(f2); // Ottiene la dimensione del file
                rewind(f2);  // Riporta il puntatore all'inizio del file
                if (size_other == size_target) {
                    fread(buffer_other, 1, size_other, f2);  // Legge tutto il file
                    // Se hanno stessa dimensione e contenuto uguale, stampo match
                    if (memcmp(buffer_target, buffer_other, size_other) == 0) {
                        printf("File con contenuto uguale: %s\n", path);
                    }
                }
            }
            fclose(f2);  // Chiude il file dopo la lettura
        }
    }
    closedir(dir);  // Chiude la directory dopo aver finito di leggere
}

int main(int argc, char *argv[]) { 

    // Controllo che ci siano gli aromenti necessari, se no gli scrivo cosa deve fare ed esco
    if (argc < 3) {
        fprintf(stderr, "Uso: %s [-s|-l|-p <N>] <file> <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *target_file;
    const char *start_dir;

   // Casi dell'esercizio 1
   if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-l") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Uso: %s [-s|-l] <file> <directory>\n", argv[0]);
            return EXIT_FAILURE;
        }
        target_file = argv[2];
        start_dir = argv[3];

        struct stat st;
        if (stat(target_file, &st) != 0) {
            perror("Errore stat file");
            return EXIT_FAILURE;
        }

        if (strcmp(argv[1], "-s") == 0) {
            search_links(target_file, start_dir, 1, 0);
        } else {
            search_links(target_file, start_dir, 0, st.st_ino);
        }

   // Genera elenco di file nella directory selezionata che hanno come contenuto i primi n byte specificati del file indicato
   } else if (strcmp(argv[1], "-p") == 0 && argc == 5) {
        int prefix_len = atoi(argv[2]);
        if (prefix_len <= 0) {
            fprintf(stderr, "La lunghezza deve essere un numero positivo.\n");
            return EXIT_FAILURE;
        }
        target_file = argv[3];
        start_dir = argv[4];
        compare_files(target_file, start_dir, 1, prefix_len);

    // Genera l'elenco di tutti i file che hanno lo stesso contenuto del file originale
    } else if (argc ==3) {
        target_file = argv[1];
        start_dir = argv[2];
        compare_files(target_file, start_dir, 0, 0);

    // Errore di utilizzo
    } else {
        fprintf(stderr, "Uso: %s [-s|-l] <file> <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}