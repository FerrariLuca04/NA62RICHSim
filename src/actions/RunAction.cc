#include "na62rich/actions/RunAction.hh"

#include "na62rich/io/OutputPaths.hh"

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
    
    analysisManager->CreateNtupleIColumn("primary_pdg_code");
    
    analysisManager->CreateNtupleDColumn("primary_momentum_GeV");
    analysisManager->CreateNtupleDColumn("primary_dir_x");
    analysisManager->CreateNtupleDColumn("primary_dir_y");
    analysisManager->CreateNtupleDColumn("primary_dir_z");

    analysisManager->CreateNtupleDColumn("primary_vertex_x_mm");
    analysisManager->CreateNtupleDColumn("primary_vertex_y_mm");
    analysisManager->CreateNtupleDColumn("primary_vertex_z_m");

    analysisManager->CreateNtupleIColumn("sensorIDs", fSensorIDs);
    analysisManager->CreateNtupleDColumn("sensor_pos_x_mm", fSensorPosX);
    analysisManager->CreateNtupleDColumn("sensor_pos_y_mm", fSensorPosY);

    analysisManager->CreateNtupleDColumn("hit_x_mm", fHitX);
    analysisManager->CreateNtupleDColumn("hit_y_mm", fHitY);
    analysisManager->CreateNtupleDColumn("hit_energy_eV", fHitEnergy);

    analysisManager->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    auto* analysisManager = G4AnalysisManager::Instance();

    const auto file =
        tmpDirectory /
        (tmpFileName +
        "_run" +
        std::to_string(run->GetRunID()) +
        ".root");

    analysisManager->OpenFile(file.string());
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    auto* analysisManager = G4AnalysisManager::Instance();

    analysisManager->Write();
    analysisManager->CloseFile();
}