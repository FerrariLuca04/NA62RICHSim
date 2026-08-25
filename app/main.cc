#include "na62rich/detector/DetectorConstruction.hh"

#include "na62rich/actions/ActionInitialization.hh"

#include "na62rich/io/RootFileMerger.hh"
#include "na62rich/io/Paths.hh"

#include "na62rich/io/DetectorConfig.hh"
#include "na62rich/io/TestDetectorConfig.hh"

#include "na62rich/io/PrimaryGeneratorConfig.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4VPhysicsConstructor.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"

#include <iostream>

int main(int argc, char** argv)
{   
    std::string macro = "";
    std::string output = "";
    std::string configDetectorFlag = "";
    std::string configGeneratorFlag = "";

    if (argc > 1) {
        for (int a = 0; a < argc; ++a){
            std::string arg = argv[a];

            // Select macro
            if (arg == "--macro") {
                macro = argv[a + 1];
                a += 1; //Skip the value
            }
            else if (arg.ends_with(".mac")) {
                macro = arg;
            }
            // Select output name
            else if (arg == "--output") {
                output = argv[a + 1];
                a += 1; //Skip the value
            }
            else if (arg.ends_with(".root")) {
                output = arg;
            }
            // Select config
            else if (arg == "--config-detector") {
                configDetectorFlag = argv[a + 1];
                a += 1; //Skip the value
            }
            else if (arg == "--config-generator") {
                configGeneratorFlag = arg;
            }
        }
    }

    //Change directories
    if (configDetectorFlag != "") {
        detectorConfigFile = configDirectory / configDetectorFlag;
    }
    if (configGeneratorFlag != "") {
        generatorConfigFile = configDirectory / configGeneratorFlag;
    }
    if (output != "") {
        outputFile = outputDirectory / output;
    }

    // ---------------------------------------------------------
    // Output setting
    // ---------------------------------------------------------

    bool created = std::filesystem::create_directories(outputDirectory);
    
    if (created) {
        std::cout<<
            "Created " << outputDirectory.string() << " for output files."
        <<std::endl;
    } else {
        std::cout<<
            "Selected " << outputDirectory.string() << " for output files."
        <<std::endl;
    }

    std::filesystem::create_directories(tmpDirectory);

    // ---------------------------------------------------------
    // Configuration setting
    // ---------------------------------------------------------

    SetDetectorParams(detectorConfigFile.string());

    TestDetectorConfig(
        *worldParams,
        *gasParams,
        *mirrorParams,
        *pmtParams
    );

    SetGeneratorParams(generatorConfigFile.string());

    TestPrimaryGeneratorConfig(
        *entranceParams,
        *decayRegionParams
    );

    // ---------------------------------------------------------
    // Physics list
    // ---------------------------------------------------------

    auto* physicsList = new FTFP_BERT();
    physicsList->RegisterPhysics(new G4OpticalPhysics());

    std::cout<<
        "Physics list created.\nIt contains:" 
    <<std::endl;

    for (G4int i = 0; ; ++i) {
        const auto* physics = physicsList->GetPhysics(i);

        if (physics == nullptr) {
            break;
        }

        std::cout
            << "  - "
            << physics->GetPhysicsName()
            << '\n';
    }

    // ---------------------------------------------------------
    // User interface
    // ---------------------------------------------------------

    G4UIExecutive* ui = nullptr;

    if (macro == "") {
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
        "Run Manager set successfully."
    <<std::endl;

    // ---------------------------------------------------------
    // Visualization
    // ---------------------------------------------------------

    auto* visManager = new G4VisExecutive();
    visManager->Initialize();

    std::cout<<
        "Vis Manager set and initialize successfully."
    <<std::endl;


    // ---------------------------------------------------------
    // UI manager
    // ---------------------------------------------------------

    auto* uiManager = G4UImanager::GetUIpointer();


    // ---------------------------------------------------------
    // Interactive or batch mode
    // ---------------------------------------------------------

    if (ui != nullptr) {
        
        std::cout<<
            "Running interactive mode."
        <<std::endl;

        uiManager->ApplyCommand(
            "/control/execute macros/init_vis.mac"
        );

        ui->SessionStart();

        delete ui;

    } else {

        std::cout<<
            "Running '" <<macro<< "' macro."
        <<std::endl;

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
        "Simulation ends successfully."
    <<std::endl;

    MergeRootFiles();

    std::cout<<
        "Temporary files merged successfully."
    <<std::endl;

    std::filesystem::remove_all(tmpDirectory);

    // ---------------------------------------------------------
    // Cleanup
    // ---------------------------------------------------------
    
    delete visManager;
    delete runManager;

    std::cout<<
        "Program is ended."
    <<std::endl;

    return 0;
}