#include "na62rich/io/DetectorConfig.hh"

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

void FillParams(const std::string& filename)
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
        if (key == "worldLength") {
            worldParams->length =
                ParseLength(value, lineNumber);
        }
        else if (key == "worldHeight") {
            worldParams->height =
                ParseLength(value, lineNumber);
        }
        else if (key == "gasLength") {
            gasParams->length =
                ParseLength(value, lineNumber);
        }
        else if (key == "gasRadius") {
            gasParams->radius =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirrorCurvatureRadius") {
            mirrorParams->curvatureRadius =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirrorOuterRadius") {
            mirrorParams->outerRadius =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirrorInnerRadius") {
            mirrorParams->innerRadius =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirrorThickness") {
            mirrorParams->thickness =
                ParseLength(value, lineNumber);
        }
        else if (key == "mirrorPositionZ") {
            mirrorParams->posZ =
                ParseLength(value, lineNumber);
        }
        else if (key == "PMTradius") {
            pmtParams->PMTradius =
                ParseLength(value, lineNumber);
        }
        else if (key == "diskRadius") {
            pmtParams->diskRadius =
                ParseLength(value, lineNumber);
        }
        else if (key == "diskThick") {
            pmtParams->thickness =
                ParseLength(value, lineNumber);
        }
        else if (key == "diskPositionZ") {
            pmtParams->posZ =
                ParseLength(value, lineNumber);
        }
        else if (key == "diskPositionR") {
            pmtParams->posR =
                ParseLength(value, lineNumber);
        }

        // Materials
        else if (key == "worldMaterial") {
            worldParams->material = value;
        }
        else if (key == "gasMaterial") {
            gasParams->material = value;
        }
        else if (key == "mirrorMaterial") {
            mirrorParams->material = value;
        }
        else if (key == "PMTmaterial") {
            pmtParams->material = value;
        }

        // Optical tables
        else if (key == "neonPhotonEnergies") {
            gasParams->photonEnergies =
                ParseEnergyVector(value, lineNumber);
        }
        else if (key == "neonRefractiveIndex") {
            gasParams->refractiveIndex =
                ParseDimensionlessVector(value, lineNumber);
        }
        else if (key == "mirrorPhotonEnergies") {
            mirrorParams->photonEnergies =
                ParseEnergyVector(value, lineNumber);
        }
        else if (key == "mirrorReflectivity") {
            mirrorParams->reflectivity =
                ParseDimensionlessVector(value, lineNumber);
        }
        else if (key == "PMTphotonEnergies") {
            pmtParams->photonEnergies =
                ParseEnergyVector(value, lineNumber);
        }
        else if (key == "PMTefficiency") {
            pmtParams->quantumEfficiency =
                ParseDimensionlessVector(value, lineNumber);
        }
    }

    worldParams->length = gasParams->length + 4.0 * m;
    worldParams->height = 2.0 * gasParams->radius + 4.0 * m;

    mirrorParams->posZ -= mirrorParams->thickness;

    pmtParams->posZ += (pmtParams->thickness) / 2.0;
}

std::string Trim(const std::string& str)
{
    const auto first = str.find_first_not_of(" \t\r\n");

    if (first == std::string::npos) {
        return "";
    }

    const auto last = str.find_last_not_of(" \t\r\n");

    return str.substr(first, last - first + 1);
}


G4double GetLengthUnit(const std::string& unit)
{
    if (unit == "mm") {
        return mm;
    }

    if (unit == "cm") {
        return cm;
    }

    if (unit == "m") {
        return m;
    }

    throw std::runtime_error(
        "Unknown length unit: " + unit
    );
}


G4double GetEnergyUnit(const std::string& unit)
{
    if (unit == "eV") {
        return eV;
    }

    if (unit == "keV") {
        return keV;
    }

    if (unit == "MeV") {
        return MeV;
    }

    if (unit == "GeV") {
        return GeV;
    }

    throw std::runtime_error(
        "Unknown energy unit: " + unit
    );
}


std::vector<G4double> ParseEnergyVector(
    const std::string& text,
    std::size_t lineNumber
)
{
    std::vector<G4double> values;

    std::stringstream stream(text);
    std::string element;

    while (std::getline(stream, element, ',')) {

        element = Trim(element);

        std::istringstream elementStream(element);

        G4double value;
        std::string unit;

        if (!(elementStream >> value >> unit)) {
            throw std::runtime_error(
                "Invalid energy value at line "
                + std::to_string(lineNumber)
                + ": " + element
            );
        }

        values.push_back(
            value * GetEnergyUnit(unit)
        );
    }

    return values;
}


std::vector<G4double> ParseDimensionlessVector(
    const std::string& text,
    std::size_t lineNumber
)
{
    std::vector<G4double> values;

    std::stringstream stream(text);
    std::string element;

    while (std::getline(stream, element, ',')) {

        element = Trim(element);

        try {
            values.push_back(std::stod(element));
        }
        catch (const std::exception&) {
            throw std::runtime_error(
                "Invalid numerical value at line "
                + std::to_string(lineNumber)
                + ": " + element
            );
        }
    }

    return values;
}


G4double ParseLength(
    const std::string& text,
    std::size_t lineNumber
)
{
    std::istringstream stream(text);

    G4double value;
    std::string unit;

    if (!(stream >> value >> unit)) {
        throw std::runtime_error(
            "Invalid length at line "
            + std::to_string(lineNumber)
            + ": " + text
        );
    }

    return value * GetLengthUnit(unit);
}