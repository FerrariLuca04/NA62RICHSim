#ifndef NA62RICH_PHOTON_SENSITIVE_DETECTOR_HH
#define NA62RICH_PHOTON_SENSITIVE_DETECTOR_HH

#include "na62rich/detector/PhotonHit.hh"

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4TouchableHistory;

class PhotonSensitiveDetector : public G4VSensitiveDetector {
    public:
        PhotonSensitiveDetector(const G4String& name);
        ~PhotonSensitiveDetector() override = default;

        void Initialize(G4HCofThisEvent* hce) override;

        G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    private:
        PhotonHitsCollection* fHitsCollection = nullptr;

        G4int fHitsCollectionID = -1;
};

#endif