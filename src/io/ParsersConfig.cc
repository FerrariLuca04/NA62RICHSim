#include "na62rich/io/ParsersConfig.hh"

#include "G4SystemOfUnits.hh"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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


G4double GetAngleUnit(const std::string& unit)
{
    if (unit == "rad") {
        return rad;
    }

    if (unit == "deg") {
        return deg;
    }

    throw std::runtime_error(
        "Unknown angle unit: " + unit
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


G4double ParseAngle(
    const std::string& text,
    std::size_t lineNumber
)
{
    std::istringstream stream(text);

    G4double value;
    std::string unit;

    if (!(stream >> value >> unit)) {
        throw std::runtime_error(
            "Invalid angle at line "
            + std::to_string(lineNumber)
            + ": " + text
        );
    }

    return value * GetAngleUnit(unit);
}