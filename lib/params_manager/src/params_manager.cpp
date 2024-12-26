#include "params_manager.hpp"
#include <iostream>

// Constructor that loads the YAML file
params_manager::params_manager() {

    try {
        config_ = YAML::LoadFile(filePath);  // Load the YAML file
    } catch (const std::exception& e) {
        std::cerr << "Error loading YAML file: " << e.what() << std::endl;
    }
}

// Method to get the configuration as a map
std::map<std::string, float> params_manager::getConfigAsMap()  {
    std::map<std::string, float> configMap;
    try {
        config_ = YAML::LoadFile(filePath);  // Load the YAML file
    } catch (const std::exception& e) {
        std::cerr << "Error loading YAML file: " << e.what() << std::endl;
    }

    // Extract drone parameters
  if (config_["drone"]) {

    auto droneNode = config_["drone"];
        try {
            if (droneNode["mass"] && droneNode["mass"].IsScalar() && droneNode["mass"].as<float>() >0 ) {
                
                configMap["mass"] = droneNode["mass"].as<float>();
            }
            else {   std::cerr << "Invalid value for 'mass': " << droneNode["mass"] << std::endl;}

        } catch (const YAML::TypedBadConversion<float>& e) {
            std::cerr << "Invalid value for 'mass': " << e.what() << std::endl;
        }

    try {
        if (droneNode["viscous_coefficient"] && droneNode["viscous_coefficient"].IsScalar()&& droneNode["viscous_coefficient"].as<float>() >=0) {
            configMap["viscous_coefficient"] = droneNode["viscous_coefficient"].as<float>();
        }
    } catch (const YAML::TypedBadConversion<float>& e) {
        std::cerr << "Invalid value for 'viscous_coefficient': " << e.what() << std::endl;
    }

    try {
        if (droneNode["max_acceleration"] && droneNode["max_acceleration"].IsScalar()) {
            configMap["max_acceleration"] = droneNode["max_acceleration"].as<float>();
        }
    } catch (const YAML::TypedBadConversion<float>& e) {
        std::cerr << "Invalid value for 'max_acceleration': " << e.what() << std::endl;
    }

    try {
        if (droneNode["max_velocity"] && droneNode["max_velocity"].IsScalar()) {
            configMap["max_velocity"] = droneNode["max_velocity"].as<float>();
        }
    } catch (const YAML::TypedBadConversion<float>& e) {
        std::cerr << "Invalid value for 'max_velocity': " << e.what() << std::endl;
    }

    try {
        if (droneNode["force_step"] && droneNode["force_step"].IsScalar()) {
            configMap["force_step"] = droneNode["force_step"].as<float>();
            
        }
    } catch (const YAML::TypedBadConversion<float>& e) {
        std::cerr << "Invalid value for 'force_step': " << e.what() << std::endl;
    }

    try {
        if (droneNode["repulsion_distance"] && droneNode["repulsion_distance"].IsScalar()&& droneNode["repulsion_distance"].as<float>() >0) {
            configMap["repulsion_distance"] = droneNode["repulsion_distance"].as<float>();
        }
    } catch (const YAML::TypedBadConversion<float>& e) {
        std::cerr << "Invalid value for 'repulsion_distance': " << e.what() << std::endl;
    }

    try {
        if (droneNode["repulsion_factor"] && droneNode["repulsion_factor"].IsScalar()&& droneNode["repulsion_factor"].as<float>() >0) {
            configMap["repulsion_factor"] = droneNode["repulsion_factor"].as<float>();
        }
    } catch (const YAML::TypedBadConversion<float>& e) {
        std::cerr << "Invalid value for 'repulsion_factor': " << e.what() << std::endl;
    }
}


if (config_["simulation"]) {
    auto simulationNode = config_["simulation"];
    if (simulationNode["time_step"] && simulationNode["time_step"].IsScalar()&& simulationNode["time_step"].as<float>() >0) {
        configMap["time_step"] = simulationNode["time_step"].as<float>();
    }
}


    return configMap;
}

// Static method to print the configuration map
void params_manager::printConfigMap(const std::map<std::string, float>& configMap) {
    for (const auto& pair : configMap) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}