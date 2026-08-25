#ifndef NA62RICH_PARSER_CONFIG_HH
#define NA62RICH_PARSER_CONFIG_HH

#include "globals.hh"

#include <string>
#include <vector>

std::string Trim(const std::string& str);

G4double GetLengthUnit(const std::string& unit);

G4double GetEnergyUnit(const std::string& unit);

G4double GetAngleUnit(const std::string& unit);

std::vector<G4double> ParseEnergyVector(
    const std::string& text,
    std::size_t lineNumber
);

std::vector<G4double> ParseDimensionlessVector(
    const std::string& text,
    std::size_t lineNumber
);

G4double ParseLength(
    const std::string& text,
    std::size_t lineNumber
);

G4double ParseAngle(
    const std::string& text,
    std::size_t lineNumber
);

#endif