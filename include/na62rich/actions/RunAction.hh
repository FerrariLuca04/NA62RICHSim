#ifndef NA62RICH_RUN_ACTION_HH
#define NA62RICH_RUN_ACTION_HH

#include "G4UserRunAction.hh"

#include <vector>

class G4Run;

class RunAction : public G4UserRunAction {
    public:
        RunAction();
        ~RunAction() override = default;

        void BeginOfRunAction(const G4Run* run) override;
        void EndOfRunAction(const G4Run* run) override;

        std::vector<G4int>& GetSensorIDs() {
            return fSensorIDs;
        }
        std::vector<G4double>& GetHitX() {
            return fHitX;
        }
        std::vector<G4double>& GetHitY() {
            return fHitY;
        }
        std::vector<G4double>& GetHitEnergy() {
            return fHitEnergy;
        }

        private:
            std::vector<G4int> fSensorIDs;
            std::vector<G4double> fHitX;
            std::vector<G4double> fHitY;
            std::vector<G4double> fHitEnergy;
};

#endif