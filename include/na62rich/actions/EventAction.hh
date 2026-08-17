#ifndef NA62RICH_EVENT_ACTION_HH
#define NA62RICH_EVENT_ACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;
class RunAction;

class EventAction : public G4UserEventAction {
    public: 
        explicit EventAction(RunAction* runAction);
        ~EventAction() override = default;

        void BeginOfEventAction(const G4Event* event) override;
        void EndOfEventAction(const G4Event* event) override;

    private:
        G4int fPhotonHitsCollectionID = -1;
        RunAction* fRunAction;
};

#endif