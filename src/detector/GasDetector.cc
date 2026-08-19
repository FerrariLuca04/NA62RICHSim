#include "na62rich/detector/GasDetector.hh"

#include "na62rich/io/DetectorConfig.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4Tubs.hh"
#include "G4PVPlacement.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

GasDetector::GasDetector(GasParams* gasParams) : fGasParams(gasParams)
{}

G4Material* GasDetector::CreateMaterial()
{
    auto* nist = G4NistManager::Instance();
    auto* gasMaterial = nist->FindOrBuildMaterial(fGasParams->material);

    auto* gasMPT = new G4MaterialPropertiesTable();

    gasMPT->AddProperty(
        "RINDEX",
        fGasParams->photonEnergies,
        fGasParams->refractiveIndex
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
            fGasParams->radius,
            fGasParams->length / 2.0,
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
