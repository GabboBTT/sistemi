#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>      // per opendir, readdir, closedir
#include <sys/stat.h>    // per stat, lstat
#include <unistd.h>      // per lstat
#include <sys/types.h>   // per tipi come ino_t

#define MAX_PATH 4096 

/*
    Esercizio 1: Linguaggio C (obbligatorio) 20 punti Scrivere il programma samecont che presi come
    parametri i pathname di un file f e di una directory d stampi l'elenco dei file che hanno la stessa
    ampiezza (numero di byte) di f ma non sono link fisici di f presenti nel sottoalbero del file system
    generato dalla directory d
*/

void search_same_size(const char *dir_path, size_t target_size, ino_t target_inode) {
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

        // Costruisce il path assoluto del file corrente: path = dir_path + "/" + nome_file
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", dir_path, entry->d_name);

        struct stat st;
        // Usa lstat per ottenere info sul file, in modo da poter gestire i link
        if (lstat(full_path, &st) == -1) {
            perror("Errore stat file");
            continue;
        }

        // Se il file è regolare e ha la stessa dimensione ma inode diverso
        if (S_ISDIR(st.st_mode)) {
            // Se è una directory, chiama ricorsivamente su di essa
            search_same_size(full_path, target_size, target_inode);
        } else if (S_ISREG(st.st_mode)) {
            // Verifica dimensione e inode
            if (st.st_size == target_size && st.st_ino != target_inode) {
                printf("%s\n", full_path);
            }
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

    /* 
        Stat sul file di riferimento per ottenere size e inode, scelgo inode, perchè devo trovare quelli 
        che hanno inode diverso cercando non i link fisici:
            - st.st_size --> dimensione del file in byte
            - st.st_ino --> inode del file, che deve essere diverso da quello del file
    */
    struct stat st;
    if (stat(target_file, &st) != 0) {
        perror("Errore stat file");
        return EXIT_FAILURE;
    }

    // Chiama la funzione per cercare file con stessa dimensione ma inode diverso
    search_same_size(start_dir, st.st_size, st.st_ino);

    return EXIT_SUCCESS;
}