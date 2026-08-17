#include "na62rich/detector/GasDetector.hh"

#include "na62rich/detector/GasDetector.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4Tubs.hh"
#include "G4PVPlacement.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

GasDetector::GasDetector(G4double radius, G4double length) : fRadius(radius), fLength(length)
{}

G4Material* GasDetector::CreateMaterial()
{
    auto* nist = G4NistManager::Instance();
    auto* gasMaterial = nist->FindOrBuildMaterial("G4_Ne");

    std::vector<G4double> photonEnergy = {
        1.5 * eV,
        10.0 * eV
    };
    std::vector<G4double> refractiveIndexGas = {
        1.000067,
        1.000067
    };

    auto* gasMPT = new G4MaterialPropertiesTable();

    gasMPT->AddProperty(
        "RINDEX",
        photonEnergy,
        refractiveIndexGas
    );

    gasMaterial->SetMaterialPropertiesTable(gasMPT);

    return gasMaterial;
}

G4VSolid* GasDetector::CreateSolid(std::string name)
{
    auto* gasSolid =
        new G4Tubs(
            name,
            0.0,
            fRadius,
            fLength / 2.0,
            0.0,
            twopi
        );

    return gasSolid;
}

std::vector<G4VPhysicalVolume*> GasDetector::Place(G4LogicalVolume* mother, G4LogicalVolume* logical, std::string name)
{
    std::vector<G4VPhysicalVolume*> physicalList;
    auto* physical =
        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),
            logical,
            name,
            mother,
            false,
            0,
            true
        );
    
    physicalList.push_back(physical);

    return physicalList;
}
