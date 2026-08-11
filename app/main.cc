#include "na62rich/detector/DetectorConstruction.hh"
#include "na62rich/actions/ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"

int main(int argc, char** argv)
{   
    // ---------------------------------------------------------
    // Physics list
    // ---------------------------------------------------------

    auto* physicsList = new FTFP_BERT();
    physicsList->RegisterPhysics(new G4OpticalPhysics());


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

    G4cout << "NA62RICHSim started successfully." << G4endl;

    auto* runManager = G4RunManagerFactory::CreateRunManager();
    
    runManager->SetUserInitialization(new DetectorConstruction);  // Geometry and materials
    runManager->SetUserInitialization(physicsList);             // Physics list
    runManager->SetUserInitialization(new ActionInitialization);  // Primary generation


    // ---------------------------------------------------------
    // Visualization
    // ---------------------------------------------------------

    auto* visManager = new G4VisExecutive();
    visManager->Initialize();


    // ---------------------------------------------------------
    // UI manager
    // ---------------------------------------------------------

    auto* uiManager = G4UImanager::GetUIpointer();


    // ---------------------------------------------------------
    // Interactive or batch mode
    // ---------------------------------------------------------

    if (ui != nullptr) {

        uiManager->ApplyCommand(
            "/control/execute macros/init_vis.mac"
        );

        ui->SessionStart();

        delete ui;

    } else {

        G4String command = "/control/execute ";
        G4String macroFile = argv[1];

        uiManager->ApplyCommand(
            command + macroFile
        );
    }


    // ---------------------------------------------------------
    // Cleanup
    // ---------------------------------------------------------
    
    delete visManager;
    delete runManager;

    return 0;
}