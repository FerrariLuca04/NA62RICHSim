#include "na62rich/detector/DetectorConstruction.hh"

#include "na62rich/detector/PhotonSensitiveDetector.hh"

#include "na62rich/detector/World.hh"
#include "na62rich/detector/GasDetector.hh"
#include "na62rich/detector/MirrorDetector.hh"
#include "na62rich/detector/PMTdetector.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4IntersectionSolid.hh"
#include "G4RotationMatrix.hh"

#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4SDManager.hh"

// World dimensions
constexpr G4double worldHeigth = 10.0 * m;
constexpr G4double worldLength = 30.0 * m;
// Gas dimensions
constexpr G4double gasRadius = 1.5 * m;
constexpr G4double gasLength = 20.0 * m;
// Mirror dimensions
constexpr G4double mirrorCurvatureRadius = 40.0 * m;
constexpr G4double mirrorThickness       = 2.5 * cm;
constexpr G4double mirrorApertureRadius  = 1.5 * m;
constexpr G4double mirrorInnerRadius     = 10.0 * cm;
constexpr G4double mirrorPosZ = 20 * m / 2.0;
// PMTs dimensions
constexpr G4double photonSDRadius  = 0.5 * m;
constexpr G4double PMTradius  = 1.0 * cm;
constexpr G4double photonSDThick   = 1 * mm;
constexpr G4double photonSDPosZ = -20.0 * m / 2.0;
constexpr G4double photonSDPosR = 0.75 * m;

G4VPhysicalVolume* DetectorConstruction::Construct()
{   
    // Build World
    fWorld = new World(
        worldHeigth,
        worldHeigth,
        worldLength
    );
    fWorld->Construct(
        nullptr,
        "World"
    );
    
    // Build Gas
    fGas = new GasDetector(
        gasRadius,
        gasLength
    );
    fGas->Construct(
        fWorld->GetLogical(),
        "Gas"
    );
    
    // Build Mirror
    fMirror = new MirrorDetector(
        mirrorCurvatureRadius,
        mirrorInnerRadius,
        mirrorApertureRadius,
        mirrorThickness,
        mirrorPosZ
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
        photonSDRadius,
        PMTradius,
        photonSDThick,
        photonSDPosZ,
        photonSDPosR
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

    auto* photonSD = new PhotonSensitiveDetector("PMTs");

    sdManager->AddNewDetector(photonSD);

    SetSensitiveDetector(
        fPMT->GetLogical(),
        photonSD
    );
}