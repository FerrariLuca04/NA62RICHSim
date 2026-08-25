#include "na62rich/io/DetectorConfig.hh"

#include "na62rich/io/ParsersConfig.hh"

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
        if (key == "ga_length") {
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