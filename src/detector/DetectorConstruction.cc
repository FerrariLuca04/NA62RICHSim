#include "na62rich/detector/DetectorConstruction.hh"

#include "na62rich/detector/PhotonSensitiveDetector.hh"
#include "na62rich/detector/World.hh"
#include "na62rich/detector/GasDetector.hh"
#include "na62rich/detector/MirrorDetector.hh"
#include "na62rich/detector/PMTdetector.hh"
#include "na62rich/io/DetectorConfig.hh"

#include "G4VPhysicalVolume.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4SDManager.hh"

G4VPhysicalVolume* DetectorConstruction::Construct()
{   
    // Build World
    fWorld = new World(
        worldParams
    );
    fWorld->Construct(
        nullptr,
        "World"
    );
    
    // Build Gas
    fGas = new GasDetector(
        gasParams
    );
    fGas->Construct(
        fWorld->GetLogical(),
        "Gas"
    );
    
    // Build Mirror
    fMirror = new MirrorDetector(
        mirrorParams
    );
    fMirror->Construct(
        fGas->GetLogical(),
        "Mirror"
    );
    for (int i = 0; i < 2; ++i) {
        fMirror->SetSurface(
            fGas->GetPhysical()[0],
            fMirror->GetPhysical()[i]
        );
    }

    // Build PMT
    fPMT = new PMTdetector(
        pmtParams
    );
    fPMT->Construct(
        fGas->GetLogical(),
        "PMT"
    );

    std::vector<G4VPhysicalVolume*> physicListWorld = fWorld->GetPhysical();

    return physicListWorld[0];
}
void DetectorConstruction::ConstructSDandField()
{
    auto* sdManager = G4SDManager::GetSDMpointer();

    auto* photonSD = new PhotonSensitiveDetector("PMTs", pmtParams);

    sdManager->AddNewDetector(photonSD);

    SetSensitiveDetector(
        fPMT->GetLogical(),
        photonSD
    );
}