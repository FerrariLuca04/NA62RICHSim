#include "na62rich/detector/PhotonSensitiveDetector.hh"

#include "G4OpticalPhoton.hh"
#include "G4PhysicalConstants.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"

PhotonSensitiveDetector::PhotonSensitiveDetector(const G4String& name)
    : G4VSensitiveDetector(name)
{}

G4bool PhotonSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
    auto* track = step->GetTrack();

    // Only optical photon
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    {
        return false;
    }

    // Only the first step
    if (step->GetPreStepPoint()->GetStepStatus() != fGeomBoundary)
    {
        return false;
    }

    const auto energytrack = track->GetTotalEnergy();
    const auto position = track->GetPosition();

    track->SetTrackStatus(fStopAndKill);

    return true;
}