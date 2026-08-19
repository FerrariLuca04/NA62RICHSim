#include "na62rich/detector/MirrorDetector.hh"

#include "na62rich/io/DetectorConfig.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4IntersectionSolid.hh"

#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <vector>

MirrorDetector::MirrorDetector(MirrorParams* mirrorParams) : fMirrorParams(mirrorParams)
{}

void MirrorDetector::SetSurface(G4VPhysicalVolume* physical1, G4VPhysicalVolume* physical2)
{
    if (physical1 != nullptr & physical2 != nullptr) {
        new G4LogicalBorderSurface(
            "NeonToMirrorSurface",
            physical1,
            physical2,
            fSurface
        );
    }
}

G4Material* MirrorDetector::CreateMaterial()
{
    auto* nist = G4NistManager::Instance();
    auto* mirrorMaterial = nist->FindOrBuildMaterial(fMirrorParams->material);
    
    fSurface = new G4OpticalSurface("MirrorSurface");

    fSurface->SetType(dielectric_metal);
    fSurface->SetModel(unified);
    fSurface->SetFinish(polished);

    auto* mirrorMPT =
        new G4MaterialPropertiesTable();

    mirrorMPT->AddProperty(
        "REFLECTIVITY",
        fMirrorParams->photonEnergies,
        fMirrorParams->reflectivity
    );

    fSurface->SetMaterialPropertiesTable(mirrorMPT);

    return mirrorMaterial;
}

G4VSolid* MirrorDetector::CreateSolid(std::string name)
{
    G4double thetaMax = std::asin(3.0 * fMirrorParams->outerRadius / fMirrorParams->curvatureRadius);

    auto* sphericalShell =
        new G4Sphere(
            "Spherical Shell",
            fMirrorParams->curvatureRadius - fMirrorParams->thickness,
            fMirrorParams->curvatureRadius,
            0.0,
            twopi,
            0.0,
            thetaMax
        );
    auto* cutter =
        new G4Tubs(
            "Cutter",
            fMirrorParams->innerRadius,
            fMirrorParams->outerRadius,
            1.0 * m,
            0.0,
            pi
        );
    auto* mirrorSolid = new G4IntersectionSolid(
        name,
        sphericalShell,
        cutter,
        nullptr,
        G4ThreeVector(0.0, fMirrorParams->outerRadius / -2.0, fMirrorParams->curvatureRadius)
    );

    return mirrorSolid;
}

std::vector<G4VPhysicalVolume*> MirrorDetector::Place(G4LogicalVolume* mother, G4LogicalVolume* logical, std::string name)
{
    std::vector<G4VPhysicalVolume*> physicalList;
    const G4double mirrorCenterZ = fMirrorParams->posZ - fMirrorParams->curvatureRadius;
    
    for (int i = 0; i < 2; ++i) {
        auto* rotation = new G4RotationMatrix();
        rotation->rotateZ((3.0 * pi/2.0) - i * pi);

        auto* physical =
            new G4PVPlacement(
                rotation,
                G4ThreeVector(((i * 2) - 1) * fMirrorParams->outerRadius / 2.0, 0.0, mirrorCenterZ),
                logical,
                name,
                mother,
                false,
                i,
                true
            );
        physicalList.push_back(physical);
    }

    return physicalList;
}