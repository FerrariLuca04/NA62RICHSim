#include "na62rich/DetectorConstruction.hh"

#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Material
    auto* nist = G4NistManager::Instance();

    auto* worldMaterial =
        nist->FindOrBuildMaterial("G4_Galactic");

    // World dimensions
    constexpr G4double worldRadius = 5.0 * m;
    constexpr G4double worldLength = 25.0 * m;

    // Solid
    auto* worldSolid =
        new G4Tubs(
            "World",
            0.0,
            worldRadius,
            worldLength / 2.0,
            0.0,
            twopi
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

    return worldPhysical;
}