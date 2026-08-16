#include "na62rich/detector/CustomDetector.hh"

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

void CustomDetector::Construct(G4LogicalVolume* mother, std::string name)
{
    fName = name;

    fMaterial = CreateMaterial();
    fSolid = CreateSolid(name);

    fLogical = new G4LogicalVolume(
        fSolid,
        fMaterial,
        name
    );

    fPhysical = Place(mother, fLogical, name);
}