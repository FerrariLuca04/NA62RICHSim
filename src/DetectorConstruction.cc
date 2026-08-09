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
const G4double mirrorThetaMax = std::asin(mirrorApertureRadius / mirrorCurvatureRadius); // Angular size of the spherical cap

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Defining materials
    DefineMaterials();

    // Build geometry
    auto* worldLogical = BuildWorld();
    auto* gasLogical = BuildGas(worldLogical);

    BuildMirror(gasLogical);

    return fWorldPhysical;
}



void DetectorConstruction::DefineMaterials()
{
    auto* nist = G4NistManager::Instance();

    // World
    fWorldMaterial =
        nist->FindOrBuildMaterial("G4_Galactic");
    // Gas
    fGasMaterial =
        nist->FindOrBuildMaterial("G4_Ne");
    //Mirror
    fMirrorMaterial =
        nist->FindOrBuildMaterial("G4_GLASS_PLATE");
}

G4LogicalVolume* DetectorConstruction::BuildWorld()
{
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
            fWorldMaterial,
            "World"
        );

    // Physical volume
    fWorldPhysical =
        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),
            worldLogical,
            "World",
            nullptr,
            false,
            0
        );
    
    return worldLogical;
}

G4LogicalVolume* DetectorConstruction::BuildGas(G4LogicalVolume* mother)
{    
    // Optical properties
    std::vector<G4double> photonEnergy = {
        1.9 * eV,
        6.2 * eV
    };
    std::vector<G4double> refractiveIndex = {
        1.000067,
        1.000067
    };

    auto* gasProperties =
        new G4MaterialPropertiesTable();

    gasProperties->AddProperty(
        "RINDEX",
        photonEnergy,
        refractiveIndex
    );

    fGasMaterial->SetMaterialPropertiesTable(gasProperties);

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
            fGasMaterial,
            "Gas"
        );

    // Physical Volume
    fGasPhysical =
        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),
            gasLogical,
            "Gas",
            mother,
            false,
            0,
            true
        );
    
    return gasLogical;
}

void DetectorConstruction::BuildMirror(G4LogicalVolume* mother)
{
    // Optical properties
    std::vector<G4double> photonEnergy_Mirror = {
        1.5 * eV,
        7.0 * eV
    };
    std::vector<G4double> reflectivity = {
        1.0,
        1.0
    };
    
    auto* mirrorSurface =
        new G4OpticalSurface("MirrorSurface");

    mirrorSurface->SetType(dielectric_metal);
    mirrorSurface->SetModel(unified);
    mirrorSurface->SetFinish(polished);

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
            fMirrorMaterial,
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
            mother,
            false,
            0,
            true
        );

    new G4LogicalBorderSurface(
        "NeonToMirrorSurface",
        fGasPhysical,
        mirrorPhysical,
        mirrorSurface
    );
}