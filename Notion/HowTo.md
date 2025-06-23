- Link utili
    - Catalogo syscall = [https://so.v2.cs.unibo.it/wiki/index.php/Il_''catalogo''_delle_System_Call](https://so.v2.cs.unibo.it/wiki/index.php/Il_%27%27catalogo%27%27_delle_System_Call)
    - Notion utile = [https://glacier-shrew-efc.notion.site/Sistemi-operativi-f073b17b59ce4ef8957574f3e6e40075](https://www.notion.so/f073b17b59ce4ef8957574f3e6e40075?pvs=21)
    - Per cercare = https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-readdir-read-entry-from-directory
    - Soluzioni varie:
        - https://etherpad.wikimedia.org/p/so2425.cs.unibo.it
        - https://etherpad.wikimedia.org/p/so2324.cs.unibo.it
        - https://etherpad.wikimedia.org/p/so2223.cs.unibo.it
        - https://etherpad.wikimedia.org/p/so2122.cs.unibo.it
    - Soluzioni di altri
        - ‣

> La prova pratica richede la soluzione di problemi di shell scripting, programmazione di sistema, programmazione python e amministrazione di sistema.
> 
> 
> Lo svolgimento della prova pratica avviene in un laboratorio del dipartimento, e la consegna avviene tramite e-mail al docente. La prova pratica è seguita da una discussione, il cui scopo è di verificare la vostra preparazione e  che lo svolgimento sia farina del vostro sacco.
> 

### Cheating policies

- Nella prova pratica, è consentito consultare libri, quaderni e ogni documentazione; è anche possibile utilizzare qualsiasi apparato. Eccezioni:
    - non è consentito consultare persone.
    - gli apparati non devono consentire comunicazione (cellulari, laptop o palmtop con interfaccia radio o infrarosso...).
    - si può consultare o utilizzare solo materiale proprio, è vietato lo scambio di qualsiasi oggetto (penna, gomma, foglio, libro o altro).
- Ogni uso di Intelligenza Artificiale generativa deve essere indicato.

Il processo è questo:

- Modifica i permessi di accesso per impedire l’accesso ad altri utenti —> `chmod 700 ~`
- Creazione della directory /public/nomeutente —> `mkdir -p /public/$(whoami)`
- Gli do i permessi 700 —> `chmod 700 /public/$(whoami)`

Per testare che il tutto sia andato a buon fine controlla:

- Controllo dei permessi nella home directory —> `ls -ld ~`
    - Devo ottenere questo
        
        ```c
        gabriel.betti@eufemia:~$ ls -ld ~
        drwx------ 21 gabriel.betti gabriel.betti 4096 Nov  2 16:05 /home/students/gabriel.betti
        ```
        
    - Se ottengo qualcosa di diverso, allora usa `chomod 700 ~`
- Controllo della directory in /pubblic, verifico se esiste con —> `ls -ld /public/$(whoami)`
    - Devo ottenere
        
        ```c
        gabriel.betti@eufemia:~$ ls -ld /public/$(whoami)
        drwx------ 2 gabriel.betti gabriel.betti 4096 Feb 20 17:30 /public/gabriel.betti
        ```
        
    - Se non esiste o ha permessi errati
        
        ```c
        mkdir -p /public/$(whoami)
        chmod 700 /public/$(whoami)
        ```
        
- Per test maggiori bisognorebbe provare ad accedere con un altro utente, ma non penso di aver accesso ad altri a meno che non provo con guest

# Esercizio in C

- Salvare il file sempre in `.c`
- Per eseguire il file usare `gcc -Wall saferm.c -o saferm`
    - `-Wall` abilita tutti i warning utili per individuare errori
    - `-o saferm` imposta come output l’eseguibile `saferm` anziché `a.out`
1. Primo step metti le librerie, qui un elenco delle più comuni
    
    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <dirent.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <sys/types.h>
    
    #define MAX_PATH 4096
    ```
    
2. Ora inizia a scrivere il main, per quanti riguarda il main l’idea è di mettere come 
    
    ```c
    int main(int argc, char *argv[]){}
    
    // Se non si passano argomenti 
    int main(){}
    ```
    
    1. Prima cosa il controllo su quanti argomenti, tu tieni a mente che il solo file lanciato è un comando, quindi se devo mettere il filepath e anche un dirpath, avrò 3 elmenti
        
        ```c
          if (argc != 3) {
              fprintf(stderr, "Uso: %s <directory>\n", argv[0]);
              return EXIT_FAILURE;
          }
        ```
        
    2. Ora definire gli argomenti che ci servono
        
        ```c
        const char *file_path = argv[1];
        const char *dir_path = argv[2];
        ```
        
    3. A questo punto abbiamo caricato gli argomenti, da qui ruota libera a seconda del problema, ma abbiamo già gli argomenti caricati (es. posso fare stat sul file per avere info)
    4. Poi tendenzialmente si chiamano le funzioni ausiliarie, che fanno la roba vera
    5. Concludi il main sempre con:
        
        ```c
        return EXIT_SUCCESS;
        ```
        
- Comandi Built-in per C
    
    ### I/O
    
    - `echo` → Serve per stampare qualcosa a schermo
        - `echo Ciao` → stampa `Ciao`
        - `echo -n Ciao` → stampa `Ciao` **senza andare a capo**
        - Puoi usarlo per salvare il risultato in una variabile:
        
        ```bash
        a=$(echo "HELLO" | tr A-Z a-z)   # a diventa "hello"
        ```
        
    - `printf` → Come echo, ma più preciso (come il printf in C)
        - Utile per numeri, formattazioni, ecc.
        - Esempio:
        
        ```bash
        PI=3.14159265358979
        PI5=$(printf "%.5f" $PI)   # PI5 sarà "3.14159"
        ```
        
    - `read` → Serve per leggere input da tastiera
        - `read nome` → aspetta che l’utente scriva qualcosa e lo salva in `nome`
        - Può fare anche cose più fighe:
        
        ```bash
        read -s -n1 -p "Premi un tasto " tasto
        echo "Hai premuto: $tasto"
        ```
        
        - `s` → non mostra quello che scrivi
        - `n1` → accetta solo 1 carattere
        - `p` → stampa un messaggio prima di leggere
    
    ### Variabili
    
    - `declare` (o `typeset`) → Serve per impostare come si comporta una variabile
        - `declare -r a=5` → la variabile `a` non potrà più essere cambiata (readonly)
        - `declare -i a=5` → dice che `a` è un numero intero (fa calcoli)
        - `declare -a v` → la variabile `v` è un array
        - `declare -x a=5` → rende `a` **visibile anche nei sottoprocessi** (come `export`)
        
        📌 Alternativa:
        
        - `export` è lo stesso di `declare -x`
        - `readonly` è lo stesso di `declare -r`
        
        ---
        
    - `let` → Serve per fare operazioni matematiche
        
        ```bash
        let a=11        # a vale 11
        let "a <<= 3"   # a viene moltiplicato per 8 (bit shift a sinistra)
        let "a /= 4"    # divide a per 4
        let "a -= 5"    # toglie 5 ad a
        
        ```
        
        📝 puoi anche usare: `(( a += 1 ))` → è lo stesso
        
    - `unset` → Cancella una variabile, come se non fosse mai esistita
        
        ```bash
        unset a
        ```
        
    
    ### Script execution
    
    - `source` (o `.`) → Serve per eseguire uno script nello stesso terminale, senza creare una shell separata
        
        Esempio:
        
        ```bash
        source mio_script.sh
        # oppure
        . mio_script.sh
        
        ```
        
        ⚠️ Se usi `./mio_script.sh`, viene eseguito **in un'altra shell**, quindi non ti cambia variabili, cartelle ecc.
        
        ### 🔁 Differenza tra `source` e `./`
        
        Supponi che in uno script tu faccia `cd /`
        
        - Se fai `./script.sh`, **il cambio directory resta solo nello script**
        - Se fai `source script.sh`, **cambia anche nel tuo terminale**
    
    ### Comandi vari
    
    - `type [comando]`→ Ti dice che tipo di comando è
        - keyword?
        - comando built-in?
        - eseguibile esterno?
        
        🧪 Esempi:
        
        ```bash
        type cd       # built-in
        type ls       # comando esterno (/bin/ls)
        type while    # keyword
        
        ```
        
    - `shopt [opzioni]`→ Serve a modificare il comportamento della shell (Bash)
        
        🧪 Esempio utile:
        
        ```bash
        shopt -s cdspell
        ```
        
        ➤ Se sbagli a scrivere una cartella con `cd`, la shell prova a correggerti.
        
    - `alias` / `unalias`→ Un alias è una scorciatoia per un comando più lungo
        
        🧪 Esempi:
        
        ```bash
        alias dir="ls -l"        # ora "dir" fa un "ls -l"
        alias rd="rm -r"         # "rd" diventa più veloce
        alias h="history -r 10"  # mostra ultimi 10 comandi
        
        unalias dir              # rimuove l’alias "dir"
        
        ```
        
        🧠 Puoi mettere gli alias nel tuo `~/.bashrc` per renderli permanenti
        
    - `history`→ Mostra tutti i comandi che hai digitato finora nel terminale
        
        🧪 Esempi:
        
        ```bash
        history        # mostra tutto
        history 10     # mostra solo gli ultimi 10
        
        ```
        
    
    ### Directory stack
    
    > È una pila (come lo stack in C) dove puoi salvare le cartelle in cui ti muovi, e poi tornare indietro facilmente.
    > 
    - `pushd dirname`→ Ti **sposta dentro `dirname` e** **salva** la cartella attuale nello stack (È come dire: “Entro qui, ma ricordati da dove sono partito!”)
        
        ```c
        // Partiamo da /home/montreso
        
        pushd /dir1
        # ti sposta in /dir1, e salva /home/montreso nello stack
        
        pushd /dir2
        # ora sei in /dir2, e nello stack ci sono: /dir1 e /home/montreso
        ```
        
    - `popd`→ **Torna indietro** alla directory precedente (quella in cima allo stack), rimuove l'ultimo `pushd` fatto
        
        ```c
        popd
        # torni in /dir1 (rimuove /dir2 dallo stack)
        
        popd
        # torni in /home/montreso (rimuove /dir1)
        ```
        
    - `dirs`→ Ti mostra **l’elenco delle cartelle salvate nello stack**
    - `$DIRSTACK`→ Variabile che contiene **le directory nello stack** (puoi anche stamparla)
- Funzioni utili in POSIX
    - `struct stat` → legge **informazioni del file** (anche se è un link simbolico), **seguendo il link**
        - **Cosa fa**: come `lstat`, riempie una struttura `stat`, ma se il file è un link, segue il percorso per ottenere info sul vero file.
        - **Quando usarla**: vuoi sapere se il file "reale" dietro un eventuale link simbolico è, ad esempio, una directory o un file.
        
        📦 Esempio d'uso:
        
        ```c
        struct stat st;
        if (stat("link.txt", &st) == 0 && S_ISREG(st.st_mode)) {
            printf("Punta a un file normale\n");
        }
        ```
        
    - `lstat` → legge le informazioni del file indicato al prim arg e le scrive nel secondo, senza eventuali link simbolici
        
        ```c
        lstat(const char *pathname, struct stat *buf)
        ```
        
        - **Scopo**: legge le **informazioni del file indicato** da `pathname` **senza seguire eventuali link simbolici**.
        - **Cosa fa**: riempie la struttura `struct stat` (puntata da `buf`) con metadati (tipo file, permessi, dimensione, ecc.).
        - **Quando usarla**: vuoi sapere **se un file è un link simbolico** e non cosa c’è "dietro" quel link.
            
            📦 Esempio d'uso:
            
            ```c
            struct stat st;
            if (lstat("link.txt", &st) == 0 && S_ISLNK(st.st_mode)) {
                printf("È un link simbolico!\n");
            }
            ```
            
    - `opendir - readdir - closedir` → aprire e leggere info su una directory (es: tutti i file e contarli)
        - `opendir()` → Apre una directory e **restituisce un "puntatore" alla directory** (di tipo `DIR *`), che poi userai per leggerne il contenuto.
            
            ```c
            DIR *dir = opendir("nome_directory");
            ```
            
            - Se la directory esiste → ritorna un puntatore valido (`DIR *`)
            - Se **fallisce** (es. path errato o permessi mancanti) → ritorna `NULL`
        - `readdir()` → Legge **una entry (un file o directory)** alla volta dal puntatore `DIR *` restituito da `opendir`.
            
            ```c
            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                printf("%s\n", entry->d_name);  // stampa nome del file/dir
            }
            ```
            
            - Restituisce un **puntatore a struct `dirent`**
            - Campo più importante: `entry->d_name` = nome del file
        - `closedir()` → Chiude la directory, fondamentale farla ogni volta che si fa `opendir()`
            - Restituisce 0 se ok
            - Restitusice 1 se errore
        
        ---
        
        Esempio di lettura di tutti i file in una dir
        
        ```c
        DIR *dir = opendir(".");
        if (dir == NULL) {
            perror("Errore");
            exit(1);
        }
        
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }
        closedir(dir);  // sempre chiudere dopo
        ```
        
    - `struct dirent` → rappresenta **un singolo elemento** (come un file o una directory) **contenuto in una directory**, ottenuto tramite la funzione `readdir()` (meno completa di `stat` )
        
        ### 🛠️ Dove si usa?
        
        Quando navighi dentro una directory (tipo `.` o `./prova/`), usi:
        
        ```c
        DIR *dir = opendir("miodir");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }
        closedir(dir);
        ```
        
        ---
        
        ### 🔍 Campi principali di `struct dirent`:
        
        | Campo | Significato |
        | --- | --- |
        | `d_name` | Il nome del file (es. `"prova.txt"`) |
        | `d_type` | Tipo dell’entry (solo se supportato): file, dir, link, ecc. |
        
        💡 `d_type` può avere questi valori (se disponibili):
        
        - `DT_REG` → file regolare
        - `DT_DIR` → directory
        - `DT_LNK` → link simbolico
        - `DT_UNKNOWN` → tipo non disponibile (in quel caso usa `stat()`!)
        
        ---
        
        ### 📦 Esempio pratico (solo nomi dei file):
        
        ```c
        #include <dirent.h>
        #include <stdio.h>
        
        int main() {
            DIR *dir = opendir(".");
            struct dirent *entry;
        
            while ((entry = readdir(dir)) != NULL) {
                printf("Nome: %s\n", entry->d_name);
            }
        
            closedir(dir);
            return 0;
        }
        ```
        
        ---
        
        ### 🧠 In sintesi:
        
        - `dirent` = **descrizione base di un file** dentro una directory.
        - È **leggera e veloce**, ma **non ti dice tutto** (es. dimensione, permessi).
        - Per informazioni complete usa `stat()` o `lstat()`.
    - `snprintf` → per scrivere delle stringhe dinamicamente (es: path composti da più parti, come path + nomeFile, con nomeFile che cambia ad ogni iter)
        
        <aside>
        💡
        
        Viene usato al posto di `sprintf` perchè non soffre di buffer overflow
        
        </aside>
        
        `snprintf()` è una funzione C che **scrive una stringa formattata** dentro un **buffer (array di caratteri)**, come fa `printf`, **ma con un limite di lunghezza**.
        
        ```c
        int snprintf(char *dest, size_t max_len, const char *format, ...);
        
        ```
        
        | Parametro | Significato |
        | --- | --- |
        | `dest` | Array dove scrivere la stringa |
        | `max_len` | Numero massimo di caratteri da scrivere (incluso `\0`) |
        | `format` | Stringa di formato (come in `printf`) |
        | `...` | Valori da inserire nei segnaposto |
        
        ---
        
        ## 🔒 A cosa serve davvero?
        
        Serve a:
        
        - **Comporre una stringa dinamicamente** (es: percorsi, messaggi, nomi)
        - **Evitare buffer overflow** (cioè scrivere oltre i limiti dell'array!)
        
        ---
        
        ## 🧪 Esempio pratico:
        
        ```c
        char percorso[100];
        snprintf(percorso, 100, "/home/%s/%s", "giacomo", "file.txt");
        
        printf("%s\n", percorso);
        // Output: /home/giacomo/file.txt
        
        ```
        
        - Scrive dentro `percorso` una stringa combinando parti diverse
        - Il `100` assicura che non vada oltre il limite del buffer
        
        ---
        
    - `unlink` → cancella il file dal filepath indicato, è come un rm da terminale
        
        Cancella il **file** indicato da `filepath`.
        
        - `unlink()` è l’equivalente in C di un `rm file` da terminale.
        - **Non cancella directory**, solo file normali (o symlink).
        
        ### 🧠 Continuando l’esempio:
        
        ```c
        unlink("/home/giacomo/.trash/file1.txt");
        ```
        
        → Rimuove fisicamente il file dal filesystem.
        
    - `rename` → rinomina/cambia destinazione a un file o directory
        
        Come funziona (concettualmente):
        
        1. Dai il **percorso attuale** del file (`vecchio_nome`)
        2. Dai il **nuovo nome o nuovo percorso** (`nuovo_nome`)
        3. Il sistema operativo:
            - **Aggiorna il riferimento al file**, non il contenuto
            - **Non copia** i dati fisici
            - Lo spostamento è **istantaneo** se rimane nello stesso filesystem
        
        ---
        
        ## 🧪 Esempi astratti:
        
        - **Rinominare**:
            
            ```c
            rename("report.txt", "report_backup.txt");
            ```
            
            ➤ Cambia solo il nome del file
            
        - **Spostare**:
            
            ```c
            rename("file.txt", "/home/utente/.trash/file.txt");
            ```
            
            ➤ Sposta il file in un’altra directory
            
    - `access` → chiede al OS se può accedere a un file o directory con i permessi richiesti
        
        ---
        
        ```c
        int access(const char *path, int mode);
        
        ```
        
        - Restituisce:
            - `0` se il controllo è positivo (OK)
            - `1` se fallisce (file non esiste o permessi insufficienti)
        - Dipende dal secondo parametro `mode`:
            
            
            | `mode` | Cosa verifica |
            | --- | --- |
            | `F_OK` | Verifica se il **file esiste** |
            | `R_OK` | Permesso di **lettura** |
            | `W_OK` | Permesso di **scrittura** |
            | `X_OK` | Permesso di **esecuzione** |
        
        ---
        
        ## 🧠 In sintesi:
        
        > access() = controlla se un file esiste o se puoi accedervi (leggere, scrivere, eseguire),
        > 
        > 
        > **senza aprirlo** e senza modificarlo.
        > 
        
        ---
        
    - `atoi` → convertire una stringa in un intero, se la stringa non è un numero da 0
        
        ```c
        int n = atoi("123");
        ```
        
        - `"123"` → è una stringa
        - `n` → sarà l’intero `123`
        
        ---
        
        ## 📦 Esempi:
        
        | Codice | Risultato |
        | --- | --- |
        | `atoi("42")` | `42` |
        | `atoi("0")` | `0` |
        | `atoi("ciao")` | `0` (errore, ma non crasha) |
        | `atoi("12abc")` | `12` (si ferma appena trova lettere) |
    - `malloc` → alloca (cioè **riserva dinamicamente** a runtime) un blocco di memoria della dimensione specificata.
        
        ```c
        malloc(size_t size)
        ```
        
        ### 🧠 **Quando si usa**
        
        - Quando non sai **a priori quanto spazio serve**, e vuoi calcolarlo a runtime.
        - Ad esempio: leggere un file intero (non sai quanto sarà grande → lo scopri con `ftell` e poi fai `malloc(size)`).
        
        ### 📥 **Parametro**
        
        - `size`: numero di **byte** da allocare
        
        ### 📤 **Valore restituito**
        
        - Puntatore (`void *`) alla memoria allocata se tutto ok
        - `NULL` se la memoria **non può essere allocata**
        
        ### 🧪 **Esempio tipico**
        
        ```c
        char *buffer = malloc(size + 1);  // +1 per aggiungere '\0' alla fine di una stringa
        if (!buffer) {
            // errore: malloc ha fallito
        }
        ```
        
        ### ❗ **Attenzione**
        
        - La memoria **non è inizializzata** (può contenere spazzatura!)
        - Devi **liberarla** con `free(buffer);` quando non ti serve più
        
        ---
        
        ### 🧠 Insieme a `fread()`:
        
        ```c
        char *buffer = malloc(size + 1);
        fread(buffer, 1, size, file);
        buffer[size] = '\0';  // chiusura della stringa
        
        ```
        
        ---
        
        Fammi sapere se vuoi anche una versione con **disegnino a blocchi** o uno schemino riassuntivo finale.
        
    - `realpath` → restituisce il percorso assoluto a un file/directory
        
        ```c
        realpath(<percorso relativo>, <buffer per il percorso assoluto>)
        ```
        
    - **`inotify`→** un **meccanismo del kernel Linux** che permette a un programma di **accorgersi quando qualcosa cambia nel filesystem**
        
        Si accorge per esempio quando:
        
        - viene **creato un file**
        - un file viene **modificato o cancellato**
        - un file viene **spostato dentro una directory**
        - e così via…
        
        ---
        
        ### ⚙️ **Come funziona in breve?**
        
        1. **`inotify_init()`**
            
            → crea un descrittore per iniziare a monitorare.
            
        2. **`inotify_add_watch(fd, path, eventi)`**
            
            → dice cosa vuoi **osservare** (es. una directory) e **quali eventi** ti interessano (es. creazione file, scrittura, spostamento…).
            
        3. **`read(fd, buffer, size)`**
            
            → il programma **aspetta** che succeda qualcosa. Quando succede, riceve una struttura `inotify_event` con **nome file, tipo di evento, ecc.**
            
        4. Il programma può reagire: es. **eseguire il file, copiarlo, cancellarlo, loggarlo…**
        
        ---
        
        ### 🎯 Esempio semplice
        
        > Monitoro la directory download/. Quando arriva un nuovo file .pdf, stampo "Nuovo PDF scaricato!"
        > 
        
        ---
        
        ### ✅ Vantaggi
        
        - Eviti di usare `sleep + stat()` continuamente (inefficiente).
        - È **reattivo**: ti accorgi subito degli eventi.
        - È **preciso**: sai esattamente quale file ha causato l'evento.
        
        ---
        
    - `getcwd` → Restituisce il percorso assoluto della directory corrente (working directory).
        
        ```c
        getcwd(char *buf, size_t size)
        ```
        
        ### ✅ Uso tipico:
        
        ```c
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directory corrente: %s\n", cwd);
        }
        
        ```
        
        ### 🔍 Note:
        
        - `cwd` deve avere spazio sufficiente (4096 è una scelta comune).
        - Se fallisce, restituisce `NULL`.
    - `chdir` → Cambia la directory corrente del processo a quella specificata.
        
        ```c
        chdir(const char *path)
        ```
        
        ### ✅ Uso tipico:
        
        ```c
        if (chdir("/tmp") != 0) {
            perror("Errore in chdir");
        }
        ```
        
        ### 🔍 Note:
        
        - Influenza le operazioni relative a file (es. apertura file con path relativo).
        - Funziona solo se hai permessi di accesso alla directory.
    
    ### Gestione file
    
    - `fopen`→ **apre un file** e ti restituisce un **puntatore a FILE** per poterlo leggere, scrivere o creare
        
        ```c
        FILE *fopen(const char *filename, const char *mode);
        ```
        
        ### 📘 Parametri:
        
        - `filename`: il **percorso del file** (es. `"file.txt"` o `"../data.txt"`).
        - `mode`: modalità di apertura (stringa):
        
        | Modalità | Descrizione |
        | --- | --- |
        | `"r"` | Legge un file già esistente |
        | `"w"` | Sovrascrive o crea nuovo file |
        | `"a"` | Aggiunge in fondo al file |
        | `"r+"` | Lettura e scrittura (esistente) |
        | `"w+"` | Lettura/scrittura (resetta file) |
        | `"a+"` | Lettura/scrittura in append |
        
        ### 📌 Restituisce:
        
        - `FILE *` se tutto va bene ✅
        - `NULL` se fallisce ❌ (es. file inesistente in modalità `"r"`)
    - `fseek` → Sposta il **cursore di lettura/scrittura** (detto anche file pointer) in una nuova posizione all’interno del file aperto.
        
        ```c
        fseek(FILE *stream, long offset, int whence)
        ```
        
        ### 📥 **Parametri**
        
        - `stream`: il file aperto (puntatore di tipo `FILE *`)
        - `offset`: di quanti byte spostarsi
        - `whence`: punto di riferimento da cui calcolare l’offset:
            - `SEEK_SET` → dall’inizio del file
            - `SEEK_CUR` → dalla posizione corrente
            - `SEEK_END` → dalla fine del file
        
        ### 🧠 **Esempio d’uso**
        
        ```c
        fseek(file, 0, SEEK_END);  // Sposta il cursore alla fine del file
        ```
        
        ### 📤 **Valore restituito**
        
        - `0` se tutto va bene
        - `1` se c’è un errore (es. file non seekable)
    - `rewind` → Riporta il **cursore di lettura/scrittura** all’**inizio del file**.
        
        ```c
        rewind(FILE *stream)
        ```
        
        ### 🧠 **Quando si usa**
        
        - Dopo aver fatto un `fseek()` alla fine del file (per conoscere la dimensione), se vuoi poi **leggere il file dall'inizio**, devi usare `rewind()` o `fseek(file, 0, SEEK_SET)`.
        
        ### 🧪 **Esempio pratico**
        
        ```c
        fseek(file, 0, SEEK_END);   // vai alla fine
        long size = ftell(file);    // ottieni la dimensione
        rewind(file);               // torni all’inizio per iniziare la lettura
        
        ```
        
        ### 📝 **Nota**
        
        - Non restituisce valore
        - Azzera anche gli **errori** e lo **stato di EOF** del file (se ci sono)
        
        ---
        
    - `ftell` → Restituisce la **posizione corrente** del cursore nel file, in byte dall'inizio.
        
        ```c
        ftell(FILE *stream)
        ```
        
        ### 📤 **Valore restituito**
        
        - Numero di byte dalla posizione iniziale del file
        - `1L` se c’è un errore
        
        ### 🧠 **Uso tipico**
        
        Dopo un `fseek` a fine file per conoscere la dimensione:
        
        ```c
        fseek(file, 0, SEEK_END);
        long size = ftell(file);  // => dimensione del file in byte
        
        ```
        
    - `fread` → Legge dati **binari o testuali** dal file e li scrive nella memoria puntata da `ptr`.
        
        ```c
        fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
        ```
        
        ### 📥 **Parametri**
        
        - `ptr`: dove salvare i dati letti
        - `size`: dimensione di ogni elemento (in byte)
        - `nmemb`: numero di elementi da leggere
        - `stream`: puntatore al file da cui leggere
        
        ### 🧠 **Uso tipico**
        
        Per leggere tutto in un solo blocco:
        
        ```c
        fread(buffer, 1, size, file);  // legge 'size' byte e li scrive in buffer
        ```
        
        ### 📤 **Valore restituito**
        
        - Numero di **elementi** letti con successo (non byte totali!)
        - Se ritorna meno di `nmemb`, può essere per fine file o errore
    - `fclose` → **Chiude** un file precedentemente aperto con `fopen`
        
        > Libera tutte le risorse associate al file (buffer, descrittori, etc.)
        > 
        
        ```c
         fclose(FILE *stream)
        ```
        
        ### 📤 **Valore restituito**
        
        - `0` se chiusura avvenuta con successo
        - `EOF` se errore
        
        ### 🧠 **Nota importante**
        
        Sempre chiamare `fclose` per **evitare memory leak** o scritture non completate.
        
    - `S_IS...` → Sequeze utili per capire tipi di file in accoppiata a `stat` o `lstat`
        
        ### 🧠 Da sapere
        
        - Tutte queste macro **non restituiscono stringhe**, ma veri/falsi (0 o diverso da 0).
        - Si applicano a `st_mode` della `struct stat` ottenuta con `stat()` o `lstat()`.
        
        | Macro | Tipo di file | Descrizione |
        | --- | --- | --- |
        | `S_ISREG(m)` | **File regolare** | Es. file `.txt`, `.c`, `.bin`, ecc. |
        | `S_ISDIR(m)` | **Directory** | Una cartella |
        | `S_ISLNK(m)` | **Link simbolico (symlink)** | Es. shortcut creato con `ln -s` |
        | `S_ISCHR(m)` | **File a caratteri** | Dispositivi che comunicano a **caratteri**, es. `/dev/tty`, `/dev/null` |
        | `S_ISBLK(m)` | **File a blocchi** | Dispositivi che comunicano a **blocchi**, es. `/dev/sda` (dischi) |
        | `S_ISFIFO(m)` | **FIFO o named pipe** | Una pipe tra processi, usata per comunicazione |
        | `S_ISSOCK(m)` | **Socket** | Endpoint per comunicazione interprocesso o di rete |
        
        ---
        
        ### 📌 Esempio di utilizzo combinato
        
        ```c
        struct stat st;
        stat("file.txt", &st);
        
        if (S_ISREG(st.st_mode)) printf("File regolare\n");
        if (S_ISDIR(st.st_mode)) printf("Directory\n");
        if (S_ISCHR(st.st_mode)) printf("Dispositivo a caratteri\n");
        if (S_ISBLK(st.st_mode)) printf("Dispositivo a blocchi\n");
        if (S_ISFIFO(st.st_mode)) printf("FIFO (named pipe)\n");
        if (S_ISSOCK(st.st_mode)) printf("Socket\n");
        if (S_ISLNK(st.st_mode)) printf("Link simbolico\n");
        ```
        
        ---
        
    
    ### Gestione stringhe
    
    - **`tolower` →** serve a **convertire un carattere maiuscolo in minuscolo**, **solo se è una lettera dell'alfabeto**.
        
        ```c
        int tolower(int c);
        ```
        
        - 🔹 **Parametro**: un carattere (tipicamente passato come `int`, ma rappresenta un `char`)
        - 🔹 **Valore di ritorno**:
            - Se `c` è una **lettera maiuscola** (`'A'`–`'Z'`), restituisce la **versione minuscola** (`'a'`–`'z'`)
            - Altrimenti, **restituisce il carattere originale**
        
        ### 🧠 **A cosa serve davvero?**
        
        Ti permette di **fare confronti senza tenere conto delle maiuscole/minuscole**. Utile ad esempio per confrontare due parole in modo "case-insensitive".
        
        ---
        
        ### 🔧 **Esempio pratico**
        
        ```c
        char c = 'T';
        char lower = tolower(c);  // lower = 't'
        ```
        
        ```c
        // confronto tra due caratteri, ignorando maiuscole
        if (tolower(a) == tolower(b)) {
            // sono uguali anche se uno è maiuscolo
        }
        ```
        
    - **`strstr` →** serve a **cercare una sottostringa all'interno di una stringa è case-sensitive**
        
        ```c
        char *strstr(const char *haystack, const char *needle);
        ```
        
        - `haystack`: la **stringa principale** (dove cerchi)
        - `needle`: la **sottostringa** da cercare
        
        ---
        
        ### 🔄 **Valore di ritorno**
        
        - Restituisce un **puntatore alla prima occorrenza** della sottostringa in `haystack`
        - Se **non trova nulla**, restituisce `NULL`
        
        ---
        
        ### 🔧 **Esempio pratico**
        
        ```c
        char *text = "Questo è un test";
        char *found = strstr(text, "test");
        
        if (found) {
            printf("Trovato! Inizia da: %s\n", found);  // stampa: test
        }
        ```
        
        ---
        
        ### ⚠️ **Nota**
        
        - La ricerca è **case-sensitive**: `"Test"` e `"test"` non sono la stessa cosa
        - Se vuoi fare una ricerca ignorando maiuscole/minuscole, devi usare `tolower()` manualmente su entrambe le stringhe
    - `strrchr` → cerca **l’ultima occorrenza** di un carattere (ad es il `.` ) in una stringa
        
        ```c
        const char *strrchr(const char *str, int c);
        ```
        
        - `str`: stringa in cui cercare
        - `c`: carattere da trovare (come `'x'`, `'.'`, ecc.)
        
        **Valore di ritorno:**
        
        - Puntatore all'**ultima** posizione del carattere nella stringa
        - Oppure `NULL` se non lo trova
        
        ```c
        const char *dot = strrchr(filename, '.');
        ```
        
        👉 Cerca l’ultima `.` nel nome del file per trovare **l’estensione del file**
        
        ---
        
    - `strcmp` → confronta **due stringhe esattamente (case-sensitive)**.
        
        ```c
        int strcmp(const char *s1, const char *s2);
        ```
        
        - Ritorna `0` se **le due stringhe sono uguali**
        - Ritorna un valore **positivo o negativo** se sono diverse
        
        **Nel codice:**
        
        ```c
        return dot && strcmp(dot, ext) == 0;
        ```
        
        👉 Restituisce `true` (1) **solo se**:
        
        - Il file ha un punto (`.`) → quindi ha estensione
        - L’estensione è **uguale** a `ext` (es. `.txt`)
    - `memcmp` → confronta i primi n byte di due arie di memoria
        
        ```c
        int memcmp(const void *s1, const void *s2, size_t n);
        ```
        
        Confronta byte per byte i **primi `n` byte** di due aree di memoria:
        
        - `s1`: puntatore al primo blocco di memoria (es. buffer 1)
        - `s2`: puntatore al secondo blocco di memoria (es. buffer 2)
        - `n`: numero di byte da confrontare
        
        Restituisce:
        
        - `0` se i blocchi sono **identici**
        - `< 0` se il primo byte diverso ha `s1[i] < s2[i]`
        - `> 0` se il primo byte diverso ha `s1[i] > s2[i]`
        
        ---
        
        ### ✅ A cosa serve
        
        È utile quando:
        
        - vuoi confrontare due file letti in memoria
        - vuoi confrontare stringhe binarie non terminate da `\0`,
        - hai buffer contenenti dati raw.
        
        ### 🧪 Esempio pratico
        
        ```c
        char a[] = {1, 2, 3, 4};
        char b[] = {1, 2, 3, 5};
        
        if (memcmp(a, b, 4) == 0)
            printf("Uguali\n");
        else
            printf("Diversi\n");
        
        ```
        
        Stampa `Diversi` perché differiscono all’ultimo byte.
        
    
    ### Gestione del tempo e timer
    
    - `timerfd_create` → Come un orologio speciale, in cui chedi “ehi Linux, voglio che mi avvisi tra *X secondi*… ma non con una sveglia che suona, ma dammi un **file** che posso controllare.”
        
        ```c
        int timerfd_create(int clockid, int flags);
        
        ```
        
        ### 💡 Come funziona passo passo:
        
        1. Chiami `timerfd_create`: il sistema ti dà un **file speciale**.
        2. Con `timerfd_settime` gli dici: “fammi scattare tra tot millisecondi”.
        3. Usi `poll` o `select` per dire:
            
            👉 “Fammi sapere quando questo file è pronto da leggere.”
            
        4. Quando il tempo è passato, **il file diventa leggibile**.
        5. Leggi da quel file e scopri che il timer è “scattato”.
        
        ---
        
        ### 🎯 In parole povere:
        
        È come chiedere al sistema:
        
        📦 “Fammi un timer che funziona come un file.
        
        Dimmi quando scade così posso controllarlo nel mio programma insieme ad altre cose.”
        
        Utile per:
        
        - Non usare `sleep`
        - Fare timeout “puliti” in modo reattivo
        - Evitare blocchi attivi (tipo `while(...)`)
        
        ### 🧩 Parametri:
        
        | Parametro | Significato |
        | --- | --- |
        | `clockid` | Quale orologio usare (es. `CLOCK_MONOTONIC` → tempo che avanza sempre, non influenzato dall'orario di sistema). |
        | `flags` | Di solito 0 oppure `TFD_CLOEXEC` o `TFD_NONBLOCK` (opzionale). |
        
        ---
        
        ### ✅ Esempio:
        
        ```c
        int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
        
        ```
        
        - Crea un timer **basato sul tempo reale crescente** (non modificabile).
        - Restituisce `tfd`, un file descriptor che puoi monitorare con `poll()`.
        - Una volta che il tempo scade, `tfd` diventa leggibile.
        
        ---
        
    - `timerfd_settime` → Imposta il timer: quanto tempo aspettare, e se deve ripetersi.
        
        ```c
        int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, 
        										struct itimerspec *old_value);
        ```
        
        ### Parametri:
        
        - `fd`: il file descriptor restituito da `timerfd_create`
        - `flags`:
            - `0`: tempo relativo (es. scatta tra 3 secondi)
            - `TFD_TIMER_ABSTIME`: tempo assoluto (es. scatta alle 12:00)
        - `new_value`: cosa vuoi che faccia il timer (quanto dura, ogni quanto ripete)
        - `old_value`: se vuoi sapere cosa faceva prima
        
        > 💡 Devi preparare una struttura itimerspec:
        > 
        
        ```c
        struct itimerspec {
            struct timespec it_interval; // se > 0 → timer ripetuto
            struct timespec it_value;    // tempo iniziale
        };
        
        ```
        
    - `timerfd_gettime` → Ti dice **quanto manca allo scatto** o qual è l’intervallo corrente del timer.
        
        ```c
        int timerfd_gettime(int fd, struct itimerspec *curr_value);
        ```
        
    - `read(fd, &buf, sizeof(uint64_t));` → **Leggi il timer** → ritorna un `uint64_t` che indica **quante volte è scattato** da quando hai letto l’ultima volta.
        
        ```c
        read(fd, &buf, sizeof(uint64_t));
        ```
        
        > ✅ Non è una timerfd_ vera e propria, ma è essenziale:
        > 
        > 
        > Leggere il timer è **obbligatorio**, altrimenti non si resetta.
        >
