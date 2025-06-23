Corso di Sistemi Operativi AA 2021/2022
Pls mettetevi un nic per riconoscerci, thx :D
Strumenti del corso
Gruppo telegram:  https://t.me/joinchat/BvTc3_YQM7hkYzVi
Bot del corso: https://t.me/so_cs_unibot
Editor condiviso:  https://etherpad.wikimedia.org/p/so2122.cs.unibo.it
Mailing list del corso: https://lists.cs.unibo.it/cgi-bin/mailman/listinfo/so
Lezioni live: http://www.cs.unibo.it/~renzo/live
Sito web: http://www.cs.unibo.it/~renzo/so
(Wiki del corso: http://so.v2.cs.unibo.it )
( https://virtuale.unibo.it/ )
Mail: renzo@cs.unibo.it
Ricevimento studenti
Materiale (slide ecc): https://www.cs.unibo.it/~renzo/so/materiale2122/

Progetto PandOS+
fase 1 -> vedi lezione 3 dicembre
consegna: domenica 20 febbraio 2022,  ore 23.59
info generali: https://www.cs.unibo.it/~renzo/so/progetto.shtml
directory del progetto: https://www.cs.unibo.it/~renzo/so/pandosplus/

-------- venerdì 24 settembre 2021 --------<(‘o'<) ^( ‘-‘ )^ (>‘o’)> v( ‘.’ )v <(‘ .’ )> <(‘.'<) ^( ‘.’ )^ (>‘.’)> v( ‘.’ )
 
esempio ristorante.pdf (link: https://www.cs.unibo.it/~renzo/so/materiale2122/ristorante.pdf )
cuoco => processore
ricetta => programma (testo inattivo)
azione di preparare la panna => processo (esegue il programma)
cameriere => dispositivo di output
addetti => dispositivi input/output
finestre => bus dati, registri di device (dà il via all'operazione)
cartelli alle finestre => stato del device
filo del cuoco => coda dei processi READY
gancio del cuoco => processo corrente
fili delle finestre => strutture dati che contengono le operazioni pendenti per quel device 
richieste in sospeso
(circa un activation record?)
il cuoco cambia il cartello alla finestra => acknowledge del interrupt
gancio alla finestra => operazione corrente di input/output
nota dell'ordine => struttura dati che tiene conto dello stato del processo
Process Control Block
abito del cuoco => kernel mode
operazioni esterne (in corsivo) => /operazioni supportate dal kernel?
lavagnetta => memoria, assegnamento di memoria
(i dati dei vari processi sono separati)
linea della ricetta => program counter
scelta al punto (*) => scheduling di CPU (decide qual'è il prossimo processo da eseguire alla CPU)
scelta al punto (**) => scheduling dei device (decide quale operazione far iniziare ai device)
istruzioni del capo cuoco K => kernel (un programma)
astrazione del cuoco che sembra preparare diversi piatti => multitasking (molti - lavori) 
zucchero a velo e panna fresca => dati
 
colui che legge le istruzioni => control path
filo della dispensa => 
per andare alla ciotola direct memory access (accesso diretto alla memoria)
ma occupando il bus (corridoio) devono mettersi d'accordo con il processore
a cosa serve? ci sono cose che ci si trova già fatte (deve spostare molti byte da 8 bit)
quando la dispensa manda l'interrupt dicendo 'ti ho dato la panna' il cuoco può già iniziare a girare.
 
il kernel può fare solo operazioni del kernel non codice del utente
distribuzione => antologia
gestita in maniera armoniosa
1 comando per aggiornare
metodo efficiente per system amministrator di tener apposto i sistemi
 
i sistemi di oggi hanno 'più cuochi'
i problemi potrebbero essere:
    accesso alla stessa risorsa
    devono coordinarsi tra loro
    bus dati condiviso
 
il kernel si accorge 
i device attirino l'attenzione del processore
interrupt (cartello)
processo diventa READY e il kernel lo mette come prioritario
 
altre estensioni: 
    si aggiunge interval timer che serve per generare interrupt dopo un certo tempo per bloccare il processo corrente e far iniziare (o proseguire) un altro processo.
 
processori: più operazioni più spendi energia
molti processori hanno stato di WAIT finchè non arriva un interrupt
astrazione fatta dagli ingegneri loop di attesa ma qualche porta logica che cambia stato per rimanere in attesa
lo stato del device potrebbe avere anche rotture, come staccare
il dispositivo in corso.  (cartello errore).
 
COSA FAREMO DA ADESSO IN POI?
approfondimento C, sviluppo ???, programmazione concorrente
programmazione di sistema
 
allenamento all'uso della command line (uso del terminale) .
 
scrittura di programmi che parlano con il sistema operativi fatta in maniera marginale da altri corsi.
 
interagire con un sistema in modo grafica
command line parlare a gesti
un tema/tesi a gesti nemmeno Massel Marceau (grande mimo)
 
vi consiglio un sistema GNU/Linux funzionante
se pigri macchina virtuale
dovreste avere sistemi nativi GNU/Linux
se volete potete fare multi-boot
 
CODICE (slide link: 
http://www.cs.unibo.it/~renzo/so/materiale2122/pensieri2122.pdf )
 

------------- 30 settembre 2021 -------------<(‘o'<) ^( ‘-‘ )^ (>‘o’)> v( ‘.’ )v <(‘ .’ )> <(‘.'<) ^( ‘.’ )^ (>‘.’)> v( ‘.’ )
stele di rosetta
alfabeto
grammatica
semantica
protocollo: regole di comportamento 
linguaggi di scripting e di programmazione
risposta sbagliata: gli uni sono interpretati gli altri sono compilati
cambia la granularità della singola operazione
linguaggi di programmazione: operazione aritmetico logica
linguaggi di scripting: operazione di esecuzione di programmi
linguaggi operativi / imperativi ->modo verbale imperativo, esempio: C, C++
linguaggi funzionali -> computazione come funzioni matematiche, esempio Lisp
linguaggi logici -> computazione per mezzo di fatti e regole (predicati logici)

Il compilatore C e' scritto in C
filesistem metafora della archiviazione delle pratiche in un ufficio
ls
ls /
ls /bin/ls
perchè /bin/? perchè è un path di ricerca per il comando
echo $PATH
ls /usr
var troviamo variabili tipo ... mailbox, log di sistema
ls /home
home directory degli utenti
pwd
directory corrente

vi hello.c
apre vim per scrivere il file hello.c

//hello.c
#include stdio.h
int main() {
    printf("hello world\n");
}

gcc hello.c
compilazione
programma in grado di gestire le fasi di compilazione ovvero:
    1. preprocessore -> editor automatico
    2. compilatore -> traduce sorgente in linguaggio macchina
./a.out
esecuzione

questo programma per la maggior parte del codice non è c
cè include della libreria
cosa fa la printf?
prepara la stringa
e systemcall write

non este la printf perchè non abbiamo ancora la systemcall write

proviamo a scrivere il programma che non faccia uso della libreria

int main() {
    return 42;
}
42 è il valore di ritorno

echo $?

#include "h.h"
#define RETVAL 42
int main() {
    return RETVAL;
}

//file h.h
int answer = 21;

i .h non sono libreria
la libbreria non si include si linka
gcc -S def.c
genera def.s in assembler
il compilatori è possibile fermarli a passi intermedi

gcc -c def.c
rm a.out

gcc def.s

int main() {
    volatile int i;
    for (i = 0; i < 100000 ; i++) {}
    return 42;
}

gcc -S -O2 test.c

aggiungiamo " volatile " int i;
vi rende chiaro quanto ci sia di controllo da parte del c di ciò che sata avvenendo
nessun linguaggio di alto livello ha (tipo java o python)
sta attento che questa variabile potrebbe essere cambiato non sotto il tuo controllo

ld test.o
dice non trova il single start
è main perchè l'ambiente di esecuzione crea la chiamata a main

uscendo dal main non termina automaticamente il programma
i programmi non muoiono mai di per loro, muoiono sempre violentemente di suicidio o omicidio...

cat > crst0r.o
cat copia in crts0r.o

start
xor di una variabile per se stessa pulisce la variabile
...
mette i vari parametri sullo stack
call main
prepara i programmi per fare la syscall

as -o crts0r.o ctrs0r.o
as per assemblare

ld per linkare
ld crts0r.o test.o

long mywrite
prende l'indirizzo e lo associa a un intero long
notate casting

man 2 write
man è manuale
1...
2...
3...
4...
5...
importante 7 per descrizione ambienti

man man

void start:
    prima chiamo la mywrite e poi la myexit

libreria C
parte in cui fa la libreria in C
dall'altra interfacciamento alla systemcall
mette i parametri nei registi e chiama syscall


------------- 01 ottobre 2021 -------------✿◕ ‿ ◕✿ ❀◕ ‿ ◕❀ ❁◕ ‿ ◕❁ ✾◕ ‿ ◕✾ (◡‿◡✿) (✿◠‿◠) ❀(◕‿-)❀
materiale di oggi: https://www.cs.unibo.it/~renzo/so/materiale2122/lso-04-shell-1p.pdf
!!! attivare l'account linux unibo !!!

Filosofia di Unix
progettato da programmatori per programmatori

Shell
è un programma che consente all'utente di eseguire programmi tramite un prompt personalizzabile.
Il linguaggio della shell è utilizzato per lo script, per inserire comandi, ma è più di questo: ti da a disposizione costrutti come un vero e proprio linguaggio di programmazione (puoi fare cicli e funzioni)
Non è nel kernel, è un programma come altri. 
Forma generale: Comandi opzioni argomenti
le opzioni iniziano con "-", "--" // con -- per formato esteso solitamente
gli argomenti indicano l'oggetto su cui eseguire il comando
Es: man command
info command
apropos keyboard
man man --il manuale è ricorsivo, esiste una pagina man nel man--

File System -> vari significati
è l'insieme dei file e delle directory che sono accessibili ad una macchina Linux
è l'organizzazione logica utilizzata da un s.o. per gestire un insieme di file in una memoria secondaria
Es: /extra --è un file system che sta in una partizione del disco--
ls /extra --ho 3 directory che sono radice di extra--
-:-:-:-
umount /extra 
ls /extra/
mount /dev/vd
mount /dev/vdb1 /mnt
ls /mnt --ho spostato extra--
il termine viene anche utilizzato per indicare una singola unità di memoria secondaria
I file sono organizzati ad albero

Gerarchia standard del file system -> comandi base
/bin    --per utenti comuni
ls -l  --serve per il list in long format, (vedi data, ora, tipo di file ecc)
/sbin  --per utenti non comuni: gestione del sistema
/dev   --device file per accedere a periferiche o sistemi di memorizzazione
/etc    --file di configurazione del sistema
/home--"Home directory" degli utenti
/lib    --librerie
/tmp --file temporanei, per eventuali programmi che ne hanno bisogno
/var  --dati variabili, code di stampa
/usr   --contiene gran perte dei programmi del s.o.
/proc --file system virtuale per dialogare col Kernel

pwd  --print working directory
cd directory  --change working directory
mkdir directory  --make a directory
rmdir directory  --remove a directory
ls directory   --view list directory

Gestione gruppi e proprietari
Ogni file è associato a: 
    - un utente proprietario del file 
    - un gruppo (i.e., insieme di utenti) con speciali diritti sul file

ls -l /usr/bin/passwd  --file per accedere alla password, l'utente non può accedervi se non tramite un eseguibile

Come cambiare i permessi?
chmod octal-numer file(s)
rm  --> remove
cp  --> copy
mv --> move
ln   --> link hard
more, less  --per vedere un file (si può usare anche cat, more è storico ma non consente di tornare indietro, less consete anche di andare a ritroso)
ln -s e ls usano due system call diverse

mount
->  Ctrl+R comando -- mi da opzioni di quel comando

top fa apparire quali informazioni stanno occupando più risorse

Gestione dei processi
nice --esegue un comando con priorità statica diversa
renice --cambia la priorità di un processo in esecuzione
kill --termina un processo, ma serve in realtà per mandare un messaggio di termine al processo ma non è detto che lo termini
kill pid == muori pls
kill -9 pid == muori!

Processi in foreground = quelli controllati dalla shell corrente 
jobs  --vedi cosa sta runnando in questo momento
(yes comando per dire si a tutto)
Processi in background = 
bg -- per fare andare in background una cosa che ho appena eseguito, es: sleep 20 //sleep per 20''


------------- 07 ottobre 2021 -------------✿◕ ‿ ◕✿ ❀◕ ‿ ◕❀ ❁◕ ‿ ◕❁ ✾◕ ‿ ◕✾ (◡‿◡✿) (✿◠‿◠) ❀(◕‿-)❀

Prin C ples : http://so.v2.cs.unibo.it/wiki/index.php?title=Prin_C_ples 

slide : https://www.cs.unibo.it/~renzo/so/materiale2122/lso-01-linguaggioc-4p.pdf .

!gc
riesegue l'ultimo comando svolto che comincia con i caratteri "gc"
(si usa perchè è più veloce che usare le frecce per ricercarlo)
ad esempio:
gcc -o oggetto1 file1.c
gcc -o oggetto file.c
!gc
riesegue "gcc -o oggetto file.c"

#include <stdio.h>
#include <stdlib.h>

unsigned int i;
unsigned int fact(int i) {
    printf("i %p\n", &i); //printf("%p", &i) stampa l'indirizzo di i
    return (i=0) ? 1: è * fact(i-1);
}
int main(int argc, char *argv[]) {
    printf("%\n", fact(atoi(argv[1])));
}

------------- 08 ottobre 2021 -------------_,.-'~'-.,__,.-'~'-.,__,.-'~'-.,__,.-'~'-.,__,.-'~'-.,_
stesse slide di ieri: https://www.cs.unibo.it/~renzo/so/materiale2122/lso-01-linguaggioc-4p.pdf
esercizi di lettura c-> Prin C ples : http://so.v2.cs.unibo.it/wiki/index.php?title=Esercizi_di_%22lettura%22_programmi_in_C_2020/21

differenze tra if e switch.
while condizione di arresto
do while condizione di continuazione
array
puntatori
strutture

int a[4]
a[i] equivalente *(a+i)

k = &a[5] - &a[1]

se esiste il puntatore non è detto che esista il dato :

int *p;
scanf("%d", p);
//p non è stato inizializzato quindi scanf fallisce.

int x;
int *p;
p = &x;
scanf("%d", p);
//p è stato inizializzato quindi scanf non fallisce.

malloc
la free libera tutto non una parte scelta

strutture
struct coord {int x; int y;};
struct coord variabile1, variabile2;

struct {int x; int y;} variabile1, variabile2;
/* non è stato dato un nome alla struttura perchè le variabili sono state inizializzate insieme alla struttura */

struct sale {char customer[21]; char item[20]; int amount};
struct mysale = {"Isaac Newton", "mela", 100};
//ma se volessimo mettere solo item
struct sale m = {.item = "mioitem"};

union
vengono definite sintatticamente come le strutture
ma le strutture sono sequenze di campi
le unioni sono sovrapposizioni di campi
ogni campo ha lo stesso indirizzo
possono servire per reinterpretare i dati in maniera diversa

#include <stdio.h>
union int2har {
    int i;
    unsigned char c[sizeof(int)];
}
int main(int argc, char *argv[]){
    //esempio: come scrivere i * e gli spazi
    char *p, c; //p è puntatore, c carattere
    char* p, c; //sembrano entrambi puntatori ma lo è solo p
    //le due scritture sono uguali!

union int2char myi2c;
myi2c.i = atoi(argv[1]);
printf("%02x:%02x:%02x:%02x", myi2c.c[0], myi2c.c[1], myi2c.c[2], myi2c.c[3]);
}

Esercizio quine.c
Esercizio 2.c -> arrays, pointers and structs //nel link
//vedi link ad inizio giornata

http://so.v2.cs.unibo.it/wiki/index.php?title=Esercizi_di_%22lettura%22_programmi_in_C_2020/21

allineamento
//esempio char long char occupa (24) più spazio di char char long (16).
//decrescente aiuta? si ma non lo possiamo fare sempre ad esempio un pacchetto di rete non si può


Esercizio comma operator
ps vedeteli tutti.. li stiamo facendo praticamente tutti gli esercizi


------------- 14 ottobre 2021 -------------^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^
Recap:
* Cosa è un sistema operativo? A cosa serve?
* Può essere visto come strato di astrazione: qual è lo strato inferiore e quale è quello superiore?
* Quale linguaggio parla il Sistema Operativo?
* Esistono sistemi di elaborazione privi di sistema operativo?

Concorrenza: con correre, correre insieme (parallelismo)
* Perché la programmazione concorrente fa parte del corso di Sistemi Operativi
* Occorrono modelli (che rappresentiino il problema, che siano implementabili)
* Concetto astratto di PROCESSO (Programma != processo)
Stati di un processo: ready running blocked
Assioma di FINITE PROGRESS
ogni processo non bloccato prima o poi avanza
* Parallelismo reale e apparente
* Multiprogramming/Multitasking, multiprocessing, distributed processing

CONCORRENZA: modelli/notazioni per rappresentare l'esecuzione concorrente
>>> problemi da risolvere: sincronizzazione e comunicazione

Dov'è la concorrenza?
(Modelli a memoria condivisa, modelli a memoria privata). Per ora memoria condivisa.

Parallelismo reale e apparente hanno necessità di modelli diversi?

int saldo = 100
void aggiorna(int valore) {
    saldo += valore;
}

valore = +10                 valore = -10
ld R0, saldo                 
add R0, valore
                             ld R0, saldo
                             add R0, valore
st R0, saldo                 
                             st R0, saldo

valore = +10                 valore = -10
ld R0, saldo                 
add R0, valore
-------------------------------------------
                             ld R0, saldo
                             add R0, valore
                             st R0, saldo
-------------------------------------------
st R0, saldo

ora poniamo che due processi concorrentemente chiamino aggiorna(10) e aggiorna(-10).
Qual è il valore finale della variabile saldo?
Azioni Atomiche modello(esame). reale (e.g. in assembly/C),<>

Race condition

Benvenuti al nuovo livello della programmazione. Ma è solo l'inizio.

>>> Esempi di casi di concorrenza (ambiente comune, risorse comuni, concorrenza conscia)

Proprietà dei programmmi: safety/liveness
es consenso:
Nei prog. sequenziali
Nei prog. concorrenti
Mutua esclusione/assenza di deadlock/assenza di starvation

Critical section: definizione, codice rappresentativo del problema.
mutua esclusione, no deadlock, no ritardi (non necessari), no starvation.
(+finite progress +terminazione fuori della sezione critica)

<csenter>
saldo += valore
<csexit>
mutua esclusione: un processo solo alla volta fra <csenter> e <Csexit>
no deadlock: (si bloccano a vicenda)
no starvation: (un processo non avanza perche' viene sempre messo in disparte)
no ritardi non necessari

process P:
    while (1) {
        non critical code
        <csenter>
        critical code
        <csexit>
 
process Q:
    while (1) {
        non critical code
        <csenter>
        critical code
        <csexit>




E' possibile risolvere il problema della sezione critica nel modello fino a qui presentato?

Dekker (Dijkstra 65)
turn, pre-wait, post-wait, try again

------------ (parte 2) 14 Ottobre 2021 ---------------^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^

domande?
1. come potremmo usare le unioni?
grep -r ...
/usr/include/linux/socket.h

#include <stdio.h>
struct mystruct {
        int type;
        union { //parte variabile
                int value;
                char *string;
        } var; //var è nome della varibile
};

/*
+-----+-----+
type  var.value
+-----+-----------+
type  var.string
sono unioni quindi sono sovrapposti !
*/

struct mystruct ms;
ms.type
ms.var.value
ms.var.string

+------------+
|umani       |
|------------|
|processi    |
|------------|
|system call |
+------------+

creare astrazione tramite syscall

non esiste sistema operativo senza kernel

Linux senza GNU sarebbe "solo" kernel
lo strato sopra il sistema operativo sono i processi (che a loro volta si interfacciano agli utenti) e sotto l'hardware

esempio del ristorante in cui il cuoco è uno solo ma serve panna per più clienti

ma non è detto che il cuoco sia uno solo, sotto potrebbero esserci più processori

Assioma di FINITE PROGRESS
"Ogni processo non bloccato prima o poi avanza"

Non è previsto che un programma si blocchi per sua volontà

stati di un processo:
    ready: può avanzare, ma può essere in attesa temporanea
    running: sta avanzando
    blocked: è stato bloccato, è in attesa che avvenga qualcosa
         esempio se ha chiesto la panna, non può proseguire senza,
         deve aspettare che un evento avvenga.
         
parallelismo reale o apparente
reale: multiprocessore
apparente: agli umani (lenti) sembra che avvenga in parallelo (poichè più veloci)

int saldo = 100
void aggiorna (int valore) {
    saldo += valore;
}
esempio:
A e B sono parenti, nel saldo ci sono 100€
A versa 10 €,
B preleva 10 €.
nel saldo dopo queste operazioni ci sono ancora 100€.
Bene in programmazione concorrente questa non è una certezza...

esempio
vanno in banca contemporaneamente (luoghi anche diversi?)
per fare l'operazione in assembler dovremmo fare qualcosa del tipo
LD R0 saldo,
ADD R0 valore,
ST R0 saldo

race-condition
esempio 1:
valore = +10                 valore = -10
ld R0, saldo                 
                             ld R0, saldo
add R0, valore               add R0, valore
st R0, saldo                 
                             st R0, saldo

esempio 2:
valore = +10                 valore = -10
ld R0, saldo                 
add R0, valore
                             ld R0, saldo
                             add R0, valore
st R0, saldo                 
                             st R0, saldo

esempio 3
-------------------------------------------
                             ld R0, saldo
                             add R0, valore
                             st R0, saldo
-------------------------------------------
st R0, saldo

parallelismo qui è apparente ma il risultato è sbagliato.

quali sono le operazioni atomiche?
"atomiche" --> esegui tutto o niente!
attenzione che alcune operazioni sono viste atomiche nel linguaggio di programmazione ad alto livello, ma non in assembly.

processori RISC: due assegnamento (della parte bassa e della parte alta poichè l'istruzione deve avere la stessa lunghezza)

i++ non è atomico
ogni calcolo di espressione non è atomico

la legge di Murphy aiuta a non prendere per buono un risultato "perchè casualmente il risultato è giusto".

per risolvere il problema della banca
il più semplice problema di concorrenza

io vorrei la maniera per dire "l'intera operazione" diventasse
atomica (tutto o niente)

mutua esclusione
come si fa a imporre la mutua esclusione?
abbiamo bisogno di definire delle proprietà
problema semplice da definire difficile da realizzare
più processi "devono mettersi d'accordo"

tutti i processi devono prima o poi rispondere
tutti i processi devono dire uno tra i valori "proposti" (?)
ogni processo "corretto" deve terminare

2 tipi di proprietà
safety -> "non succederà qualcosa di male"
liveness -> "succederà qualcosa di buono"

no deadlock: (lo definiremo dettagliamo)
i processi si aspettano a vicenda e nessuno può andare avanti.
esempio:
    processo 1: voglio prendere A poi voglio prendere B
    processo 2: voglio prendere B poi voglio prendere A
    il primo processo prende A, il secondo B ma non riusciranno mai a prendere l'altro.

no starvation:
    esempio fila (ho un amico davanti quindi passo davanti a te...)
    (letteralmente: morir di fame)

no ritardi non necessari:
    non basta dire uno solo alla volta
    non basta dire una volta te e una io pk se io devo fare 10 operazioni e tu meno non potremmo funzionare
    non basta mettere priorità perchè si creerebbe starvation.

mutua esclusione -> safety
se servono sequenze infinite -> liveness
no ritardi non necessari -> safety
no starvation -> liveness
deadlock -> liveness o safety ? (fiumi di inchiostro)
             safety perchè è sbagliato permanentemente

      <------------ Nota del 24 ottobre 2021 --------------->

come si emula un programma concorrente in C?
(vedi tutto il materiale su https://so.v2.cs.unibo.it/wiki/index.php/Esperimenti_sugli_algoritmi_di_Dekker_e_Peterson )

//file.c
#include <stdio.h>
#include <pthread.h>
void * proc (void *argc) { ... }
int main (int argc, char *argv) {
    pthread_t p1, p2;
    pthread_create(&p1, NULL, proc, NULL);
    pthread_create(&p2, NULL, proc, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
}

complilato con
gcc -o eseguibile file.c -lpthread
(va linkato pthread tramite il flag -lpthread)

pthread è la maniera POSIX di creare parti concorrenti all'interno di un processo.
Serve solo per avere un idea di come funzionano le cose.
Della funzione proc in questo codice ne vengono messe due in esecuzione contemporanea.

------------------ 15 ottobre 2021 ------------^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^
Link utili oggi-> catalogo system call: http://so.v2.cs.unibo.it/wiki/index.php?title=Il_%27%27catalogo%27%27_delle_System_Call

man qsort
man gets (da non usare più), tenuta solo per retrocompatibilità.
getline
open_memstreem
man 3 funzioni di libreria che potremmo scrivere anche noi.
man 2 funzioni che non potremmo scrivere noi poichè richiedono l'interazione con il kernel tramite systemcall esempio printf = sprintf + write, la write non la potremmo scrivere.
quindi manuale 2 sono systemcall
printf = sprintf + write

fork()
il processo genitore e il processo figlio hanno memoria "privata"
tutto quello che è successo prima viene salvato anche per il figlio

getpid() -> indice del processo
getppid() -> prende pid del padre (si scrive con doppia p)

esempio: printf("mypid = %d, and my father is %d\n", getpid(), getppid());

man 2 write
man 2 wait

codice :
//file wait.c
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
int main (int argc, char *argv[]) {
        pid_t pid;
        printf ("genitore getpid() = %d\n", getpid());
        
        if(pid = fork()) {
                int ws;
                printf("%d something is true\n", getpid());
                printf("new pid is %d\n", pid);
                waitpid(pid, &ws, 0);
                //qui invece di aspettare pid potresti metterci 42, per scatenare l'errore
                if(waitpid(pid, &ws, 0) < 0)
//if(waitpid(42, &ws, 0) < 0)
                        printf("waitpid ERROR %d\n", errno);
                else if(WIFSIGNALED(ws))
                        printf("killed %d\n", WTERMSIG(ws));
                else
                        printf("wait ok %d\n", WEXITSTATUS(ws));
        }
        else {
                int *kaboom = (void *) 42; //indirizzo in memoria a caso
                sleep(1); //dormo un secondo
                printf("%d something is false\n", getpid());
                printf("parent proc is %d\n", getppid());
                *kaboom = 0;
                _exit(42);
        }
}

http://so.v2.cs.unibo.it/wiki/index.php?title=Il_%27%27catalogo%27%27_delle_System_Call


--------21 ottobre 2021-------*-+=+-*-+=+-*-+=+-*-+=+-*-+=+-*-+=+-*-+=+-*-+=+-+-*-+=+-+-*-+
Slide di oggi: https://www.cs.unibo.it/~renzo/so/materiale2122/concorrenza2122-1p.pdf

Processo = è un'attività controllata da un programma che si svolge su un processore (entità dinamica)
Programma = specifica una sequenza di instruzioni (entità statica)
Processo != programma

Assioma di finite progress

------------------ 22 ottobre 2021 ----------------~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^

differenza tra processi e thread:
un processo può avere più thread e questi possono condividere le risorse tipo la memoria tra loro, diversi processi invece non possono condividere le risorse direttamente (hanno memoria privata) ma possono usare altre tecniche come message passing. Nulla vieta di avere molti processi che collaborano tra loro con message passing e ogniuno di loro è multi-threading.

man strace
fa vedere tutte le systemcall che il processo ha chiamato.
esempio di uso:
    gcc -static -o file file.c
    strace ./file
(-static serve per vedere esattamente cosa fa file.c con strace senza mischiarlo con le systemcall chiamate dal compilatore e linker)

man 3 exec (tante)
man 2 execve 
simile alla fork(), ma mentre la fork crea un nuovo processo execve crea nuovo thread.
printenv

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc, char *argv[]) {
        printf("%d", getpid());
        int status;
        switch( fork()) {
                case 0:
                        execvp(argv[1], argv+1);
                        perror("exec");
                        return 0;
                default:
                        wait(&status);
                        printf("done!\n");
                        break;
                case -1:
                        perror("fork");
                        exit(-1);
        }
}

man 2 brk
-> break
man 2 read


//file copia.c //copia che usa le sistemcall
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#define  BUFLEN 1

int main (int argc, char *argv[]) {
        //vogliamo copiare file
        char buf[BUFLEN];
        ssize_t n;
        //apri il file primo argomento in lettura
        int fin = open(argv[1], O_RDONLY);
        //file output in scrittura or crealo or e se esiste butta via il contenuto.
        int fout = open(argv[2], O_WRONLY | O_CREATE | O_TRUNC , 0666);
        while( (n = read(fin, buf, BUFLEN)) > 0)
                write(fout, buf, n);
        }
        close(fin);
        close(fout);
}

0666 : 0 perchè è scritto in ottale, 666 per i permessi (rwd)

//fcopia.c //copia che usa i file
#include <stdio.h>

int main (int argc, char *argv[]) {
        //vogliamo copiare file
        int buf;
        //apri il file primo argomento in lettura
        FILE *fin = fopen(argv[1], "r");
        //file output in scrittura or crealo or e se esiste butta via il contenuto.
        FILE *fout = fopen(argv[2], "w");
        while( (buf = getc(fin) ) != EOF) {
                putc(buf, fout);
        }
        fclose(fin);
        fclose(fout);
}

se compiliamo copia.c e fcopia.c e poi facciamo
time ./copia filein fileout
e
time ./fcopia filein filout
notiamo che copia che usa le sistemcall per ogni byte usa molto più tempo della liberia c

//nel ultima modifica abbiamo messo
#define BUFLEN 8000 
per copiare 8k alla volta e non solo un byte per volta.

----------------- 29 ottobre 2021 -------------/^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\ /^(o.o)^\

riassunt(azz)o:
systemcall
erno contiene codice dell'errore

creare processi ed eseguire programmi sono separati
esempio fork()
non fate mai la fork bomb : while(1) {fork()} !
riempite di processi il sistema con 2^n

getpid getppid

dualità programma processo
exec
execque prende come parametri pathname del file da eseguire e come secondo parametro v il vettore di argomenti e l'environment (lista di stringhe nome = valore),
char **environment
vettore di puntatori, ogni puntatori punta alla stringa
puntatore nulla posto alla fine

cosa succede alle risorse in exec e execque
viene ereditato il pid
non viene eriditato nel fork
apparente copiata memoria
non copiata la memoria con exec

systemcall correlate ai file
open
read
write
close
e riprendiamo da qui

man 2 start
man 2 chown
cambia proprietari di un file (non è l'unico)
quelle che non hanno prefisso
quelle cha hanno prefisso: file aperto con descriptor???
man 2 fstat o man 2 stat

man 2 pipe


----------4 novembre 2021---------c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c
slide di oggi: https://www.cs.unibo.it/~renzo/so/materiale2122/concorrenza2122-1p.pdf

Algoritmo di Peterson
+ dimostrazione

Disabilitazione degli interrupt
e se l'utente gli disattiva ma non li riattiva? blocco del sistema
quindi deve farlo il kernel in qualche modo...

Test & Set
TS(x,y) ..........
Istruzioni Speciali

semafori                                          (pag 94 delle slide concorrenza-1p)
   V (dal olandese Verhogen ==> Segnalare) non è bloccante
      esempio: voglio comunicare che la risorsa (che stavo usando) è disponibile.
   P (dal olandese Proberen ==> Attendere) può essere bloccante.
      esempio: ho bisogno di una risorsa, la risorsa è disponibile, non mi blocco.
esempio 2: (pag. 98)
    chi prepara la torta finisce dice la torta è pronta (fa una V)
    quando arriva chi vorrebbe mangiare la torta (fa una P)
    se chi mangia è molto vorace (fa una P) e la torta non è ancora pronta il valore è negativo quindi deve aspettare.

Semafori binari (= variante dei semafori in cui il valore può assumere solo i valori 0 e 1)
    ha lo stesso potere espressivo, ma nei semafori binari anche la V è bloccante.
é possibile utilizzare un semaforo binario per implementare un semaforo generale(p111)
un semaforo mutex per garantire mutua esclusione sulle variabili
un semaforo privato creato in allocazione dinamica per ogni processo che partecipa
una coda per garantire fairness


--------5 novembre 2021--------

--------11 novembre 2021 --------c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c"c
slide concorrenza :
     https://www.cs.unibo.it/~renzo/so/materiale2122/concorrenza2122-1p.pdf 
recap su semafori  P(), V()

- produttore/consumatore (slide 116)

Nota1: notiamo che non siamo in critical section per produrre o consumare un valore, ma ne abbiamo bisogno per condividere la variabile. Se ci servisse la critical section per produrre o consumare mentre il produttore dovrebbe stare fermo, ma non serve perchè la variabile è stata salvata in una sua variabile locale quindi non viene persa.
Nota2: Inoltre non possiamo usare un solo semaforo poichè non possiamo permettere che si consumano più risorse di quante vengono prodotte (ad esempio), cosa che invece è permessa dalla critical section con un solo semaforo (se l'altro non ne ha bisogno possiamo entrare ed uscire dalla critical section a piacimento, ma questo viola il principio di prima).

-buffer limitato

Nota3:
    per avere più di una risorsa (slide 118), occorre Mutex per poter evitare la race condition sulla coda.
    empty = new Semaphore(SIZE) => partiamo con SIZE risorse vuote
    full = new Semaphore(0) => partiamo con 0 risorse già pronte
    mutex = new Semaphore(1) => partiamo con 1 processo che può andare in mutua esclusione alla volta
Nota4: non cè possibilità di deadlock perchè un produttore non aspetta mai un consumatore che aspetta un produttore
e non cè possibilità di starvation purchè sia fair (è come se si usasse il ticket della coop)

- filosofi a cena (slide 120)

problemi:
    deadlock (esempio prendono tutti la bacchetta a destra partendo in contemporanea, ed è deadlock)
    congiura dei filosofi, starvation.
    ovvero si organizzano per impedire di far mangiare un dato filosofo.
soluzioni sbagliate proposte:
    - uno alla volta: cerco di risolvere il problema della concorrenza togliendo la concorrenza, non è una proposta valida.
    - mutex prima di prendere le bacchette e dopo: un processo che non riesce a prendere le bacchette blocca tutti gli altri perchè è nella mutex, deadlock sicuro.
    - non disponibili, rilascio la bacchetta: creo starvation aspetto un tempo random e sono sfortunato
    - coppia già assegnata: tutti possono rilasciare la bacchetta in contemporanea per pensare
    e dopo cè lo stesso problema di prima.
    
soluzione: inserire la asimmetria (quindi non tutti mancini o non tutti destrorsi)
- lettori e scrittori (slide 134)
problemi: i lettori creano starvation per gli scrittori


-----------18 novembre 2021------------*@*-*@*-*@*-*@*-*@*-*@*-*@*-*@*-*@*-*@*-*@*-*@*-*@*-*@*-*@*-
slide concorrenza:
 https://www.cs.unibo.it/~renzo/so/materiale2122/concorrenza2122-1p.pdf 

Computer nuovo :D
Ripasso lettori e scrittori + filosofi a cena

/* Variabili condivise */ 
int nr = 0; 
Semaphore rw = new Semaphore(1); 
Semaphore mutex = new Semaphore(1); 
void startRead() { 
    mutex.P(); 
    if (nr == 0) rw.P(); 
    nr++; mutex.V(); 
} 
void startWrite() { 
    rw.P(); 
} 
void endRead() {
    mutex.P(); 
    nr--; 
    if (nr == 0) rw.V(); 
    mutex.V(); 
} 
void endWrite() { rw.V(); }

==> o uno scrittore o (uno o più) lettori !
(non è una mutex di un solo processo ma solo 1 scrittore o molti lettori (1 o più))

slide 134 --> 151

MONITOR
= è un modulo software che consiste di dati locali, una sequenza di inizializzazione e una o più "procedure"
Sintassi:
monitor name {
variable declarations . . .
procedure entry type procedurename1 (args . . .) {
. . .
}
type procedurename2(args . . .) {
 . . .
}
name(args . . .) {
. . .
}
}

Caratteristiche base:
solo un processo alla volta può essere all'interno del monitor
-> il monitor fornisce un meccanismo di mutua esclusione

Meccanismi di sincronizzazione
- dichiarazione di variabili di condizioni CV: 
condition c;
- operazioni definite sulle CV: 
c.wait()  = sospende il processo richiamato in una coda di attesa della condizione c
c.signal() = chi chiama viene messo in attesa e verrà riattivato solo quando il processo che è stato riattivato avrà rilasciato la mutua esclusione (urgent stack)

=> vedi immagine della slide 160

Signal urgent (SU) 

-----------19 novembre 2021----------

----------- 2 dicembre 2021 ------------^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^
sezione 6 message passing di concorrenza-1p.pdf

message passing
paradigma di programmazione concorrente diverso
prima parte
aveveamo ambienti a memoria condivisa
non era difficile trasferire informazioni
il problema era la sincronizzazione

qui invece siamo nel duale
tutti i processi hanno la propria memoria privata
tutte le sol che prevedono memoria condivisa è sbagliata
risolvere il problema segnato usando passaggio di messaggi

vogliamo spedire a un destinatario specificato
receive da un mittente specifico o qualsiasi
siano fifo siano fair
il primo mandato è il primo ricevuto

se un processo manda prima m1 poi m2
siete sicuri che riceva prima m1 poi m2
se due processi mandano rispettivamente m1 e m2
non vi è alcuna certezza del ordine

chi è bloccante chi non lo è ? tassonomia vedi slide 186
sincrono => entrambe bloccanti
Asincrono => asend non è bloccante
COMPLETAMENTE Asincrono => entrambe non bloccanti

quale modello di message passing stiamo usando?
ssend e sreceive per la sincrona
asend e areceive per asincono
asend non bloccante quindi i messaggi inseriti in una coda (FIFO)
nb-receive (not blocking receive) per completamente asincrono

mp sincrono dato quello asincrono (slide 192)

slide 193
p è da interpretare come getpid()
p mittente q destinatario
quindi nel server è giusto che sia rcv(q,p) invece che rcv(p,q)

sezione 7 conclusioni
pag 198

potere espressivo pag 199
riflessiva antisimmetrica e transitiva

esercizio:
message passing
asincrono o sincrono a scelta
send a quello dato
receive riceve SEMPRE da chiunque (sempre any)

hanno lo stesso potere espressivo rispetto a quello che deve specificare il mittente?

anyreceive:

caso 1a: dato: asend(m, d); m = arecv(s) ==> aarsend(m, d): m = aarrecv()
def aarsend(m, d): asend(m, d)
def aarrecv(): return arecv(ANY)

caso 1b: dato: aarsend(m, d); m = aaerecv(s) ==> asend(m, d): m = arecv()
def asend(m,d): aarsend(<getpid(), m>, d)
def arecv(s):
        NOOOOOOOO! : <sender, m> = aarecv(); if(sender == s) return m;
        problemi: consumo un messaggio non per me
        NOOOOOOOO! : do {<sender, m> = aarecv(); } while(sender != s); return m;
        problemi: perdo i messaggi degli altri
                questi devono essere salvati in una struttura dati (ovviamente privata)
        OK! : while ((m = msgdb.get(s)) == NULL) {
                  <sender, m> = aarecv();
                  msgdb.add(sender, m);
              }
              return m;
        //finchè non ricevo messaggio buono provo a ricevere altri messaggi

caso 2a: dato: ssend(m, d); m = srecv(s) ==> sarsend(m, d): m = sarrecv()
def sarsend(m, d): ssend(m, d)
def sarrecv(): return srecv(ANY)

caso 2b: dato: sarsend(m, d); m = sarrecv(s) ==> ssend(m, d): m = srecv()
def ssend(m, d) : 
    sarsend(<getpid(), m>, d)
def  srecv(s):
    while((m = msgdb.get(s)) == NULL) {
    <sender, m> = sarrecv();
    msgdb.add(sender, m);
    }
    return m;
problemi: se il receiver ha messo il messaggio nel buffer
non dovrebbe liberare il mittente che glielo ha mandato
perchè non ha effettivamente "letto" il messaggio
per farlo corretto dobbiamo "cambiare le carte in tavola"
la ssend oltre a questo deve aspettare un attimo
la ssreceive invece di mdgdb usa smsgdb perchè ...?
quindi il codice diventa:
 caso 2a riformulato: dato: ssend(m, d); m = srecv(s) ==> sarsend(m, d): m = sarrecv()
def sarsend(m, d): ssend(m, d)
def sarrecv(): return srecv(ANY)

caso 2b rifortulato: dato: sarsend(m, d); m = sarrecv(s) ==> ssend(m, d): m = srecv()
def ssend(m, d) : 
    sarsend(<getpid(), m>, d)
    sarrecv();  <<--- aggiunta ora per aspettare fittiziamente la lettura effettiva
    # oppure al posto di sarrecv() metto:
    while(1) {
        <sender, m> = sarrecv();
        if (sender == d && m == ack) break;
        smsgdb.add(sender, m);
    }
def  srecv(s):
    while((<sender, m> = smsdb.get(s)) == NULL) {  <<--- usare Smsgdb invece che msgdb
    <sender, m> = sarrecv();
    smsdb.add(sender, m);
    }
    sarsend(<getpid(), ack>, sender);  <<---
    return m


╔══barzelletta di oggi:══════════════════════════════════════════════╗
║       tutti questi scienziati devono uscire dal carcere                                                                       ║
║       hanno a disposizione una scatoletta di tonno                                                                          ║
║       l'ingegnere ha forgiato una chiave ed è uscito                                                                         ║
║       il fisico ha bombardato di particelle ed è uscito                                                                       ║
║       il matematico non è riuscito e dopo settimane tornano a vedere la cella                             ║
║       e con il sangue c'è scritto:                                                                                                           ║
║       supponiamo per assurdo di essere riusciti a uscire dalla galera ...                                         ║


nei message passing sincroni non è permesso mandare messaggi a se stessi perchè altrimenti sarebbe automaticamente deadlock!

=^..^=   =^..^=   =^..^=    =^..^=    =^..^=

e se invece volessi LIFO come si può fare?
per capire qual'è l'ultimo devo riceverli tutti e poi vedere
per il asincrono solamente mi automando un messaggio
limite (bound) "tutti quelli ricevuti prima del messaggio che mi sono mandato io"
ricevere tutti i messaggi in giacenza -> mi automando un messaggio

caso 1a: dato: asend(m ,d); m = arecv(s) ==> lifoasend(m, d); m = lifoarecv()
def lifoasend(m, d):
            asend(<getpid(), m>, d);
def lifoarecv(s):
        asend(<getpid(), TAG>, getpid());
                while (1) {
               <sender, m> = arecv(ANY);
                if (sender == getpid() && m == TAG) break;
                msglifodb.add(sender, m);
        }
        while ((m = msglifodb.get(s)) == NULL) {
                <sender, m> = arecv(ANY);
                msglifodb.add(sender, m);
        }
        return m;

decalogo di programmazione concorrente
http://so.v2.cs.unibo.it/wiki/index.php?title=Decalogo_di_Programmazione_Concorrente

Votazioni per fare python anche con lui
risultati : 17 si
1 yes

---------3 dicembre2021-------u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.
slide descrittive del progetto: https://www.cs.unibo.it/~renzo/so/pandosplus/docs/Presentazione_uMPS3.pdf
slide 19 contiene link per tutte le risore utilizzabili:
    - http://www.cs.unibo.it/~renzo/so/pandos/docs/uMPS3princOfOperations.pdf
    - https://github.com/virtualsquare/umps3
    - https://wiki.virtualsquare.org/##!education/umps.md


PandaOS+
MIPS
installazione: http://github.com/virtualsquare/umps3
compilatore -> su github

WAIT()  -> invoca l'istruzione assembler, mettendo in attesa il processore in attesa di un interrupt
MCTL_POWER
DEV_REG_ADDR

manuale dell'emulatore -> è descritto tutto lì

Specifiche fase 1:
slide fase 1: https://www.cs.unibo.it/~renzo/so/pandosplus/docs/phase1_2122.pdf
livello 0 e livello 1 già forniti!
livello 2: gestione di strutture dati
- devo avere in esecuzione una serie di processi
implementare delle funzioni ai PCB:
* allocazione e deallocazione 
* gestione delle Code
* gestione dell'Albero
* gestione di una Active Semaphore Liste (ASL)
[...]------8<------------------
Liste del Kernel di Linux


Consegna fase 1:
La deadline di consegna è fissata per il giorno: Domenica 20 Febbraio 2022, ore 23.59
CONSEGNARE IL PROPRIO PROGETTO (un unico file .tar.gz) NELLA CARTELLA DI CONSEGNA ASSOCIATA AL PROPRIO GRUPPO.
CONSEGNARE ENTRO LA DEADLINE FISSATA. 
Dopo la deadline vi verra’ inviata un’email automatica con il resoconto della consegna. 
CONTROLLATELA per verificare che tutto sia andato bene (e.g. l’archivio consegnato non era corretto, era vuoto,...)
Cosa consegnare: 
– Sorgenti del progetto (TUTTI) 
– Makefile per la compilazione 
– Documentazione (scelte progettuali, eventuali modifiche effettuate nelle specifiche) 
– File AUTHORS, README (eventuale)
Inserire commenti nel codice per favorire la leggibilita’ e la correzione … 
PROGETTI non COMMENTATI NON SARANNO VALUTATI.


-----9 dicembre 2021------
slide shell: https://www.cs.unibo.it/~renzo/so/materiale2122/lso-04-shell-1p.pdf

> Script
> variabili built-in per lo scripting

script
cosa sono?  sono testi (scritti con qualsiasi editor)
 a cui successivamente è stato attivato il bit di esecuzione
          gedit SCRIPTNAME    //scrive il file con un editor di testo tipo gedit
          chmod +x SCRIPTNAME //attiva il bit di esecuzione per tutti
come si scrivono?
comandi di shell e altre cose viste dopo:
la prima riga indica chi deve eseguire lo script
quindi si scrive
#!bin/bash
i commenti si scrivono con #

uso delle virgolette semplici, doppie e rovesciate

esempio di script:

#!/bin/bash

dest = !"renzo"

sed 's/^/-->/' << ENDTAG

caro $dest
buona giornata
saluti a $dest

ENDTAG

exit 42

#nota: >> appende, > tronca
# ENDTAG dice quando inizia e quando finisce sed, e può essere usato anche con altri termini
#basta che l'inizio e la fine siano uguali (esempio ENDTAGXXX ... ENDTAGXXX)
expr comando che valuta le espressioni
let a = 2+2 per le variabili

cmp confronto (compare)
diff per i file di testo, cmp per i file binari, e se abbiamo bisogno di fare solo compare ma non la mappa delle differenze facciamo cmp (esempio non abbiamo bisogno che ci dica " i due flle differiscono alla linea 5")

altro script:
#!bin/bash
if cmp -s $1 $2
then
    echo uguali
else
    echo diversi
fi
#alcune strutture di controllo sono il reverse del comando
#if finisce con fi
#case finisce con esac

[] serve per chiamare il comando [] che è un test
if [-n "$2"] 
significa se il parametro due è non nullo
. (punto) è un alias di source
è come l'include dei c

[ 23 -le 123]
echo $?
è vero che 23 è minore uguale a 123 (stringa interpretata come numero)
[ 23 \< 123 ]
il primo carattere 2 è più grande del primo carattere 1 (stringa interpretata come stringa)

esempio di script
#!bin/bash
whille [ -n "$1" ]
do
    echo process $1
    shift
done
# si usa done invece di while al contrario (elihw)
#se non metto le "" al 4 (/script tizio caio sempronio) cè il parametro ma vuoto

nelle slide pag 74
*[!0-9]* significa fai il test se la stringa contiene almeno un carattere che non sia un numero

du -s                 //significa  disk user (du) summarize (-s)
sort -gr vecchia , nuovi si usa -nr 
in ordine numerico r reverse
head -$lines      //prendi le prime N (parametro contenuto nella variabile lines) linee

basenime dirname

come fare debug?

set -x lo script mentre funziona faccia vedere che comandi ha invocato

dove era l'errore? il trattino non era un trattino (copia)



--------10 dicembre 2021--------Esercizi di "lettura" programmi in Python
http://so.v2.cs.unibo.it/wiki/index.php?title=Esercizi_di_%22lettura%22_programmi_in_Python

Scopo di C in Sistemi Operativi
- giudato dalla sintassi
- pensi all'architettura del compilatore
- variabili def, con un loro scopo
- efficienza, usato nei punti cruciali -> il codice macchina consente di fare le peggiori d(i)avolerie XD
- gestione dell'errore -> non prevista

Scopo di Python in Sistemi Operativi
> tu scrivi il programma, lui pensa dove è stato memorizzato un dato
> visuale del codice
> variabili esistono quando vengono usate con un tipo che dipende il valore assegnato (-> una stessa variabile può cambiare tipo all'interno dello stesso programma)
interi hanno una memorizzazzione di qualsiasi grandezza
> gestione dell'errore -> prevista

=> c'è sempre un interprete con una macchina astratta

# Es 1
def dd(s):
  d,m,y = s.split('.')                  #assegnamento tra tuple = asssegnamento di elementi corrispondenti
  d,m,y = int(d),int(m),int(y)  #casting
  md = [0,31,28,31,30,31,30,31,31,30,31,30,31]   #lunghezza dei mesi escludendo il mese 0, poichè gennaio=1
  if y % 4 == 0:
    md[2]=29
  dy = d
  for i in range(1,m):               #
    dy += md[i]
  return dy
 if __name__ == "__main__":
  data=input("Dammi una data (es. 14.03.2014): ")
  print("il risultato della funzione misteriosa e' ", dd(data))

# Es 3 - codice decriptazione di Cesare
def r13(s):
        e = ''
        for c in s:
                if 'A' <= c <= 'Z':
                        e += chr(((ord(c)-ord('A')) + 13)%26 + ord('A'))
                elif 'a' <= c <= 'z':
                        e += chr(((ord(c)-ord('a')) + 13)%26 + ord('a'))
                else:
                        e += c
        return e
 if __name__ == "__main__":
        s = input("scrivi una semplice frase (es: 'Ciao Mondo') ")
        r = r13(s)
        print("il risultato della funzione misteriosa e': ", r)
        print("consiglio: prova a eseguire nuovamente il programma scrivendo")
        print("la frase: ", r)

# Es 4 - Scomposizione in fattori primi
def pf(n):
        f = []
        for i in range(2,n+1):
                while n % i == 0:    #fattori primi del numero
                        f.append(i)
                        n //= i
                if n == 1:
                        break
        return f
 if __name__ == "__main__":
        n = int(input("dammi un numero (consigliato da 2 a 1000): "))
        print("il risultato della funzione misteriosa e' ", pf(n))

# Es 5 - Stima dell'area di una figura attraverso la somma dei punti (circonferenza)
import turtle             # (richiede il modulo turtle)
import random
 def cy(r):
    turtle.setup(400,400)
    turtle.penup()
    turtle.hideturtle()
    turtle.speed(0)
    for i in range(1000):
        x,y = random.randint(-200,200),random.randint(-200,200)
        if (x*x + y*y)**0.5 < r:
            color = "red"
        else:
            color = "blue"
        turtle.goto(x,y)
        turtle.dot(5,color)
 if __name__ == "__main__":
    r = float(input("scrivi un numero (consigliato da 100 a 200): "))    #inserisci il raggio della circonferenza
    print("guarda il risultato della funzione misteriosa")
    cy(r)
    turtle.exitonclick()

# Es 7
Questo e' contorto. Se vi piace... iniziate a preoccuparvi: state diventando informatici ;-)
f="f={0}{1}{0};print(f.format(chr(34),f))";
print(f.format(chr(34),f))
NB: non e' necessario capire questo programma, e' solo per chi si vuole cimentare in una sfida...

# Es 8 - Cerca i fattori con mcm e MCD
-> non lo incollo perchè è troppo lungo... andate sul link a guardarvelo

fgcd -> funzione ricorsiva con rgcd, dichiarata al suo interno
se fa il merge è ricorsivo nelle liste prendendo gli elementi ripetuti o meno

# Es 9 e 10 -  come 8 ma con dichiarazione dei divisori è dichiarativa
-> non lo incollo perchè è troppo lungo... andate sul link a guardarvelo

def div(n):
        return {x for x in range(1,n+1) if n % x == 0}
#alternativa visione procedurale:
    #def div(n):
        #       d=set()
        #       for x in range(1,n+1):
            #               if n % x == 0: d.add(x)
            #       return d

#stessa cosa con i moltiplicatori nella funzione mul

#Es 13 - esce la canzone della vecchia fattoria grazie a Knuth <3 che ha analizzato la complessità delle canzoni

Struttura di un programma python:
    def funzioni
    variabili
    e codice eseguibile in fondo al file come se fosse ilmain, si può mettere anche nel mezzo del codice ma ne riduce la leggibilità
if __name__ = "__main__":
    Do Something here

Python è un linguagio ricco di suo, perchè ci sono molte strutture dati e tanta ricchezza sintattica
Python è ricco di famiglia, è stata creata una gama di librerie specializzate ampissima che completano
Identazione fondamentale-> TAB != 4 spazi
Funzioni != Procedure -> return NUN (=NULL)
tutto ciò che è NULL -> è False    # così come in C
i : aprono un nuovo contesto -> c'è un livello di identazione in più
gli * messi come paramentro, se c'è un argomento *x, nella variabile x vogliamo che vengano messi tutti gli elementi posizionali nella lista

# Es 17 - calcolatrice a notazione polaccca inversa

# Es 18 - Torre di Hanoi con integrazioni seguenti
File sq.py
def square(x):
print(f"square {__name__}")
return x*x
if __name__=="__main__":
print (f"MAIN {square(42)}")

File m.py
import sq
if __name__ == "__main__:
    print(f"MAIN{sq.square(42)}")

# Es 19 - Problema di dato un numero in cifra, scrivere il numero in lettere
# Es 20 - Fibonacci

liste, insiemi -> sono mutabili
tuple.... -> non sono mutabili

File flexibleinput.py
import sys

mysecretpasswd = "33trentini"
#try:
i = eval(sys.argv[1])    #python deve valutare che tipo è, così se inserisci un numero di qualsiasi tipo funziona: interi, float, numeri complessi. MA ha dei problemi
#except ValueError:
#pass  #ramo vuoto
print(i)
se uso le cose commentate posso convertirlo con tutti i tipi di variabile (usando try ed except):
    i = int(sys.argv[1])    oppure     i =  float(sys.argv[1])     oppure      i = complex(sys.argv[1])

lbyl (=look before you leap) eafp (=easy ask forgiveness than permission)
(ovvero "guarda prima di saltare" versus  "è più facile chiedere perdono che permesso")

Espressione with
import sys

with open(sys.argv[1], "r") as f:    #dichiarativo, considero f come open, fai questo:
#f= open(sys.argv[1], "r")    #leggiamo un file
for line in f:
#print(line)   #stampa il file in input una riga si e una no, perchè la linea contiene già "\n"
print(line, end="")   #metto il delimitatore a null
#f.close()   # non serve più chiudere il file se usiamo with, perchè lo chiude automaticamente

Libreria os
import os
dir(os)   #mi mostra le sue funzioni (fork, execv, execve, execvp ...)
dir(2)     #cosa posso fare con l'intero 2? -> mi da le directory che posso usare
-> funzioni __QUALCOSA__ vengono chiamate automaticamente quando python fa il parsing, puoi definire una classe che vuoi e puoi definire cosa per la tua funzione fa, puoi definire le tue espressioni algebriche con il simbolo che preferisci e lo considererà tale (?)
__str__ = quando vuoi trasformare il valore di una variabile in una stringa
__getitem__ = puoi stabilire cosa vuol dire ([]) c'è un qualche concetto di indice che puoi definire tu se dichiari una funzione getitem

u.u Vedi Esempi didattici in Python>Coda e Pila: http://so.v2.cs.unibo.it/wiki/index.php?title=Esempi_didattici_in_Python#Coda.2FPila_e_programmazione_a_oggetti
class queue:
__init__ = costruttore

Domande finali
cosa vogliamo fare la settimana prossima?
a. approfondimenti su caratteristiche che abbiamo visto, sia su linguaggi, che su approfondimenti visti a lezione
b. esercizi su programmazione concorrente
c. festa XD

----------- 24 febbraio 2022 ------------
slide 0_intro.pdf
link: https://www.cs.unibo.it/~renzo/so/materiale2122/00-intro.pdf

#SO come macchina estesa
in questa visione un SO: nasconde ai programmatori i dettagli dell’hw e fornisce una API conveniente e facile da usare, agisce come intermediario tra programmatore e hw. indipendenza dall’hw, comodità d’uso, programmabilità.
xes: floppy disk drive, per accedere al floppy mandando messaggi al controller deve inizializzare il device, accendere il motore e aspettare il numero di giri corretto, spostare la testina, leggere il settore, spegnere il motore
servizi estesi offerti da un SO: esecuzione di programmi, accesso semplificato/unificato ai dispositivi io, accesso a file system, accesso a networking, accesso al sistema, rilevazione e risposta agli errori, accounting
core dunk, in caso di errori (xes segmet fault) posso settare un coredunk, viene scaricato il nucleo in un file lo stato della momoria al momento dell’incidente, post mortem debugging

# Storia dei sistemi operativi:
Generazione 0 (1792-1871):
Babbage: prima idea di esecuzione di programma, cerca di costruire la macchina analitica, programmabile, meccanica
Periodo di grandi cambiamenti, rivoluzione francese, costituzione americana

Generazione 1 (1945-1955):
Costruiti con valvole termoioniche (invenzione di Edison) e tavole di commutazione


