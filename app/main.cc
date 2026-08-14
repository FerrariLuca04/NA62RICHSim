#include "na62rich/detector/DetectorConstruction.hh"
#include "na62rich/actions/ActionInitialization.hh"
#include "na62rich/io/RootFileMerger.hh"
#include "na62rich/io/OutputPaths.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"

#include <iostream>

int main(int argc, char** argv)
{   
    std::filesystem::create_directories(tmpDirectory);

    // ---------------------------------------------------------
    // Physics list
    // ---------------------------------------------------------

    auto* physicsList = new FTFP_BERT();
    physicsList->RegisterPhysics(new G4OpticalPhysics());

    std::cout<<
        "Physics list created." << std::endl;

    // ---------------------------------------------------------
    // User interface
    // ---------------------------------------------------------

    G4UIExecutive* ui = nullptr;

    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }


    // ---------------------------------------------------------
    // Run manager
    // ---------------------------------------------------------

    auto* runManager = G4RunManagerFactory::CreateRunManager();
    
    runManager->SetUserInitialization(new DetectorConstruction);    // Geometry and materials
    runManager->SetUserInitialization(physicsList);                 // Physics list
    runManager->SetUserInitialization(new ActionInitialization);    // Primary generation

    std::cout<<
        "Run Manager set successfully."<<std::endl;

    // ---------------------------------------------------------
    // Visualization
    // ---------------------------------------------------------

    auto* visManager = new G4VisExecutive();
    visManager->Initialize();

    std::cout<<
        "Vis Manager set and initialize successfully."<<std::endl;


    // ---------------------------------------------------------
    // UI manager
    // ---------------------------------------------------------

    auto* uiManager = G4UImanager::GetUIpointer();


    // ---------------------------------------------------------
    // Interactive or batch mode
    // ---------------------------------------------------------

    if (ui != nullptr) {
        
        std::cout<<
            "Running interactive mode."<<std::endl;

        uiManager->ApplyCommand(
            "/control/execute macros/init_vis.mac"
        );

        ui->SessionStart();

        delete ui;

    } else {

        std::cout<<
            "Running '" <<argv[1]<< "' macro."<<std::endl;

        G4String command = "/control/execute ";
        G4String macroFile = argv[1];

        uiManager->ApplyCommand(
            command + macroFile
        );
    }

    // ---------------------------------------------------------
    // Output files
    // ---------------------------------------------------------

    std::cout<<
        "Simulation end successfully."<<std::endl;

    MergeRootFiles();

    std::cout<<
        "Temporary files merged successfully."<<std::endl;

    std::filesystem::remove_all(tmpDirectory);

    // ---------------------------------------------------------
    // Cleanup
    // ---------------------------------------------------------
    
    delete visManager;
    delete runManager;

    std::cout<<
        "Program is ended."<<std::endl;

    return 0;
}