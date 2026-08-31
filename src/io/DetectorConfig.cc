#include "na62rich/io/DetectorConfig.hh"

#include "na62rich/io/ParsersConfig.hh"

#include "TFile.h"
#include "TTree.h"

#include "G4SystemOfUnits.hh"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

WorldParams* worldParams;
GasParams* gasParams;
MirrorParams* mirrorParams;
PMTParams* pmtParams;

void SetDetectorParams(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Cannot open detector configuration file: " + filename
        );
    }

    if (worldParams == nullptr) {
        worldParams = new WorldParams{};
    }

    if (gasParams == nullptr) {
        gasParams = new GasParams{};
    }

    if (mirrorParams == nullptr) {
        mirrorParams = new MirrorParams{};
    }

    if (pmtParams == nullptr) {
        pmtParams = new PMTParams{};
    }


    std::string line;
    std::size_t lineNumber = 0;

    while (std::getline(file, line)) {

        ++lineNumber;

        // Remove comments
        const auto commentPosition = line.find('#');

        if (commentPosition != std::string::npos) {
            line.erase(commentPosition);
        }

        line = Trim(line);

        // Ignore empty lines
        if (line.empty()) {
            continue;
        }


        // Find '='
        const auto equalPosition = line.find('=');

        if (equalPosition == std::string::npos) {
            throw std::runtime_error(
                "Missing '=' at line "
                + std::to_string(lineNumber)
            );
        }


        const std::string key =
            Trim(line.substr(0, equalPosition));

        const std::string value =
            Trim(line.substr(equalPosition + 1));


        // Geometry
        if (key == "gas_length") {
            gasParams->length =
                ParseLength(value, lineNumber);
        }
        else if (key == "gas_radius") {
            gasParams->radius =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirror_curvature_radius") {
            mirrorParams->curvatureRadius =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirror_outer_radius") {
            mirrorParams->outerRadius =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirror_inner_radius") {
            mirrorParams->innerRadius =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirror_thickness") {
            mirrorParams->thickness =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirror_position_z") {
            mirrorParams->posZ =
                ParseLength(value, lineNumber);
        }
        else if (key == "PMT_radius") {
            pmtParams->PMTradius =
                ParseLength(value, lineNumber);
        }
        else if (key == "disk_radius") {
            pmtParams->diskRadius =
                ParseLength(value, lineNumber);
        }
        else if (key == "disk_thick") {
            pmtParams->thickness =
                ParseLength(value, lineNumber);
        }
        else if (key == "disk_position_z") {
            pmtParams->posZ =
                ParseLength(value, lineNumber);
        }
        else if (key == "disk_position_r") {
            pmtParams->posR =
                ParseLength(value, lineNumber);
        }

        // Materials
        else if (key == "world_material") {
            worldParams->material = value;
        }
        else if (key == "gas_material") {
            gasParams->material = value;
        }
        else if (key == "mirror_material") {
            mirrorParams->material = value;
        }
        else if (key == "PMT_material") {
            pmtParams->material = value;
        }

        // Optical tables
        else if (key == "gas_photon_energies") {
            gasParams->photonEnergies =
                ParseEnergyVector(value, lineNumber);
        }
        else if (key == "gas_refractive_index") {
            gasParams->refractiveIndex =
                ParseDimensionlessVector(value, lineNumber);
        }
        else if (key == "mirror_photon_energies") {
            mirrorParams->photonEnergies =
                ParseEnergyVector(value, lineNumber);
        }
        else if (key == "mirror_reflectivity") {
            mirrorParams->reflectivity =
                ParseDimensionlessVector(value, lineNumber);
        }
        else if (key == "PMT_photon_energies") {
            pmtParams->photonEnergies =
                ParseEnergyVector(value, lineNumber);
        }
        else if (key == "PMT_efficiency") {
            pmtParams->quantumEfficiency =
                ParseDimensionlessVector(value, lineNumber);
        }
    }

    worldParams->length = gasParams->length + 4.0 * m;
    worldParams->height = 2.0 * gasParams->radius + 4.0 * m;

    mirrorParams->posZ -= mirrorParams->thickness;

    pmtParams->posZ += (pmtParams->thickness) / 2.0;
}

void WriteDetectorConfig(TFile& file)
{
    file.cd();

    TTree tree(
        "DetectorConfig",
        "Detector configuration"
    );

    // -----------------------------------------------------
    // World
    // -----------------------------------------------------

    double worldLengthM =
        worldParams->length / m;

    double worldHeightM =
        worldParams->height / m;

    std::string worldMaterial =
        worldParams->material;


    // -----------------------------------------------------
    // Gas
    // -----------------------------------------------------

    double gasLengthM =
        gasParams->length / m;

    double gasRadiusM =
        gasParams->radius / m;

    std::string gasMaterial =
        gasParams->material;

    std::vector<double> gasPhotonEnergiesEV;
    gasPhotonEnergiesEV.reserve(
        gasParams->photonEnergies.size()
    );

    for (const auto energy : gasParams->photonEnergies) {
        gasPhotonEnergiesEV.push_back(
            energy / eV
        );
    }

    std::vector<double> gasRefractiveIndex(
        gasParams->refractiveIndex.begin(),
        gasParams->refractiveIndex.end()
    );


    // -----------------------------------------------------
    // Mirror
    // -----------------------------------------------------

    double mirrorCurvatureRadiusM =
        mirrorParams->curvatureRadius / m;

    double mirrorOuterRadiusMm =
        mirrorParams->outerRadius / mm;

    double mirrorInnerRadiusMm =
        mirrorParams->innerRadius / mm;

    double mirrorThicknessMm =
        mirrorParams->thickness / mm;

    double mirrorPositionZM =
        mirrorParams->posZ / m;

    std::string mirrorMaterial =
        mirrorParams->material;

    std::vector<double> mirrorPhotonEnergiesEV;
    mirrorPhotonEnergiesEV.reserve(
        mirrorParams->photonEnergies.size()
    );

    for (const auto energy : mirrorParams->photonEnergies) {
        mirrorPhotonEnergiesEV.push_back(
            energy / eV
        );
    }

    std::vector<double> mirrorReflectivity(
        mirrorParams->reflectivity.begin(),
        mirrorParams->reflectivity.end()
    );


    // -----------------------------------------------------
    // PMT
    // -----------------------------------------------------

    double pmtRadiusMm =
        pmtParams->PMTradius / mm;

    double pmtDiskRadiusMm =
        pmtParams->diskRadius / mm;

    double pmtThicknessMm =
        pmtParams->thickness / mm;

    double pmtPositionRMm =
        pmtParams->posR / mm;

    double pmtPositionZM =
        pmtParams->posZ / m;

    std::string pmtMaterial =
        pmtParams->material;

    std::vector<double> pmtPhotonEnergiesEV;
    pmtPhotonEnergiesEV.reserve(
        pmtParams->photonEnergies.size()
    );

    for (const auto energy : pmtParams->photonEnergies) {
        pmtPhotonEnergiesEV.push_back(
            energy / eV
        );
    }

    std::vector<double> pmtQuantumEfficiency(
        pmtParams->quantumEfficiency.begin(),
        pmtParams->quantumEfficiency.end()
    );


    // -----------------------------------------------------
    // World branches
    // -----------------------------------------------------

    tree.Branch(
        "world_length_m",
        &worldLengthM
    );

    tree.Branch(
        "world_height_m",
        &worldHeightM
    );

    tree.Branch(
        "world_material",
        &worldMaterial
    );


    // -----------------------------------------------------
    // Gas branches
    // -----------------------------------------------------

    tree.Branch(
        "gas_length_m",
        &gasLengthM
    );

    tree.Branch(
        "gas_radius_m",
        &gasRadiusM
    );

    tree.Branch(
        "gas_material",
        &gasMaterial
    );

    tree.Branch(
        "gas_photon_energies_eV",
        &gasPhotonEnergiesEV
    );

    tree.Branch(
        "gas_refractive_index",
        &gasRefractiveIndex
    );


    // -----------------------------------------------------
    // Mirror branches
    // -----------------------------------------------------

    tree.Branch(
        "mirror_curvature_radius_m",
        &mirrorCurvatureRadiusM
    );

    tree.Branch(
        "mirror_outer_radius_mm",
        &mirrorOuterRadiusMm
    );

    tree.Branch(
        "mirror_inner_radius_mm",
        &mirrorInnerRadiusMm
    );

    tree.Branch(
        "mirror_thickness_mm",
        &mirrorThicknessMm
    );

    tree.Branch(
        "mirror_position_z_m",
        &mirrorPositionZM
    );

    tree.Branch(
        "mirror_material",
        &mirrorMaterial
    );

    tree.Branch(
        "mirror_photon_energies_eV",
        &mirrorPhotonEnergiesEV
    );

    tree.Branch(
        "mirror_reflectivity",
        &mirrorReflectivity
    );


    // -----------------------------------------------------
    // PMT branches
    // -----------------------------------------------------

    tree.Branch(
        "pmt_radius_mm",
        &pmtRadiusMm
    );

    tree.Branch(
        "pmt_disk_radius_mm",
        &pmtDiskRadiusMm
    );

    tree.Branch(
        "pmt_thickness_mm",
        &pmtThicknessMm
    );

    tree.Branch(
        "pmt_position_r_mm",
        &pmtPositionRMm
    );

    tree.Branch(
        "pmt_position_z_m",
        &pmtPositionZM
    );

    tree.Branch(
        "pmt_material",
        &pmtMaterial
    );

    tree.Branch(
        "pmt_photon_energies_eV",
        &pmtPhotonEnergiesEV
    );

    tree.Branch(
        "pmt_quantum_efficiency",
        &pmtQuantumEfficiency
    );


    // -----------------------------------------------------
    // Single configuration entry
    // -----------------------------------------------------

    tree.Fill();
    tree.Write();
}