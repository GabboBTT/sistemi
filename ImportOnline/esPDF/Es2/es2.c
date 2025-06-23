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
    if (!file)
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
