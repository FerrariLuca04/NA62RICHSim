#include "na62rich/DetectorConstruction.hh"

#include "G4RunManagerFactory.hh"
#include "FTFP_BERT.hh"

int main()
{
    G4cout << "NA62RICHSim started successfully." << G4endl;

    auto* runManager =
        G4RunManagerFactory::CreateRunManager();
    
    runManager->SetUserInitialization(new DetectorConstruction);
    runManager->SetUserInitialization(new FTFP_BERT);

    runManager->Initialize();

    delete runManager;

    return 0;
}