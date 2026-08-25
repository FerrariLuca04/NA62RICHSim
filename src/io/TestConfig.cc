#include "na62rich/io/TestConfig.hh"

#include "na62rich/io/DetectorConfig.hh"
#include "na62rich/io/PrimaryGeneratorConfig.hh"

#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


void AddError(
    std::vector<std::string>& errors,
    const std::string& message
)
{
    errors.push_back(message);
}

void CheckFieldSet(
    G4double value,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    if (!std::isfinite(value)) {
        AddError(
            errors,
            "Parameter '" + name + "' is not set."
        );
    }
}


void CheckPositive(
    G4double value,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    if (std::isfinite(value) && value <= 0.0) {
        AddError(
            errors,
            "Parameter '" + name + "' must be greater than zero."
        );
    }
}


void CheckNonNegative(
    G4double value,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    if (std::isfinite(value) && value < 0.0) {
        AddError(
            errors,
            "Parameter '" + name + "' cannot be negative."
        );
    }
}


void CheckStringNotEmpty(
    const std::string& value,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    if (value.empty()) {
        AddError(
            errors,
            "Parameter '" + name + "' is empty."
        );
    }
}


void CheckVectorNotEmpty(
    const std::vector<G4double>& values,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    if (values.empty()) {
        AddError(
            errors,
            "Vector '" + name + "' is empty."
        );
    }
}

void CheckSameSize(
    const std::vector<G4double>& first,
    const std::vector<G4double>& second,
    const std::string& firstName,
    const std::string& secondName,
    std::vector<std::string>& errors
)
{
    if (first.size() != second.size()) {
        AddError(
            errors,
            "Vectors '" + firstName
            + "' and '" + secondName
            + "' have different sizes: "
            + std::to_string(first.size())
            + " != "
            + std::to_string(second.size())
        );
    }
}

void CheckIncreasing(
    const std::vector<G4double>& values,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    for (std::size_t i = 1; i < values.size(); ++i) {

        if (values[i] <= values[i - 1]) {
            AddError(
                errors,
                "Vector '" + name
                + "' is not strictly increasing at indices "
                + std::to_string(i - 1)
                + " and "
                + std::to_string(i)
                + "."
            );

            return;
        }
    }
}

void CheckProbabilityVector(
    const std::vector<G4double>& values,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    for (std::size_t i = 0; i < values.size(); ++i) {

        if (!std::isfinite(values[i])) {
            AddError(
                errors,
                "Vector '" + name
                + "' contains an invalid value at index "
                + std::to_string(i)
                + "."
            );

            continue;
        }

        if (values[i] < 0.0 || values[i] > 1.0) {
            AddError(
                errors,
                "Value of '" + name
                + "' at index "
                + std::to_string(i)
                + " is outside [0, 1]."
            );
        }
    }
}

void CheckRefractiveIndex(
    const std::vector<G4double>& values,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    for (std::size_t i = 0; i < values.size(); ++i) {

        if (!std::isfinite(values[i])) {
            AddError(
                errors,
                "Vector '" + name
                + "' contains an invalid value at index "
                + std::to_string(i)
                + "."
            );

            continue;
        }

        if (values[i] < 1.0) {
            AddError(
                errors,
                "Refractive index at index "
                + std::to_string(i)
                + " is smaller than 1."
            );
        }
    }
}

void CheckEnergyVector(
    const std::vector<G4double>& energies,
    const std::string& name,
    std::vector<std::string>& errors
)
{
    for (std::size_t i = 0; i < energies.size(); ++i) {

        if (
            !std::isfinite(energies[i])
            || energies[i] <= 0.0
        ) {
            AddError(
                errors,
                "Invalid energy in '" + name
                + "' at index "
                + std::to_string(i)
                + "."
            );
        }
    }

    CheckIncreasing(
        energies,
        name,
        errors
    );
}

void TestDetectorConfig(
    const WorldParams& world,
    const GasParams& gas,
    const MirrorParams& mirror,
    const PMTParams& pmt
)
{
    std::vector<std::string> errors;


    // ---------------------------------------------------------
    // Check that all scalar parameters are set
    // ---------------------------------------------------------

    CheckFieldSet(world.length, "world.length", errors);
    CheckFieldSet(world.height, "world.height", errors);

    CheckFieldSet(gas.length, "gas.length", errors);
    CheckFieldSet(gas.radius, "gas.radius", errors);

    CheckFieldSet(
        mirror.curvatureRadius,
        "mirror.curvatureRadius",
        errors
    );

    CheckFieldSet(
        mirror.outerRadius,
        "mirror.outerRadius",
        errors
    );

    CheckFieldSet(
        mirror.innerRadius,
        "mirror.innerRadius",
        errors
    );

    CheckFieldSet(
        mirror.thickness,
        "mirror.thickness",
        errors
    );

    CheckFieldSet(
        mirror.posZ,
        "mirror.posZ",
        errors
    );

    CheckFieldSet(
        pmt.PMTradius,
        "pmt.PMTradius",
        errors
    );

    CheckFieldSet(
        pmt.diskRadius,
        "pmt.diskRadius",
        errors
    );

    CheckFieldSet(
        pmt.thickness,
        "pmt.thickness",
        errors
    );

    CheckFieldSet(
        pmt.posR,
        "pmt.posR",
        errors
    );

    CheckFieldSet(
        pmt.posZ,
        "pmt.posZ",
        errors
    );


    // ---------------------------------------------------------
    // Materials
    // ---------------------------------------------------------

    CheckStringNotEmpty(
        world.material,
        "world.material",
        errors
    );

    CheckStringNotEmpty(
        gas.material,
        "gas.material",
        errors
    );

    CheckStringNotEmpty(
        mirror.material,
        "mirror.material",
        errors
    );

    CheckStringNotEmpty(
        pmt.material,
        "pmt.material",
        errors
    );


    // ---------------------------------------------------------
    // Basic geometrical validity
    // ---------------------------------------------------------

    CheckPositive(world.length, "world.length", errors);
    CheckPositive(world.height, "world.height", errors);

    CheckPositive(gas.length, "gas.length", errors);
    CheckPositive(gas.radius, "gas.radius", errors);

    CheckPositive(
        mirror.curvatureRadius,
        "mirror.curvatureRadius",
        errors
    );

    CheckPositive(
        mirror.outerRadius,
        "mirror.outerRadius",
        errors
    );

    CheckNonNegative(
        mirror.innerRadius,
        "mirror.innerRadius",
        errors
    );

    CheckPositive(
        mirror.thickness,
        "mirror.thickness",
        errors
    );

    CheckPositive(
        pmt.PMTradius,
        "pmt.PMTradius",
        errors
    );

    CheckPositive(
        pmt.diskRadius,
        "pmt.diskRadius",
        errors
    );

    CheckPositive(
        pmt.thickness,
        "pmt.thickness",
        errors
    );

    CheckNonNegative(
        pmt.posR,
        "pmt.posR",
        errors
    );


    // ---------------------------------------------------------
    // Detector geometrical constraints
    // ---------------------------------------------------------

    if (
        std::isfinite(mirror.outerRadius)
        && std::isfinite(gas.radius)
        && mirror.outerRadius > gas.radius
    ) {
        AddError(
            errors,
            "Mirror outer radius is larger than gas radius."
        );
    }


    if (
        std::isfinite(pmt.posR)
        && std::isfinite(pmt.diskRadius)
        && std::isfinite(gas.radius)
        && pmt.posR + pmt.diskRadius > gas.radius
    ) {
        AddError(
            errors,
            "PMT disk exceeds the outer gas radius: "
            "pmt.posR + pmt.diskRadius > gas.radius."
        );
    }


    if (
        std::isfinite(pmt.posR)
        && std::isfinite(pmt.diskRadius)
        && pmt.posR - pmt.diskRadius < 0.0
    ) {
        AddError(
            errors,
            "PMT disk crosses the detector axis: "
            "pmt.posR - pmt.diskRadius < 0."
        );
    }


    if (
        std::isfinite(mirror.posZ)
        && std::isfinite(gas.length)
        && std::abs(mirror.posZ) > gas.length / 2.0
    ) {
        AddError(
            errors,
            "Mirror Z position lies outside the gas volume."
        );
    }


    if (
        std::isfinite(pmt.posZ)
        && std::isfinite(gas.length)
        && std::abs(pmt.posZ) > gas.length / 2.0
    ) {
        AddError(
            errors,
            "PMT Z position lies outside the gas volume."
        );
    }


    if (
        std::isfinite(pmt.PMTradius)
        && std::isfinite(pmt.diskRadius)
        && pmt.PMTradius > pmt.diskRadius
    ) {
        AddError(
            errors,
            "PMT radius is larger than disk radius."
        );
    }


    // ---------------------------------------------------------
    // Additional mirror checks
    // ---------------------------------------------------------

    if (
        std::isfinite(mirror.innerRadius)
        && std::isfinite(mirror.outerRadius)
        && mirror.innerRadius > mirror.outerRadius
    ) {
        AddError(
            errors,
            "Mirror inner radius is larger than outer radius."
        );
    }


    if (
        std::isfinite(mirror.thickness)
        && std::isfinite(mirror.curvatureRadius)
        && mirror.thickness >= mirror.curvatureRadius
    ) {
        AddError(
            errors,
            "Mirror thickness must be smaller than "
            "its curvature radius."
        );
    }


    // ---------------------------------------------------------
    // Check vectors are not empty
    // ---------------------------------------------------------

    CheckVectorNotEmpty(
        gas.photonEnergies,
        "gas.photonEnergies",
        errors
    );

    CheckVectorNotEmpty(
        gas.refractiveIndex,
        "gas.refractiveIndex",
        errors
    );

    CheckVectorNotEmpty(
        mirror.photonEnergies,
        "mirror.photonEnergies",
        errors
    );

    CheckVectorNotEmpty(
        mirror.reflectivity,
        "mirror.reflectivity",
        errors
    );

    CheckVectorNotEmpty(
        pmt.photonEnergies,
        "pmt.photonEnergies",
        errors
    );

    CheckVectorNotEmpty(
        pmt.quantumEfficiency,
        "pmt.quantumEfficiency",
        errors
    );


    // ---------------------------------------------------------
    // Check vector sizes
    // ---------------------------------------------------------

    CheckSameSize(
        gas.photonEnergies,
        gas.refractiveIndex,
        "gas.photonEnergies",
        "gas.refractiveIndex",
        errors
    );

    CheckSameSize(
        mirror.photonEnergies,
        mirror.reflectivity,
        "mirror.photonEnergies",
        "mirror.reflectivity",
        errors
    );

    CheckSameSize(
        pmt.photonEnergies,
        pmt.quantumEfficiency,
        "pmt.photonEnergies",
        "pmt.quantumEfficiency",
        errors
    );


    // ---------------------------------------------------------
    // Optical properties
    // ---------------------------------------------------------

    CheckEnergyVector(
        gas.photonEnergies,
        "gas.photonEnergies",
        errors
    );

    CheckEnergyVector(
        mirror.photonEnergies,
        "mirror.photonEnergies",
        errors
    );

    CheckEnergyVector(
        pmt.photonEnergies,
        "pmt.photonEnergies",
        errors
    );


    CheckRefractiveIndex(
        gas.refractiveIndex,
        "gas.refractiveIndex",
        errors
    );

    CheckProbabilityVector(
        mirror.reflectivity,
        "mirror.reflectivity",
        errors
    );

    CheckProbabilityVector(
        pmt.quantumEfficiency,
        "pmt.quantumEfficiency",
        errors
    );


    // ---------------------------------------------------------
    // Final result
    // ---------------------------------------------------------

    if (!errors.empty()) {

        std::ostringstream message;

        message
            << "Invalid detector configuration. "
            << errors.size()
            << " error(s) found:\n";

        for (const auto& error : errors) {
            message << "  - " << error << '\n';
        }

        throw std::runtime_error(message.str());
    }
}


void TestPrimaryGeneratorConfig(
    const EntranceParams& entrance,
    const DecayRegionParams& decayRegion,
    const ParticleParams& particle
)
{
    std::vector<std::string> errors;


    // ---------------------------------------------------------
    // Check that all scalar parameters are set
    // ---------------------------------------------------------

    CheckFieldSet(
        entrance.rMin,
        "entrance.rMin",
        errors
    );

    CheckFieldSet(
        entrance.rMax,
        "entrance.rMax",
        errors
    );

    CheckFieldSet(
        entrance.phiMin,
        "entrance.phiMin",
        errors
    );

    CheckFieldSet(
        entrance.phiMax,
        "entrance.phiMax",
        errors
    );

    CheckFieldSet(
        decayRegion.start,
        "decayRegion.start",
        errors
    );

    CheckFieldSet(
        decayRegion.length,
        "decayRegion.length",
        errors
    );

    CheckFieldSet(
        decayRegion.sigmaX,
        "decayRegion.sigmaX",
        errors
    );

    CheckFieldSet(
        decayRegion.sigmaY,
        "decayRegion.sigmaY",
        errors
    );

    CheckStringNotEmpty(
        particle.type,
        "particle.type",
        errors
    );


    // ---------------------------------------------------------
    // Basic validity
    // ---------------------------------------------------------

    CheckNonNegative(
        entrance.rMin,
        "entrance.rMin",
        errors
    );

    CheckPositive(
        entrance.rMax,
        "entrance.rMax",
        errors
    );

    CheckPositive(
        decayRegion.start,
        "decayRegion.start",
        errors
    );

    CheckPositive(
        decayRegion.length,
        "decayRegion.length",
        errors
    );

    CheckNonNegative(
        decayRegion.sigmaX,
        "decayRegion.sigmaX",
        errors
    );

    CheckNonNegative(
        decayRegion.sigmaY,
        "decayRegion.sigmaY",
        errors
    );

    CheckNonNegative(
        particle.pMin,
        "particleParams.EMin",
        errors
    );

    CheckPositive(
        particle.pMax,
        "particleParams.EMax",
        errors
    );


    // ---------------------------------------------------------
    // Entrance geometrical constraints
    // ---------------------------------------------------------

    if (
        std::isfinite(entrance.rMin)
        && std::isfinite(entrance.rMax)
        && entrance.rMin >= entrance.rMax
    ) {
        AddError(
            errors,
            "Entrance minimum radius must be smaller "
            "than entrance maximum radius."
        );
    }


    if (
        std::isfinite(entrance.phiMin)
        && std::isfinite(entrance.phiMax)
        && entrance.phiMin >= entrance.phiMax
    ) {
        AddError(
            errors,
            "Entrance minimum phi must be smaller "
            "than entrance maximum phi."
        );
    }


    if (
        std::isfinite(entrance.phiMin)
        && std::isfinite(entrance.phiMax)
        && (entrance.phiMin >= 360 * deg || entrance.phiMax > 360 * deg)
    ) {
        AddError(
            errors,
            "Entrance minimum and maximum phi "
            "must be smaller than 360 deg."
        );
    }


    // ---------------------------------------------------------
    // Decay region constraints
    // ---------------------------------------------------------

    if (
        std::isfinite(decayRegion.start)
        && std::isfinite(decayRegion.length)
        && decayRegion.start <= decayRegion.length
    ) {
        AddError(
            errors,
            "Decay region start must be greater "
            "than decay region length."
        );
    }


    // ---------------------------------------------------------
    // Momentum constraint
    // ---------------------------------------------------------

    if (
        std::isfinite(particle.pMin)
        && std::isfinite(particle.pMax)
        && particle.pMin >= particle.pMax
    ) {
        AddError(
            errors,
            "Minimum momoentum must be smaller "
            "than maximum momentum."
        );
    }
    

    // ---------------------------------------------------------
    // Final result
    // ---------------------------------------------------------

    if (!errors.empty()) {

        std::ostringstream message;

        message
            << "Invalid primary generator configuration. "
            << errors.size()
            << " error(s) found:\n";

        for (const auto& error : errors) {
            message << "  - " << error << '\n';
        }

        throw std::runtime_error(message.str());
    }
}