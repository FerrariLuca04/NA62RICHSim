#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>

#include <iostream>
#include <vector>
#include <string>


// ---------------------------------------------------------
// Primary generator validation
// ---------------------------------------------------------

int ValidatePrimaryGenerator(
    const char* filename,
    int expectedPdg
)
{
    // -----------------------------------------------------
    // Open ROOT file
    // -----------------------------------------------------

    TFile file(filename, "READ");

    if (file.IsZombie()) {
        std::cerr
            << "[FAIL] Cannot open file: "
            << filename
            << '\n';

        return 1;
    }


    // -----------------------------------------------------
    // Get PrimaryGeneratorConfig tree
    // -----------------------------------------------------

    TTree* configTree = nullptr;

    file.GetObject(
        "PrimaryGeneratorConfig",
        configTree
    );

    if (!configTree) {
        std::cerr
            << "[FAIL] PrimaryGeneratorConfig tree not found\n";

        return 2;
    }


    // -----------------------------------------------------
    // Check configuration entries
    // -----------------------------------------------------

    if (configTree->GetEntries() != 1) {
        std::cerr
            << "[FAIL] PrimaryGeneratorConfig must contain "
            << "exactly one entry, got "
            << configTree->GetEntries()
            << '\n';

        return 3;
    }


    // -----------------------------------------------------
    // Check required configuration branches
    // -----------------------------------------------------

    const std::vector<std::string> requiredConfigBranches = {
        "particle_momentum_min_GeV",
        "particle_momentum_max_GeV"
    };

    for (const auto& branch : requiredConfigBranches) {

        if (!configTree->GetBranch(branch.c_str())) {
            std::cerr
                << "[FAIL] Missing configuration branch: "
                << branch
                << '\n';

            return 4;
        }
    }


    // -----------------------------------------------------
    // Read configuration
    // -----------------------------------------------------

    Double_t pMin = 0.0;
    Double_t pMax = 0.0;

    configTree->SetBranchAddress(
        "particle_momentum_min_GeV",
        &pMin
    );

    configTree->SetBranchAddress(
        "particle_momentum_max_GeV",
        &pMax
    );

    configTree->GetEntry(0);


    // -----------------------------------------------------
    // Check configuration
    // -----------------------------------------------------

    if (pMin > pMax) {
        std::cerr
            << "[FAIL] Invalid momentum interval in "
            << "PrimaryGeneratorConfig: ["
            << pMin
            << ", "
            << pMax
            << "] GeV/c\n";

        return 5;
    }


    // -----------------------------------------------------
    // Get PhotonHits tree
    // -----------------------------------------------------

    TTree* tree = nullptr;

    file.GetObject(
        "PhotonHits",
        tree
    );

    if (!tree) {
        std::cerr
            << "[FAIL] PhotonHits tree not found\n";

        return 6;
    }


    // -----------------------------------------------------
    // Check entries
    // -----------------------------------------------------

    const Long64_t entries =
        tree->GetEntries();

    if (entries == 0) {
        std::cerr
            << "[FAIL] PhotonHits tree contains no entries\n";

        return 7;
    }


    // -----------------------------------------------------
    // Check required branches
    // -----------------------------------------------------

    if (!tree->GetBranch("primary_pdg_code")) {
        std::cerr
            << "[FAIL] Missing branch: "
            << "primary_pdg_code\n";

        return 8;
    }

    if (!tree->GetBranch("primary_momentum_GeV")) {
        std::cerr
            << "[FAIL] Missing branch: "
            << "primary_momentum_GeV\n";

        return 9;
    }


    // -----------------------------------------------------
    // Connect branches
    // -----------------------------------------------------

    Int_t pdgCode = 0;
    Double_t momentum = 0.0;

    tree->SetBranchAddress(
        "primary_pdg_code",
        &pdgCode
    );

    tree->SetBranchAddress(
        "primary_momentum_GeV",
        &momentum
    );


    // -----------------------------------------------------
    // Validate events
    // -----------------------------------------------------

    for (Long64_t i = 0; i < entries; ++i) {

        tree->GetEntry(i);


        // -------------------------------------------------
        // Particle type
        // -------------------------------------------------

        if (pdgCode != expectedPdg) {

            std::cerr
                << "[FAIL] Event "
                << i
                << ": expected PDG "
                << expectedPdg
                << ", got "
                << pdgCode
                << '\n';

            return 10;
        }


        // -------------------------------------------------
        // Momentum range
        // -------------------------------------------------

        if (
            momentum < pMin ||
            momentum > pMax
        ) {

            std::cerr
                << "[FAIL] Event "
                << i
                << ": momentum = "
                << momentum
                << " GeV/c, expected in ["
                << pMin
                << ", "
                << pMax
                << "] GeV/c\n";

            return 11;
        }
    }


    // -----------------------------------------------------
    // Success
    // -----------------------------------------------------

    std::cout
        << "[PASS] Primary generator test passed\n"
        << "       PDG: "
        << expectedPdg
        << '\n'
        << "       momentum range: ["
        << pMin
        << ", "
        << pMax
        << "] GeV/c\n"
        << "       events: "
        << entries
        << '\n';

    return 0;
}


// ---------------------------------------------------------
// ROOT / CTest entry point
// ---------------------------------------------------------

void test_primary_generator(
    const char* filename,
    int expectedPdg
)
{
    const int result =
        ValidatePrimaryGenerator(
            filename,
            expectedPdg
        );

    gSystem->Exit(result);
}