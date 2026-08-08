#include "na62rich/DetectorConstruction.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"

#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4MaterialPropertiesTable.hh"

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
    // GAS
    // ---------------------------------------------------

    // Material
    auto* gasMaterial =
        nist->FindOrBuildMaterial("G4_Ne");

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


    // ---------------------------------------------------
    // MIRROR
    // ---------------------------------------------------

    constexpr G4double mirrorCurvatureRadius = 2.0 * gasLength;
    constexpr G4double mirrorThickness       = 2.5 * cm;
    constexpr G4double mirrorApertureRadius  = gasRadius;

    // Angular size of the spherical cap
    const G4double mirrorThetaMax = std::asin(mirrorApertureRadius / mirrorCurvatureRadius);

    // Material
    auto* mirrorMaterial =
        nist->FindOrBuildMaterial("G4_GLASS_PLATE");

    // Optical properties
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
    
    // Solid
    auto* mirrorSolid =
        new G4Sphere(
            "Mirror",
            mirrorCurvatureRadius - mirrorThickness,
            mirrorCurvatureRadius,
            0.0,
            twopi,
            0.0,
            mirrorThetaMax
        );

    // Logical volume
    auto* mirrorLogical =
        new G4LogicalVolume(
            mirrorSolid,
            mirrorMaterial,
            "Mirror"
        );

    // Physical volume

    const G4double mirrorVertexZ = gasLength / 2.0 - 1.0 * cm;
    const G4double mirrorCenterZ = mirrorVertexZ - mirrorCurvatureRadius;

    auto* mirrorPhysical =
        new G4PVPlacement(
            nullptr,
            G4ThreeVector(0.0, 0.0, mirrorCenterZ),
            mirrorLogical,
            "Mirror",
            gasLogical,
            false,
            0,
            true
        );

    new G4LogicalBorderSurface(
        "NeonToMirrorSurface",
        gasPhysical,
        mirrorPhysical,
        mirrorSurface
    );


    return worldPhysical;
}