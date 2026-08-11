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

    auto* analysisManager =
        G4AnalysisManager::Instance();

    for (std::size_t i = 0; i < hitsCollection->entries(); ++i)
    {
        const auto* hit = (*hitsCollection)[i];

        analysisManager->FillNtupleIColumn(
            0,
            event->GetEventID()
        );

        analysisManager->FillNtupleIColumn(
            1,
            hit->GetSensorID()
        );

        analysisManager->FillNtupleDColumn(
            2,
            hit->GetPosition().x() / mm
        );

        analysisManager->FillNtupleDColumn(
            3,
            hit->GetPosition().y() / mm
        );

        analysisManager->FillNtupleDColumn(
            4,
            hit->GetEnergy() / eV
        );

        analysisManager->AddNtupleRow();
    }

}