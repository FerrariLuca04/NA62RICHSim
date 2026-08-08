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