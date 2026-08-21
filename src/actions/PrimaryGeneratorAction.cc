#include "na62rich/actions/PrimaryGeneratorAction.hh"

#include "na62rich/io/DetectorConfig.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "G4PhysicalConstants.hh"


#include "Randomize.hh"

constexpr G4double decayRegionStart = -250 * m;
constexpr G4double decayRegionLength = 65 * m;

constexpr G4double decayCaracteristicLength = 3.711 * m;
constexpr G4double meanEnergyKaon = 75 * GeV;
constexpr G4double massKaon = 493.677 * MeV;

constexpr G4double rMin = 10 * cm;
constexpr G4double rMax = 1.5 * m;

constexpr G4double phiMin = 0.0;
constexpr G4double phiMax = twopi;

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    particleGun_ = new G4ParticleGun(1);

    auto* particleTable = G4ParticleTable::GetParticleTable();
    auto* pion = particleTable->FindParticle("pi+");

    particleGun_->SetParticleDefinition(pion);
    particleGun_->SetParticleMomentum(20.0 * GeV);
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particleGun_;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    const G4double uR = G4UniformRand();
    const G4double r =
        std::sqrt(
            rMin * rMin
            + uR * (
                rMax * rMax
                - rMin * rMin
            )
        );
    
    const G4double uP = G4UniformRand();
    const G4double phi = 
        phiMin
        + uP * (
            phiMax - phiMin
        );
    
    const G4ThreeVector entrancePoint(
        r * std::cos(phi),
        r * std::sin(phi),
        (-1 * gasParams->length / 2.0) - 5 * mm
    );

    particleGun_->SetParticlePosition(
        entrancePoint
    );

    const G4double uL = G4UniformRand();
    const G4double zVirtualVertex = decayRegionStart + (uL * decayRegionLength);
    const G4ThreeVector virtualVertex(
        0.0,
        0.0,
        zVirtualVertex
    );

    particleGun_->SetParticleMomentumDirection(
        entrancePoint - virtualVertex
    );

    particleGun_->GeneratePrimaryVertex(event);
}