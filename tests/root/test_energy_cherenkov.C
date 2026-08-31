#include <TCanvas.h>
#include <TFile.h>
#include <TH2D.h>
#include <TTree.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TSystem.h>


#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <stdexcept>

// ---------------------------------------------------------
// Utility functions
// ---------------------------------------------------------

double interpolate(
    double energy,
    const std::vector<double>& energies,
    const std::vector<double>& values
) {
    if (energies.size() != values.size() || energies.size() < 2) {
        return 0.0;
    }

    if (energy < energies.front() ||
        energy > energies.back()) {
        return 0.0;
    }

    auto upper = std::lower_bound(
        energies.begin(),
        energies.end(),
        energy
    );

    if (upper == energies.begin()) {
        return values.front();
    }

    if (upper == energies.end()) {
        return values.back();
    }

    const std::size_t i =
        std::distance(energies.begin(), upper);

    const double e1 = energies[i - 1];
    const double e2 = energies[i];

    const double y1 = values[i - 1];
    const double y2 = values[i];

    return y1
        + (energy - e1)
        * (y2 - y1)
        / (e2 - e1);
}

std::vector<double>* gRefractiveIndexEnergies = nullptr;
std::vector<double>* gRefractiveIndex = nullptr;

std::vector<double>* gQuantumEfficiencyEnergies = nullptr;
std::vector<double>* gQuantumEfficiency = nullptr;

double predictedSpectrumFunction(
    double* x,
    double* par
) {
    const double energy = x[0];

    if (!gRefractiveIndexEnergies ||
        !gRefractiveIndex ||
        !gQuantumEfficiencyEnergies ||
        !gQuantumEfficiency) {
        return 0.0;
    }

    const double n = interpolate(
        energy,
        *gRefractiveIndexEnergies,
        *gRefractiveIndex
    );

    const double qe = interpolate(
        energy,
        *gQuantumEfficiencyEnergies,
        *gQuantumEfficiency
    );

    if (n <= 1.0 || qe <= 0.0) {
        return 0.0;
    }

    const double normalization = par[0];

    return normalization
        * (1.0 - 1.0 / (n * n))
        * qe;
}

// ---------------------------------------------------------
// Plot ring radius vs primary momentum
// ---------------------------------------------------------

int ValidateCherenkovEnergy(
    const char* filename,

    int energyBins = 50
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
        !tree->GetBranch("hit_energy_eV")
    ) {
        std::cerr
            << "[FAIL] Missing required branch\n";

        return 1;
    }

    // -----------------------------------------------------
    // Connect branches
    // -----------------------------------------------------

    double momentumGeV = 0.0;

    std::vector<double>* hitEnergy = nullptr;

    tree->SetBranchAddress(
        "primary_momentum_GeV",
        &momentumGeV
    );

    tree->SetBranchAddress(
        "hit_energy_eV",
        &hitEnergy
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
        !treeDetector->GetBranch("gas_photon_energies_eV") ||
        !treeDetector->GetBranch("gas_refractive_index") ||
        !treeDetector->GetBranch("pmt_photon_energies_eV") ||
        !treeDetector->GetBranch("pmt_quantum_efficiency") ||
        
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

    std::vector<double>* gasEnergiesVector = nullptr;
    std::vector<double>* refractiveIndexVector = nullptr;
    std::vector<double>* pmtEnergiesVector = nullptr;
    std::vector<double>* quantumEfficiencyVector = nullptr;

    treeDetector->SetBranchAddress(
        "gas_photon_energies_eV",
        &gasEnergiesVector
    );
    
    treeDetector->SetBranchAddress(
        "gas_refractive_index",
        &refractiveIndexVector
    );

    treeDetector->SetBranchAddress(
        "pmt_photon_energies_eV",
        &pmtEnergiesVector
    );
    
    treeDetector->SetBranchAddress(
        "pmt_quantum_efficiency",
        &quantumEfficiencyVector
    );


    treeGenerator->GetEntry(0);
    treeDetector->GetEntry(0);

    if (
        !gasEnergiesVector ||
        !refractiveIndexVector ||
        !pmtEnergiesVector ||
        !quantumEfficiencyVector
    ) {
        std::cerr
            << "[FAIL] Invalid optical property vectors\n";

        return 1;
    }
    if (
        gasEnergiesVector->size() != refractiveIndexVector->size() ||

        pmtEnergiesVector->size() != quantumEfficiencyVector->size()
    ) {
        std::cerr
            << "[FAIL] Optical property vectors "
            << "have inconsistent sizes\n";

        return 1;
    }
    if (
        gasEnergiesVector->size() < 2 ||
        pmtEnergiesVector->size() < 2
    ) {
        std::cerr
            << "[FAIL] Not enough optical property points\n";

        return 1;
    }

    // -----------------------------------------------------
    // Create histograms
    // -----------------------------------------------------

    const double energyMin = std::max(
        gasEnergiesVector->front(),
        pmtEnergiesVector->front()
    );

    const double energyMax = std::min(
        gasEnergiesVector->back(),
        pmtEnergiesVector->back()
    );

    TH1D* histogram = new TH1D(
        "hPhotonEnergy",

        "Detected photon energy spectrum;"
        "Photon energy [eV];"
        "Detected photons",
        
        energyBins,
        energyMin,
        energyMax
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
            !hitEnergy ||
            hitEnergy->empty()
        ) {
            continue;
        }


        // -------------------------------------------------
        // Fill the spectrum
        // -------------------------------------------------

        for (double energy : *hitEnergy) {
            if (energy >= energyMin &&
                energy <= energyMax) {

                histogram->Fill(energy);
                
                ++validEvents;
            }
        }
    }

        
    // -----------------------------------------------------
    // Draw histogram
    // -----------------------------------------------------

    auto* canvas = new TCanvas(
        "energy_spectrum_canvas",
        "energy_spectrum_mes_vs_predict",
        900,
        700
    );

    histogram->Draw("COLZ");

    // -----------------------------------------------------
    // Draw expected graph
    // -----------------------------------------------------
    
    gRefractiveIndexEnergies = gasEnergiesVector;
    gRefractiveIndex = refractiveIndexVector;

    gQuantumEfficiencyEnergies = pmtEnergiesVector;
    gQuantumEfficiency = quantumEfficiencyVector;

    TF1 predictedSpectrum(
        "predictedSpectrum",
        predictedSpectrumFunction,
        energyMin,
        energyMax,
        1
    );

    predictedSpectrum.SetParName(
        0,
        "Normalization"
    );

    predictedSpectrum.SetParameter(
        0,
        histogram->GetMaximum() / 1e-4
    );

    TFitResultPtr fitResult = histogram->Fit(
        &predictedSpectrum,
        "SI"
    );

    predictedSpectrum.SetLineWidth(2);
    predictedSpectrum.SetLineColor(kRed);
    predictedSpectrum.Draw("L SAME");

    canvas->Modified();
    canvas->Update();

    canvas->SaveAs(
        "output/energy_spectrum.pdf"
    );

    std::cout
        << "Plot saved in: "
        << "output/energy_spectrum.pdf"
        << '\n';

    // -----------------------------------------------------
    // Print summary
    // -----------------------------------------------------

    const double normalization =
        predictedSpectrum.GetParameter(0);

    const double normalizationError =
        predictedSpectrum.GetParError(0);

    const double chi2 =
        fitResult->Chi2();

    const int ndf =
        fitResult->Ndf();

    const double pValue =
        fitResult->Prob();
    
    std::cout
    << "Normalization = "
    << normalization
    << " +/- "
    << normalizationError
    << '\n';

    std::cout
        << "chi2 = "
        << chi2
        << '\n';

    std::cout
        << "ndf = "
        << ndf
        << '\n';

    std::cout
        << "chi2 / ndf = "
        << chi2 / ndf
        << '\n';

    std::cout
        << "p-value = "
        << pValue
        << '\n';

    constexpr double minPValue = 0.01;

    if (pValue < minPValue) {
        std::cerr << "TEST FAILED\n";
        return 1;
    }

    return 0;
}



// ---------------------------------------------------------
// ROOT / CTest entry point
// ---------------------------------------------------------

void test_energy_cherenkov(
    const char* filename,
    int energyBins = 50
)
{
    const int result =
        ValidateCherenkovEnergy(
            filename,
            energyBins
        );

    gSystem->Exit(result);
}