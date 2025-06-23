#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <limits.h>

const char *target_name;  // Nome del file da eseguire
char **target_args;       // Argomenti da passare al file eseguibile

void execute_in_directory(const char *path, const char *dir) {
    // Creo un nuovo processo
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        // Processo figlio: cambia working directory
        if (chdir(dir) != 0) {
            perror("chdir");
            exit(1);
        }

        // Costruisce il nome base del file da eseguire
        const char *base = strrchr(path, '/');
        base = base ? base + 1 : path;

        fprintf(stderr, "Eseguo '%s' in dir '%s' con args: ", base, dir);
        for (char **p = target_args; *p; ++p) {
            fprintf(stderr, "'%s' ", *p);
        }
        fprintf(stderr, "\n");

        // Esegue l'eseguibile con gli argomenti specificati
        execvp(base, target_args);

        // Se exec fallisce
        perror("execvp");
        exit(1);
    } else {
        // Processo padre: attende il figlio
        waitpid(pid, NULL, 0);
    }
}


void search_and_run(const char *dirpath) {
    DIR *D = opendir(dirpath);
    if (!D) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(D)) != NULL) {
        // Ignora le directory speciali "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) < 0) {
            perror("stat");
            continue;
        }

        // Se è una directory, ricorsivamente cerca al suo interno
        if (S_ISDIR(st.st_mode)) {
            search_and_run(fullpath);
            continue;
        }

        // Se è un file regolare, controlla se è eseguibile
        else if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) {
            // Ricavo il nome dopo l'ultimo /
            const char *base = strrchr(fullpath, '/');
            base = base ? base + 1 : fullpath;

            // Se il nome è quello cercato
            if (strcmp(base, target_name) == 0) {

                // Crea una copia del path completo per poter modificare la stringa (fullpath è const)
                char dircopy[PATH_MAX];
                strncpy(dircopy, fullpath, sizeof(dircopy));

                // Trova l'ultima '/' nel path copiato per isolare la directory dal nome del file
                char *last_slash = strrchr(dircopy, '/');

                // Se esiste almeno un /, allora tronca la stringa subito prima del nome file → resta solo la directory
                if (last_slash) {
                    *last_slash = '\0';  // Termina prima del nome file

                // Se non ho /, vuol dire che è solo nome e quindi questa directory
                } else {
                    strcpy(dircopy, ".");  // Il file è nella dir corrente
                }
                execute_in_directory(fullpath, dircopy);
            }
        }
        
    }

    closedir(D);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program_name> [args...]\n", argv[0]);
        return 1;
    }

    target_name = argv[1];

    // Calcolo il numero di argomenti da passare (compreso target_name, escluso argv[0])
    int num_args = argc - 1;
    target_args = malloc((num_args + 1) * sizeof(char *));
    if (!target_args) {
        perror("malloc");
        return 1;
    }

    // Copio tutti gli argomenti da argv[1] in poi
    for (int i = 0; i < num_args; ++i)
        target_args[i] = argv[i + 1];
    target_args[num_args] = NULL;

    search_and_run(".");

    free(target_args);
    return 0;
}