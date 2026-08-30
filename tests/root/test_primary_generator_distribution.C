#include <TFile.h>
#include <TMath.h>
#include <TSystem.h>
#include <TTree.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>


// ---------------------------------------------------------
// Utility functions
// ---------------------------------------------------------

double WrapToTwoPi(double angle)
{
    const double twoPi = 2.0 * TMath::Pi();

    angle = std::fmod(angle, twoPi);

    if (angle < 0.0) {
        angle += twoPi;
    }

    return angle;
}


double Mean(const std::vector<double>& values)
{
    if (values.empty()) {
        return 0.0;
    }

    return std::accumulate(
        values.begin(),
        values.end(),
        0.0
    ) / static_cast<double>(values.size());
}


// One-sample Kolmogorov-Smirnov test against U(0,1)
double UniformKSPValue(std::vector<double> values)
{
    if (values.empty()) {
        return 0.0;
    }

    std::sort(
        values.begin(),
        values.end()
    );

    const double n =
        static_cast<double>(values.size());

    double dPlus = 0.0;
    double dMinus = 0.0;

    for (std::size_t i = 0; i < values.size(); ++i) {

        const double x = values[i];

        const double empiricalUpper =
            static_cast<double>(i + 1) / n;

        const double empiricalLower =
            static_cast<double>(i) / n;

        dPlus = std::max(
            dPlus,
            empiricalUpper - x
        );

        dMinus = std::max(
            dMinus,
            x - empiricalLower
        );
    }

    const double d =
        std::max(dPlus, dMinus);

    const double sqrtN =
        std::sqrt(n);

    const double lambda =
        (sqrtN + 0.12 + 0.11 / sqrtN) * d;

    return TMath::KolmogorovProb(lambda);
}


// ---------------------------------------------------------
// Primary-generator distribution validation
// ---------------------------------------------------------

int ValidatePrimaryGeneratorDistribution(
    const char* filename,
    Long64_t expectedEntries = -1,
    double minimumPValue = 1.0e-3
)
{
    // -----------------------------------------------------
    // Open ROOT file
    // -----------------------------------------------------

    TFile file(filename, "READ");

    if (file.IsZombie()) {
        std::cerr
            << "[FAIL] Cannot open ROOT file: "
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
            << "[FAIL] TTree 'PrimaryGeneratorConfig' "
            << "not found\n";

        return 2;
    }

    if (configTree->GetEntries() != 1) {
        std::cerr
            << "[FAIL] PrimaryGeneratorConfig must contain "
            << "exactly one entry, got "
            << configTree->GetEntries()
            << '\n';

        return 3;
    }


    // -----------------------------------------------------
    // Check configuration branches
    // -----------------------------------------------------

    const std::vector<std::string> requiredConfigBranches = {
        "particle_momentum_min_GeV",
        "particle_momentum_max_GeV",
        "entrance_radius_min_mm",
        "entrance_radius_max_mm",
        "entrance_phi_min_deg",
        "entrance_phi_max_deg"
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
    // Read generator configuration
    // -----------------------------------------------------

    Double_t pMinGeV = 0.0;
    Double_t pMaxGeV = 0.0;

    Double_t rMinMm = 0.0;
    Double_t rMaxMm = 0.0;

    Double_t phiMinDeg = 0.0;
    Double_t phiMaxDeg = 0.0;

    configTree->SetBranchAddress(
        "particle_momentum_min_GeV",
        &pMinGeV
    );

    configTree->SetBranchAddress(
        "particle_momentum_max_GeV",
        &pMaxGeV
    );

    configTree->SetBranchAddress(
        "entrance_radius_min_mm",
        &rMinMm
    );

    configTree->SetBranchAddress(
        "entrance_radius_max_mm",
        &rMaxMm
    );

    configTree->SetBranchAddress(
        "entrance_phi_min_deg",
        &phiMinDeg
    );

    configTree->SetBranchAddress(
        "entrance_phi_max_deg",
        &phiMaxDeg
    );

    configTree->GetEntry(0);


    // -----------------------------------------------------
    // Validate configuration
    // -----------------------------------------------------

    if (pMinGeV >= pMaxGeV) {
        std::cerr
            << "[FAIL] Invalid momentum interval in "
            << "PrimaryGeneratorConfig\n";

        return 5;
    }

    if (rMinMm >= rMaxMm) {
        std::cerr
            << "[FAIL] Invalid entrance radial interval in "
            << "PrimaryGeneratorConfig\n";

        return 6;
    }

    const double phiMin =
        phiMinDeg * TMath::Pi() / 180.0;

    const double phiMax =
        phiMaxDeg * TMath::Pi() / 180.0;

    const double phiWidth =
        phiMax - phiMin;

    if (
        phiWidth <= 0.0 ||
        phiWidth > 2.0 * TMath::Pi()
    ) {
        std::cerr
            << "[FAIL] Invalid entrance phi interval in "
            << "PrimaryGeneratorConfig\n";

        return 7;
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

        return 8;
    }


    // -----------------------------------------------------
    // Check required event branches
    // -----------------------------------------------------

    const std::vector<std::string> requiredBranches = {
        "primary_momentum_GeV",
        "primary_vertex_x_mm",
        "primary_vertex_y_mm"
    };

    for (const auto& branch : requiredBranches) {

        if (!tree->GetBranch(branch.c_str())) {
            std::cerr
                << "[FAIL] Missing PhotonHits branch: "
                << branch
                << '\n';

            return 9;
        }
    }


    // -----------------------------------------------------
    // Number of events
    // -----------------------------------------------------

    const Long64_t entries =
        tree->GetEntries();

    if (entries == 0) {
        std::cerr
            << "[FAIL] PhotonHits contains no entries\n";

        return 10;
    }

    if (
        expectedEntries >= 0 &&
        entries != expectedEntries
    ) {
        std::cerr
            << "[FAIL] Expected "
            << expectedEntries
            << " entries, got "
            << entries
            << '\n';

        return 11;
    }


    // -----------------------------------------------------
    // Connect PhotonHits branches
    // -----------------------------------------------------

    Double_t momentumGeV = 0.0;

    Double_t vertexXmm = 0.0;
    Double_t vertexYmm = 0.0;

    tree->SetBranchAddress(
        "primary_momentum_GeV",
        &momentumGeV
    );

    tree->SetBranchAddress(
        "primary_vertex_x_mm",
        &vertexXmm
    );

    tree->SetBranchAddress(
        "primary_vertex_y_mm",
        &vertexYmm
    );


    // -----------------------------------------------------
    // Normalized distributions
    //
    // Correct generation implies:
    //
    // uP   ~ U(0,1)
    // uR2  ~ U(0,1)
    // uPhi ~ U(0,1)
    // -----------------------------------------------------

    std::vector<double> normalizedMomentum;
    std::vector<double> normalizedRadiusSquared;
    std::vector<double> normalizedPhi;

    normalizedMomentum.reserve(entries);
    normalizedRadiusSquared.reserve(entries);
    normalizedPhi.reserve(entries);

    const double rMinSquared =
        rMinMm * rMinMm;

    const double rMaxSquared =
        rMaxMm * rMaxMm;

    constexpr double tolerance = 1.0e-9;


    // -----------------------------------------------------
    // Loop over events
    // -----------------------------------------------------

    for (Long64_t i = 0; i < entries; ++i) {

        tree->GetEntry(i);


        // -------------------------------------------------
        // Sanity check
        // -------------------------------------------------

        if (
            !std::isfinite(momentumGeV) ||
            !std::isfinite(vertexXmm) ||
            !std::isfinite(vertexYmm)
        ) {
            std::cerr
                << "[FAIL] Non-finite value at entry "
                << i
                << '\n';

            return 12;
        }


        // -------------------------------------------------
        // Momentum
        // -------------------------------------------------

        if (
            momentumGeV < pMinGeV - tolerance ||
            momentumGeV > pMaxGeV + tolerance
        ) {
            std::cerr
                << "[FAIL] Momentum outside configured range "
                << "at entry "
                << i
                << ": "
                << momentumGeV
                << " GeV/c\n";

            return 13;
        }

        double uP =
            (momentumGeV - pMinGeV) /
            (pMaxGeV - pMinGeV);

        uP = std::clamp(
            uP,
            0.0,
            1.0
        );

        normalizedMomentum.push_back(uP);


        // -------------------------------------------------
        // Entrance radius
        // -------------------------------------------------

        const double rSquared =
            vertexXmm * vertexXmm +
            vertexYmm * vertexYmm;

        if (
            rSquared < rMinSquared - tolerance ||
            rSquared > rMaxSquared + tolerance
        ) {
            std::cerr
                << "[FAIL] Entrance point outside "
                << "configured radial range at entry "
                << i
                << '\n'
                << "       r = "
                << std::sqrt(rSquared)
                << " mm\n";

            return 14;
        }

        double uR2 =
            (rSquared - rMinSquared) /
            (rMaxSquared - rMinSquared);

        uR2 = std::clamp(
            uR2,
            0.0,
            1.0
        );

        normalizedRadiusSquared.push_back(uR2);


        // -------------------------------------------------
        // Entrance phi
        // -------------------------------------------------

        double phi =
            std::atan2(
                vertexYmm,
                vertexXmm
            );

        phi = WrapToTwoPi(phi);

        const double wrappedPhiMin =
            WrapToTwoPi(phiMin);

        const double deltaPhi =
            WrapToTwoPi(
                phi - wrappedPhiMin
            );

        if (
            deltaPhi > phiWidth + tolerance
        ) {
            std::cerr
                << "[FAIL] Entrance phi outside "
                << "configured range at entry "
                << i
                << '\n'
                << "       phi = "
                << phi * 180.0 / TMath::Pi()
                << " deg\n";

            return 15;
        }

        double uPhi =
            deltaPhi / phiWidth;

        uPhi = std::clamp(
            uPhi,
            0.0,
            1.0
        );

        normalizedPhi.push_back(uPhi);
    }


    // -----------------------------------------------------
    // Kolmogorov-Smirnov tests
    // -----------------------------------------------------

    const double pMomentum =
        UniformKSPValue(
            normalizedMomentum
        );

    const double pRadiusSquared =
        UniformKSPValue(
            normalizedRadiusSquared
        );

    const double pPhi =
        UniformKSPValue(
            normalizedPhi
        );


    // -----------------------------------------------------
    // Print configuration
    // -----------------------------------------------------

    std::cout
        << "\nPrimary generator configuration\n"
        << "-----------------------------------\n"
        << "Momentum: ["
        << pMinGeV
        << ", "
        << pMaxGeV
        << "] GeV/c\n"
        << "Entrance radius: ["
        << rMinMm
        << ", "
        << rMaxMm
        << "] mm\n"
        << "Entrance phi: ["
        << phiMinDeg
        << ", "
        << phiMaxDeg
        << "] deg\n";


    // -----------------------------------------------------
    // Print statistical results
    // -----------------------------------------------------

    std::cout
        << "\nPrimary generator distribution test\n"
        << "-----------------------------------\n"
        << "Events: "
        << entries
        << "\n\n";

    std::cout
        << "Momentum:\n"
        << "  normalized mean = "
        << Mean(normalizedMomentum)
        << '\n'
        << "  KS p-value      = "
        << pMomentum
        << '\n';

    std::cout
        << "\nEntrance radius squared:\n"
        << "  normalized mean = "
        << Mean(normalizedRadiusSquared)
        << '\n'
        << "  KS p-value      = "
        << pRadiusSquared
        << '\n';

    std::cout
        << "\nEntrance phi:\n"
        << "  normalized mean = "
        << Mean(normalizedPhi)
        << '\n'
        << "  KS p-value      = "
        << pPhi
        << "\n\n";


    // -----------------------------------------------------
    // Decide PASS / FAIL
    // -----------------------------------------------------

    bool success = true;

    if (pMomentum < minimumPValue) {

        std::cerr
            << "[FAIL] Momentum distribution "
            << "is not compatible with uniformity\n";

        success = false;

    } else {

        std::cout
            << "[PASS] Momentum distribution\n";
    }


    if (pRadiusSquared < minimumPValue) {

        std::cerr
            << "[FAIL] r^2 distribution "
            << "is not compatible with uniformity\n";

        success = false;

    } else {

        std::cout
            << "[PASS] Entrance r^2 distribution\n";
    }


    if (pPhi < minimumPValue) {

        std::cerr
            << "[FAIL] Phi distribution "
            << "is not compatible with uniformity\n";

        success = false;

    } else {

        std::cout
            << "[PASS] Entrance phi distribution\n";
    }


    if (!success) {
        return 16;
    }


    std::cout
        << "\n[PASS] Primary generator "
        << "distribution test passed\n";

    return 0;
}


// ---------------------------------------------------------
// ROOT / CTest entry point
// ---------------------------------------------------------

void test_primary_generator_distribution(
    const char* filename,
    Long64_t expectedEntries = -1,
    double minimumPValue = 1.0e-3
)
{
    const int result =
        ValidatePrimaryGeneratorDistribution(
            filename,
            expectedEntries,
            minimumPValue
        );

    gSystem->Exit(result);
}