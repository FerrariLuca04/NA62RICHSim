#include "na62rich/actions/EventAction.hh"
#include "na62rich/actions/RunAction.hh"
#include "na62rich/detector/PhotonHit.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

#include <vector>

EventAction::EventAction(RunAction* runAction)
    : fRunAction(runAction)
{}

void EventAction::BeginOfEventAction(const G4Event* event)
{
    fRunAction->GetSensorIDs().clear();

    fRunAction->GetSensorPosX().clear();
    fRunAction->GetSensorPosY().clear();

    fRunAction->GetHitX().clear();
    fRunAction->GetHitY().clear();

    fRunAction->GetHitEnergy().clear();
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    // --------------------------------------------------------
    // Get primary info
    // --------------------------------------------------------

    const auto* primaryVertex = event->GetPrimaryVertex(0);
    
    if (primaryVertex == nullptr)
    {
        return;
    }

    const auto* primaryParticle = primaryVertex->GetPrimary(0);
    
    if (primaryParticle == nullptr)
    {
        return;
    }

    const G4double primaryX = primaryVertex->GetX0();
    const G4double primaryY = primaryVertex->GetY0();
    const G4double primaryZ = primaryVertex->GetZ0();

    const G4ThreeVector primaryMomentum(
        primaryParticle->GetPx(),
        primaryParticle->GetPy(),
        primaryParticle->GetPz()
    );

    const G4double momentum = primaryMomentum.mag();
    const G4ThreeVector direction = primaryMomentum.unit();

    const G4int pdgCode = primaryParticle->GetPDGcode();



    // --------------------------------------------------------
    // Get photons infos
    // --------------------------------------------------------

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

    auto* analysisManager = G4AnalysisManager::Instance();


    // --------------------------------------------------------
    // Fill the Ntuple
    // --------------------------------------------------------
    /*********************************************************/
    analysisManager->FillNtupleIColumn(
        0,
        event->GetEventID()
    );
    /*********************************************************/
    analysisManager->FillNtupleIColumn(
        1,
        pdgCode
    );
    /*********************************************************/
    analysisManager->FillNtupleDColumn(
        2,
        momentum / GeV
    );
    analysisManager->FillNtupleDColumn(
        3,
        direction.x()
    );
    analysisManager->FillNtupleDColumn(
        4,
        direction.y()
    );
    analysisManager->FillNtupleDColumn(
        5,
        direction.z()
    );
    /*********************************************************/
    analysisManager->FillNtupleDColumn(
        6,
        primaryX / mm
    );
    analysisManager->FillNtupleDColumn(
        7,
        primaryY / mm
    );
    analysisManager->FillNtupleDColumn(
        8,
        primaryZ / m
    );
    /*********************************************************/

    auto& sensorIDs = fRunAction->GetSensorIDs();
    auto& sensorPosX = fRunAction->GetSensorPosX();
    auto& sensorPosY = fRunAction->GetSensorPosY();
    
    auto& hitX = fRunAction->GetHitX();
    auto& hitY = fRunAction->GetHitY();
    auto& hitEnergy = fRunAction->GetHitEnergy();

    for (std::size_t i = 0; i < hitsCollection->entries(); ++i)
    {
        const auto* hit = (*hitsCollection)[i];
        
        /*********************************************************/
        sensorIDs.push_back(
            hit->GetSensorID()
        );
        /*********************************************************/
        sensorPosX.push_back(
            hit->GetSensorPosition().x() / mm
        );
        sensorPosY.push_back(
            hit->GetSensorPosition().y() / mm
        );
        /*********************************************************/
        hitX.push_back(
            hit->GetPosition().x() / mm
        );
        hitY.push_back(
            hit->GetPosition().y() / mm
        );
        /*********************************************************/
        hitEnergy.push_back(
            hit->GetEnergy() / eV
        );
    }
    
    analysisManager->AddNtupleRow();

}