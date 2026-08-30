#include <TCanvas.h>
#include <TFile.h>
#include <TH2D.h>
#include <TTree.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------
// Utility functions
// ---------------------------------------------------------

double EstimateRingRadius(
    const std::vector<double>& hitX,
    const std::vector<double>& hitY,
    double centerX,
    double centerY
)
{
    if (
        hitX.empty() ||
        hitX.size() != hitY.size()
    ) {
        std::cerr << "[FAIL] Invalid hit vectors\n";
        return -1.0;
    }

    double radiusSum = 0.0;

    for (std::size_t i = 0; i < hitX.size(); ++i) {

        const double localCenterX =
            (hitX[i] > 0.0)
            ? centerX
            : -centerX;

        const double dx =
            hitX[i] - localCenterX;

        const double dy =
            hitY[i] - centerY;

        radiusSum += std::sqrt(
            dx * dx +
            dy * dy
        );
    }

    return radiusSum /
        static_cast<double>(hitX.size());
}

double GetMaximumHitCoordinate(
    TTree* tree,
    const std::string& branchName
)
{
    if (!tree) {
        std::cerr
            << "[ERROR] Invalid TTree pointer\n";

        return 0.0;
    }

    if (
        branchName != "hit_x_mm" &&
        branchName != "hit_y_mm"
    ) {
        std::cerr
            << "[ERROR] Invalid branch: "
            << branchName
            << '\n';

        return 0.0;
    }

    if (!tree->GetBranch(branchName.c_str())) {
        std::cerr
            << "[ERROR] Branch not found: "
            << branchName
            << '\n';

        return 0.0;
    }


    // -----------------------------------------------------
    // Connect branch
    // -----------------------------------------------------

    std::vector<double>* values = nullptr;

    tree->SetBranchAddress(
        branchName.c_str(),
        &values
    );


    // -----------------------------------------------------
    // Find global maximum
    // -----------------------------------------------------

    double maximum = 0.0;

    bool valueFound = false;

    const Long64_t entries =
        tree->GetEntries();

    for (Long64_t i = 0; i < entries; ++i) {

        tree->GetEntry(i);

        if (!values || values->empty()) {
            continue;
        }

        for (const double value : *values) {

            maximum = std::max(
                maximum,
                std::abs(value)
            );

            valueFound = true;
        }
    }


    // -----------------------------------------------------
    // No hits found
    // -----------------------------------------------------

    if (!valueFound) {
        std::cerr
            << "[ERROR] No values found in branch: "
            << branchName
            << '\n';

        return 0.0;
    }

    return maximum;
}

// ---------------------------------------------------------
// Plot ring radius vs primary momentum
// ---------------------------------------------------------

int ValidateCherenkovProcess(
    const char* filename,

    int momentumBins = 500,
    int radiusBins = 200
)
{
    // -----------------------------------------------------
    // Open ROOT file
    // -----------------------------------------------------

    TFile file(
        filename,
        "READ"
    );

    if (file.IsZombie()) {
        std::cerr
            << "[FAIL] Cannot open ROOT file: "
            << filename
            << '\n';

        return 1;
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
            << "[FAIL] TTree 'PhotonHits' not found\n";

        return 1;
    }


    // -----------------------------------------------------
    // Check required branches
    // -----------------------------------------------------

    if (
        !tree->GetBranch("primary_momentum_GeV") ||
        !tree->GetBranch("hit_x_mm") ||
        !tree->GetBranch("hit_y_mm")
    ) {
        std::cerr
            << "[FAIL] Missing required branch\n";

        return 1;
    }

    // -----------------------------------------------------
    // Get radius scale
    // -----------------------------------------------------

    double radiusScale = GetMaximumHitCoordinate(tree, "hit_y_mm");
    tree->ResetBranchAddresses();

    // -----------------------------------------------------
    // Connect branches
    // -----------------------------------------------------

    double momentumGeV = 0.0;

    std::vector<double>* hitX = nullptr;
    std::vector<double>* hitY = nullptr;

    tree->SetBranchAddress(
        "primary_momentum_GeV",
        &momentumGeV
    );

    tree->SetBranchAddress(
        "hit_x_mm",
        &hitX
    );

    tree->SetBranchAddress(
        "hit_y_mm",
        &hitY
    );

    // -----------------------------------------------------
    // Get Config tree
    // -----------------------------------------------------

    TTree* treeDetector = nullptr;
    TTree* treeGenerator = nullptr;

    file.GetObject(
        "DetectorConfig",
        treeDetector
    );
    file.GetObject(
        "PrimaryGeneratorConfig",
        treeGenerator
    );

    if (!treeDetector || !treeGenerator) {
        std::cerr
            << "[FAIL] Configurations' TTree not found\n";

        return 1;
    }


    // -----------------------------------------------------
    // Check required branches
    // -----------------------------------------------------

    if (
        !treeDetector->GetBranch("mirror_outer_radius_mm") ||
        
        !treeGenerator->GetBranch("particle_momentum_min_GeV") ||
        !treeGenerator->GetBranch("particle_momentum_max_GeV")
    ) {
        std::cerr
            << "[FAIL] Missing required branch in configurations' TTree\n";

        return 1;
    }

    // -----------------------------------------------------
    // Exctract configurations' parameters
    // -----------------------------------------------------

    double mirrorOuterRadius = 0.0;
    double momentumMinGeV = 0.0;
    double momentumMaxGeV = 0.0;

    treeDetector->SetBranchAddress(
        "mirror_outer_radius_mm",
        &mirrorOuterRadius
    );

    treeGenerator->SetBranchAddress(
        "particle_momentum_min_GeV",
        &momentumMinGeV
    );
    treeGenerator->SetBranchAddress(
        "particle_momentum_max_GeV",
        &momentumMaxGeV
    );

    treeGenerator->GetEntry(0);
    treeDetector->GetEntry(0);

    // -----------------------------------------------------
    // Create histogram
    // -----------------------------------------------------

    auto* histogram = new TH2D(
        "ring_radius_vs_momentum",

        "Ring radius vs primary momentum;"
        "Primary momentum [GeV/c];"
        "Ring radius [mm]",

        momentumBins,
        momentumMinGeV,
        momentumMaxGeV,

        radiusBins,
        0.0,
        radiusScale * 1.25
    );

    histogram->SetDirectory(nullptr);


    // -----------------------------------------------------
    // Loop over events
    // -----------------------------------------------------

    const Long64_t entries = tree->GetEntries();

    Long64_t validEvents = 0;

    for (Long64_t i = 0; i < entries; ++i) {

        tree->GetEntry(i);


        // -------------------------------------------------
        // Skip events without hits
        // -------------------------------------------------

        if (
            !hitX ||
            !hitY ||
            hitX->empty() ||
            hitX->size() != hitY->size()
        ) {
            continue;
        }


        // -------------------------------------------------
        // Estimate ring radius
        // -------------------------------------------------

        const double radiusMm =
            EstimateRingRadius(
                *hitX,
                *hitY,
                mirrorOuterRadius / 2.0,
                0.0
            );

        if (radiusMm <= 0.0) {
            continue;
        }


        // -------------------------------------------------
        // Fill histogram
        // -------------------------------------------------

        histogram->Fill(
            momentumGeV,
            radiusMm
        );

        ++validEvents;
    }


    // -----------------------------------------------------
    // Print summary
    // -----------------------------------------------------

    std::cout
        << "Events in tree: "
        << entries
        << '\n'
        << "Events with reconstructed radius: "
        << validEvents
        << '\n';


    // -----------------------------------------------------
    // Draw histogram
    // -----------------------------------------------------

    auto* canvas = new TCanvas(
        "ring_radius_canvas",
        "Ring radius vs primary momentum",
        900,
        700
    );

    histogram->Draw("COLZ");

    canvas->Modified();
    canvas->Update();

    canvas->SaveAs(
        "output/ring_radius_vs_momentum.pdf"
    );

    std::cout
        << "Plot saved in: "
        << "output/ring_radius_vs_momentum.pdf"
        << '\n';

    return 0;
}

// ---------------------------------------------------------
// ROOT / CTest entry point
// ---------------------------------------------------------

void test_cherenkov(
    const char* filename,
    int momentumBins = 500,
    int radiusBins = 200
)
{
    const int result =
        ValidateCherenkovProcess(
            filename
        );

    gSystem->Exit(result);
}