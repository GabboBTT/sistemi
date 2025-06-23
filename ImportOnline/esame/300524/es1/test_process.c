#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Processo di test in esecuzione. PID: %d\n", getpid());
    while (1) {
        sleep(1); // Mantiene il processo attivo
    }
    return 0;
}
