# Classificazione di Macchine Parallele

## Tassonomia di Flynn

La tassonomia di Flynn è una tassonomia _(studio della teoria e delle regole di classificazione)_ che classifica le macchine in base al numero di flussi di istruzioni e di dati che possono essere eseguiti in parallelo. La tassonomia è la seguente:

- **SISD**: Single Instruction Single Data. Una macchina SISD è una macchina sequenziale.
- **SIMD**: Single Instruction Multiple Data. Una macchina SIMD è una macchina che esegue la stessa istruzione su più dati in parallelo.
- **MISD**: Multiple Instruction Single Data. Una macchina MISD è una macchina che esegue più istruzioni su uno stesso dato in parallelo.
- **MIMD**: Multiple Instruction Multiple Data. Una macchina MIMD è una macchina che esegue più istruzioni su più dati in parallelo.

### SISD

Una macchina SISD è una macchina sequenziale. Questo significa che la macchina esegue una singola istruzione su un singolo dato alla volta. Questo tipo di macchine non sono in grado di eseguire più istruzioni su più dati in parallelo.

### SIMD

Una macchina SIMD è una macchina che esegue la stessa istruzione su più dati in parallelo. Questo tipo di macchine sono in grado di eseguire più istruzioni su più dati in parallelo, ma le istruzioni devono essere le stesse.

### MISD

Una macchina MISD è una macchina che esegue più istruzioni su uno stesso dato in parallelo. Questo tipo di macchine sono in grado di eseguire più istruzioni su più dati in parallelo, ma i dati devono essere gli stessi.

### MIMD

Una macchina MIMD è una macchina che esegue più istruzioni su più dati in parallelo. Questo tipo di macchine sono in grado di eseguire più istruzioni su più dati in parallelo e le istruzioni e i dati possono essere diversi.

## Esempi di macchine

### SISD

<center>

|             |       |
| ----------- | ----- |
| Instruction | +     |
| Data        | {A,B} |
| Result      | A+B   |

</center>

Una singola istruzione viene eseguita su un singolo dato alla volta.

### SIMD

<center>

|             |       |       |
| ----------- | ----- | ----- |
| Instruction | +     |
| Data        | {A,B} | {C,D} |
| Result      | A+B   | C+D   |

Una singola istruzione viene eseguita su più dati in parallelo.

</center>

### MISD

<center>

|             |       |     |
| ----------- | ----- | --- |
| Instruction | +     | -   |
| Data        | {A,B} |     |
| Result      | A+B   | A-B |

</center>

Più istruzioni vengono eseguite su uno stesso dato in parallelo.

### MIMD

<center>

|             |       |       |
| ----------- | ----- | ----- |
| Instruction | +     | -     |
| Data        | {A,B} | {C,D} |
| Result      | A+B   | C-D   |

</center>

Più istruzioni vengono eseguite su più dati in parallelo.

## MIMD

Le macchine MIMD sono in grado di eseguire più istruzioni su più dati in parallelo. Questo significa che le macchine MIMD sono in grado di eseguire più programmi in parallelo. Le macchine MIMD sono divise in due categorie:

- **SPMD**: Single Program Multiple Data. Le macchine SPMD eseguono lo stesso programma su più dati in parallelo.
  - I programmi SPMD solitamente vengono programmati attraverso la tecnica del **branching**, cioè i processi eseguono solo la parte di programma per il quale sono stati designati.
- **MPMD**: Multiple Program Multiple Data. Le macchine MPMD eseguono più programmi su più dati in parallelo. Ogni processore esegue un programma diverso su dati diversi.

## Memoria

Le macchine parallele possono essere divise in due categorie:

- **Memoria condivisa**: i processori condividono la memoria.
- **Memoria distribuita**: i processori hanno la propria memoria locale.

### Memoria condivisa

Condividere la memoria significa che tutti i processori possono accedere alla stessa memoria. Questo significa che i processori possono comunicare tra di loro attraverso la memoria. La memoria condivisa può essere divisa in categorie:

- **UMA**: Uniform Memory Access. Tutti i processori accedono alla memoria con la stessa velocità.
- **NUMA**: Non Uniform Memory Access. I processori accedono alla memoria con velocità diverse.
  - Dipendentemente dalla posizione del processore, la velocità di accesso alla memoria può variare.
- e molti altri ancora...

Condividere la memoria è molto comodo, ma può portare a problemi di sincronizzazione. Questo perché i processori possono accedere alla memoria in qualsiasi momento. Se due processori accedono alla memoria nello stesso momento, possono accedere alla stessa locazione di memoria. Questo può portare a problemi di sincronizzazione. Per evitare problemi di sincronizzazione, i processori devono essere sincronizzati (duh).

### Memoria distribuita

La memoria distribuita è una memoria locale, ovvero, ogni processore ha la propria memoria. I processori possono comunicare tra di loro attraverso l'invio di messaggi. Questo significa che i processori non possono accedere alla memoria degli altri processori, ma grazie all'invio di messaggi possono comunicare tra di loro e condividere i dati.

### Paradigmi di programmazione

#### OpenMP

OpenMP è un'API per la programmazione parallela su architetture con memoria condivisa. OpenMP è un'API per il linguaggio C, C++ e Fortran. OpenMP è una libreria che permette di scrivere programmi paralleli attraverso l'uso di direttive. Le direttive sono delle istruzioni che vengono interpretate dal compilatore.

#### MPI

MPI è un'API per la programmazione parallela su architetture con memoria distribuita. MPI è un'API per il linguaggio C, C++ e Fortran. MPI è una libreria che permette di scrivere programmi paralleli attraverso l'uso di funzioni che permettono di inviare e ricevere messaggi.

## Conclusioni

- Le macchine parallele possono essere classificate in base al numero di flussi di istruzioni e di dati che possono essere eseguiti in parallelo.
- Le macchine parallele possono essere classificate in base all'utilizzo della memoria.
- Per programmare le macchine parallele bisogna utilizzare il paradigma di programmazione adatto.
