#include "na62rich/actions/EventAction.hh"
#include "na62rich/detector/PhotonHit.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

void EventAction::EndOfEventAction(const G4Event* event)
{
    if (fPhotonHitsCollectionID < 0)
    {
        fPhotonHitsCollectionID = 
            G4SDManager::GetSDMpointer()->GetCollectionID("PhotonHitsCollection");
    }

    auto* hce = event->GetHCofThisEvent();

    if (hce == nullptr) 
    {
        return;
    }

    auto* hitsCollection = 
        static_cast<PhotonHitsCollection*>(
            hce->GetHC(fPhotonHitsCollectionID)
        );

    if (hitsCollection == nullptr)
    {
        return;
    }
}