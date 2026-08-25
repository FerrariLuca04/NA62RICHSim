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

### v.0.4.0 [17/08/26]:
- Nuova geometria piú realistica con due dischi con un mosaico di PMT a 'honeycomb' e due specchi con due assi di focalizzazione diversi,  a paralleli;
- Nuova classe `CustomDetector` da cui derivano le altre classi per la costruzioni delle singole parti del detector;
- Aggiunto `sensor_pos_x(y)_mm` nel file di output per imagazzinare la posizione dei PMT colpiti.

### v.0.4.1 [18/08/26]:
- Nuove `struct` per definire un oggetto unico con tutti i parametri fondamentali di un'oggetto in `DetectorCostruction` per inizializzare i detector;
- Possibilitá di modificare la geometria, materiali e caratteristiche ottiche dei detector tramire `config/detector_default.conf` senza dover ricompilare il codice.
- Aggiunta l'efficienza quantica dei PMT.

### v.0.4.2 [25/08/26]:
Modifiche sulla generazione delle particelle primarie con aggiunta del file `config/generator_default.conf`.
- Nuove struct per definire un oggetto unico con tutti i parametri per la generazione dei primari;
- Randomizzazione del punto d'ingresso della particella primaria correlata anche alla direzione, si utilizza l'estrazione randomica anche di un vertice primario;
- Randomizzazione del momento della particella primaria usando una distribuzione uniforme;
- Scelta della particella tramire file.conf;
- Nuovo file di configurazione per i parametri della particella primaria.
