#include "na62rich/actions/PrimaryGeneratorAction.hh"

#include "na62rich/io/DetectorConfig.hh"
#include "na62rich/io/PrimaryGeneratorConfig.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "G4PhysicalConstants.hh"


#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    particleGun_ = new G4ParticleGun(1);

    auto* particleTable = G4ParticleTable::GetParticleTable();
    auto* typeParticle = particleTable->FindParticle(particleParams->type);
    particleGun_->SetParticleDefinition(typeParticle);
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particleGun_;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{   
    // Entrance point
    const G4double uR = G4UniformRand();
    const G4double r =
        std::sqrt(
            entranceParams->rMin * entranceParams->rMin
            + uR * (
                entranceParams->rMax * entranceParams->rMax
                - entranceParams->rMin * entranceParams->rMin
            )
        );
    
    const G4double uP = G4UniformRand();
    const G4double phi = 
        entranceParams->phiMin
        + uP * (
            entranceParams->phiMax - entranceParams->phiMin
        );
    
    const G4ThreeVector entrancePoint(
        r * std::cos(phi),
        r * std::sin(phi),
        (-1 * gasParams->length / 2.0) - 5 * mm
    );

    particleGun_->SetParticlePosition(
        entrancePoint
    );


    // Virtual decay vertex
    const G4double uL = G4UniformRand();
    const G4double zVirtualVertex = (uL * decayRegionParams->length) - decayRegionParams->start;
    const G4ThreeVector virtualVertex(
        G4RandGauss::shoot(0.0, decayRegionParams->sigmaX),
        G4RandGauss::shoot(0.0, decayRegionParams->sigmaY),
        zVirtualVertex
    );

    particleGun_->SetParticleMomentumDirection(
        entrancePoint - virtualVertex
    );


    // Particle
    const G4double uE = G4UniformRand();
    const G4double momentum = 
        particleParams->pMin + uE * (particleParams->pMax - particleParams->pMin);
    
    const G4double mass =
        particleGun_->GetParticleDefinition()->GetPDGMass();

    const G4double kineticEnergy =
        std::sqrt(momentum * momentum + mass * mass) - mass;

    particleGun_->SetParticleEnergy(kineticEnergy);

    particleGun_->GeneratePrimaryVertex(event);
}