/*
    Esercizio 1: Linguaggio C (obbligatorio) 20 punti
    Scrivere un programma cloneproc dato il pid di un processo passato come unico parametro, è in
    grado di eseguirne una copia. (deve rieseguire lo stesso file con lo stresso argv.
    consiglio: cercare in /proc/pid/exe e /proc/pid/cmdline le informazioni necessarie (dove pid è il numero
    di processo.
    scrivere inoltre un semplice programma che ne dimostri il funzionamento
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

void clone_process(pid_t pid) {
    char exe_path[4096]; // Buffer per il percorso dell'eseguibile
    char cmdline_path[4096]; // Buffer per il percorso del file cmdline
    char exe[4096]; // Buffer per il nome dell'eseguibile
    char cmdline[4096]; // Buffer per gli argomenti del comando
    char *argv[128]; // Array per gli argomenti del comando
    int argc = 0; // Contatore per gli argomenti

    // Costruisce i percorsi per /proc/pid/exe e /proc/pid/cmdline
    snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);
    snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", pid);

    // Legge il percorso dell'eseguibile
    ssize_t len = readlink(exe_path, exe, sizeof(exe) - 1);  // Legge il link simbolico di /proc/pid/exe, per ottenere il percorso dell'eseguibile
    if (len < 0) {
        perror("readlink");
        exit(1);
    }
    exe[len] = '\0';

    // Legge gli argomenti del comando
    FILE *cmdline_file = fopen(cmdline_path, "r"); // Apre il file cmdline
    if (!cmdline_file) {
        perror("fopen");
        exit(1);
    }

    // Inizializza il buffer cmdline e lo legge dal file
    len = fread(cmdline, 1, sizeof(cmdline) - 1, cmdline_file); // Legge il contenuto del file cmdline
    fclose(cmdline_file); // Chiude il file cmdline
    if (len < 0) {
        perror("fread");
        exit(1);
    }
    cmdline[len] = '\0';

    // Divide gli argomenti in un array
    char *token = cmdline;
    while (token < cmdline + len) { // Scorre il buffer cmdline
        argv[argc++] = token;
        token += strlen(token) + 1; // Avanza al prossimo argomento (i campi sono separati da null terminator)
    }
    argv[argc] = NULL; // Termina l'array con NULL

    execv(exe, argv); // Esegue il processo clonato

    perror("execv"); // Se execv fallisce, stampa l'errore
    exit(1); // Esce con errore
}

int main(int argc, char *argv[]) {
    if ( argc != 2) {
        fprintf(stderr, "Uso: %s <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Converti il pid da stringa a intero
    pid_t pid = atoi(argv[1]);
    clone_process(pid);

    return 0;
}