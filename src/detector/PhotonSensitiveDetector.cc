#include "na62rich/detector/PhotonSensitiveDetector.hh"

#include "na62rich/config/DetectorConfig.hh"

#include "G4OpticalPhoton.hh"
#include "G4PhysicalConstants.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"

#include "G4PhysicsFreeVector.hh"

#include "G4SDManager.hh"

#include "Randomize.hh"

PhotonSensitiveDetector::PhotonSensitiveDetector(
    const G4String& name,
    const PMTParams* pmtParams
)
    : G4VSensitiveDetector(name),
      fQuantumEfficiency(
          pmtParams->photonEnergies,
          pmtParams->quantumEfficiency
      )
{
    collectionName.insert("PhotonHitsCollection");
}

void PhotonSensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new PhotonHitsCollection(
        SensitiveDetectorName,
        collectionName[0]
    );

    if (fHitsCollectionID < 0) {
        fHitsCollectionID =
            G4SDManager::GetSDMpointer()
                ->GetCollectionID(fHitsCollection);
    }

    hce->AddHitsCollection(
        fHitsCollectionID,
        fHitsCollection
    );
}

G4bool PhotonSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
    auto* track = step->GetTrack();

    // Only optical photon
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    {
        return false;
    }

    // Only the first step
    auto* preStep = step->GetPreStepPoint();
    if (preStep->GetStepStatus() != fGeomBoundary)
    {
        return false;
    }

    const auto energy = track->GetKineticEnergy();

    const G4double qe = fQuantumEfficiency.Value(energy);

    if (G4UniformRand() >= qe)
    {
        track->SetTrackStatus(fStopAndKill);
        return false;
    }

    auto* hit = new PhotonHit;

    hit->SetEnergy(energy);
    hit->SetPosition(preStep->GetPosition());

    const auto touchable = preStep->GetTouchableHandle();

    const G4int sensorID = touchable->GetCopyNumber();

    const auto* physical = touchable->GetVolume();

    const G4ThreeVector sensorPosition = physical->GetTranslation();

    hit->SetSensorID(sensorID);
    hit->SetSensorPosition(sensorPosition);

    fHitsCollection->insert(hit);

    track->SetTrackStatus(fStopAndKill);

    return true;
}