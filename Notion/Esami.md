### Es PDF

- `saferm` - Scrivere uno script che abbia le funzionalità di rm, ma che invece di cancellare
definitivamente i file li sposti in una directory .trash nella vostra home, se file già in trash con stesso nome, rinnominare il file già presente concatenando la sua data
    
    ![image.png](attachment:9f1066ed-041a-469a-a976-668a37edfd1f:image.png)
    
    ![image.png](attachment:9a72dab3-f223-498e-8617-fb2190d428e2:image.png)
    
    ```c
    #include <stdio.h>      // per printf, perror, fprintf
    #include <stdlib.h>     // per getenv, exit
    #include <string.h>     // per strcmp, snprintf
    #include <unistd.h>     // per unlink, rename, access
    #include <sys/stat.h>   // per mkdir, stat
    #include <sys/types.h>  // per struct stat, DIR
    #include <dirent.h>     // per DIR, opendir, readdir
    #include <errno.h>      // per errno
    #include <time.h>       // per time()
    
    #define MAX_PATH 4096   // dimensione massima per i percorsi
    
    // Mostra messaggio se si usa male il comando, per esempio facendo safrem -L -P, che non e' possibile
    void usage() {
        fprintf(stderr, "Uso: saferm [-L | -P | -R file | file ...]\n");
        exit(EXIT_FAILURE);            // Dopo aver scritto l'errore, esce 
    }
    
    // Crea la directory .trash nella home se non esiste
    void ensure_trash_exists(const char *trash_path) {
        struct stat st;   // Variabile struct che uso per salvare info(dimensione, etc...) sulla directory .trash nel caso esista
        /*
            stat in POSIX controlla se esiste o no una determinata directory:
                - trash_path --> percorso da controllare
                - &st --> se trova la directory, salva info sulla directory in st
            Se ritorna -1, non trovata
        */
        if (stat(trash_path, &st) == -1) {
            mkdir(trash_path, 0700);  // crea la directory solo se non esiste e gli fornisco l'accesso completo all'utente con 0700
        }
    }
    
    // Elenca i file presenti nel cestino .trash
    void list_trash(const char *trash_path) {
        DIR *dir = opendir(trash_path);
        struct dirent *entry;
    
        // dir con problemi, allora problemi con la dir esco con errore
        if (!dir) {
            perror("Errore apertura .trash");
            exit(EXIT_FAILURE);
        }
    
        // Legge ogni file in dir fino a che non e' vuoto e stampo il nome
        while ((entry = readdir(dir)) != NULL) {
            // Esclude i file nascosti come "." e ".."
            if (entry->d_name[0] != '.')
                printf("%s\n", entry->d_name);
        }
    
        closedir(dir);
    }
    
    // Svuota completamente il contenuto del cestino
    void purge_trash(const char *trash_path) {
        DIR *dir = opendir(trash_path);
        struct dirent *entry;
        char filepath[MAX_PATH];
    
        if (!dir) {
            perror("Errore apertura .trash");
            exit(EXIT_FAILURE);
        }
    
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] != '.') {
                // Costruisce il path mettendo il path per la cartella trash + il file appena letto da readdir, 
                // si usa snprintf perche' si parla di stringhe dinamiche, che cambiano ad ogni iter
                snprintf(filepath, MAX_PATH, "%s/%s", trash_path, entry->d_name);
                unlink(filepath);  // rimuove il file
            }
        }
    
        closedir(dir);
    }
    
    /* Sposta il file nel cestino, se esiste gia un file con lo stesso nome, bisogna:
        - Rinominare il file esistende aggiungendo la data (timestamp)
        - Solo dopo, spostare il nuovo file nel cestino col suo nome originale
    */ 
    void move_to_trash(const char *trash_path, const char *filename) {
        char new_path[MAX_PATH];
        snprintf(new_path, MAX_PATH, "%s/%s", trash_path, filename);
    
        // Se il file esiste gia nel cestino
        if (access(new_path, F_OK) == 0) {   // access controlla in questo caso avendo F_OK, se esiste il file
            char backup_path[MAX_PATH];
            time_t now = time(NULL);  // Ottieni timestamp attuale
            // Prende il path di prima e aggiunge il timestamp attuale
            snprintf(backup_path, MAX_PATH, "%s/%s.%ld", trash_path, filename, now);
    
            // Rinominare il file gia nel cestino con timestamp
            if (rename(new_path, backup_path) == -1) {
                perror("Errore nel rinominare il file esistente nel cestino");
                exit(EXIT_FAILURE);
            }
        }
    
        // Ora possiamo spostare il nuovo file normalmente
        if (rename(filename, new_path) == -1) {
            perror("Errore spostamento nel cestino");
            exit(EXIT_FAILURE);
        }
    }
    
    // Ripristina un file dal cestino nella directory corrente
    void restore_file(const char *trash_path, const char *filename) {
        char from[MAX_PATH];
        snprintf(from, MAX_PATH, "%s/%s", trash_path, filename);
    
        // Sposta il file dal cestino alla directory corrente
        if (rename(from, filename) == -1) {
            perror("Errore ripristino file");
            exit(EXIT_FAILURE);
        }
    }
    
    int main(int argc, char *argv[]) {
        if (argc < 2) usage();  // almeno un argomento necessario
    
        char trash_path[MAX_PATH];
        const char *home = getenv("HOME");  // recupera la directory home dell'utente
        snprintf(trash_path, MAX_PATH, "%s/.trash", home);  // costruisce il path di .trash
    
        ensure_trash_exists(trash_path);  // assicura che .trash esista
    
        // Gestione opzione -L → lista il cestino
        if (strcmp(argv[1], "-L") == 0) {
            if (argc != 2) usage();  // opzione esclusiva
            list_trash(trash_path);
        }
        // Gestione opzione -P → svuota il cestino
        else if (strcmp(argv[1], "-P") == 0) {
            if (argc != 2) usage();  // opzione esclusiva
            purge_trash(trash_path);
        }
        // Gestione opzione -R file → ripristina file dal cestino
        else if (strcmp(argv[1], "-R") == 0) {
            if (argc != 3) usage();  // richiede un file
            restore_file(trash_path, argv[2]);
        }
        // Nessuna opzione: sposta i file nel cestino
        else {
            for (int i = 1; i < argc; ++i) {
                move_to_trash(trash_path, argv[i]);
            }
        }
    
        return 0;
    }
    
    ```
    
    ### Fase test
    
    - Descrizione di cosa è stato fatto
        
        Perfetto! Ora che hai capito tutto il codice, ecco **come eseguire `saferm` correttamente** e **testarlo passo-passo**, seguendo le regole del tuo ambiente (✅ no `system()`, ✅ tutto in C puro):
        
        ---
        
        ## ✅ 1. 📂 **Salva il file**
        
        Crea il file `saferm.c` nella tua home o in una directory di lavoro:
        
        ```bash
        nano saferm.c
        # Incolla dentro il codice completo
        # Salva con CTRL+O, invio, poi CTRL+X
        
        ```
        
        ---
        
        ## 🛠️ 2. **Compila con `gcc`**
        
        Compila in modo sicuro:
        
        ```bash
        gcc -Wall saferm.c -o saferm
        
        ```
        
        - `Wall`: attiva tutti i warning
        - `o saferm`: produce l’eseguibile chiamato `saferm`
        
        Verifica che funzioni:
        
        ```bash
        ./saferm -L    # dovrebbe mostrare "niente" se .trash è vuota
        
        ```
        
        ---
        
        ## 🔬 3. 🧪 **Test passo passo**
        
        ### 🔸 A. Prepara ambiente
        
        ```bash
        mkdir ~/test_trash
        cd ~/test_trash
        echo "ciao" > file1.txt
        echo "mondo" > file2.txt
        
        ```
        
        ---
        
        ### 🔸 B. Testa lo spostamento nel cestino
        
        ```bash
        ~/saferm file1.txt file2.txt
        
        ```
        
        Controlla che i file siano finiti in `~/.trash`:
        
        ```bash
        ls ~/.trash
        # Output atteso: file1.txt  file2.txt
        
        ```
        
        ---
        
        ### 🔸 C. Test `L` (elenca cestino)
        
        ```bash
        ~/saferm -L
        # Output atteso: file1.txt file2.txt
        
        ```
        
        ---
        
        ### 🔸 D. Test `R` (ripristina)
        
        ```bash
        ~/saferm -R file1.txt
        ls             # file1.txt dovrebbe essere tornato qui
        ls ~/.trash    # dovrebbe restare solo file2.txt
        
        ```
        
        ---
        
        ### 🔸 E. Test `P` (pulisce cestino)
        
        ```bash
        ~/saferm -P
        ls ~/.trash
        # Output atteso: directory vuota
        ```
        
        ### 🔸 F. Test file con lo stesso nome
        
        1. Crea due file con lo stesso nome:
            
            ```bash
            echo "prima versione" > test.txt
            ./saferm test.txt
            echo "seconda versione" > test.txt
            ./saferm test.txt
            ```
            
        2. Poi guarda il contenuto del cestino:
            
            ```bash
            ls ~/.trash
            cat ~/.trash/test.txt
            cat ~/.trash/test.txt.*
            ```
            
        3. Risultato atteso:
            - `test.txt` → ultima versione
            - `test.txt.XXXXXXXXXX` → versione vecchia salvata
        
        ---
        
        ## 🧪 4. Testa anche errori
        
        ### ❌ File non esiste
        
        ```bash
        ~/saferm pippo.txt
        # Output: Errore spostamento nel cestino: No such file or directory
        ```
        
        ### ❌ Uso errato delle opzioni
        
        ```bash
        ~/saferm -L -P
        # Output: messaggio di uso errato
        ```
        
        ---
        
        ## ✅ In sintesi
        
        | Operazione | Comando |
        | --- | --- |
        | Compilazione | `gcc -Wall saferm.c -o saferm` |
        | Spostamento file | `./saferm file.txt` |
        | Lista cestino | `./saferm -L` |
        | Ripristina file | `./saferm -R file.txt` |
        | Svuota cestino | `./saferm -P` |
        
        ---
        
        Se vuoi, posso anche creare un piccolo **Makefile** per automatizzare la compilazione. Vuoi che lo generi?
        
    
    ```c
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ./saferm -L
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ mkdir testTrash
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ cd testTrash/
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem/testTrash$ echo "ciao" > file1.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem/testTrash$ echo "mondo" > file2.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem/testTrash$ ./saferm file1.txt file2.txt
    -bash: ./saferm: No such file or directory
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem/testTrash$ ../saferm file1.txt file2.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem/testTrash$ ls
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem/testTrash$ cd ..
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls
    saferm  saferm.c  testTrash
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls ~/.trash
    file1.txt  file2.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ./saferm -L
    file1.txt
    file2.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ./saferm -R file1.t
    xt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls
    file1.txt  saferm  saferm.c  testTrash
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ mv file1.txt testTrash/
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls
    saferm  saferm.c  testTrash
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls ~/.trash
    file2.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ~/saferm -P
    -bash: /home/gabbo/saferm: No such file or directory
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ./saferm -P
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls ~/.trash
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ./saferm pippo.txt
    Errore spostamento nel cestino: No such file or directory
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ./saferm -L -P
    Uso: saferm [-L | -P | -R file | file ...]
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ echo "prima versione" > test.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ gcc -Wall saferm.c
    -o saferm
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ echo "prima versione" > test.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls
    saferm  saferm.c  test.txt  testTrash
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ./saferm test.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ echo "seconda versione" > test.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls
    saferm  saferm.c  test.txt  testTrash
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ./saferm test.txt
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ ls ~/.trash
    test.txt  test.txt.1749726063
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ cat ~/.trash/test.txt
    seconda versione
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$ cat ~/.trash/test.txt.*
    prima versione
    gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esPDF/Safrem$
    ```
    
- Esercizio 2 (pag. 130)
    
    ![image.png](attachment:1133c933-a5df-4306-a259-f84082f66e1b:image.png)
    
    ### Soluzione
    
    ```c
    #include <stdio.h>      // per fopen, fread, fwrite, printf, etc.
    #include <stdlib.h>     // per malloc, free, exit
    #include <string.h>     // per strcmp, strstr, strlen
    #include <dirent.h>     // per opendir, readdir, closedir
    #include <sys/stat.h>   // per stat
    #include <unistd.h>     // per access()
    #include <ctype.h>      // per tolower()
    
    #define MAX_PATH 4096
    
    // Legge il contenuto del file da mettere poi come scritta nei file, restituisce una stringa
    char *load_file_content(const char *path) {
        FILE *file = fopen(path, "r");  // Legge il file, al path specificato, r determina che deve solo leggere
        if !file
            return NULL;
    
        fseek(file, 0, SEEK_END);   // Sposta il cursore alla fine del file per calcolare la dimensione
        // Restituisce la posizione corrente del cursore, quindi qui corrisponde alla dimensione del file in byte
        long size = ftell(file);  
        rewind(file);  // Riporta il cursore all'inizio del file per poterlo leggere dall'inizio
    
        // Alloca dinamicamente un buffer abbastanza grande da contenere l'intero file + 1 byte per il terminatore '\0'
        char *buffer = malloc(size + 1);
        if (!buffer) return NULL;
    
        fread(buffer, 1, size, file);  // Legge il contenuto del file nel buffer
        buffer[size] = '\0';  // Aggiunge il carattere di terminazione della stringa
        fclose(file);   // Chiude il file dopo averlo letto
        return buffer;  // Restituisce il contenuto del file
    }
    
    // Scansione ricorsiva di directory
    void process_directory(const char *dir_path, const char *header_text, const char *keyword,
                           const char *extension, int case_sensitive) {
        DIR *dir = opendir(dir_path);  // Apriamo la dir
        if (!dir) {
            perror("Errore apertura directory");
            return;
        }
    
        // Puntatore a una struttura che rappresenta un file o directory dentro la cartella
        struct dirent *entry;
    
        
        // Scorre tutti i file e cartelle all interno della directory aperta precedentemente    
        while ((entry = readdir(dir)) != NULL) {
            // Salta le directory speciali "." (corrente) e ".." (padre)
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
    
            // Costruisce il path assoluto del file corrente: path = dir_path + "/" + nome_file
            char path[MAX_PATH];
            snprintf(path, MAX_PATH, "%s/%s", dir_path, entry->d_name);
    
            // Usa stat per ottenere info sul file; se fallisce (es. file eliminato nel frattempo), salta
            struct stat st;
            if (stat(path, &st) == -1) continue;
    
            // Se e' una directory, chiama ricorsivamente la stessa funzione per esplorarla
            if (S_ISDIR(st.st_mode)) {
                process_directory(path, header_text, keyword, extension, case_sensitive);
            
            // Se e' un file normale (non directory, link, ecc.) e ha l''estensione giusta
            } else if (S_ISREG(st.st_mode) && has_extension(path, extension)) {
                // Inserisce l’header nel file, se necessario
                insert_header_if_needed(path, header_text, keyword, case_sensitive);
            }
        }
        closedir(dir);
    }
    
    // Inserisce header se la keyword non e' presente nel file
    void insert_header_if_needed(const char *filepath, const char *header_text, const char *keyword, int case_sensitive) {
        char *original_content = load_file_content(filepath);   // Carica il contenuto del file in memoria
        if (!original_content) return;
    
        // Se il file non contiene la keyword
        if (!contains_keyword(original_content, keyword, case_sensitive)) {
            FILE *file = fopen(filepath, "w");  // Apri il file in modalità scrittura
            if (!file) {
                perror("Errore apertura file per scrittura");
                free(original_content);
                return;
            }
            // Scrive prima il testo dell'header, poi il contenuto originale del file
            fprintf(file, "%s\n%s", header_text, original_content);
            fclose(file);  // Poi chiude
        }
        free(original_content);
    }
    
    /* Funzione che controlla se una stringa (content) contiene una parola chiave (keyword)
       -> Restituisce 1 se la parola chiave viene trovata, 0 altrimenti
       case_sensitive: se 0 -> ignora maiuscole/minuscole; se 1 -> confronto esatto
    */
    int contains_keyword(const char *content, const char *keyword, int case_sensitive) {
        if (!keyword) return 0;
    
        // Se non e' case sensitive
        if (!case_sensitive) {
            // Scorre ogni stringa di content e la confronta con ogni stringa di content
            for (int i = 0; content[i]; i++) {
                int match = 1;
                for (int j = 0; keyword[j]; j++) {
                    /* tolower prende un carattere maiuscolo o minuscolo e lo trasfoema in minuscolo, 
                       se il carattere di content e di keyword e' diverso, allora non fa match e diventa 0 */
                    if (tolower(content[i + j]) != tolower(keyword[j])) {
                        match = 0;
                        break;
                    }
                }
                if (match) return 1;
            }
            return 0;
        } else {
            // Ricerca case sensitive, content contiene keyword
            return strstr(content, keyword) != NULL;
        }
    }
    
    // Controlla se un file ha una determinata estensione
    int has_extension(const char *filename, const char *ext) {
        if (!ext) return 1;
        const char *dot = strrchr(filename, '.');   // Controlla se ha il punto, quindi ha un estensione
        return dot && strcmp(dot, ext) == 0;   // Ritorna quello che c'e' dopo il punto
    }
    
    int main(int argc, char *argv[]) {
        // Controllo numero minimo di argomenti
        if (argc < 3) {
            fprintf(stderr, "Uso: %s <directory> <file_header> [keyword] [estensione] [case-sensitive=0|1]\n", argv[0]);
            return EXIT_FAILURE;
        }
    
        // Argomenta da linea di comenado
    
        // argv[1] -> directory da esplorare (es: ./src)
        const char *directory_path = argv[1];
    
        // argv[2] -> path al file di testo da inserire all'inizio di ogni file (es: copyright.txt)
        const char *header_file_path = argv[2];
    
        // argv[3] (opzionale) -> parola chiave che NON deve essere presente nei file (es: "COPYRIGHT")
        const char *keyword = (argc >= 4) ? argv[3] : NULL;  // argc >= 4, perche' se viene messa la keyword ci saranno almeno 4 argomenti
    
        // argv[4] (opzionale) -> estensione dei file da modificare (es: .c, .txt, .py)
        const char *extension = (argc >= 5) ? argv[4] : NULL;
    
        // argv[5] (opzionale) -> 0 = non case-sensitive (default), 1 = case-sensitive
        int case_sensitive = (argc >= 6) ? atoi(argv[5]) : 0;
        // --> atoi trasfoma una stringa in intero, se la stringa non e traducibile tipo "ciao", allora scrive 0
    
        /* Legge il contenuto del file header*/
    
        // Carica il contenuto del file (quello da aggiungere a inizio di ogni file)
        char *header_text = load_file_content(header_file_path);
    
        // Se non lo trova errore
        if (!header_text) {
            fprintf(stderr, "Errore nella lettura del file header: %s\n", header_file_path);
            return EXIT_FAILURE;
        }
    
        // Avvia la scansione ricorsiva della directory
        process_directory(directory_path, header_text, keyword, extension, case_sensitive);
    
        free(header_text);   // Scuota il puntatore a header_text
        return EXIT_SUCCESS;
    }
    ```
    
    - Spiegazione logica
        
        L’obiettivo è:
        
        > Visitare ricorsivamente una directory, e per ogni file con una certa estensione, se non contiene una certa parola chiave, inserire in cima il contenuto di un file “header”.
        > 
        
        ---
        
        ## 🔁 **Flusso di esecuzione (ordine logico)**
        
        ### `main()`
        
        1. **Legge gli argomenti** da linea di comando:
            - `directory_path` → cartella da esplorare
            - `header_file_path` → file che contiene il testo da inserire
            - `keyword` → parola da evitare (opzionale)
            - `extension` → estensione dei file su cui intervenire (opzionale)
            - `case_sensitive` → se 1, distingue maiuscole/minuscole nella keyword
        2. **Carica il contenuto dell’header** con:
            
            ```c
            char *header_text = load_file_content(header_file_path);
            
            ```
            
            - Questa funzione legge e restituisce una `stringa` con il contenuto del file header.
        3. **Chiama la funzione principale:**
            
            ```c
            process_directory(directory_path, header_text, keyword, extension, case_sensitive);
            
            ```
            
            - Da qui inizia la **scansione ricorsiva** della directory.
        
        ---
        
        ### `process_directory(...)`
        
        1. Apre la directory con `opendir`.
        2. Scorre ogni voce contenuta (`readdir`) e per ognuna:
            - **Salta** `"."` e `".."`.
            - Costruisce il path assoluto (con `snprintf`).
            - Recupera info sul file (`stat`):
                - Se è una **directory**, la esplora **ricorsivamente** → chiamata a sé stessa.
                - Se è un **file normale** con estensione corretta:
                    
                    ```c
                    insert_header_if_needed(path, header_text, keyword, case_sensitive);
                    
                    ```
                    
        
        ---
        
        ### `insert_header_if_needed(...)`
        
        1. **Carica il contenuto originale del file**:
            
            ```c
            char *original_content = load_file_content(filepath);
            
            ```
            
        2. Se il file **non contiene la parola chiave**, allora:
            - Apre il file in modalità scrittura (`fopen(..., "w")`)
            - Scrive prima l’`header_text`, poi il contenuto originale
        
        ---
        
        ### `load_file_content(...)`
        
        Questa funzione:
        
        1. Apre un file in sola lettura.
        2. Usa `fseek + ftell` per calcolarne la dimensione.
        3. Alloca un buffer.
        4. Usa `fread` per caricare tutto il contenuto.
        5. Ritorna il buffer con il testo letto.
        
        ---
        
        ### `contains_keyword(...)`
        
        1. Se `case_sensitive == 0`: confronta trasformando tutto in minuscolo (`tolower`)
        2. Se `case_sensitive == 1`: usa `strstr()` per trovare esattamente la keyword.
        
        ---
        
        ### `has_extension(...)`
        
        1. Cerca l’ultimo `.` nel nome file (`strrchr`)
        2. Confronta ciò che segue con l’estensione data (`strcmp`)
        
        ---
        
        ## ✅ Riassunto del **flusso chiamate**
        
        ```
        main
        └── load_file_content        // legge il file header
        └── process_directory        // esplora tutta la dir
            ├── has_extension        // controlla estensione
            ├── insert_header_if_needed
            │   ├── load_file_content
            │   └── contains_keyword // decide se inserire o no
        
        ```
        
        ---
        
        Se vuoi, posso fornirti anche **un diagramma a blocchi** della logica o una **lista di test per verificarlo**. Fammi sapere!
        
    
    ### Fase di Test
    
    - **TEST 1**
        
        ```bash
        ./inserth src header.txt
        
        ```
        
        ### ✅ **Obiettivo**
        
        Inserire l’header in **tutti i file** dentro `src/` (e sottocartelle), **senza alcun filtro**.
        
        ### 🔍 Comportamento atteso
        
        - Ogni file, di qualunque tipo e contenuto, riceve l’header.
        - Non vengono controllate keyword o estensioni.
        
        ### ✅ Risultato
        
        - Header inserito in **tutti i file**: `.txt`, `.c`, `.md`, ecc.
        
        ---
        
    - **TEST 2**
        
        ```bash
        ./inserth src header.txt "" .txt 0
        
        ```
        
        ### ✅ **Obiettivo**
        
        Inserire l’header **solo nei file `.txt`**, senza escludere keyword (stringa vuota), e in **modo case-insensitive**.
        
        ### 🔍 Comportamento atteso
        
        - Solo file `.txt` modificati.
        - Vengono ignorati tutti gli altri file.
        - Non si tiene conto di cosa contiene il file.
        
        ### ✅ Risultato
        
        - Solo `file1.txt` e `file2.txt` modificati.
        
        ---
        
    - **TEST 3**
        
        ```bash
        ./inserth src header.txt KEYWORD "" 1
        
        ```
        
        ### ✅ **Obiettivo**
        
        Inserire l’header in **tutti i file**, **tranne** quelli che **già contengono la parola `KEYWORD` (case-sensitive)**.
        
        ### 🔍 Comportamento atteso
        
        - `file2.txt` → contiene `"KEYWORD"` → **non modificato**
        - `file4.c` → contiene `"KEYWORD"` → **non modificato**
        - Gli altri file ricevono l’header.
        
        ### ✅ Risultato atteso (dopo test su cartella pulita)
        
        - `file1.txt`, `file3.c`, `file5.md` → **modificati**
        - `file2.txt`, `file4.c` → **non modificati**
        
        ---
        
    - **Comando finale di verifica**
        
        ```bash
        grep -R "HEADER TEST" -n src
        
        ```
        
        ### ✅ **Scopo**
        
        Controllare quali file sono stati effettivamente modificati (cioè, che hanno ricevuto l’header).
        
        ### ✅ **Risultato coerente**
        
        Se i test sono eseguiti in ordine corretto, **solo i file ammessi dai filtri** conterranno la stringa `HEADER TEST`.
        
        ---
        
    - **Suggerimento**
        
        Per testare sempre da zero:
        
        ```bash
        rm -rf src && mkdir -p src/dirA src/dirB
        echo "ciao mondo" > src/file1.txt
        echo "Questo contiene KEYWORD" > src/file2.txt
        echo "int main() { return 0; }" > src/dirA/file3.c
        echo "// KEYWORD è qui" > src/dirB/file4.c
        echo "# markdown" > src/dirB/file5.md
        
        ```
        

### Esame

- 13/02/2025
    
    ![image.png](attachment:d3444330-5be6-46d2-b6e4-92a6b666e90e:image.png)
    
    - Es. 1
        
        ```c
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
        
        int main(int argc, char *argv[]) { 
        
            // Controllo che ci siano gli aromenti necessari, se no gli scrivo cosa deve fare ed esco
            if (argc != 4) {
                fprintf(stderr, "Uso: %s [-s|-l] <file> <directory>\n", argv[0]);
                return EXIT_FAILURE;
            }
        
            const char *option = argv[1];
            const char *target_file = argv[2];
            const char *start_dir = argv[3];
        
            // Verifico info sul file e se non esiste, allora errore di permessi probabile
            struct stat st;
            if (stat(target_file, &st) != 0) {
                perror("Errore stat file");
                return EXIT_FAILURE;
            }
        
            if (strcmp(option, "-s") == 0) {
                search_links(target_file, start_dir, 1, 0);
            } else if (strcmp(option, "-l") == 0) {
                search_links(target_file, start_dir, 0, st.st_ino);
            } else {
                fprintf(stderr, "Opzione non valida. Usa -s o -l.\n");
                return EXIT_FAILURE;
            }
        
            return EXIT_SUCCESS;
        }
        ```
        
        ### Test
        
        - TEST 1 — Errore su file inesistente
            
            ```bash
            ./ckfile -s inesistente.txt testdir
            
            ```
            
            ✅ Deve stampare: `Errore stat file: No such file or directory`
            
        - TEST 2 — Link simbolici corretti
            
            ```bash
            ./ckfile -s testdir/file.txt testdir
            
            ```
            
            ✅ Deve stampare solo i **link simbolici** che puntano a `file.txt`, ad esempio:
            
            ```
            Link simbolico trovato: testdir/symlink1.txt -> ...
            Link simbolico trovato: testdir/subdir/symlink2.txt -> ...
            
            ```
            
            ❌ **NON** devono apparire hard link o file normali.
            
        - TEST 3 — Link fisici (hardlink)
            
            ```bash
            ./ckfile -l testdir/file.txt testdir
            
            ```
            
            ✅ Deve stampare:
            
            - `testdir/file.txt`
            - `testdir/hardlink1.txt`
            - `testdir/subdir/hardlink2.txt`
            
            ❌ Non deve stampare link simbolici come `symlink1.txt`.
            
        - TEST 4 — Uso errato (argomenti insufficienti)
            
            ```bash
            ./ckfile
            
            ```
            
            ✅ Deve stampare: `Uso: ./ckfile [-s|-l] <file> <directory>`
            
        - TEST 5 — Opzione errata
            
            ```bash
            ./ckfile -x testdir/file.txt testdir
            
            ```
            
            ✅ Deve stampare: `Opzione non valida. Usa -s o -l.`
            
    - Es. 2
        
        ```c
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
        ```
        
        ### Test
        
        **Ambiente di Test Creato**
        
        Hai creato una directory chiamata `testdir` contenente i seguenti file:
        
        | Percorso | Contenuto | Scopo |
        | --- | --- | --- |
        | `testdir/file.txt` | Test file di riferimento | 🔹 File base |
        | `testdir/uguale.txt` | Copia identica di `file.txt` | ✅ Deve essere match completo |
        | `testdir/prefisso.txt` | Stessi primi 10 byte di `file.txt` | ✅ Deve essere match con `-p 10`, ❌ con match completo |
        | `testdir/diverso.txt` | Contenuto completamente diverso | ❌ Nessun match |
        | `testdir/subdir/copia.txt` | Altra copia identica di `file.txt` in sottodirectory | ✅ Deve essere match completo e con prefisso |
        
        **Test Eseguiti**
        
        - **Test di contenuto completo**
            
            ```bash
            ./ckfile testdir/file.txt testdir
            ```
            
            📌 **Aspettativa**: stampa tutti i file che hanno lo **stesso contenuto** di `file.txt`.
            
            📤 **Output corretto**:
            
            ```
            File con contenuto uguale: testdir/file.txt
            File con contenuto uguale: testdir/uguale.txt
            File con contenuto uguale: testdir/subdir/copia.txt
            ```
            
        - **Test con prefisso**
            
            ```bash
            ./ckfile -p 10 testdir/file.txt testdir
            
            ```
            
            📌 **Aspettativa**: stampa tutti i file che iniziano con **gli stessi 10 byte** di `file.txt`.
            
            📤 **Output corretto**:
            
            ```
            File con prefisso uguale: testdir/file.txt
            File con prefisso uguale: testdir/prefisso.txt
            File con prefisso uguale: testdir/uguale.txt
            File con prefisso uguale: testdir/subdir/copia.txt
            
            ```
            
    - Es. 3
        
        ```c
        # same_size.py
        # Obiettivo: trovare tutti i file in una directory (ricorsivamente)
        # che hanno la stessa dimensione di un file f, ma NON sono suoi hard link
        # (cioè devono avere inode diverso)
        
        import os
        import sys
        
        def same_size_not_hardlink(target_path, root_dir):
            # Ottieni le informazioni del file target
            try:
                target_stat = os.stat(target_path)
            except OSError:
                return  # Se il file non esiste o non è leggibile
        
            target_size = target_stat.st_size   # numero di byte
            target_inode = target_stat.st_ino   # inode (per identificare hard link)
        
            # Esplora ricorsivamente il sottoalbero partendo da root_dir
            for dirpath, _, filenames in os.walk(root_dir):
                for name in filenames:
                    full_path = os.path.join(dirpath, name)
        
                    try:
                        stat_info = os.stat(full_path)
                    except OSError:
                        continue  # Se non posso accedere a quel file, lo ignoro
        
                    # Stampa solo se:
                    # - stessa dimensione in byte
                    # - inode diverso (quindi non hard link)
                    if stat_info.st_size == target_size and stat_info.st_ino != target_inode:
                        print(full_path)
        
        if __name__ == "__main__":
            # Controlla numero di argomenti
            if len(sys.argv) != 3:
                print("Uso: python3 same_size.py <file> <directory>")
                sys.exit(1)
        
            # Lancia la funzione
            same_size_not_hardlink(sys.argv[1], sys.argv[2])
        
        ```
        
- 21/01/2025
    
    ![image.png](attachment:c1675791-6526-4bf3-8913-aa84e6c73395:image.png)
    
    - Es. 1
        
        ```c
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
        ```
        
        ### Test
        
        - **Crea una directory di lavoro:**
            
            ```bash
            mkdir testdir
            cd testdir
            ```
            
        - **Crea un file originale (target):**
            
            ```bash
            echo "Questo è un file di esempio." > file.txt
            ```
            
        - **Crea un link fisico a `file.txt` (non deve essere stampato):**
            
            ```bash
            ln file.txt hardlink.txt
            ```
            
        - **Crea un file con identico contenuto (ma inode diverso):**
            
            ```bash
            cp file.txt copia.txt
            ```
            
        - **Crea un file con stessa dimensione ma contenuto diverso:**
            
            ```bash
            echo "Questo è un altro contenuto." > falso.txt
            truncate -s $(stat -c%s file.txt) falso.txt  # Forza stessa dimensione
            ```
            
        - **Crea una sottodirectory con un file valido:**
            
            ```bash
            mkdir subdir
            cp file.txt subdir/altro.txt
            ```
            
        - **Crea un link simbolico (non deve essere considerato):**
            
            ```bash
            ln -s file.txt symlink.txt
            ```
            
        - **Esegui il programma:**
            
            Assumendo che il binario sia `samecont` e che tu sia nella cartella *superiore* a `testdir`, esegui:
            
            ```bash
            ./samecont testdir/file.txt testdir
            
            ```
            
        
        ### 📌 **Ti aspetti di vedere stampati (stessa size, inode diverso):**
        
        ```
        testdir/copia.txt
        testdir/falso.txt
        testdir/subdir/altro.txt
        
        ```
        
        ### 🛑 **Non devono comparire:**
        
        - `hardlink.txt` (stesso inode → link fisico)
        - `symlink.txt` (è un link simbolico)
    - Es. 2
        
        ```c
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <dirent.h>
        #include <sys/stat.h>
        #include <unistd.h>
        #include <limits.h>
        
        #define MAX_PATH 4096
        /*
            Esercizio 2: Linguaggio C: 10 punti Scrivere un programma che presi come parametri i pathname di
            un file f e di una directory d stampi l'elenco dei link simbolici che puntano a f presenti nel sottoalbero
            del file system generato dalla directory d.
        */
        
        void search_symlinks(const char *target_file, const char *dir_path, const char *abs_target) {
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
        
                // Costruisce il percorso assoluto del file corrente
                char full_path[MAX_PATH];
                snprintf(full_path, MAX_PATH, "%s/%s", dir_path, entry->d_name);
        
                struct stat st;
                if (lstat(full_path, &st) == -1) {
                    perror("Errore stat file");
                    continue;
                }
        
                // Se è un link simbolico
                if (S_ISLNK(st.st_mode)) {
                    char resolved[MAX_PATH];
                    ssize_t len = readlink(full_path, resolved, sizeof(resolved) - 1);
                    if (len != -1) {
                        resolved[len] = '\0'; // Termina la stringa
        
                        char abs_resolved[MAX_PATH];
                        if (realpath(resolved, abs_resolved)) {
                            if (strcmp(abs_resolved, abs_target) == 0) {
                                printf("Link simbolico trovato: %s -> %s\n", full_path, abs_resolved);
                            }
                        }
                    }
                } else if (S_ISDIR(st.st_mode)) {
                    // Se è una directory, chiama ricorsivamente su di essa
                    search_symlinks(target_file, full_path, abs_target);
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
        
            char abs_target[MAX_PATH];
            // Ottiene il percorso assoluto del file target
            if (realpath(target_file, abs_target) == NULL) {
                perror("Errore nel risolvere il percorso assoluto del file target");
                return EXIT_FAILURE;
            }
        
            // Chiama la funzione per cercare file con stessa dimensione ma inode diverso
            search_symlinks(target_file, start_dir, abs_target);
        
            return EXIT_SUCCESS;
        }
        ```
        
        ### Test
        
        ### ✅ **1. Crea l’ambiente di test**
        
        ```bash
        mkdir -p test_slink/subdir
        cd test_slink
        echo "ciao" > file.txt
        echo "altro" > altro.txt
        ln -s ../file.txt subdir/link_to_file.txt
        ln -s ../altro.txt subdir/link_to_altro.txt
        ln -s "$(realpath file.txt)" subdir/abs_link.txt
        
        ```
        
        🔎 Ora controlla che siano stati creati:
        
        ```bash
        ls -l
        ls -l subdir
        
        ```
        
        ---
        
        ### ✅ **2. Compila il tuo script**
        
        Se il file sorgente è ad esempio `script.c`, compila con:
        
        ```bash
        gcc -Wall script.c -o symlink_finder
        
        ```
        
        ---
        
        ### ✅ **3. Lancia il programma**
        
        Supponiamo che il binario si chiami `symlink_finder`. Esegui:
        
        ```bash
        ./symlink_finder test_slink/file.txt test_slink
        
        ```
        
        ---
        
        ### ✅ **4. Cosa deve stampare**
        
        L'output **corretto** dovrebbe essere:
        
        ```
        Link simbolico trovato: test_slink/subdir/link_to_file.txt -> test_slink/file.txt
        Link simbolico trovato: test_slink/subdir/abs_link.txt -> /path/assoluto/file.txt
        
        ```
        
        🔴 **Non deve stampare** il link `link_to_altro.txt` (perché punta a `altro.txt`).
        
        ---
        
        ### Vuoi che generi uno script `.sh` con tutto questo per automatizzare i test?
        
    - Es. 3
        
        ```c
        # depth_sort.py
        #
        # Obiettivo: stampare i file presenti nel sottoalbero di una directory,
        # ordinati prima per profondità (più profondi prima),
        # poi per ordine alfabetico (lessicografico crescente)
        
        import os
        import sys
        
        # Calcola la profondità di un path contando gli slash "/"
        def profondita(path):
            return path.count(os.sep)
        
        # Esplora la directory e raccoglie i file
        def lista_ordinata(root):
            result = []
        
            # os.walk visita ricorsivamente tutte le directory
            for dirpath, _, filenames in os.walk(root):
                for f in filenames:
                    full_path = os.path.join(dirpath, f)
                    result.append(full_path)
        
            # Ordina:
            #  - prima per profondità (più alta → valore più negativo)
            #  - poi per nome crescente
            result.sort(key=lambda p: (-profondita(p), p))
            return result
        
        if __name__ == "__main__":
            # Controllo argomenti
            if len(sys.argv) != 2:
                print("Uso: python3 depth_sort.py <directory>")
                sys.exit(1)
        
            # Stampa i risultati ordinati
            for path in lista_ordinata(sys.argv[1]):
                print(path)
        
        ```
        
- 11/09/2024
    
    ![image.png](attachment:639ee4b3-a488-439d-826c-09e4b04eadc3:image.png)
    
    - Es. 1
        
        ```c
        /*
         * timeout.c
         *
         * Obiettivo: eseguire un programma e terminarlo se impiega più di N millisecondi.
         * Uso: ./timeout 5000 sleep 2
         *      ./timeout 3000 sleep 5
         *
         * Implementazione:
         * - fork del processo figlio
         * - pidfd_open per controllare lo stato del processo
         * - timerfd_create per impostare un timeout preciso
         * - poll per attendere l’evento che avviene per primo (fine processo o scadenza timer)
         */
        
        #define _GNU_SOURCE
        #include <stdio.h>
        #include <stdlib.h>
        #include <unistd.h>
        #include <sys/timerfd.h>
        #include <sys/poll.h>
        #include <sys/types.h>
        #include <sys/wait.h>
        #include <sys/syscall.h>
        #include <fcntl.h>
        #include <string.h>
        #include <errno.h>
        
        // wrapper per syscall pidfd_open (non c’è in tutte le libc)
        int pidfd_open(pid_t pid, unsigned int flags) {
            return syscall(SYS_pidfd_open, pid, flags);
        }
        
        int main(int argc, char *argv[]) {
            // Controlla numero argomenti
            if (argc < 3) {
                fprintf(stderr, "Uso: %s <timeout_ms> <programma> [args...]\n", argv[0]);
                return 1;
            }
        
            // Legge timeout in millisecondi
            int timeout_ms = atoi(argv[1]);
            if (timeout_ms <= 0) {
                fprintf(stderr, "Durata non valida.\n");
                return 1;
            }
        
            // Crea un processo figlio
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                return 1;
            }
        
            if (pid == 0) {
                // figlio: esegue il programma richiesto
                execvp(argv[2], &argv[2]);
                perror("execvp");
                exit(1);
            }
        
            // padre: controlla stato del processo figlio
            int pidfd = pidfd_open(pid, 0);
            if (pidfd == -1) {
                perror("pidfd_open");
                return 1;
            }
        
            // crea un timer (non bloccante)
            int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
            if (tfd == -1) {
                perror("timerfd_create");
                return 1;
            }
        
            // configura il timer
            struct itimerspec ts = {0};
            ts.it_value.tv_sec = timeout_ms / 1000;
            ts.it_value.tv_nsec = (timeout_ms % 1000) * 1000000;
            if (timerfd_settime(tfd, 0, &ts, NULL) == -1) {
                perror("timerfd_settime");
                return 1;
            }
        
            // poll su entrambi: pidfd (fine processo) e tfd (timeout)
            struct pollfd pfd[2];
            pfd[0].fd = pidfd;
            pfd[0].events = POLLIN;
            pfd[1].fd = tfd;
            pfd[1].events = POLLIN;
        
            int poll_res = poll(pfd, 2, -1);
            if (poll_res == -1) {
                perror("poll");
                return 1;
            }
        
            // timeout scaduto
            if (pfd[1].revents & POLLIN) {
                kill(pid, SIGKILL);
                printf("Timeout raggiunto. Processo terminato.\n");
            }
            // processo finito da solo
            else if (pfd[0].revents & POLLIN) {
                printf("Processo completato entro il timeout.\n");
            }
        
            // aspetta la terminazione del processo (evita zombie)
            waitpid(pid, NULL, 0);
            return 0;
        }
        ```
        
    - Es. 3
        
        ```c
        import os
        import sys
        
        def profondita(path):
            return path.count(os.sep)
        
        def è_interno(link_path, root_path):
            try:
                destinazione = os.readlink(link_path)
                abs_target = os.path.abspath(os.path.join(os.path.dirname(link_path), destinazione))
                root_abs = os.path.abspath(root_path)
                return os.path.commonpath([abs_target, root_abs]) == root_abs
            except:
                return False
        
        def scan_symlinks(root):
            interni = []
            esterni = []
            for dirpath, _, _ in os.walk(root):
                for entry in os.listdir(dirpath):
                    path = os.path.join(dirpath, entry)
                    if os.path.islink(path):
                        if è_interno(path, root):
                            interni.append(path)
                        else:
                            esterni.append(path)
            return interni, esterni
        
        if __name__ == "__main__":
            root = sys.argv[1] if len(sys.argv) > 1 else "."
            interni, esterni = scan_symlinks(root)
        
            print("INTERNI:")
            for i in sorted(interni, key=lambda p: (-profondita(p), p)):
                print(os.path.relpath(i, root))
        
            print("ESTERNI:")
            for e in sorted(esterni, key=lambda p: (-profondita(e), e)):
                print(os.path.relpath(e, root))
        
        ```
        
- 23/07/2024
    
    ![image.png](attachment:7cda2022-e481-460a-b792-d0306929b508:image.png)
    
    - Es1
        
        ```c
        #include <stdio.h>
        #include <stdlib.h>
        #include <dirent.h>
        #include <sys/stat.h>
        #include <unistd.h>
        #include <string.h>
        #include <errno.h>
        #include <limits.h>
        
        #define ARCHIVE_DIR "archive"
        #define PATH_MAX 4096
        
        /*
        Esercizio 1: Linguaggio C (obbligatorio) 20 punti
        Scrivere un programma che crei nella directory corrente (se non esiste già) una sottodirectory di
        nome ... (tre punti).
        Tutti i file (regolari) presenti nella directory devono essere spostati nella sottodirectory ... (tre punti) e
        ogni file deve essere sostituito nella dir corrente con un link simbolico (relativo, non assoluto) alla
        nuova locazione. Usare la system call rename per fare la sostituzione in modo atomico (in nessun
        istante il file deve risultare inesistente).
        */
        
        // Controlla se è un file e non un link
        int is_regular_file(const char *path) {
            struct stat st;
            return stat(path, &st) == 0 && S_ISREG(st.st_mode);
        }
        
        int main () {
            DIR *dir = opendir(".");   // Apro la directory attuale
            if (!dir) {
                perror("opendir");
                return 1;
            }
        
            // Creo la sottodirectory se non esiste
            struct stat st;
            if (stat(ARCHIVE_DIR, &st) == -1) {      // Controllo se esiste la dir richiesta
                if (mkdir(ARCHIVE_DIR, 0755) == -1) {  // Se non esiste la creo e le do il permessi
                    perror("mkdir");
                    return 1;
                }
            }
            
            struct dirent *entry; // Funzione associata a opendir, per avere info base sulla dir
            // Itero sulla directory ogni file
            while ((entry = readdir(dir)) != NULL) {
                const char *name = entry->d_name;
        
                // Salta "." e ".."
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;
        
                // Ignoro se è già un link simbolico o non un file
                if (!is_regular_file(name))  
                    continue;
        
                // Costruisce path di destinazione
                char dest_path[PATH_MAX];
                snprintf(dest_path, sizeof(dest_path), "%s/%s", ARCHIVE_DIR, name);
        
                // Sposta il file con rename() in modo atomico
                if (rename(name, dest_path) == -1) {
                    perror("rename");
                    continue;
                }
        
                // Crea il symlink relativo nella directory corrente
                char rel_path[PATH_MAX];
                snprintf(rel_path, sizeof(rel_path), "%s/%s", ARCHIVE_DIR, name);
                if (symlink(rel_path, name) == -1) {
                    perror("symlink");
                }
            }
        
            closedir(dir);
            return 0;
        }
        ```
        
        ### Test
        
        ```bash
        touch a.txt b.txt
        ./archiver
        ls -l
        ls archive/
        
        # Che diventa
        gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esame/230724/es1$ touch a.txt b.txt
        gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esame/230724/es1$ ./archiver
        gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esame/230724/es1$ ls -l
        total 4
        lrwxrwxrwx 1 gabbo gabbo   13 Jun 21 11:22 a.txt -> archive/a.txt
        drwxr-sr-x 2 gabbo gabbo 4096 Jun 21 11:22 archive
        lrwxrwxrwx 1 gabbo gabbo   16 Jun 21 11:22 archiver -> archive/archiver
        lrwxrwxrwx 1 gabbo gabbo   13 Jun 21 11:22 b.txt -> archive/b.txt
        lrwxrwxrwx 1 gabbo gabbo   16 Jun 21 11:22 script.c -> archive/script.c
        gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esame/230724/es1$ ls archive
        a.txt  archiver  b.txt  script.c
        gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esame/230724/es1$
        
        ```
        
    - Es2
        
        ```c
        #include <stdio.h>
        #include <stdlib.h>
        #include <dirent.h>
        #include <unistd.h>
        #include <string.h>
        #include <sys/stat.h>
        #include <errno.h>
        
        #define ARCHIVE_DIR "archive"
        #define PATH_MAX 4096
        
        /*
            Esercizio 2: Linguaggio C: 10 punti
            (undo dell'esercizio 1) Scrivere un programma che sostituisca tutti i link simbolici presenti nella
            directory corrente che puntano a .../nomefile con i veri file che l'esercizio 1 aveva spostato nella
            directory tre punti. Usare la system call rename per fare la sostituzione in modo atomico (in nessun
            istante il file deve risultare inesistente).
        */
        
        // Controlla se il link simbolico punta a un file nella directory "archive"
        int is_symlink_to_archive(const char *path, char *target_buf, size_t bufsize) {
            ssize_t len = readlink(path, target_buf, bufsize - 1); // Legge il link simbolico
            if (len == -1) return 0; 
            target_buf[len] = '\0'; // Termina la stringa
            return (strncmp(target_buf, "archive/", 8) == 0); // Controlla se il link punta a "archive/"
        }
        
        int main(){
            DIR *dir = opendir(".");
            if (!dir) {
                perror("opendir");
                return 1;
            }
        
            struct dirent *entry;
            char link_target[PATH_MAX]; // Path del file che i link devono puntare
        
            while ((entry = readdir(dir)) != NULL) {
                // Continua solo se è un link simbolico
                struct stat sb;
                if (lstat(entry->d_name, &sb) == -1) {
                    perror("lstat");
                    continue;
                }
                if (!S_ISLNK(sb.st_mode)) continue;
        
                // Costruisce il percorso del link simbolico della directory corrente
                if (!is_symlink_to_archive(entry->d_name, link_target, sizeof(link_target))) continue;
        
                // Calcola path src da dove il link simbolico punta
                char src_path[PATH_MAX];
                snprintf(src_path, sizeof(src_path), "%s", link_target);
        
                // Calcola path destinazione (nomefile) dove spostare il file
                char dst_path[PATH_MAX];
                snprintf(dst_path, sizeof(dst_path), "%s", entry->d_name);
        
                // Rimuovi il link simbolico
                if (unlink(dst_path) != 0) {
                    perror("unlink");
                    continue;
                }
        
                // Rinomina il file spostato nella directory "archive" al suo nome originale
                if (rename(src_path, dst_path) != 0) {
                    perror("rename");
                    continue;
                }
        
                printf("Ripristinato: %s\n", dst_path);
            }
        
            closedir(dir);
            return 0;
        }
        ```
        
        ### Test
        
        ```c
        gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esame/230724/es12$ ./undo
        Ripristinato: a.txt
        Ripristinato: archiver
        Ripristinato: b.txt
        Ripristinato: script.c
        gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esame/230724/es12$ ls -la
        total 60
        drwxr-sr-x 3 gabbo gabbo  4096 Jun 21 11:59 .
        drwxr-sr-x 3 gabbo gabbo  4096 Jun 21 11:40 ..
        -rw-r--r-- 1 gabbo gabbo     0 Jun 21 11:22 a.txt
        drwxr-sr-x 2 gabbo gabbo  4096 Jun 21 11:59 archive
        -rwxr-xr-x 1 gabbo gabbo 17248 Jun 21 11:22 archiver
        -rw-r--r-- 1 gabbo gabbo     0 Jun 21 11:22 b.txt
        -rw-r--r-- 1 gabbo gabbo  2446 Jun 21 11:22 script.c
        -rw-r--r-- 1 gabbo gabbo  2434 Jun 21 11:58 script2.c
        -rwxr-xr-x 1 gabbo gabbo 17288 Jun 21 11:59 undo
        gabbo@LAPTOP-0F0BRMGK:~/Sistemi/Pratico/esame/230724/es12$
        ```
        
    - Es3
        
        ```c
        # shadel.py
        #
        # Obiettivo: eliminare da due directory tutti i file che hanno lo stesso hash SHA1
        # - I file devono essere confrontati per contenuto (non per nome)
        # - Se un hash è presente in entrambe, vengono rimossi tutti i file con quell'hash
        # - Se un hash è presente in una sola directory, NON viene toccato
        
        import os
        import sys
        import hashlib
        
        # Calcola l’hash SHA1 del contenuto di un file
        def sha1_of_file(path):
            try:
                with open(path, "rb") as f:
                    return hashlib.sha1(f.read()).hexdigest()
            except:
                return None
        
        # Costruisce una mappa: { hash → [file1, file2, ...] }
        def hash_map(directory):
            result = {}
            for entry in os.listdir(directory):
                path = os.path.join(directory, entry)
                if os.path.isfile(path):
                    digest = sha1_of_file(path)
                    if digest:
                        result.setdefault(digest, []).append(path)
            return result
        
        def main():
            # Controllo argomenti
            if len(sys.argv) != 3:
                print("Uso: python3 shadel.py <dir1> <dir2>")
                sys.exit(1)
        
            dir1, dir2 = sys.argv[1], sys.argv[2]
        
            # Ottiene hash→file per entrambe le directory
            hashes1 = hash_map(dir1)
            hashes2 = hash_map(dir2)
        
            # Trova gli hash comuni
            comuni = set(hashes1.keys()) & set(hashes2.keys())
        
            # Rimuove i file con hash in comune
            for h in comuni:
                for path in hashes1[h] + hashes2[h]:
                    try:
                        os.remove(path)
                    except:
                        pass
        
        if __name__ == "__main__":
            main()
        
        ```
        
- 24/06/2024
    
    ![image.png](attachment:df496c30-5a86-4598-914b-92c816b8bf87:image.png)
    
    - Es1
        
        ```c
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <unistd.h>
        #include <sys/inotify.h>
        #include <sys/types.h>
        #include <sys/wait.h>
        #include <dirent.h>
        #include <errno.h>
        #define EVENT_SIZE (sizeof(struct inotify_event))
        #define BUF_LEN (1024 * (EVENT_SIZE + 16))
        
        /*
            Esercizio 1: Linguaggio C (obbligatorio) 20 punti
            Usando inotify scrivere un programma inotirun che ha come parametro il pathname di una
            directory vuota che chiameremo D. Quando vengono inseriti file in D questi vengono eseguiti (uno
            alla volta) e cancellati. I file in D hanno il seguente formato:
            * il pathname dell'eseguibile
            * una riga per ogni elemento di argv.
            Es:
            /bin/ls
            ls
            -l
            /tmp
        */
        
        void esegui_e_cancella(const char *dir, const char *file) {
            char path[4096];  // Buffer per il percorso completo del file
            snprintf(path, sizeof(path), "%s/%s", dir, file);  // Costruisce il percorso completo
        
            FILE *f = fopen(path, "r");   // Apre il file con permission di lettura
            if (!f) return;
        
            char *argv[128];   // Array per gli argomenti del comando
            char line[4096];   // Buffer per leggere le linee dal file
            int argc = 0;      // Contatore per gli argomenti
        
            // Legge il file riga per riga e popola argv
            while (fgets(line, sizeof(line), f) && argc < 127) {
                line[strcspn(line, "\n")] = 0; // rimuove newline
                argv[argc++] = strdup(line);  // Duplica la stringa per evitare problemi di memoria
            }
            argv[argc] = NULL; // Termina l'array di argomenti con NULL
            fclose(f);
        
            if (argc > 0) {
                pid_t pid = fork();   // Crea un nuovo processo per eseguire il comando
        
                // Se pid è 0, siamo nel processo figlio
                if (pid == 0) {
                    execv(argv[0], argv);  // Esegue il comando specificato nel file
                    perror("execv");    // Se execv fallisce, stampa l'errore
                    exit(1);
        
                // Se pid è maggiore di 0, siamo nel processo padre
                } else if (pid > 0) {
                    waitpid(pid, NULL, 0);  // Attende che il processo figlio termini
                }
            }
        
            // Libera la memoria allocata per gli argomenti
            for (int i = 0; i < argc; ++i) free(argv[i]);
            // Cancella il file dopo l'esecuzione
            unlink(path);
        }
        
        int main(int argc, char *argv[]) {
            if (argc != 2) {
                fprintf(stderr, "Uso: %s <directory>\n", argv[0]);
                exit(1);
            }
            const char *dir = argv[1];
        
            // Inizializza inotify
            int fd = inotify_init();
            if (fd < 0) {
                perror("inotify_init");
                exit(1);
            }
        
            /* 
                Aggiungi la directory da monitorare, con gli eventi che ci interessano, ovvero:
                    - IN_CLOSE_WRITE: file chiuso dopo scrittura
                    - IN_MOVED_TO: file spostato nella directory
                    - IN_CREATE: file creato nella directory
            */
            int wd = inotify_add_watch(fd, dir, IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE);
            if (wd < 0) {
                perror("inotify_add_watch");
                exit(1);
            }
        
            // Buffer per gli eventi
            char buf[BUF_LEN];
            while (1) {   // Ciclo infinito per monitorare gli eventi
                int len = read(fd, buf, BUF_LEN);   // Legge gli eventi da inotify
                if (len < 0) {
                    perror("read");
                    break;
                }
                int i = 0;
                // Il while scorre gli eventi letti e li gestisce
                while (i < len) {  
                    // Cast del buffer a struct inotify_event, ovvero la struttura che contiene le informazioni sugli event
                    struct inotify_event *event = (struct inotify_event *)&buf[i];
        
                    // Se l'evento ha un nome e il tipo di evento è uno di quelli che ci interessano
                    if (event->len && (event->mask & (IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE))) {
                        esegui_e_cancella(dir, event->name);
                    }
                    i += EVENT_SIZE + event->len; // Avanza l'indice per il prossimo evento
                }
            }
            close(fd);
            return 0;
        }
        
        ```
        
        ## Test
        
        ### 1. Compilazione
        
        Hai già compilato il file `script.c` in `inotirun`.
        
        ```bash
        gcc -Wall script.c -o inotirun
        
        ```
        
        ### 2. Creazione ambiente di test
        
        ```bash
        mkdir testdir
        
        ```
        
        ### 3. Creazione file di test
        
        Hai creato un file `testfile` contenente:
        
        ```
        /bin/ls
        -l
        
        ```
        
        Questo rappresenta il comando da eseguire:
        
        ```bash
        /bin/ls -l
        
        ```
        
        ### 4. Avvio del programma `inotirun` in background
        
        ```bash
        ./inotirun testdir &
        
        ```
        
        ### 5. Spostamento del file nella directory monitorata
        
        ```bash
        mv testfile testdir/
        
        ```
        
        ### 6. Verifica del comportamento
        
        - Il programma ha rilevato l’arrivo del file
        - Ha letto il contenuto e l'ha eseguito con `execv`
        - Ha aspettato il termine del processo figlio
        - Ha **eliminato** automaticamente il file `testfile`
        
        ---
        
    - Es2
        
        ```c
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <unistd.h>
        #include <sys/inotify.h>
        #include <sys/types.h>
        #include <sys/wait.h>
        #include <dirent.h>
        #include <errno.h>
        #define EVENT_SIZE (sizeof(struct inotify_event))
        #define BUF_LEN (1024 * (EVENT_SIZE + 16))
        
        /*
            Esercizio 1: Linguaggio C (obbligatorio) 20 punti
            Usando inotify scrivere un programma inotirun che ha come parametro il pathname di una
            directory vuota che chiameremo D. Quando vengono inseriti file in D questi vengono eseguiti (uno
            alla volta) e cancellati. I file in D hanno il seguente formato:
            * il pathname dell'eseguibile
            * una riga per ogni elemento di argv.
            Es:
            /bin/ls
            ls
            -l
            /tmp
        */
        
        void esegui_e_cancella(const char *dir, const char *file) {
            char path[4096];
            snprintf(path, sizeof(path), "%s/%s", dir, file);
        
            FILE *f = fopen(path, "r");
            if (!f) return;
        
            char *argv[128];
            char line[4096];
            int argc = 0;
        
            while (fgets(line, sizeof(line), f)) {
                // Rimuovi newline
                line[strcspn(line, "\n")] = 0;
        
                // Riga vuota → esegui comando corrente
                if (strlen(line) == 0 && argc > 0) {
                    argv[argc] = NULL;
        
                    pid_t pid = fork();
                    if (pid == 0) {
                        execv(argv[0], argv);
                        perror("execv");
                        exit(1);
                    } else if (pid > 0) {
                        waitpid(pid, NULL, 0);
                    }
        
                    // Libera e resetta
                    for (int i = 0; i < argc; ++i) free(argv[i]);
                    argc = 0;
                } else if (strlen(line) > 0) {
                    argv[argc++] = strdup(line);
                }
            }
        
            // Esegue l'ultimo comando se non terminava con una riga vuota
            if (argc > 0) {
                argv[argc] = NULL;
                pid_t pid = fork();
                if (pid == 0) {
                    execv(argv[0], argv);
                    perror("execv");
                    exit(1);
                } else if (pid > 0) {
                    waitpid(pid, NULL, 0);
                }
                for (int i = 0; i < argc; ++i) free(argv[i]);
            }
        
            fclose(f);
            unlink(path);
        }
        
        int main(int argc, char *argv[]) {
            if (argc != 2) {
                fprintf(stderr, "Uso: %s <directory>\n", argv[0]);
                exit(1);
            }
            const char *dir = argv[1];
        
            // Inizializza inotify
            int fd = inotify_init();
            if (fd < 0) {
                perror("inotify_init");
                exit(1);
            }
        
            /* 
                Aggiungi la directory da monitorare, con gli eventi che ci interessano, ovvero:
                    - IN_CLOSE_WRITE: file chiuso dopo scrittura
                    - IN_MOVED_TO: file spostato nella directory
                    - IN_CREATE: file creato nella directory
            */
            int wd = inotify_add_watch(fd, dir, IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE);
            if (wd < 0) {
                perror("inotify_add_watch");
                exit(1);
            }
        
            // Buffer per gli eventi
            char buf[BUF_LEN];
            while (1) {   // Ciclo infinito per monitorare gli eventi
                int len = read(fd, buf, BUF_LEN);   // Legge gli eventi da inotify
                if (len < 0) {
                    perror("read");
                    break;
                }
                int i = 0;
                // Il while scorre gli eventi letti e li gestisce
                while (i < len) {  
                    // Cast del buffer a struct inotify_event, ovvero la struttura che contiene le informazioni sugli event
                    struct inotify_event *event = (struct inotify_event *)&buf[i];
        
                    // Se l'evento ha un nome e il tipo di evento è uno di quelli che ci interessano
                    if (event->len && (event->mask & (IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE))) {
                        esegui_e_cancella(dir, event->name);
                    }
                    i += EVENT_SIZE + event->len; // Avanza l'indice per il prossimo evento
                }
            }
            close(fd);
            return 0;
        }
        ```
        
        ### Test
        
        ```bash
        mkdir testdir
        
        ```
        
        ---
        
        ### 🚀 3. **Avvia il programma in background**
        
        ```bash
        ./inotimrun testdir &
        
        ```
        
        > 🟡 Lascia questo processo attivo — sta monitorando la cartella testdir.
        > 
        
        ---
        
        ### 🧪 4. **Crea un primo file di test**
        
        ```bash
        cat > testfile1 <<EOF
        /bin/echo
        echo
        Ciao mondo!
        EOF
        
        ```
        
        ---
        
        ### 📂 5. **Sposta il file nella cartella monitorata**
        
        ```bash
        mv testfile1 testdir/
        
        ```
        
        ---
        
        ### ✅ 6. **Verifica che l’output sia corretto**
        
        Dovresti vedere:
        
        ```
        Ciao mondo!
        
        ```
        
        ---
        
        ### 🔁 7. **Fai altri test (esempio con `ls`)**
        
        ```bash
        cat > testfile2 <<EOF
        /bin/ls
        ls
        -l
        EOF
        
        mv testfile2 testdir/
        
        ```
        
        ---
        
        ### ❌ 8. **Test con errore (comando inesistente)**
        
        ```bash
        cat > testfile3 <<EOF
        /bin/nonexistent
        nonexistent
        EOF
        
        mv testfile3 testdir/
        
        ```
        
        > Dovresti vedere un messaggio di errore simile a:
        > 
        
        ```
        execv: No such file or directory
        
        ```
        
        ---
        
        ### 🔚 9. **(Facoltativo) Ferma il processo `inotimrun`**
        
        Trova il PID (se non lo sai):
        
        ```bash
        ps | grep inotimrun
        
        ```
        
        Poi termina:
        
        ```bash
        kill <PID>
        
        ```
        
        Oppure se stai ancora nella shell dove l’hai avviato:
        
        ```bash
        fg
        # poi premi CTRL+C
        
        ```
        
    - Es3
        
        ```c
        # permdir.py
        #
        # Obiettivo:
        # - Per ogni file regolare in una directory:
        #   - Calcolare i permessi in formato stringa es. -rw-r--r--
        #   - Creare una directory con quel nome (se non esiste)
        #   - Inserire lì un link simbolico al file, con lo stesso nome
        
        import os
        import sys
        import stat
        
        # Converte il mode (int) dei permessi in stringa stile ls -l
        def perm_string(mode):
            chars = ['-'] * 10
            perms = [
                (stat.S_IRUSR, 1), (stat.S_IWUSR, 2), (stat.S_IXUSR, 3),
                (stat.S_IRGRP, 4), (stat.S_IWGRP, 5), (stat.S_IXGRP, 6),
                (stat.S_IROTH, 7), (stat.S_IWOTH, 8), (stat.S_IXOTH, 9)
            ]
            chars[0] = '-' if stat.S_ISREG(mode) else '?'
            for bit, pos in perms:
                if mode & bit:
                    chars[pos] = "rwxrwxrwx"[pos-1]
            return ''.join(chars)
        
        def main():
            if len(sys.argv) != 2:
                print("Uso: python3 permdir.py <directory>")
                sys.exit(1)
        
            indir = sys.argv[1]
        
            for filename in os.listdir(indir):
                path = os.path.join(indir, filename)
        
                # Considera solo file regolari
                if not os.path.isfile(path):
                    continue
        
                try:
                    info = os.lstat(path)
                    perm = perm_string(info.st_mode)
        
                    # Crea la directory con nome dei permessi
                    os.makedirs(perm, exist_ok=True)
        
                    # Crea il link simbolico al file nella directory giusta
                    dest = os.path.join(perm, filename)
                    os.symlink(path, dest)
        
                except Exception:
                    pass  # ignora errori (file già linkati, ecc.)
        
        if __name__ == "__main__":
            main()
        
        ```
        
- 30/05/2024 - non testato
    
    ![image.png](attachment:449da74b-3b96-4127-acd2-5d4532f0af75:image.png)
    
    - Es1 - non testati
        
        ```c
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
        ```
        
        ### Test
        
        - Creo il codice per il test → `test_process.c`
            
            ```c
            #include <stdio.h>
            #include <unistd.h>
            
            int main(int argc, char *argv[]) {
                printf("Processo di test in esecuzione. PID: %d\n", getpid());
                while (1) {
                    sleep(1); // Mantiene il processo attivo
                }
                return 0;
            }
            ```
            
        1. **Assicurati che il processo di test sia ancora in esecuzione**: Prima di eseguire `cloneproc`, verifica che il processo di test sia attivo con:
            
            ps aux | grep test_process
            
            Se il processo non è attivo, riavvialo:
            
            ./test_process &
            
        2. **Ottieni il PID corretto**: Usa `ps aux | grep test_process` per ottenere il PID del processo di test. Supponiamo che il PID sia `11320`.
        3. **Esegui `cloneproc` con il PID corretto**:
            
            ./cloneproc 11320
            
        
        ---
        
        ### **Debug con `strace`**
        
        Se il problema persiste, usa `strace` per analizzare il comportamento di `cloneproc`:
        
        strace ./cloneproc 11320
        
        ---
        
        ### **Pulizia**
        
        Dopo il test, termina tutti i processi `test_process` per evitare di lasciare processi zombie:
        
        killall test_process
        
    - Es2 - non testati
        
        ```c
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
        ```
        
        ### Test
        
        Per testare **correttamente** la tua implementazione di `cloneproc+`, che clona il processo dato da un PID mantenendo **argv**, **cwd** e **environment**, segui **questi passaggi pratici**, uno alla volta:
        
        ---
        
        ### ✅ **1. Compilare il programma**
        
        ```bash
        gcc -Wall cloneproc_plus.c -o cloneproc_plus
        
        ```
        
        ---
        
        ### ✅ **2. Scrivere un programma di test**
        
        Esempio `test_env.c`:
        
        ```c
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
        
        ```
        
        ---
        
        ### ✅ **3. Compila anche questo**
        
        ```bash
        gcc test_env.c -o test_env
        
        ```
        
        ---
        
        ### ✅ **4. Esegui in un'altra directory**
        
        ```bash
        mkdir testdir
        cd testdir
        ../test_env uno due tre &
        
        ```
        
        *Annota il PID (es. `1234`)*
        
        ---
        
        ### ✅ **5. In una nuova shell, lancia cloneproc+**
        
        ```bash
        cd ..
        ./cloneproc_plus 1234
        
        ```
        
        ---
        
        ### ✅ **6. Risultato atteso**
        
        Il nuovo processo (figlio) deve:
        
        - Stampare la stessa `ARGV` (`uno due tre`)
        - Mostrare la stessa `CWD` (`testdir`)
        - Stampare lo stesso `ENV` (o compatibile)
        - Eseguire lo stesso binario (`test_env`)
        
        ---
        
        ### 🔎 Se vuoi automatizzare i test:
        
        Puoi usare `strace`, `ps` o `ls /proc/<pid>` per controllare `cwd`, `exe`, `environ`, ecc.
        
        Se vuoi te li preparo anche.
        
        Fammi sapere se vuoi un **versione migliorata** del codice per renderlo più robusto.
        
    - Es3
        
        ```c
        # lscmd.py
        #
        # Obiettivo:
        # - Accede a /proc
        # - Per ogni processo, legge /proc/<pid>/exe (link all'eseguibile)
        # - Raggruppa i processi per path dell'eseguibile
        # - Stampa path + lista di PID associati
        
        import os
        from collections import defaultdict
        
        def main():
            result = defaultdict(list)  # mappa: path eseguibile → lista di pid
        
            for pid in os.listdir("/proc"):
                if not pid.isdigit():
                    continue
        
                exe_path = f"/proc/{pid}/exe"
                try:
                    exe = os.readlink(exe_path)  # link simbolico all'eseguibile
                    result[exe].append(pid)
                except:
                    continue  # processo potrebbe essere terminato
        
            for exe in sorted(result):
                pids = sorted(result[exe], key=int)  # ordina per numero di PID
                print(exe, ' '.join(pids))
        
        if __name__ == "__main__":
            main()
        
        ```
        

---

- 14/02/2024
    
    ![image.png](attachment:91a151f0-c454-48c7-9d50-6cb525b30f71:image.png)
    
    - Es1
        
        ```c
        #define _GNU_SOURCE
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <dirent.h>
        #include <unistd.h>
        #include <sys/stat.h>
        #include <limits.h>
        #include <fcntl.h>
        #include <sys/wait.h>
        
        const char *search_name;
        
        void process_file(const char *path, struct stat *st) {
            // Ignora se non è un file regolare (es: directory, link simbolico, ecc.)
            if (!S_ISREG(st->st_mode)) return;   
            // Ignora se il file NON ha il permesso di esecuzione per l’utente
            if (!(st->st_mode & S_IXUSR)) return;  
        
            // Estrae il nome del file dal percorso completo (basename)
            const char *base = strrchr(path, '/'); // cerca l'ultimo '/' --> ovvero se ho cane/gatto/topo, cerca il / prima di topo
            base = base ? base + 1 : path;         // se '/' trovato, prendi il nome dopo (ovvero topo e ignora il /); altrimenti usa l'intero path
        
            // Confronta il nome del file con il nome da cercare
            if (strcmp(base, search_name) != 0) return;
        
            // Prova ad aprire il file in sola lettura
            int fd = open(path, O_RDONLY);
            if (fd < 0) {
                perror("open"); // stampa errore se non si può aprire
                return;
            }
        
            // Legge i primi 4 byte del file per identificare il formato
            unsigned char buf[4];
            ssize_t r = read(fd, buf, sizeof(buf));
            close(fd);
            if (r < 2) return; // se meno di 2 byte letti, non possiamo fare controlli
        
            // Stampa il percorso del file
            printf("%s: ", path);
        
            // Se inizia con "#!", è uno script (es. bash, python, ecc.)
            if (buf[0] == '#' && buf[1] == '!') {
                printf("script\n");
        
            // Se inizia con 0x7f 'E' 'L' 'F', è un eseguibile ELF Linux
            } else if (r >= 4 && buf[0] == 0x7f && buf[1] == 'E' && buf[2] == 'L' && buf[3] == 'F') {
                printf("ELF executable\n");
        
            // Altrimenti non riconosciamo il formato (ma resta eseguibile)
            } else {
                printf("eseguibile (formato non riconosciuto)\n");
            }
        }
        
        void recurse(const char *dirpath) {
            DIR *D = opendir(dirpath);
            if (!D) {
                perror("opendir");
                return;
            }
        
            struct dirent *d;
            while ((d = readdir(D)) != NULL) {
                // Scarto le directory "." e ".." per evitare loop infiniti
                if (strcmp(d->d_name, ".")==0 || strcmp(d->d_name, "..")==0)
                    continue;
                
                // Creo il path assoluto al file o directory corrente
                char fullpath[PATH_MAX];
                snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, d->d_name);
        
                // Recupero informazioni sul file o directory
                struct stat st;
                if (lstat(fullpath, &st) < 0) {
                    perror(fullpath);
                    continue;
                }
        
                // Se è una directory, chiamo ricorsivamente la funzione
                if (S_ISDIR(st.st_mode)) {
                    recurse(fullpath);
                
                // Se è un file normale, allora passo alla funzione di elaborazione
                } else {
                    process_file(fullpath, &st);
                }     
            }
            
            closedir(D);
        }
        
        int main(int argc, char *argv[]) {
            if (argc < 2 ) {
                fprintf(stderr, "Usage: %s <name>\n", argv[0]);
                return 1;
            }
        
            search_name = argv[1];
            recurse(".");
            return EXIT_SUCCESS;
        }
        ```
        
        ### Test
        
        Verificare che il programma `search_name`:
        
        - Scorra ricorsivamente tutte le sottodirectory
        - Riconosca file eseguibili con nome esatto dato
        - Distingua tra script (`#!`) ed eseguibili ELF (`0x7fELF`)
        
        ---
        
        ### 🧪 Preparazione ambiente di test
        
        ```bash
        mkdir -p ~/test_search
        cd ~/test_search
        
        ```
        
        ### ➕ Crea sottodirectory:
        
        ```bash
        mkdir -p dir1 dir2/dir3
        
        ```
        
        ### 📝 Crea uno script chiamato `testprog`
        
        ```bash
        echo '#!/bin/bash' > testprog
        chmod +x testprog
        
        ```
        
        ### 📂 Copia lo script in una sottocartella
        
        ```bash
        cp testprog dir1/testprog
        
        ```
        
        ### ⚙️ Crea un eseguibile ELF chiamato `testprog`
        
        ```bash
        echo -e '#include <stdio.h>\nint main(){return 0;}' > dir2/dir3/testprog.c
        gcc dir2/dir3/testprog.c -o dir2/dir3/testprog
        
        ```
        
        ---
        
        ### 🧠 Modifica importante nel codice C
        
        Nel file `search_name.c`, è necessario aggiungere **la dichiarazione della funzione `process_file()` prima dell’uso in `recurse()`**:
        
        ```c
        void process_file(const char *path, struct stat *st); // <- DA AGGIUNGERE
        
        ```
        
        ---
        
        ### 🔨 Compilazione del programma
        
        Se `search_name.c` è altrove, compila con:
        
        ```bash
        gcc -Wall -Wextra -o search_name /percorso/search_name.c
        
        ```
        
        Nel nostro caso:
        
        ```bash
        gcc -Wall -Wextra -o search_name ../Sistemi/Pratico/esame/140224/es1/search_name.c
        
        ```
        
        ---
        
        ### 🚀 Esecuzione del test
        
        ```bash
        ./search_name testprog
        
        ```
        
        ### 📤 Output atteso
        
        ```
        ./testprog: script
        ./dir1/testprog: script
        ./dir2/dir3/testprog: ELF executable
        
        ```
        
        ---
        
        ### 🧹 Pulizia (facoltativa)
        
        ```bash
        rm -r ~/test_search
        
        ```
        
        ---
        
        ### ✅ Test effettuati:
        
        | File | Tipo | Risultato previsto |
        | --- | --- | --- |
        | `./testprog` | Script | `script` |
        | `./dir1/testprog` | Script | `script` |
        | `./dir2/dir3/testprog` | ELF Executable | `ELF executable` |
        | File non eseguibili | Ignorati | Nessun output |
        | File con nome diverso | Ignorati | Nessun output |
        
        ---
        
        Fammi sapere se vuoi aggiungere **test per casi particolari** (senza permessi, con symlink, ecc.)!
        
    - Es2
        
        ```c
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
        
        // Funzione che esegue il file `path` con working directory `dir`
        void execute_in_directory(const char *path, const char *dir) {
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
        
        // Ricorsione nel filesystem
        void search_and_run(const char *dirpath) {
            DIR *D = opendir(dirpath);
            if (!D) {
                perror("opendir");
                return;
            }
        
            struct dirent *d;
            while ((d = readdir(D)) != NULL) {
                if (strcmp(d->d_name, ".")==0 || strcmp(d->d_name, "..")==0)
                    continue;
        
                char fullpath[PATH_MAX];
                snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, d->d_name);
        
                struct stat st;
                if (lstat(fullpath, &st) < 0) {
                    perror("lstat");
                    continue;
                }
        
                if (S_ISDIR(st.st_mode)) {
                    // Ricorsione se è directory
                    search_and_run(fullpath);
                } else if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) {
                    // Se è file eseguibile con nome esatto
                    const char *base = strrchr(fullpath, '/');
                    base = base ? base + 1 : fullpath;
                    if (strcmp(base, target_name) == 0) {
                        // Estrai directory padre
                        char dircopy[PATH_MAX];
                        strncpy(dircopy, fullpath, sizeof(dircopy));
                        char *last_slash = strrchr(dircopy, '/');
                        if (last_slash) {
                            *last_slash = '\0';  // Termina prima del nome file
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
            target_args = &argv[1];  // Puntiamo direttamente al nome + args
        
            search_and_run(".");
        
            return 0;
        }
        
        ```
        
    - Es3
        
        ```c
        # catls.py
        #
        # Obiettivo:
        # - Scandisce i file di una directory (di default ".")
        # - Usa il comando `file` per determinare il tipo
        # - Raggruppa i file per tipo
        # - Stampa un catalogo con intestazione per tipo
        
        import os
        import sys
        import subprocess
        from collections import defaultdict
        
        def main():
            root = sys.argv[1] if len(sys.argv) > 1 else "."
            catalogo = defaultdict(list)
        
            for fname in sorted(os.listdir(root)):
                path = os.path.join(root, fname)
        
                # Considera solo file regolari
                if not os.path.isfile(path):
                    continue
        
                try:
                    # Esegue "file path"
                    out = subprocess.check_output(["file", path], text=True).strip()
        
                    # Estrae solo la descrizione dopo il primo ":"
                    descr = out.split(":", 1)[1].strip()
        
                    catalogo[descr].append(fname)
                except:
                    continue
        
            # Stampa raggruppata per tipo
            for tipo in sorted(catalogo):
                print(f"{tipo}:")
                for f in catalogo[tipo]:
                    print(f"  {f}")
                print()
        
        if __name__ == "__main__":
            main()
        
        ```
        
- 19/01/2024
    
    ![image.png](attachment:56eb9416-d4e9-47d1-a349-05a116ffdf8f:image.png)
    
    - Es1
        - argsend
            
            ```c
            /*
             * argsend.c
             * Converte argv[1..n] in una sequenza null-terminated:
             * scrive su stdout ogni stringa seguita da '\0'
             */
            
            #include <stdio.h>
            #include <string.h>
            
            int main(int argc, char *argv[]) {
                for (int i = 1; i < argc; i++) {
                    // scrive i caratteri del parametro
                    fwrite(argv[i], 1, strlen(argv[i]), stdout);
                    // aggiunge terminatore nullo
                    fputc('\0', stdout);
                }
                return 0;
            }
            
            ```
            
        - argcvev
            
            ```c
            /*
             * argcvec.c
             * Legge da stdin una sequenza di stringhe null-terminate (es. output argsend)
             * Ricostruisce argv[] e esegue il comando
             */
            
            #include <stdio.h>
            #include <stdlib.h>
            #include <unistd.h>
            #include <string.h>
            
            #define MAX_BUF 8192
            #define MAX_ARGS 256
            
            int main() {
                char buf[MAX_BUF];
            
                // legge tutto da stdin
                ssize_t len = read(0, buf, MAX_BUF);
                if (len <= 0) return 1;
            
                // prepara argv[] in base ai '\0'
                char *argv[MAX_ARGS];
                int argc = 0;
            
                for (int i = 0; i < len;) {
                    argv[argc++] = &buf[i];
                    i += strlen(&buf[i]) + 1;
                }
            
                argv[argc] = NULL;
            
                // esegue il comando
                execvp(argv[0], argv);
                perror("execvp");
                return 1;
            }
            
            ```
            
    - Es3
        
        ```c
        #!/usr/bin/env python3
        import os
        import sys
        
        def get_paths(root):
            result = set()
            for dirpath, dirnames, filenames in os.walk(root):
                rel_dir = os.path.relpath(dirpath, root)
                if rel_dir == ".":
                    rel_dir = ""
                for name in dirnames + filenames:
                    rel_path = os.path.join(rel_dir, name).strip("./")
                    result.add("./" + rel_path)
            return result
        
        def usage():
            print("Uso: tcmp.py [-1|-2] dir1 dir2")
            sys.exit(1)
        
        def main():
            args = sys.argv[1:]
        
            mode = "both"
            if len(args) == 3 and args[0] in ("-1", "-2"):
                mode = args[0]
                dir1, dir2 = args[1], args[2]
            elif len(args) == 2:
                dir1, dir2 = args
            else:
                usage()
        
            if not (os.path.isdir(dir1) and os.path.isdir(dir2)):
                print("Entrambe le directory devono esistere.")
                sys.exit(1)
        
            paths1 = get_paths(dir1)
            paths2 = get_paths(dir2)
        
            if mode == "both":
                common = sorted(paths1 & paths2)
                for path in common:
                    print(path)
            elif mode == "-1":
                only1 = sorted(paths1 - paths2)
                for path in only1:
                    print(path)
            elif mode == "-2":
                only2 = sorted(paths2 - paths1)
                for path in only2:
                    print(path)
        
        if __name__ == "__main__":
            main()
        
        ```
