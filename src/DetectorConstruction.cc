#include "na62rich/DetectorConstruction.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


G4VPhysicalVolume* DetectorConstruction::Construct()
{
    auto* nist = G4NistManager::Instance();

    // ---------------------------------------------------
    // WORLD
    // ---------------------------------------------------

    // Material
    auto* worldMaterial =
        nist->FindOrBuildMaterial("G4_Galactic");

    // World dimensions
    constexpr G4double worldHeigth = 10.0 * m;
    constexpr G4double worldLength = 30.0 * m;

    // Solid
    auto* worldSolid =
        new G4Box(
            "World",
            worldHeigth / 2.0,
            worldHeigth / 2.0,
            worldLength / 2.0
        );

    // Logical volume
    auto* worldLogical =
        new G4LogicalVolume(
            worldSolid,
            worldMaterial,
            "World"
        );

    // Physical volume
    auto* worldPhysical =
        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),
            worldLogical,
            "World",
            nullptr,
            false,
            0
        );


    
    
    // ---------------------------------------------------
    // WORLD
    // ---------------------------------------------------

    // Material
    auto* gasMaterial =
        nist->FindOrBuildMaterial("G4_Ne");

    // Gas dimensions
    constexpr G4double gasRadius = 1.5 * m;
    constexpr G4double gasLength = 20.0 * m;

    // Solid
    auto* gasSolid =
        new G4Tubs(
            "Gas",
            0.0,
            gasRadius,
            gasLength / 2.0,
            0.0,
            twopi
        );

    // Logical Volume
    auto* gasLogical =
        new G4LogicalVolume(
            gasSolid,
            gasMaterial,
            "Gas"
        );

    // Physical Volume
    auto* gasPhysical =
        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),
            gasLogical,
            "Gas",
            worldLogical,
            false,
            0,
            true
        );


    return worldPhysical;
}