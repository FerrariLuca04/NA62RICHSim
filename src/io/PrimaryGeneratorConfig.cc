#include "na62rich/io/PrimaryGeneratorConfig.hh"

#include "na62rich/io/ParsersConfig.hh"

#include "G4SystemOfUnits.hh"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

EntranceParams* entranceParams;
DecayRegionParams* decayRegionParams;
ParticleParams* particleParams;

void SetGeneratorParams(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Cannot open primary generator configuration file: " + filename
        );
    }

    if (entranceParams == nullptr) {
        entranceParams = new EntranceParams{};
    }

    if (decayRegionParams == nullptr) {
        decayRegionParams = new DecayRegionParams{};
    }

    if (particleParams == nullptr) {
        particleParams = new ParticleParams{};
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

        //Entrance
        if (key == "entrance_radius_min") {
            entranceParams->rMin =
                ParseLength(value, lineNumber);
        }
        else if (key == "entrance_radius_max") {
            entranceParams->rMax =
                ParseLength(value, lineNumber);
        }
        else if (key == "entrance_phi_min") {
            entranceParams->phiMin =
                ParseAngle(value, lineNumber);
        }
        else if (key == "entrance_phi_max") {
            entranceParams->phiMax =
                ParseAngle(value, lineNumber);
        }
        //Decay region
        else if (key == "decay_region_start") {
            decayRegionParams->start =
                ParseLength(value, lineNumber);
        }
        else if (key == "decay_region_length") {
            decayRegionParams->length =
                ParseLength(value, lineNumber);
        }
        else if (key == "decay_region_sigma_x") {
            decayRegionParams->sigmaX =
                ParseLength(value, lineNumber);
        }
        else if (key == "decay_region_sigma_y") {
            decayRegionParams->sigmaY =
                ParseLength(value, lineNumber);
        }
        //Particle
        else if (key == "particle_momentum_min") {
            particleParams->pMin =
                ParseEnergy(value, lineNumber);
        }
        else if (key == "particle_momentum_max") {
            particleParams->pMax =
                ParseEnergy(value, lineNumber);
        }
        else if (key == "particle_type") {
            particleParams->type = value;
        }
    }
}