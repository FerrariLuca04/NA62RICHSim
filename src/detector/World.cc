#include "na62rich/detector/World.hh"

#include "na62rich/detector/World.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"

#include "G4Box.hh"
#include "G4PVPlacement.hh"

World::World(G4double dimX, G4double dimY, G4double dimZ)
    : fDimX(dimX), fDimY(dimY), fDimZ(dimZ)
{}

G4Material* World::CreateMaterial()
{
    auto* nist = G4NistManager::Instance();
    auto* worldMaterial = nist->FindOrBuildMaterial("G4_Galactic");

    return worldMaterial;
}

G4VSolid* World::CreateSolid(std::string name)
{
    auto* worldSolid =
        new G4Box(
            name,
            fDimX / 2.0,
            fDimY / 2.0,
            fDimZ / 2.0
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