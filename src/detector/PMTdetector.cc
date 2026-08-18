#include "na62rich/detector/PMTdetector.hh"

#include "na62rich/config/DetectorConfig.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4IntersectionSolid.hh"

#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <vector>

PMTdetector::PMTdetector(PMTParams* pmtParams) : fPMTParams(pmtParams)
{}

G4Material* PMTdetector::CreateMaterial()
{
    auto* nist = G4NistManager::Instance();
    auto* PMTmaterial = nist->FindOrBuildMaterial(fPMTParams->material);

    std::vector<G4double> photonEnergy = {
        1.5 * eV,
        10.0 * eV
    };
    std::vector<G4double> refractiveIndexPMT = {
        1.0,
        1.0
    };

    auto* PMTmpt = 
        new G4MaterialPropertiesTable();

    PMTmpt->AddProperty(
        "RINDEX",
        photonEnergy,
        refractiveIndexPMT
    );

    PMTmaterial->SetMaterialPropertiesTable(PMTmpt);

    return PMTmaterial;
}

G4VSolid* PMTdetector::CreateSolid(std::string name)
{
    constexpr G4int nSides = 6;
    constexpr G4int nZPlanes = 2; 
    
    G4double zPlane[nZPlanes] = {
        -fPMTParams->thickness / 2.0,
        +fPMTParams->thickness / 2.0
    };
    G4double rInner[nZPlanes] = {
        0.0, 0.0
    };
    G4double rOuter[nZPlanes] = {
        fPMTParams->PMTradius, fPMTParams->PMTradius
    };

    auto* hexagonalPrism = new G4Polyhedra(
        name,
        0.0,
        twopi,
        nSides,
        nZPlanes,
        zPlane,
        rInner,
        rOuter
    );

    return hexagonalPrism;
}

std::vector<G4VPhysicalVolume*> PMTdetector::Place(G4LogicalVolume* mother, G4LogicalVolume* logical, std::string name)
{
    std::vector<G4VPhysicalVolume*> physicalList;
    
    const G4double circumRadius =
        fPMTParams->PMTradius / std::cos(pi / 6.0);

    const G4double dx =
        1.5 * circumRadius;

    const G4double dy =
        std::sqrt(3.0) * circumRadius;

    const G4int n =
        static_cast<G4int>(
            std::ceil(
                fPMTParams->diskRadius / fPMTParams->PMTradius
            )
        ) + 2;

    G4int copyNumber = 0;

    for (int i = 0; i < 2; ++i) {

        for (G4int q = -n; q <= n; ++q) {

            for (G4int r = -n; r <= n; ++r) {

                const G4double x = dx * q;

                const G4double y = dy * (r + 0.5 * q);

                if (!IsHexagonInsideCircle(x, y, fPMTParams->PMTradius, fPMTParams->diskRadius)) {
                    continue;
                }

                auto* physical = new G4PVPlacement(
                    nullptr,
                    G4ThreeVector(
                        (i * 2.0 - 1) * fPMTParams->posR + x,
                        y,
                        fPMTParams->posZ
                    ),
                    logical,
                    name,
                    mother,
                    false,
                    copyNumber,
                    true
                );

                ++copyNumber;
                physicalList.push_back(physical);
            }
        }
    }

    return physicalList;
}

bool IsHexagonInsideCircle(
    G4double x,
    G4double y,
    G4double hexRadius,
    G4double circleRadius
)
{
    for (G4int i = 0; i < 6; ++i) {

        const G4double phi = i * pi / 3.0;

        const G4double vertexX =
            x + hexRadius * std::cos(phi);

        const G4double vertexY =
            y + hexRadius * std::sin(phi);

        if (
            vertexX * vertexX +
            vertexY * vertexY
            >
            circleRadius * circleRadius
        ) {
            return false;
        }
    }

    return true;
}