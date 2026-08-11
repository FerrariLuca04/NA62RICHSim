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

Header: `include/na62rich/PrimaryGeneratorAction.hh`

Implementazione: `src/PrimaryGeneratorAction.cc`

## 08-07 -- First Action Initialization

### Obiettivo

Implementare un primo esempio di `ActionInitialization.hh` e `ActionInitialization.cc`.

### Scelta progettuale

Si definisce una classe `ActionInitialization` derivata da `G4VUserActionInitialization` dove dobbiamo definire anche il metodo `Build()`.
```c++
void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction());
}
```
### Codice

Header: `include/na62rich/ActionInitialization.hh`

Implementazione: `src/ActionInitialization.cc`

## 08-07 -- First macro for visualization

### Obiettivo

Implementare una prima macro di inizializzazione `init_vis.mac` e `vis.mac` per aprire la UI di Geat4 con una visualizzazione grafica dei volumi simulati da cui poter lanciare delle simulazioni brevi con i comandi di Geat4.

### Scelta progettuale

Si modifica il `main.cc` per inizializzare un `visManager`.

Si va a definire
```c++
G4UIExecutive* ui = nullptr;

if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
}
```
e anche
```c++
auto* uiManager = G4UImanager::GetUIpointer();

if (ui != nullptr) {

    uiManager->ApplyCommand(
        "/control/execute macros/init_vis.mac"
    );

    ui->SessionStart();

    delete ui;

} else {

    G4String command = "/control/execute ";
    G4String macroFile = argv[1];

    uiManager->ApplyCommand(
        command + macroFile
    );
}
```
in questo modo se si esegue solo il file eseguibile senza specificare la macro di Geat4 si apre la UI con una console a parte e la visualizzazione grafica, altrimenti viene eseguita la macro specificata.

### Codice

Implementazione: `app/main.cc` `macros/init_vis.mac` `macros/vis.mac`

## 08-08 -- Gas volume construction

### Obiettivo

Costruire il volume, con dimensioni provvisorie, del gas neon per fare una prima prova della luce Cherenkov.

### Scelta progettuale

Costruisco il volume come il World ma aggiungo le proprietá ottiche per permettere l'effetto Cherenkov.
```c++
// Optical properties
auto* gasProperties =
    new G4MaterialPropertiesTable();

// Photon energy range
std::vector<G4double> photonEnergy = {
    1.9 * eV,
    6.2 * eV
};

// Refractive index
std::vector<G4double> refractiveIndex = {
    1.000067,
    1.000067
};

gasProperties->AddProperty(
    "RINDEX",
    photonEnergy,
    refractiveIndex
);

gasMaterial->SetMaterialPropertiesTable(gasProperties);
```
Per aggiungere `"RINDEX"` si deve definire una sorta di tabella con energie (`photonEnergy`) e il valore dell'indice di rifrazione (`refractiveIndex`), poi Geat4 interpola tra i punti della tabella.

### Nota

Provvisoriamente ho inserito un indice costante con l'energia per i fotoni nell'UV e nel visibile.

### Codice

Implementazione: `src/DetectorConstruction.cc`

## 08-08 -- Mirror implementation

### Obiettivo

Costruire uno specchio sferico provvisorio per poter iniziare ad osservare degli anelli Cherenkov

### Scelta progettuale

Come materiale si è utilizzato `G4_GLASS_PLATE` e si sono settate le proprietá riflettenti con il seguente blocco
```c++
auto* mirrorSurface =
    new G4OpticalSurface("MirrorSurface");

mirrorSurface->SetType(dielectric_metal);
mirrorSurface->SetModel(unified);
mirrorSurface->SetFinish(polished);

std::vector<G4double> photonEnergy_Mirror = {
    1.5 * eV,
    7.0 * eV
};

std::vector<G4double> reflectivity = {
    1.0,
    1.0
};

auto* mirrorMPT =
    new G4MaterialPropertiesTable();

mirrorMPT->AddProperty(
    "REFLECTIVITY",
    photonEnergy_Mirror,
    reflectivity
);

mirrorSurface->SetMaterialPropertiesTable(mirrorMPT);
```
ovvero definisce una superficie ottica, definisce la tabella della riflettivitá e associa questa tabella alla superficie ottica. Inoltre si definisce l'interfaccia come dal Neon allo specchio con
```c++
new G4LogicalBorderSurface(
    "NeonToMirrorSurface",
    gasPhysical,
    mirrorPhysical,
    mirrorSurface
);
```
che rende riflettente solo línterfaccia tra i due volumi.

### Note

Per ora la riflettivitá è 1 per tutte le energie, in seguito si potrá aggiungere dei valori realistici.

### Codice

Implementazione: `src/DetectorConstruction.cc`

## 08-07 -- Re-organize DetectorConstruction

### Obiettivo

Riscrivere la forma della classe `DetectorConstructor` usando anche metodi e altri field privati ausiliari per ottenre una forma piú leggibile e facilmente riscalabile di `Construct()`.

### Scelta progettuale

Sono state implementati altri metodi privati che costruiscono ogni parte dell'intero apparato da simulare, in modo che l'implementazione della funzione `Construct()` risultasse piú leggibile, minimale ed ordinata.

Ogni funzione implementa blocchi di codice giá presenti, semplicemente sono riorganizzati in diversi blocchi e con diverse variabili.

### Note

L'idea é anche di rendere piú facile il riscalamento per aggiungere e/o modificare diverse parti della geometria.

### Codice

Header: `DetectorConstruction.hh`

Implementazione: `DetectorConstruction.cc`

## 10-08 -- Photon Sensitive Detector

### Obiettivo

Implementare un volume sensibile per la ricezione di fotoni ottici.

### Scelta progettuale

Il volume é stato implementato come tutti gli altri. Si é scelto come materiale `"G4_SILICON_DIOXIDE"` con provvisoriamente un indice di rifrazione pari a 1 (per far propagare i fotoni anche all'interno del volume sensibile).

Si definisce anche la classe `PhotonSensitiveDetector` derivata da `G4VSensitiveDetector`. Ha un metodo `ProcessHits` che programma cosa fare quando avviene attavrsata da una traccia (una particella), questo metodo deve avere `return true` per registrare la hit, oppure `return false` viceversa.

Si selezionano solo i fotoni ottici con
```c++
if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
{
    return false;
} 
```

Si selezionano solo il primo step della traccia che entra nel volume sensibile con
```c++
if (step->GetPreStepPoint()->GetStepStatus() != fGeomBoundary)
{
    return false;
}
```
E infine si killano i fotoni che sono entrati nel volume
```c++
track->SetTrackStatus(fStopAndKill);
```

Per associare il detector al volume geometrico si deve definire un altro metodo pubblico a `DetectorConstruction` nominata `ConstructSDandField()`, e deve contenere
```c++
SetSensitiveDetector(
    fPhotonSDLogical, //Volume logico del detector
    photonSD          //Volume sensibile associato
);
```
### Note

**WARNING**: In questo modo prima di chiamare `ConstructSDandField()` si deve rempire il puntatore `fPhotonSDLogical`, che viene riempito da `Construct()`. Geat4 esegue automaticamente prima il `Construct()` poi il resto.

### Codice

Header: `DetectorConstruction.hh` `PhotonSensitiveDetector.hh`

Implementazione: `DetectorConstruction.cc` `PhotonSensitiveDetector.cc`

## 11-08 -- Defining PhotonHit

### Obiettivo

Implementare una classe per imaggazzinare le informazioni delle hit sul volume sensibile.

### Scelta progettuale

Si è definita la classe `PhotonHit` derivata da `G4VHit` con tre campi: `fPosition`, `fEnergy` e `fSensorID`, ovvero la posizione e l'energia del fotone quando entra nel volume sensibile, e l'ID del sensore.

### Note

Ad ora l'ID puó essere solo 0 in quanto c'è solo un volume sensibile.

### Codice

Implementazione: `PhotonHit.hh` `PhotonSensitiveDetector.cc`

## 11-08 -- Implementing EventAction

### Obiettivo

Implementare la classe `EventAction` e definire il metodo `EndOfEventAction`, necessario per registrare una "lista" di hit da poi salvare in un file di output.

### Scelta progettuale

Seguendo vari esempi, si dichiara una classe `EventAction` derivata da `G4UserEventAction`. Il Metodo piú importante è `EndOfEventAction` che viene eseguito alla fine di ogni evento.

Il metodo `EndOfEventAction` va a registrare l'ID della "hit collection" registrata dal "sensitive detector" (che viene generata e riempita con `ProcessHit`)
```c++
if (fPhotonHitsCollectionID < 0)
{
    fPhotonHitsCollectionID = 
        G4SDManager::GetSDMpointer()->GetCollectionID("PhotonHitsCollection");
}
```
Poi registra la "hit collection" in un `static_cast<PhotonHitsCollection*>`
```c++
auto* hitsCollection = 
    static_cast<PhotonHitsCollection*>(
        hce->GetHC(fPhotonHitsCollectionID)
    );
```
### Note

L'oggetto `PhotonHitsCollection` vine definito in `PhotonHit.hh`con la riga
```c++
using PhotonHitsCollection = G4THitsCollection<PhotonHit>;
```

### Codice

Header: `EventAction.hh`

Implemmentazione: `EventAction.cc` `PhotonHit.hh`