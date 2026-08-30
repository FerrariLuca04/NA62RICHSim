#include <TCanvas.h>
#include <TFile.h>
#include <TH2D.h>
#include <TTree.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TSystem.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <numeric>

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

double ExpectedCherenkovRadius(
    double momentumGeV,
    double particleMassGeV,
    double refractiveIndex,
    double focalLengthMm
)
{
    const double energyGeV =
        std::sqrt(
            momentumGeV * momentumGeV +
            particleMassGeV * particleMassGeV
        );

    const double beta =
        momentumGeV / energyGeV;

    const double cosTheta =
        1.0 / (refractiveIndex * beta);

    // Below Cherenkov threshold
    if (cosTheta >= 1.0) {
        return -10.0;
    }

    const double theta =
        std::acos(cosTheta);

    return focalLengthMm *
        std::tan(theta);
}

double GetParticleMassGeV(int pdgCode)
{
    const auto* particle =
        TDatabasePDG::Instance()
            ->GetParticle(pdgCode);

    if (!particle) {
        std::cerr
            << "[ERROR] Unknown PDG code: "
            << pdgCode
            << '\n';

        return -1.0;
    }

    return particle->Mass();
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
        !tree->GetBranch("hit_y_mm") ||
        !tree->GetBranch("primary_pdg_code")
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

    int pdgCode = 0;

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

    tree->SetBranchAddress(
        "primary_pdg_code",
        &pdgCode
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
        !treeDetector->GetBranch("mirror_curvature_radius_m") ||

        !treeDetector->GetBranch("gas_refractive_index") ||
        
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
    double mirrorCurvatureRadius = 0.0;

    std::vector<double>* refractiveIndexVector = nullptr;

    double momentumMinGeV = 0.0;
    double momentumMaxGeV = 0.0;

    treeDetector->SetBranchAddress(
        "mirror_outer_radius_mm",
        &mirrorOuterRadius
    );

    treeDetector->SetBranchAddress(
        "mirror_curvature_radius_m",
        &mirrorCurvatureRadius
    );


    treeDetector->SetBranchAddress(
        "gas_refractive_index",
        &refractiveIndexVector
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

    if (!refractiveIndexVector || refractiveIndexVector->empty()) {
        std::cerr
            << "[FAIL] Invalid refractive index configuration\n";

        return 1;
    }

    const double focalLengthMm = mirrorCurvatureRadius * 1000.0 / 2.0;

    const double meanRefractiveIndex =
        std::accumulate(
            refractiveIndexVector->begin(),
            refractiveIndexVector->end(),
            0.0
        ) /
        static_cast<double>(refractiveIndexVector->size());

    // -----------------------------------------------------
    // Create histograms
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

    auto* residualHistogram = new TH1D(
        "relative_radius_residual",

        "Relative Cherenkov radius residual;"
        "(R_{meas} - R_{exp}) / R_{exp};"
        "Events",

        100,
        -0.1,
        0.1
    );

    residualHistogram->SetDirectory(nullptr);

    histogram->SetDirectory(nullptr);


    // -----------------------------------------------------
    // Loop over events
    // -----------------------------------------------------

    const Long64_t entries = tree->GetEntries();

    double deltaSum = 0.0;
    double deltaSquaredSum = 0.0;

    std::set<double> particleMassesGeV;

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
        // Predict ring radius
        // -------------------------------------------------

        const double massGeV = GetParticleMassGeV(pdgCode);
        particleMassesGeV.insert(massGeV);

        const double radiusPredictMm =
            ExpectedCherenkovRadius(
                momentumGeV,
                massGeV,
                meanRefractiveIndex,
                focalLengthMm
            );
        
        if (radiusPredictMm <= 0.0) {
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
        // Residuals
        // -------------------------------------------------

        double delta = (radiusMm - radiusPredictMm) / radiusPredictMm;

        deltaSum += delta;
        deltaSquaredSum += delta * delta;

        residualHistogram->Fill(delta);

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
    // Draw histogram
    // -----------------------------------------------------

    auto* canvas = new TCanvas(
        "ring_radius_canvas",
        "Ring radius vs primary momentum",
        900,
        700
    );

    histogram->Draw("COLZ");

    // -----------------------------------------------------
    // Draw expected graph
    // -----------------------------------------------------

    int theoryPoints = momentumBins * 10;

    for (const double m : particleMassesGeV) {
        auto* expectedRadiusGraph =
            new TGraph(theoryPoints);

        for (int i = 0; i < theoryPoints; ++i) {

            const double fraction =
                static_cast<double>(i) /
                static_cast<double>(theoryPoints - 1);

            const double momentum =
                momentumMinGeV +
                fraction *
                (momentumMaxGeV - momentumMinGeV);

            const double radius =
                ExpectedCherenkovRadius(
                    momentum,
                    m,
                    meanRefractiveIndex,
                    focalLengthMm
                );

            expectedRadiusGraph->SetPoint(
                i,
                momentum,
                radius
            );
        }

        expectedRadiusGraph->SetLineWidth(1);
        expectedRadiusGraph->SetLineColor(kRed);
        expectedRadiusGraph->Draw("L SAME");
    }

    canvas->Modified();
    canvas->Update();

    canvas->SaveAs(
        "output/ring_radius_vs_momentum.pdf"
    );

    std::cout
        << "Plot saved in: "
        << "output/ring_radius_vs_momentum.pdf"
        << '\n';

    auto* residualCanvas = new TCanvas(
        "residuals_canvas",
        "Residuals histrogram",
        900,
        700
    );

    residualHistogram->Draw("COLZ");

    residualCanvas->Modified();
    residualCanvas->Update();

    residualCanvas->SaveAs(
        "output/residuals.pdf"
    );

    std::cout
        << "Plot saved in: "
        << "output/residuals.pdf"
        << '\n';

    // -----------------------------------------------------
    // Compare bias and RMS estimator
    // -----------------------------------------------------

    if (validEvents == 0) {
        std::cerr
            << "[FAIL] No valid events for Cherenkov validation\n";

        return 1;
    }

    const double meanDelta = deltaSum / validEvents;
    const double rmsDelta = std::sqrt(deltaSquaredSum / validEvents);

    constexpr double maxMeanDelta = 0.01;
    constexpr double maxSigmaDelta = 0.03;

    if (
        std::abs(meanDelta) > maxMeanDelta ||
        rmsDelta > maxSigmaDelta
    ) {
        std::cerr
            << "[FAIL] Cherenkov radius validation failed\n";

        return 1;
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
        << '\n'
        << "Cherenkov radius validation\n"
        << "  Mean residual:  "
        << meanDelta * 100.0
        << " %\n"
        << "  RMS residual:   "
        << rmsDelta * 100.0
        << " %\n";


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