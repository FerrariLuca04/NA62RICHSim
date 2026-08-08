#include "na62rich/PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    particleGun_ = new G4ParticleGun(1);

    auto* particleTable = G4ParticleTable::GetParticleTable();
    auto* pion = particleTable->FindParticle("pi+");

    particleGun_->SetParticleDefinition(pion);
    particleGun_->SetParticleMomentum(20.0 * GeV);
    particleGun_->SetParticleMomentumDirection(
        G4ThreeVector(0.0, 0.0, 1.0)
    );
    particleGun_->SetParticlePosition(
        G4ThreeVector(0.0, 0.0, -15.0 * m)
    );
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particleGun_;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    particleGun_->GeneratePrimaryVertex(event);
}