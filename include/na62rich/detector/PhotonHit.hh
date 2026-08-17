#ifndef NA62RICH_PHOTON_HIT_HH
#define NA62RICH_PHOTON_HIT_HH

#include "G4VHit.hh"
#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"
#include "globals.hh"

class PhotonHit : public G4VHit {
    public:
        PhotonHit() = default;
        ~PhotonHit() override = default;

        void SetPosition(const G4ThreeVector& position) {
            fPosition = position;
        };
        void SetEnergy(G4double energy) {
            fEnergy = energy;
        };
        void SetSensorID(G4int sensorID) {
            fSensorID = sensorID;
        };
        void SetSensorPosition(const G4ThreeVector& sensorPosition) {
            fSensorPosition = sensorPosition;
        };


        const G4ThreeVector& GetPosition() const {
            return fPosition;
        };
        G4double GetEnergy() const {
            return fEnergy;
        };
        G4int GetSensorID() const {
            return fSensorID;
        };
        const G4ThreeVector& GetSensorPosition() const {
            return fSensorPosition;
        };

    private:
        G4ThreeVector fPosition;
        G4double fEnergy = 0;
        G4int fSensorID = -1;
        G4ThreeVector fSensorPosition;
};

using PhotonHitsCollection = G4THitsCollection<PhotonHit>;

#endif