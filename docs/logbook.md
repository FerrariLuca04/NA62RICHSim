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

### Codice

Implementazione: `app/main.cc`

## 08-07 -- First Primary Generator Action

### Obiettivo

Scrivere un primo esempio di generazione di particelle primarie.

### Scelta progettuale

Continuo sulle scelte standard, dichiarando una classe `PrimaryGeneratorAction` derivata da `G4VUserPrimaryGeneratorAction`.
Bisogna implementare obbligatoriamente il metodo `GeneratorPrimaries` con 
```c++
void GeneratorPrimaries(G4Event* event) override;
```
perché è un metodo virtuale puro dennal classe originaria.

Nell'implementazione vado inizialmente a generate come primari dei pioni carichi monocromatici con 20.0MeV/c, generati nell'origine e lungo z.

### Note

Nell'implementazione si utilizzano i vari comandi:

1)
```c++
particleGun_ = new G4ParticleGun(/* numero di particelle */);
```
Definisce la variabile privata della classe definita `PrimaryGeneratorAction` e sceglie il numero di particelle da generare per ogni chiamata.

2)
```c++
particleGun_->SetParticleDefinition(/* tipo di particella */);
```
Definisce la particella che deve essere generata e viene definita tramite una `G4ParticleTable` con il metoddo `particletable->FindParticle("nome particella")`.

3)
```c++
particleGun_->SetParticleMomentum(/* Modulo del momento */);
particleGun_->SetParticleMomentumDirection(/* Direzione */);
particleGun_->SetParticlePosition(/* Punto di generazione */);
```
Con questi si definiscono le caratteristice principali della particella.

Il metodo che poi effettivamente genera la particella è `GeneratePrimaries(G4Event* event)` che utilizza il metodo
```c++
particleGun_->GeneratePrimaryVertex(event);
```


### Codice

Header: `PrimaryGeneratorAction.hh`

Implementazione: `PrimaryGeneratorAction.cc`