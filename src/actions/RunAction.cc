#include "na62rich/actions/RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"

RunAction::RunAction()
{
    auto* analysisManager = G4AnalysisManager::Instance();

    analysisManager->CreateNtuple(
        "PhotonHits",
        "Detected optical photons"
    );

    analysisManager->CreateNtupleIColumn("eventID");
    analysisManager->CreateNtupleIColumn("sensorID");
    
    analysisManager->CreateNtupleDColumn("x_mm");
    analysisManager->CreateNtupleDColumn("y_mm");
    analysisManager->CreateNtupleDColumn("energy_eV");

    analysisManager->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    auto* analysisManager =
        G4AnalysisManager::Instance();

    analysisManager->OpenFile("photon_hits.root");
}

void RunAction::EndOfRunAction(const G4Run*)
{
    auto* analysisManager =
        G4AnalysisManager::Instance();

    analysisManager->Write();
    analysisManager->CloseFile();
}