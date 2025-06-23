
Prova pratica - es 1 del 14/02/2024

#include <stdio.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define BUFSIZE sizeof(char) * 256

void dirproc(char* path, char* filter){
    DIR * dirstream = opendir(path);//apri cartella
    struct dirent* entryiterator;
    while((entryiterator = readdir(dirstream))){
        struct stat statbuffer;
        char filename[BUFSIZE];
        strcpy(filename, path);//costruisci pathname della directory entry
        strcat(filename, "/");
        strcat(filename, entryiterator->d_name);
        stat(filename, &statbuffer); //consider fstat, file descriptors should be easier to work with :)
        if(strcmp(entryiterator->d_name, filter) == 0) printf("%s\n", entryiterator->d_name); //se è il nome che cerchiamo stampa (manca check executable)
        if(((statbuffer.st_mode & S_IFMT) == S_IFDIR) && (strcmp(entryiterator->d_name, ".") != 0) && (strcmp(entryiterator->d_name, "..") != 0)){ //masking per bit relativi al tipo di file con S_IFMT, se uguale a IFDIR allora è una directory, verifico poi non sia . o ..
            dirproc(filename, filter); //directory diversa dal parent e dall'attuale? allora apro
        }
    }
    closedir(dirstream);
}

int main(int argc, char* argv[]){
    if(argc == 1){
        printf("too few arguments! given: 0, needed: 1\n");
        return 1;
    }
    if(argc > 2){
        printf("too many arguments! given: %d, needed: 1\n", argc-1);
        return 1;
    }

    //get cwd
    char cwd[BUFSIZE]; //per pathnames meglio PATH_MAX
    getcwd(cwd, BUFSIZE);
    
    dirproc(cwd, argv[1]);

    return 0;
}



Prova pratica - es 3 del 14/02/2024
scandire la cartella
eseguire il comando file
directory per catalogare --> per ogni tipo mettiamo il file nella directory
o meglio vocabolario di liste
prettyprinting ordine delle chiavi, e file dello stesso tipo
in python si usa os.walk per navigare l'intero albero, si puo` usare per un solo livello senza entrare in ricorsione
https://docs.python.org/3/library/os.html
subprocess.run esegue un comando e ne prende l'output




https://www.cs.unibo.it/~renzo/so/.01.19.pdf  pratico
Esercizio 1
Scrivere il programma stdin2pipe che prenda in input due righe di testo, ogni riga contiene un comando coi rispettivi parametri. Il programma deve lanciare entrambi i comandi in modo tale l'output del primo diventi input del secondo. Per esempio, dato il file cmds: ls -l awk '{print $1}' l'esecuzione di: stdin2pipe < cmds sia equivalente al comando: ls -l | awk '{print $1}'

questo non funziona perché provo a usare execvp e per farlo devo passare gli args, che divido con uno ' ' e quindi divide 'print $1'. Come posso fare?

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  char command1[100], command2[100];
  FILE *pipe1, *pipe2;

  // Leggi i due comandi da stdin
  fgets(command1, sizeof(command1), stdin);
  fgets(command2, sizeof(command2), stdin);

  // Rimuovi il carattere newline alla fine dei comandi
  command1[strcspn(command1, "\n")] = '\0';
  command2[strcspn(command2, "\n")] = '\0';

  printf("%s | %s\n", command1, command2);
  int pipefd[2];
  pipe(pipefd);

  if (fork() == 0) {
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);

    char *args[20];
    char *token = strtok(command2, " ");
    int i = 0;
    while (token != NULL) {
      args[i++] = token;
      token = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Execute the command
    // execvp(args[0], args);
    char *args1[] = {"awk", "{print $1}", NULL};
    execvp(args1[0], args1);
    perror("command2");
  } else {

    close(pipefd[0]);

    // Collega stdout all'estremità di scrittura della pipe
    dup2(pipefd[1], STDOUT_FILENO);

    char *args[20];
    char *token = strtok(command1, " ");
    int i = 0;
    while (token != NULL) {
      args[i++] = token;
      token = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Execute the command
    execvp(args[0], args);
    perror("command1");
  }

  return 0;
}


https://www.cs.unibo.it/~renzo/so/pratiche/2023.02.16.pdf  -- pratico
Esercizio 1: Linguaggio C (obbligatorio) 20 punti Fare un programma di copia parallelo di file. pcp file1 file2 pcp deve creare due processi figli; mentre un processo copia la prima meta’ del file, l'altro deve copiare l’altra meta’. Esercizio 2: Linguaggio C: 10 punti usando “getopt” consentire di scegliere il grado di parallelismo voluto: pcp -j 8 file1 file2 deve creare 8 processi, ogni processo copia 1/8 del file

qua invece non riesco a scrivere in ordine

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

void child_process(int fd_in, int fd_out, off_t offset, off_t length) {
  char buffer[BUFFER_SIZE];

  // Imposta l'offset nel file
  lseek(fd_out, offset, SEEK_SET);
  lseek(fd_in, offset, SEEK_SET);

  // Legge i dati e li scrive nel file di output
  unsigned long int done = 0;
  while (done + BUFFER_SIZE < length) {
    read(fd_in, buffer, BUFFER_SIZE);
    write(fd_out, buffer, BUFFER_SIZE);
    done += BUFFER_SIZE;
  }
  read(fd_in, buffer, length - done);
  printf("len: %d\n", length - done);
  printf("%d stampati \n", write(fd_out, buffer, length - done));
}

int main(int argc, char *argv[]) {

  int num_processes = 2; // Default
  int opt;

  while ((opt = getopt(argc, argv, "j:")) != -1) {
    switch (opt) {
    case 'j':
      num_processes = atoi(optarg);
      break;
    default:
      fprintf(
          stderr,
          "Uso: %s -j <numero_processi> <file_origine> <file_destinazione>\n",
          argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (optind + 2 != argc) {
    fprintf(stderr,
            "Uso: %s -j <numero_processi> <file_origine> <file_destinazione>\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  char *file_in = argv[optind];
  char *file_out = argv[optind + 1];

  // Apertura del file in lettura
  int fd_in = open(file_in, O_RDONLY);

  // Apertura del file in scrittura (creazione se non esiste)
  int fd_out = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

  off_t file_size = lseek(fd_in, 0, SEEK_END);

  // Calcola la dimensione della metà del file
  off_t chunk_size = file_size / num_processes;
  off_t remaining_bytes = file_size % num_processes;
  off_t offset = 0;

  // Creazione dei processi figli
  for (int i = 0; i < num_processes; i++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) { // Codice eseguito dal processo figlio
      off_t length =
          chunk_size + (i == num_processes - 1 ? remaining_bytes : 0);
      printf("Child %d, offest %d, len %d\n", i, offset, length);
      child_process(fd_in, fd_out, offset, length);
      exit(EXIT_SUCCESS);
    } else { // Codice eseguito dal processo padre
      offset += chunk_size;
    }
  }

  // Codice eseguito dal processo padre
  // Attende la terminazione dei processi figli
  for (int i = 0; i < num_processes; i++) {
    wait(NULL);
  }

  // Chiude i file
  close(fd_in);

  close(fd_out);

  printf("Copia completata con successo.\n");

  return 0;
}



20240119 - pratico
#!/bin/python3
"""
Scrivere un programma python o uno script bash chiamato tcmp che confronti gli alberi del file
system di due directory.. A seconda del parametro deve elencare i pathname di file e di directory che
• sono comuni ad entrambi i sottoalberi, se manca il parametro
• esistono solo nel primo sottoalbero, se il parametro è -1
• esistono solo nel secondo sottoalbero se il parametro è -2
esempi:
$ ./tcmp dir1 dir2
stampa l'elenco dei path che esistono sia in dir1 sia in dir2
$ ./tmcp -1 dir1 dir2
stampa l'elenco dei path che esistono in dir1 ma non in dir2
"""

import os
import sys

def get_sub_path(current_path):
    paths = []
    for root, dirs, files in os.walk(current_path):
        for name in files:
            #print(os.path.abspath(os.path.join(root, name)))
            paths.append(os.path.abspath(os.path.join(root, name)))

    return paths
            


if __name__ == "__main__":
    print(f"Arguments count: {len(sys.argv)}")
    for i, arg in enumerate(sys.argv):
        print(f"Argument {i:>6}: {arg}")

    cwd = os.getcwd()
    list_dir1 = get_sub_path(cwd + f'/{sys.argv[2]}')
    list_dir2 = get_sub_path(cwd + f'/{sys.argv[3]}')
    
    if(sys.argv[1] == '-1'):
        for path in list_dir1:
            if path not in list_dir2:
                print(path)
    elif(sys.argv[1] == '-2'):
        for path in list_dir2:
            if path not in list_dir1:
                print(path)
    else:
        for path in list_dir2:
            if path in list_dir1:
                print(path)


ESERCIZIO 3 - Prova 2022.09.07 - Pratica
questa soluzione risulta problematica quando i nomi dei file contengono whitespace o ':'
vedere ls --zero + pipe in xargs -0 per aggirare il problema del whitespace
if [ $# -eq 0 ]; then
    echo "noargs"
else
    elfsize=0
    for arg in "$@"
    do
        for filename in $(ls $arg)
        do
            type=$(file $arg/$filename | cut -d : -f 2-2)
            if [[ $type =~ 'ELF' ]]; then
                cursize=$(wc -c <"$arg/$filename")
                elfsize=$((elfsize+cursize))
            fi
        done
    done
    echo $elfsize
fi

ESERCIZIO 2

#include <bits/posix_opt.h>
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]){
    char** args = ++argv;
    
    while(1){
        int execpid = fork();
        if(execpid == 0){ //child instructions
            execvp(args[0], args);
        }
        //parent instructions
        struct timespec start, end;
        int exitstatus;
        
        clock_gettime(CLOCK_REALTIME, &start);

        wait(&exitstatus);
        
        clock_gettime(CLOCK_REALTIME, &end);
        
        if(!(end.tv_sec - start.tv_sec)){
            return 0; //technically successful execution
        }
        if(exitstatus != 0){
            return exitstatus;
        }
    }
    return 0;
}





    
    
20230119 - https://cs.unibo.it/~renzo/so/pratiche/2023.01.19.pdf
es2 - pratico
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_COMMANDS 10
#define MAX_BUFFERS 10

void execute_commands(int num_commands, char *commands[], int num_buffers, char *buffers[]) {
    FILE *pipes[MAX_COMMANDS];
    int pipefd[2 * (num_commands - 1)];
    pid_t pid;

    // Create the necessary pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipefd + i * 2) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process

            // Redirect the input from the previous pipe (if not the first command)
            if (i > 0) {
                if (dup2(pipefd[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Redirect the output to the next pipe (if not the last command)
            if (i < num_commands - 1) {
                if (dup2(pipefd[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefd[j]);
            }

            // Execute the command
            execl("/bin/sh", "sh", "-c", commands[i], (char *) NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipe file descriptors in the parent
    for (int i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipefd[i]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}

int main() {
    char *commands[MAX_COMMANDS];
    char command_line[MAX_COMMAND_LENGTH];
    int num_commands = 0;

    // Read commands from stdin
    while (fgets(command_line, MAX_COMMAND_LENGTH, stdin) != NULL && num_commands < MAX_COMMANDS) {
        command_line[strcspn(command_line, "\n")] = '\0';  // Remove newline character
        commands[num_commands] = strdup(command_line);  // Duplicate the command string
        num_commands++;
    }

    // Execute the commands in a pipeline
    execute_commands(num_commands, commands, 0, NULL);

    // Free the allocated memory for the commands
    for (int i = 0; i < num_commands; i++) {
        free(commands[i]);
    }

    return 0;
}



-----------------------------------------------------------------------------------------------------------------------


20230216
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

sem_t file_sem;

#define MAXBUF 2048
int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
    exit(1);
  }

  FILE *f1 = fopen(argv[1], "r");
  FILE *f2 = fopen(argv[2], "w");

  unsigned numprocs = 2;

  // Initialize the semaphore with a value of 1 (binary semaphore)
  sem_init(&file_sem, 0, 1);

  if (f1 == NULL || f2 == NULL)
  {
    fprintf(stderr, "Error opening files\n");
    exit(1);
  }

  fseek(f1, 0, SEEK_END);
  long fsize = ftell(f1);

  fseek(f1, 0, SEEK_SET);

  char *buffer = malloc(fsize / numprocs);

  int pid = fork();
  if (pid == 0){
    sem_wait(&file_sem);
    fread(buffer, 1, fsize / numprocs, f1);
    fwrite(buffer, 1, fsize / numprocs, f2);
    sem_post(&file_sem);
    exit(0);
  } else if (pid > 0) {
    // parent
    sem_wait(&file_sem);
    fseek(f1, fsize / numprocs, SEEK_SET);
    fread(buffer, 1, fsize / numprocs, f1);
    fwrite(buffer, 1, fsize / numprocs, f2);
    sem_post(&file_sem);
  } else {
    fprintf(stderr, "Error forking\n");
    exit(1);
  }

  int status;
  pid_t child_pid = wait(&status);  // Wait for the child process to finish

  if (child_pid == -1) {
    perror("wait");
    return 1;
  }

  fclose(f1);
  fclose(f2);
  free(buffer);
  sem_destroy(&file_sem);
  
  return EXIT_SUCCESS;
}

   
# 2020 02 21 esercizio pratico 1

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limiti.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]) {
        if (argc != 2) {
                printf("Utilizzo: abslink [nome-file-link simbolico]");
                ritorno 1;
        }
        
        struct stat statbuf;
        
        
        int ret_stat = lstat(argv[1], &statbuf);
        se (ret_stat != 0) {
                printf("ERRORE: ricevuto errno %d\n", errno);
                ritorno 1;
        }
        
        printf("modalità file %d\n", statbuf.st_mode);
        
        if ((statbuf.st_mode & S_IFLNK) != S_IFLNK) {
                perror("il file non è un collegamento simbolico");
                ritorno 1;
        }
        
        char *abspath = realpath(argv[1], NULL);
        
        printf("trovato percorso %s\n", abspath);
        
        unlink(argv[1]);
        symlink(argv[1], abspath);
        int wstato;
        
         //ci sono due syscall: unlink per rm, symlink per ln -s
         //NON USARE COMANDI AL POSTO DELLE SYSTEMCALL ! 
        /*se (fork()) {
                execlp("rm", "rm", argv[1], NULL);
        } else wait(&wstatus);
       
        se (fork()) {
                // crea un collegamento simbolico e forza la creazione
                execlp("ln", "ln", "-s", abspath, argv[1], NULL); 
        } altro {
                aspetta(&wstato); // finché i figli non terminano
                printf("figli terminati\n");
        }*/
        
        libero(aspatico);
        
        ritorno 0;
}

Domanda: il modo corretto di distinguere una libreria condivisa da un eseguibile (2021-07-15 es2)
proviamo l'exec

//file "lancia.c"
#include <stdio.h> //printf
#include <unistd.h> //dlopen, dlsym
#include <dlfnc.h>
#include <limits.h> //percorso reale
#include <stdlib.h>

//tipo di puntatore a funzione fatto come il principale
typedef int (*maintype) (int argc, char *argv[]);

int main(int argc, char *argv[] ) {
    char *abspath = realpath(argv[1], NULL);
    void *handle = dlopen(abspath, RTLD_NOW);
    printf("%p\n", handle);
    //alternativa (per stampare il puntatore conoscendo l'architettura)
    //printf("%llx \n", (unsigned long long) handle);
    if( handle == NULL) {
        return execv(argv[1], argv+1);
    } altro {
        maintype mymain = (void *) dlsym(handle, "main");
        printf("principale %p\n", mioprincipale);
        return miaprincipale(argv+1); 
    }
}

// teoria:
int f (carattere c); //funzione che prende come argomento un char e restituisce int
int *g (carattere c); //rispetto af restituisce un puntatore a int
int (*h) (carattere c); //h è un puntatore a funzione
h = f;
int *(*j) (carattere c); //j è un puntatore a funzione che restituisce un puntatore
j = sol;
typedef int (*maintype) (int argc, char *argv[]);
//maintype è un puntatore a funzione che restituisce un intero e prende come parametri (int argc, char argv[])


Esercizio c.1 2022 06 01

Monitorare il ritardo {
    int tempo;
    lista ordinata ListProc = nuova lista ordinata <int n, condizione c >;
    int procBloccati;

    ritardo () {
        tempo = 0;
        procBloccati = 0;
        ListProc.vuoto();
    }

    segno di spunta vuoto() {
        tempo+=1;
        ListProc.decrement(1);
        /* decrementa tutte le chiavi di 1 */

        procBloccati = ListProc.length();

        list procDaSbloccare = ListProc.returnAll(0);
        /* ritorna tutti i processi la cui chiave è 0,
         e li elimina dalla lista listaordinata */ 
                
        while (procDaSbloccare.isNotEmpty()) {
            s = procDaSbloccare.removeHead();
            scSIGNAL();
        }
    }

    int wait_tick(int nticks) {
        condizione c;
        ordine List.add(<nticks, c >);
        c.WAIT();
        return procBloccati;
    }
}

prova pratica 2022 02 17 esercizio 1:

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stringa a .h>

int main(int argc, char *argv[]){
        struct stat buf;
        lstat(argv[1], &buf);

        if(S_ISDIR(buf.st_mode)){
                char *p ercorso = percorso reale(argv[1], NULL);
                DIR *dir = opendir(percorso);
                printf("apertura cartella %s\n", percorso );

                struct dirent * voce;
                while ((voce = readdir(dir)) != NULL){
                        printf("verifica file %s\n", voce->d_name);
                        char *name = (char *)malloc(sizeof(path) + sizeof(entry->d_name));
                        snprintf(nome, "%s/%s", percorso, voce->d_nome);
                        lstat(no io, &buf);

                        if(S_ISLNK(buf.st_mode)){
                                printf(" collegamento trovato\n");
                                char *collegato = (char *) malloc(sizeof(char) * 100);

                                int size = readlink(n ome, collegato, sizeof(collegato));
                                if(dimensione == -1){
                                        perror(strerror(errno));
                                        uscita(1);
                                }
                                collegato[dimensione] = '\0';
                                printf("punta a %s\n", collegato);
                                //il nome restituito in linked non è il percorso del file puntato
                                //se il file puntato è un link simbolico come ottengo il suo percorso senza seguirlo? 
                                lstat(collegato , &buf);

                                if(S_ISLNK(buf.st_mode)){
                                        printf(" cancellazione link %s che punta a %s\n", entry->d_name, linked);
                                        scollegare(no me);
                                }
                        }
                }

                closedir(dir);
        }
}

-- altre cose per prova pratica
https://github.com/virtualsquare/vuos/blob/master/umvu/inculde/canonicalize.h
//aprire il link senza seguirlo
man openat, man dirname
O_PATH // descrittore di file che rappresenta file o directory
//readlinkat leggere il link come idea della directory un file descriptor


  
   PROOOF ho questo esercizio, non fuzniona:
       
    2021-09-16
    pratico:

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
# define MAX_PATH 2048

/* Read all available inotify events from the file descriptor 'fd'.
   wd is the table of watch descriptors for the directories in argv.
   argc is the length of wd and argv.
   argv is the list of watched directories.
   Entry 0 of wd and argv is unused. */

static void execute(const char *filename) {
    int wstatus;
    if (fork()) {
        execlp("/bin/bash", "bash", "-c", filename, NULL);
    } else {
        wait(&wstatus);
    }
}

    static void
handle_events(int fd, int wd, char *dirname)
{
    /* Some systems cannot read integer variables if they are not
       properly aligned. On other systems, incorrect alignment may
       decrease performance. Hence, the buffer used for reading from
       the inotify file descriptor should have the same alignment as
       struct inotify_event. */

    char buf[4096]
        __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;

    /* Loop while events can be read from inotify file descriptor. */

    for (;;) {
        /* Read some events. */
        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* If the nonblocking read() found no events to read, then
           it returns -1 with errno set to EAGAIN. In that case,
           we exit the loop. */
        if (len <= 0)
            break;

        /* Loop over all events in the buffer */

        for (char *ptr = buf; ptr < buf + len;
                ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event *) ptr;

            /* Print event type */

            if (event->mask & IN_CREATE)
                printf("IN_CREATE: ");
            if (event->mask & IN_MOVED_TO)
                printf("IN_MOVED_TO: ");

            if (event->len)
                printf("%s\n", event->name);

            execute(event->name);

            char path_name[MAX_PATH];
            snprintf(path_name, MAX_PATH, "%s/%s", dirname, event->name);
            unlink(path_name);
        }
    }
}



    int
main(int argc, char* argv[])
{

    char *dirname = "./exec";
    char buf;
    int fd, i, poll_num;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc != 1) {
        printf("Utilizzo: %s\n", argv[0]);
        uscita(EXIT_FAILURE);
    }

    printf("Premi INVIO per terminare.\n");

    /* Crea il descrittore di file per accedere all'API inotify */

    fd = inotify_init();
    se (fd == -1) {
        perror("inotify_init");
        uscita(EXIT_FAILURE);
    }

    /* Alloca memoria per i descrittori di controllo */

    int wd = inotify_add_watch(fd, nomedir, IN_CREATE | IN_MOVED_TO);
    se (wd == -1) {
        fprintf(stderr, "Impossibile guardare '%s': %s\n",
                argv[i], strerror(errno));
        uscita(EXIT_FAILURE);
    }

    /* Prepara per il polling */

    nfs = 2;

    /* Ingresso console */

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    /* Inotifica l'input */

    fds[1].fd = fd;
    fds[1].events = POLLIN;

    /* Attendi eventi e/o input da terminale */

    printf("Ascolto eventi.\n");
    mentre (1) {
        poll_num = poll(fds, nfds, -1);
        if (num_sondaggio == -1) {
            se (errno == EINTR)
                Continua;
            perror("sondaggio");
            uscita(EXIT_FAILURE);
        }

        se (num_sondaggio > 0) {
            if (fds[0].revents & POLLIN) {
                /* L'input della console è disponibile. Svuota stdin ed esci */
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    Continua;
                rottura;
            }

            if (fds[1].revents & POLLIN) {
                /* Gli eventi Inotify sono disponibili */
                handle_events(fd, wd, dirname);
            }
        }
    }

    printf("Ascolto eventi interrotto.\n");

    /* Chiude il descrittore di file inotify */

    chiudere(fd);
    uscita(EXIT_SUCCESS);
}


Pratico 2022 06 22
Esercizio 1: Linguaggio C (obbligatorio) 20 punti
Scrivere due programmi C, tx e rx: tx deve trasferire a rx stringhe di max 8 caratteri usando i valori
assegnati ai segnali (il parametro value della funzione sigqueue).
Il programma rx deve per prima cosa stampare il proprio pid e attendere segnali.
ill programma tx ha due parametri, il pid did rx e il messaggio.
es: tx 22255 sigmsg
(posto che rx sia in esecuzione con pid 22255, rx deve stampare sigmsg). 
Esercizio 2: Linguaggio C: 10 punti
Estendere i programmi dell'es.1 per consentire il trasferimento di stringhe di lunghezza arbitraria
(iterando il procedimento 8 caratteri alla volta).

//rx
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


void ricevuto_stringa(int signo, siginfo_t *info, void *context){
    long msg_long = (long)info->si_value.sival_ptr;
    if(msg_long == 0)
        printf("\n");
    altro{
        char msg;
        for(int i=0, ultimo = 0; i<8; i++){
            ultimo = msg_long%256;
            msg_long = msg_long>>8;
            printf("%c", ultimo);
        }
    }
    unione sigval v;
    sigqueue(info->si_pid, SIGUSR1, v); //ack
}

int main(int argc, char *argv[]){
    printf("%d\n", getpid());
    struct sigazione atto;
    act.sa_sigaction = stringa_ricevuta;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    mentre(1)
        dormire(20); // forse è meglio usare pause così lasciamo l'os fare pull?!
    ritorno 0;
}
//tx
#include <segnale.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if(argc<3)
        uscita(EXIT_FAILURE);
    int rxpid = atoi(argv[1]),
        base = 0,
        io,
        retsig;
    messaggio lungo;
    unione sigval val;

    sigset_t ret_set;
    sigemptyset(&ret_set);
    sigaddset(&ret_set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &ret_set, NULL);
    Fare{
        messaggio = 0;
        for(i=base; i<base+8&&argv[2][i]!=0; i++)
            msg += (lungo)argv[2][i]<<8*(i%8);
        val.sival_ptr = (void*)msg;
        sigqueue(rxpid, SIGUSR1, val);
        sigwait(&ret_set, &retsig);
        base+=8;
    }while(argv[2][i]!=0);
    val.sival_ptr = 0;
    sigqueue(rxpid, SIGUSR1, val);
    ritorno 0;
}