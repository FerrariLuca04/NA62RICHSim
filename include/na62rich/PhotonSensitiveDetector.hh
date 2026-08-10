#ifndef NA62RICH_PHOTON_SENSITIVE_DETECTOR_HH
#define NA62RICH_PHOTON_SENSITIVE_DETECTOR_HH

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4TouchableHistory;

class PhotonSensitiveDetector : public G4VSensitiveDetector {
    public:
        PhotonSensitiveDetector(const G4String& name);
        ~PhotonSensitiveDetector() override = default;

        G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
};

#endif