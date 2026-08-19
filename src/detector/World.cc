#include "na62rich/detector/World.hh"

#include "na62rich/io/DetectorConfig.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"

#include "G4Box.hh"
#include "G4PVPlacement.hh"

World::World(WorldParams* worldParams)
    : fWorldParams(worldParams)
{}

G4Material* World::CreateMaterial()
{
    auto* nist = G4NistManager::Instance();
    auto* worldMaterial = nist->FindOrBuildMaterial(fWorldParams->material);

    return worldMaterial;
}

G4VSolid* World::CreateSolid(std::string name)
{
    auto* worldSolid =
        new G4Box(
            name,
            fWorldParams->height / 2.0,
            fWorldParams->height / 2.0,
            fWorldParams->length / 2.0
        );

    return worldSolid;
}

std::vector<G4VPhysicalVolume*> World::Place(G4LogicalVolume* mother, G4LogicalVolume* logical, std::string name)
{
    std::vector<G4VPhysicalVolume*> physicalList;
    auto* physical =
        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),
            logical,
            name,
            nullptr,
            false,
            0
        );
    
    physicalList.push_back(physical);

    return physicalList;
}