#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
    char cwd[4096];
    getcwd(cwd, sizeof(cwd));
    printf("PID: %d\n", getpid());
    printf("CWD: %s\n", cwd);
    printf("ARGV:\n");
    for (int i = 0; argv[i]; ++i)
        printf("  argv[%d] = %s\n", i, argv[i]);
    printf("ENV:\n");
    for (int i = 0; envp[i]; ++i)
        printf("  %s\n", envp[i]);
    pause(); // lascia il processo vivo per testarlo
    return 0;
}
