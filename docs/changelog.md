# NA62 RICH Sim - changelog

### v.0.1.0 [08/08/26]: 
Prima struttura del progetto.

- primo generatore di primari (pioni monocromatici);
- primo volume di gas neon con proprietá ottiche per la produzione Cherenkov.

### v.0.2.0 [08/08/26]
Prima implementazione dello specchio sferico.

### v.0.2.1 [09/08/26]:
Alcuni bugfixes e riscrittura del codice per maggiore leggibilitá.

### v.0.3.0 [10/08/26]:
Prima implementazione di un volume sensibile per la rivelazione dei fotoni.

### v.0.3.1 [11/08/26]:
Implementazione della classe `PhotonHit` e `EventAction` per la raccolta dei dati utilidal volume sensibile.

### v.0.3.2 [11/08/26]:
Prima versione di scrittura per file output in un file `.root`.

- I dati raccolti in `PhotonHit` vengono riscritti in un `TTree` chiamato `PhotonHits` nel file di output;
- Per ogni thread utilizzato si genera un file diverso pe rperettere il multi-thread nell'esecuzione

### v.0.3.3 [16/08/26]:
I file di output vengono mergiati in un unico file `.root` che contiene un unico `TTree`.