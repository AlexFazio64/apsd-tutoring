# Algoritmi Paralleli e Sistemi Distribuiti

## Introduzione

### Cos'è un algoritmo?

Un algoritmo è una sequenza fin<ita di istruzioni che, partendo da un insieme di dati iniziali, permette di ottenere un risultato finale in un numero finito di passi.

### Cos'è un algoritmo parallelo?

Un algoritmo parallelo è un algoritmo che può essere eseguito su più processori in parallelo. L'obiettivo è quello di ottenere un risultato più velocemente rispetto ad un algoritmo sequenziale.

### Cos'è un sistema distribuito?

Un sistema distribuito è un insieme di componenti hardware e software che comunicano attraverso una rete di calcolatori. L'obiettivo è quello di ottenere un risultato più velocemente rispetto ad un sistema centralizzato.

## Limiti fisici

### Incremento delle prestazioni

Per incrementare le prestazioni di un sistema possiamo agire su due fattori:

- **Tempo di esecuzione**: tempo necessario per eseguire un programma.
- **Potenza di calcolo**: potenza di calcolo di un processore.

Facile. Per incrementare le prestazioni di un sistema possiamo ridurre il tempo di esecuzione di un programma e/o incrementare la potenza di calcolo di un processore. Questo corso non serve a nulla, possiamo andare a casa.

### Legge di Moore

La legge di Moore è una legge empirica che descrive l'evoluzione della potenza di calcolo dei processori. La legge è la seguente:

$$ P(n) = P_0 \cdot 2^n $$

Dove:

- $P(n)$ è la potenza di calcolo di un processore dopo $n$ anni.
- $P_0$ è la potenza di calcolo di un processore oggi.
- $n$ è il numero di anni.
- $2^n$ è il fattore di crescita.

La legge di Moore ci dice che la potenza di calcolo dei processori raddoppia ogni due anni. Questo significa che la potenza di calcolo dei processori aumenta esponenzialmente.

La legge di Moore non è più valida (alcuni affermano che lo sia ancora, alcuni no, non è importante...). Questo perché i processori sono diventati troppo piccoli e per far si che la legge di Moore sia ancora valida, i componenti dei processori dovrebbero essere più piccoli di un atomo (non fattibile).

In sostanza, la legge di Moore ci dice che la potenza di calcolo dei processori aumenta esponenzialmente, ma non può aumentare all'infinito.

### Speedup

Il parallelismo è la capacità di eseguire più operazioni in parallelo. Generalmente è possibile separare le operazioni in due categorie:

- **Task paralleli**: operazioni che possono essere eseguite in parallelo.
- **Task sequenziali**: operazioni che devono essere eseguite in sequenza.

I programmi sequenziali possono essere divisi in task sequenziali e task paralleli, almeno in parte. Non è sempre possibile parallelizzare un programma, ma quando è possibile possiamo ottenere un miglioramento delle prestazioni. Il miglioramento delle prestazioni è dato dal rapporto tra il tempo di esecuzione sequenziale e il tempo di esecuzione parallelo.

$$S = \frac{T_{seq}}{T_{par}} $$

### Legge di Amdahl

La legge di Amdahl è una legge empirica che descrive il miglioramento delle prestazioni che può essere ottenuto con l'uso di più processori. La legge è la seguente:

$$ S(n) = \frac{1}{(1 - p) + \frac{p}{n}} $$

Dove:

- $S(n)$ è il miglioramento delle prestazioni, anche detto **Speedup**.
- $n$ è il numero di processori.
- $p$ è la frazione di codice che può essere parallelizzato.
- $1 - p$ è la frazione di codice che non può essere parallelizzato.
- $\frac{p}{n}$ è la frazione di codice che può essere parallelizzato e che viene eseguita in parallelo.

Quando $n$ tende a infinito, $S(n)$ tende a $\frac{1}{1 - p}$. Questo significa che il miglioramento delle prestazioni è limitato dalla frazione di codice che non può essere parallelizzato. Ad esempio, se il 90% del codice può essere parallelizzato, il miglioramento delle prestazioni è limitato a 10 volte. Questo perché il miglioramento delle prestazioni è limitato a $\frac{1}{1 - 0.9} = 10$.

In sostanza, la legge di Amdahl ci dice che il miglioramento delle prestazioni è limitato dalla frazione di codice che non può essere parallelizzato.

### Riepilogo

- Il parallelismo è la capacità di eseguire più operazioni in parallelo.
- I programmi sequenziali possono essere divisi in task sequenziali e task paralleli, almeno in parte.
- Per incrementare le prestazioni di un sistema possiamo ridurre il tempo di esecuzione di un programma e/o incrementare la potenza di calcolo di un processore.
- Il miglioramento delle prestazioni è dato dal rapporto tra il tempo di esecuzione sequenziale e il tempo di esecuzione parallelo.
- La legge di Moore ci dice che la potenza di calcolo dei processori aumenta esponenzialmente, ma non può aumentare all'infinito.
- La legge di Amdahl ci dice che il miglioramento delle prestazioni è limitato dalla frazione di codice che non può essere parallelizzato.
