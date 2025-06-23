#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/wait.h>
#include <sys/timerfd.h>
#include <signal.h>
#include <stdint.h>  // uint64_t

/*
    Scrivere un programma timeout che esegua un programma e lo termini se supera una durata
    massima prefissata. timeout ha almeno due argomenti: il primo è la durata massima in millisecondi, i
    parametri dal secondo in poi sono il programma da lanciare coi rispettivi argomenti.
    Es:
    timeout 5000 sleep 2
    temina in due secondi (sleep termina in tempo).
    timeout 3000 sleep 5
    passati tre secondi il programma sleep viene terminato.
    Tmeout deve essere scritto usando le system call poll, pidfd_open, timerfd*.
*/

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <timeout_ms> <comando> [argomenti...]\n", argv[0]);
        return 1;
    }

    // atoi converte la stringa in un intero, che sara' il timeout in millisecondi
    int timeout_ms = atoi(argv[1]);
    if (timeout_ms <= 0) {
        fprintf(stderr, "Durata non valida: %s\n", argv[1]);
        return 1;
    }

    // Crea un timer file descriptor, che permette di gestire il timeout
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd == -1) {
        perror("timerfd_create");
        return 1;
    }

    struct itimerspec timer; // Struttura per definire il timer
    timer.it_value.tv_sec = timeout_ms / 1000; // Secondi
    timer.it_value.tv_nsec = (timeout_ms % 1000) * 1000000; // Nanosecondi
    timer.it_interval.tv_sec = 0; // Non ripetere il timer
    timer.it_interval.tv_nsec = 0; // Non ripetere il timer

    // Imposta il timer con il tempo specificato
    if (timerfd_settime(tfd, 0, &timer, NULL) == -1) {
        perror("timerfd_settime");
        return 1;
    }

    // Crea un processo figlio per eseguire il comando specificato, perchè il processo padre deve poter 
    // monitorare il timeout e la fork
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execvp(argv[2], &argv[2]);
        perror("execvp");
        exit(1);
    }

    struct pollfd pfds[1];
    pfds[0].fd = tfd;
    pfds[0].events = POLLIN;

    while (1) {
        int ret = poll(pfds, 1, 100);  // 100ms
        if (ret == -1) {
            perror("poll");
            kill(pid, SIGKILL);
            break;
        }

        int status;
        pid_t done = waitpid(pid, &status, WNOHANG);
        if (done == pid) {
            printf("Processo terminato in tempo.\n");
            break;
        }

        if (pfds[0].revents & POLLIN) {
            uint64_t exp;
            read(tfd, &exp, sizeof(exp));
            printf("Tempo scaduto! Uccido il processo.\n");
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            break;
        }
    }

    close(tfd);
    return 0;
}
