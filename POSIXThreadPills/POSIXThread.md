# POSIX thread (pthread.h library)
La libreria dei thread POSIX offre un'API standard per gestire i thread in C/C++. Consente di creare processi concorrenti in modo efficiente, soprattutto su sistemi multi-processore o multi-core. Rispetto alla creazione di nuovi processi, l'uso dei thread richiede meno risorse, permettendo un'esecuzione più veloce del software. I thread in questo caso condividono lo stesso spazio di memoria (memoria virtuale che viene allocata quando un programma viene eseguito e quindi viene generato un processo), a differenza di MPI in cui ogni thread ha il suo spazio di memoria privato in cui copiare le informazioni. Tuttavia, mentre i thread sono limitati a un singolo sistema informatico, le tecnologie di programmazione parallela come MPI e PVM sono utilizzate anche in ambienti di calcolo distribuito.

## Creazione/terminamento dei Thread e passaggio di argomenti
Quando viene generato un thread, questo viene fatto partire con l'assegnazione di una _thread function_ (una normale funziona che però è di tipo void* e ha un singolo parametro sempre void*), quando questa termina (_return_), anche il thread termina. Ogni thread assegnato ad un processo viene riconosciuto tramite _thread ID_ che può essere richiamato nel codice dichiarando il tipo **pthread_t**.

Iniziamo creando un semplice thread che stampa la scritta HelloWord:

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function( void *ptr );

main()
{
     pthread_t thread;
     char *message = "Hello World!";
     int  iret

     iret = pthread_create(&threadID, NULL, print_message_function, (void*) message);

     pthread_join(thread, NULL);

     printf("Thread returns: %d\n", iret);
     exit(0);
}

void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
}
```
Nell'ordine abbiamo dichiarato un thread ID, un puntatore a char che rappresenterà il messaggio e un intero che ci verrà successivamente restituito dalla funzione _pthread_create_, questa restituirà il codice 0 se l'esecuzione del thread è andata a buon fine e un codice diverso da 0 se ci sono stati errori durante la creazione o l'esecuzione dello stesso.

**pthread_join** viene utilizzata per aspettare la terminazione di un thread specifico. Quando un thread principale (o un altro thread) chiama pthread_join su un thread "figlio", il thread chiamante attende fino a quando il thread "figlio" specificato termina la sua esecuzione. Questa funzione accetta due parametri: l'identificatore del thread da attendere e un puntatore che riceverà il valore di ritorno del thread terminato, se ce n'è uno. Il valore di ritorno può essere impostato con la funzione **pthread_exit**, questa viene utilizzata per terminare il thread corrente in modo esplicito, restituendo un valore opzionale. Quando un thread chiama _pthread_exit_, termina immediatamente la sua esecuzione e restituisce il valore specificato. Il sistema operativo libera automaticamente le risorse associate al thread (come la memoria) una volta che è terminato.

Vediamo un esempio più esteso:
```c
#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS 5

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int iret;
   long t;

   for(t = 0; t < NUM_THREADS; t++) {
      printf("In main: creating thread %ld\n", t);
      iret = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
      if (iret) {
         printf("ERROR; return code from pthread_create() is %d\n", iret);
         exit(-1);
      }
   }
   
   pthread_exit(NULL);
}
```
In questo caso andiamo a generare più thread dichiarando un array di _NUM_THREADS_ elementi a cui successivamene assegniamo una funzione per stampare la propria posizione in lista. In questo caso utilizzare o meno la funzione _pthread_exit_ non altera il comportamento del codice (vediamolo come una return ma riferito allo stato del thread!).

Abbiamo quindi visto come il metodo _pthread_create()_ consente a noi sviluppatori di passare un singolo argomento al thread generato (nel nostro caso il messaggio da stampare). Come facciamo se vogliamo passarne più di uno? Possiamo bypassare questa cosa creando una struttura contenente tutti i nostri argomenti e passarla alla funzione tramite puntatore:
```c
// Come viene creata la struttura.
struct thread_data{
   int  thread_id;
   int  sum;
   char *message;
};

struct thread_data thread_data_array[NUM_THREADS];

void *PrintHello(void *threadarg)
{
    // Come passare ed estrapolare le informazioni dalla struttura.
   struct thread_data *my_data;
   ...
   my_data = (struct thread_data *) threadarg;
   taskid = my_data->thread_id;
   sum = my_data->sum;
   hello_msg = my_data->message;
   ...
}

int main (int argc, char *argv[])
{
    // Suppondiamo di aver creato un array di thread_data e ad ciclo lo riempiamo con le nostre info.
   ...
   thread_data_array[t].thread_id = t;
   thread_data_array[t].sum = sum;
   thread_data_array[t].message = messages[t];
   iret = pthread_create(&threads[t], NULL, PrintHello, (void *) &thread_data_array[t]);
   ...
}
```
## Unione e distacco dei Threads (funzioni _join_ e _detach_)
Quando un programma viene eseguito, automaticamente viene generato un processo assegnato ad un *Master Thread*, questo può creare a sua volta altri thread per la distribuzione parallela del carico di lavoro. A loro volta questi thread "figli" possono avviare altri thread formando una struttura ad albero. La gestione di tutti questi thread avviene mediante sincronizzazione. Tra i metodi utilizzati per la sincronizzare i POSIX threads ci sono il **join** (che vedremo tra poco), **mutex** e **variabili di condizione** (che vedremo in seguito).

![Sync routing using join method](image.png)
Questa è la routine di sincronizzazione che avviene con l'utilizzo dei metodi _create_/_join_/_exit_. La funzione **pthread_join()** blocca il thread chiamante finché non termina il thread specificato (in questo caso metterà in pausa il Master Thread fin quanto i worker threads non avreanno terminato la loro esecuzione). Tramite la funzione **pthread_exit()** è possibile termianare i worker threads e consentire al programmatore mediante codici di errore di gestire il proseguo del programma (solitamente se la funzione exit ritorna come codice di ritorno _zero_, il Master Thread può continuare la normale esecuzione del programma).

La funzione **pthread_detach()**, invece, può essere banalmente utilizzata per scollegare esplicitamente un thread anche se è stato creato come unibile (di base tutti quelli creati come nei precedenti esempi sono unibili).

Vediamo un esempio sull'utilizzo del _join_ (sembra difficile ma non è nulla di diverso da quello che abbiamo già visto!):

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Definiamo il numero di thread
#define NUM_THREADS	4

// Implementiamo una fuzione complessa (non ci interessa il contento)
void *BusyWork(void *t)
{
   int i;
   long tid;
   double result = 0.0;
   tid = (long)t;
   printf("Thread %ld starting...\n",tid);
   for (i=0; i<1000000; i++)
   {
      result = result + sin(i) * tan(i);
   }
   printf("Thread %ld done. Result = %e\n", tid, result);
   pthread_exit((void*) t);
}

int main (int argc, char *argv[])
{
   pthread_t thread[NUM_THREADS];
   pthread_attr_t attr;
   int rc;
   long t;
   void *status;

   // In questo modo inizializziamo i futuri thread come joinabili (di default già lo sono).
   // Nota che per tutti i precedenti esercizi il paramentro &attr era NULL. Ora gli diamo un senso!
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   // Creiamo e controlliamo lo status code sulla creazione di ogni thread.
   for(t = 0; t < NUM_THREADS; t++) {
      printf("Main: creating thread %ld\n", t);
      rc = pthread_create(&thread[t], &attr, BusyWork, (void *)t);
      if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

   // Attendiamo e controlliamo lo status code sulla terminazione di ogni thread.
   pthread_attr_destroy(&attr);
   for(t=0; t<NUM_THREADS; t++) {
      rc = pthread_join(thread[t], &status);
      if (rc) {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(-1);
      }
      printf("Main: completed join with thread %ld having a status
             of %ld\n",t,(long)status);
   }

   printf("Main: program completed. Exiting.\n");
   pthread_exit(NULL);
}
```

Per riassumere, la sincronizzazione dei thread è un processo spesso obbligatorio in quanto, introducendo il parallelismo, l'ordine di esecuzione del codice non è riga per riga come appare su schermo (e come avviene nel codice sequenziale), ma può variare in quanto l'ordine in cui creiamo i thread non è sempre quello in cui vengono avviati (oltre al fatto che potrebbero avere velocità diverse). Questo può portare ad un problema di **race condition**, ossia quella condizione in cui i thread fanno "gara" per terminare ma ciò avviene in tempi e ordini differenti e potrebbero restituire risulati inaspettati.

Un altro problema problema riguarda l'utilizzo di variabili uniche globali/statiche a cui i thread potrebbero accedere e leggere/modificare scorrettamente. 