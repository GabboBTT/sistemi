#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

extern char **environ; // Accesso all'ambiente globale

/*
    Esercizio 2: Linguaggio C: 10 punti
    Scrivere una estensione del programma dell'esercizio1 cloneproc+ in grado di clonare anche la
    directory corrente e l'ambiente (environment) .
    scrivere inoltre un semplice programma che ne dimostri il funzionamento.
*/

void clone_process_plus(pid_t pid) {
    char exe_path[4096]; // Buffer per il percorso dell'eseguibile
    char cmdline_path[4096]; // Buffer per il percorso del file cmdline
    char exe[4096]; // Buffer per il nome dell'eseguibile
    char cmdline[4096]; // Buffer per gli argomenti del comando
    char *argv[128]; // Array per gli argomenti del comando
    int argc = 0; // Contatore per gli argomenti
    char cwd[4096]; // Buffer per la directory corrente

    // Costruisce i percorsi per /proc/pid/exe e /proc/pid/cmdline
    snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);
    snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", pid);

    // Legge il percorso dell'eseguibile
    ssize_t len = readlink(exe_path, exe, sizeof(exe) - 1);
    if (len < 0) {
        perror("readlink");
        exit(1);
    }
    exe[len] = '\0';

    // Legge gli argomenti del comando
    FILE *cmdline_file = fopen(cmdline_path, "r");
    if (!cmdline_file) {
        perror("fopen");
        exit(1);
    }

    len = fread(cmdline, 1, sizeof(cmdline) - 1, cmdline_file);
    fclose(cmdline_file);
    if (len <= 0) {
        perror("fread");
        exit(1);
    }
    cmdline[len] = '\0';

    // Divide gli argomenti in un array
    char *token = cmdline;
    while (token < cmdline + len) {
        argv[argc++] = token;
        token += strlen(token) + 1;
    }
    argv[argc] = NULL;

    // Ottiene la directory corrente
    if (!getcwd(cwd, sizeof(cwd))) { 
        perror("getcwd");
        exit(1);
    }

    // Cambia la directory corrente al processo clonato
    if (chdir(cwd) < 0) {
        perror("chdir");
        exit(1);
    }

    // Esegue il processo clonato con l'ambiente
    execve(exe, argv, environ);

    // Se execve fallisce
    perror("execve");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Converti il pid da stringa a intero
    pid_t pid = atoi(argv[1]);
    clone_process_plus(pid);

    return 0;
}