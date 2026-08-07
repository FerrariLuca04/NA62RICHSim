# NA62 RICH Simulation - Development Logbook

## 08-07 -- DetectorConstruction

### Obiettivo

Implementare la classe che costruirá la geometria del rivelatore.

### Scelta progettuale

Ho creato una classe `DetectorConstruction` derivata da `G4VUserDetectorConstruction`.

Geat4 richiede che la geometria venga fornita da una classe che implementa línterfaccia definita da `G4VUserDetectorConstruction`.

### Codice

Header: `include/na62rich/DetectorConstruction.hh`

Implementazione: `src/DetectorConstruction.cc`

### Note

Per ora la geometria contiene soli il world volume.

## 08-07 -- First RunManager initialization

### Obiettivo

Inizializzare il kernel Geat4 con il comando 
```c++
runManager->Initialization();
```

### Scelta progettuale

Seguo gli esempi di Geat4 e creo il Run Manager con 

```c++
auto* runManager = G4RunManagerFactory::CreateRunManager();
```
Per poi inizializzare la geometria e la fisica con il comando `setUserInitialization(/* puntatore alla classe */)`.

### Note

Per ora la fisica utilizzata è "FTFP_BERT", in seguito posso aggiungere una lista personalizzata.