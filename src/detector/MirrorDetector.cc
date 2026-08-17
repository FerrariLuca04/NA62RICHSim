#include "na62rich/detector/MirrorDetector.hh"

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

MirrorDetector::MirrorDetector(G4double curvatureRadius, G4double innerRadius, G4double outerRadius, G4double thickness, G4double posZ)
    : fCurvatureRadius(curvatureRadius), fInnerRadius(innerRadius), fOuterRadius(outerRadius), fThickness(thickness)
{
    fThetaMax = std::asin(3.0 * fOuterRadius / fCurvatureRadius);
    
    fPosZ = posZ - thickness;
}

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
    auto* mirrorMaterial = nist->FindOrBuildMaterial("G4_GLASS_PLATE");

    std::vector<G4double> photonEnergy = {
        1.5 * eV,
        10.0 * eV
    };
    std::vector<G4double> reflectivityMirror = {
        1.0,
        1.0
    };
    
    fSurface = new G4OpticalSurface("MirrorSurface");

    fSurface->SetType(dielectric_metal);
    fSurface->SetModel(unified);
    fSurface->SetFinish(polished);

    auto* mirrorMPT =
        new G4MaterialPropertiesTable();

    mirrorMPT->AddProperty(
        "REFLECTIVITY",
        photonEnergy,
        reflectivityMirror
    );

    fSurface->SetMaterialPropertiesTable(mirrorMPT);

    return mirrorMaterial;
}

G4VSolid* MirrorDetector::CreateSolid(std::string name)
{
    auto* sphericalShell =
        new G4Sphere(
            "Spherical Shell",
            fCurvatureRadius - fThickness,
            fCurvatureRadius,
            0.0,
            twopi,
            0.0,
            fThetaMax
        );
    auto* cutter =
        new G4Tubs(
            "Cutter",
            fInnerRadius,
            fOuterRadius,
            1.0 * m,
            0.0,
            pi
        );
    auto* mirrorSolid = new G4IntersectionSolid(
        name,
        sphericalShell,
        cutter,
        nullptr,
        G4ThreeVector(0.0, fOuterRadius / -2.0, fCurvatureRadius)
    );

    return mirrorSolid;
}

std::vector<G4VPhysicalVolume*> MirrorDetector::Place(G4LogicalVolume* mother, G4LogicalVolume* logical, std::string name)
{
    std::vector<G4VPhysicalVolume*> physicalList;
    const G4double mirrorCenterZ = fPosZ - fCurvatureRadius;
    
    for (int i = 0; i < 2; ++i) {
        auto* rotation = new G4RotationMatrix();
        rotation->rotateZ((3.0 * pi/2.0) - i * pi);

        auto* physical =
            new G4PVPlacement(
                rotation,
                G4ThreeVector(((i * 2) - 1) * fOuterRadius / 2.0, 0.0, mirrorCenterZ),
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