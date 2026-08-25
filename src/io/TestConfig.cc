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

    CheckFieldSet(world.length, "world_length", errors);
    CheckFieldSet(world.height, "world_height", errors);

    CheckFieldSet(gas.length, "gas_length", errors);
    CheckFieldSet(gas.radius, "gas_radius", errors);

    CheckFieldSet(
        mirror.curvatureRadius,
        "mirror_curvature_radius",
        errors
    );

    CheckFieldSet(
        mirror.outerRadius,
        "mirror_outer_radius",
        errors
    );

    CheckFieldSet(
        mirror.innerRadius,
        "mirror_inner_radius",
        errors
    );

    CheckFieldSet(
        mirror.thickness,
        "mirror_thickness",
        errors
    );

    CheckFieldSet(
        mirror.posZ,
        "mirror_position_z",
        errors
    );

    CheckFieldSet(
        pmt.PMTradius,
        "PMT_radius",
        errors
    );

    CheckFieldSet(
        pmt.diskRadius,
        "disk_radius",
        errors
    );

    CheckFieldSet(
        pmt.thickness,
        "disk_thick",
        errors
    );

    CheckFieldSet(
        pmt.posR,
        "disk_position_z",
        errors
    );

    CheckFieldSet(
        pmt.posZ,
        "disk_position_r",
        errors
    );


    // ---------------------------------------------------------
    // Materials
    // ---------------------------------------------------------

    CheckStringNotEmpty(
        world.material,
        "world_material",
        errors
    );

    CheckStringNotEmpty(
        gas.material,
        "gas_material",
        errors
    );

    CheckStringNotEmpty(
        mirror.material,
        "mirror_material",
        errors
    );

    CheckStringNotEmpty(
        pmt.material,
        "PMT_material",
        errors
    );


    // ---------------------------------------------------------
    // Basic geometrical validity
    // ---------------------------------------------------------

    CheckPositive(world.length, "world_length", errors);
    CheckPositive(world.height, "world_height", errors);

    CheckPositive(gas.length, "gas_length", errors);
    CheckPositive(gas.radius, "gas_radius", errors);

    CheckPositive(
        mirror.curvatureRadius,
        "mirror_curvature_radius",
        errors
    );

    CheckPositive(
        mirror.outerRadius,
        "mirror.outerRadius",
        errors
    );

    CheckNonNegative(
        mirror.innerRadius,
        "mirror_inner_radius",
        errors
    );

    CheckPositive(
        mirror.thickness,
        "mirror_thickness",
        errors
    );

    CheckPositive(
        pmt.PMTradius,
        "PMT_radius",
        errors
    );

    CheckPositive(
        pmt.diskRadius,
        "disk_radius",
        errors
    );

    CheckPositive(
        pmt.thickness,
        "disk_thick",
        errors
    );

    CheckNonNegative(
        pmt.posR,
        "disk_position_r",
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
            "disk_position_r + disk_radius > gas_radius."
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
            "disk_position_r - disk_radius < 0."
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
        "gas_photon_energies",
        errors
    );

    CheckVectorNotEmpty(
        gas.refractiveIndex,
        "gas_refractive_index",
        errors
    );

    CheckVectorNotEmpty(
        mirror.photonEnergies,
        "mirror_photon_energies",
        errors
    );

    CheckVectorNotEmpty(
        mirror.reflectivity,
        "mirror_reflectivity",
        errors
    );

    CheckVectorNotEmpty(
        pmt.photonEnergies,
        "PMT_photon_energies",
        errors
    );

    CheckVectorNotEmpty(
        pmt.quantumEfficiency,
        "PMT_efficiency",
        errors
    );


    // ---------------------------------------------------------
    // Check vector sizes
    // ---------------------------------------------------------

    CheckSameSize(
        gas.photonEnergies,
        gas.refractiveIndex,
        "gas_photon_energies",
        "gas_refractive_index",
        errors
    );

    CheckSameSize(
        mirror.photonEnergies,
        mirror.reflectivity,
        "mirror_photon_energies",
        "mirror_reflectivity",
        errors
    );

    CheckSameSize(
        pmt.photonEnergies,
        pmt.quantumEfficiency,
        "PMT_photon_energies",
        "PMT_efficiency",
        errors
    );


    // ---------------------------------------------------------
    // Optical properties
    // ---------------------------------------------------------

    CheckEnergyVector(
        gas.photonEnergies,
        "gas_photon_energies",
        errors
    );

    CheckEnergyVector(
        mirror.photonEnergies,
        "mirror_photon_energies",
        errors
    );

    CheckEnergyVector(
        pmt.photonEnergies,
        "PMT_photon_energies",
        errors
    );


    CheckRefractiveIndex(
        gas.refractiveIndex,
        "gas_refractive_index",
        errors
    );

    CheckProbabilityVector(
        mirror.reflectivity,
        "mirror_reflectivity",
        errors
    );

    CheckProbabilityVector(
        pmt.quantumEfficiency,
        "PMT_efficiency",
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
        "entrance_radius_min",
        errors
    );

    CheckFieldSet(
        entrance.rMax,
        "entrance_radius_max",
        errors
    );

    CheckFieldSet(
        entrance.phiMin,
        "entrance_phi_min",
        errors
    );

    CheckFieldSet(
        entrance.phiMax,
        "entrance_phi_max",
        errors
    );

    CheckFieldSet(
        decayRegion.start,
        "decay_region_start",
        errors
    );

    CheckFieldSet(
        decayRegion.length,
        "decay_region_length",
        errors
    );

    CheckFieldSet(
        decayRegion.sigmaX,
        "decay_region_sigma_x",
        errors
    );

    CheckFieldSet(
        decayRegion.sigmaY,
        "decay_region_sigma_y",
        errors
    );

    CheckFieldSet(
        particle.pMin,
        "particle_momentum_min",
        errors
    );

    CheckFieldSet(
        particle.pMax,
        "particle_momentum_max",
        errors
    );

    CheckStringNotEmpty(
        particle.type,
        "particle_type",
        errors
    );


    // ---------------------------------------------------------
    // Basic validity
    // ---------------------------------------------------------

    CheckNonNegative(
        entrance.rMin,
        "entrance_radius_min",
        errors
    );

    CheckPositive(
        entrance.rMax,
        "entrance_radius_max",
        errors
    );

    CheckNonNegative(
        entrance.phiMin,
        "entrance_phi_min",
        errors
    );

    CheckPositive(
        entrance.phiMax,
        "entrance_phi_max",
        errors
    );

    CheckPositive(
        decayRegion.start,
        "decay_region_start",
        errors
    );

    CheckPositive(
        decayRegion.length,
        "decay_region_length",
        errors
    );

    CheckNonNegative(
        decayRegion.sigmaX,
        "decay_region_sigma_x",
        errors
    );

    CheckNonNegative(
        decayRegion.sigmaY,
        "decay_region_sigma_y",
        errors
    );

    CheckNonNegative(
        particle.pMin,
        "particle_momentum_min",
        errors
    );

    CheckPositive(
        particle.pMax,
        "particle_momentum_max",
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