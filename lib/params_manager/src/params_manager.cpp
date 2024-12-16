#include "params_manager.hpp"


ParamsManager::ParamsManager(const std::string& filename){
        configsFileName = filename;
        try {
            // Load the YAML file
            config = YAML::LoadFile(filename);
        } catch (const YAML::Exception& e) {
            std::cerr << "Error loading YAML file: " << e.what() << std::endl;
        }
}

void ParamsManager::refresh(){
    try {
            // Load the YAML file
            config = YAML::LoadFile(configsFileName);
            
        } catch (const YAML::Exception& e) {
            std::cerr << "Error loading YAML file: " << e.what() << std::endl;
        }
}

/**
std::map<std::string, float> ParamsManager::getConfigAsMap() const
{

    std::map<std::string, float> configMap;

    // Iterate over the top-level keys in the YAML file
    for (YAML::const_iterator it = config.begin(); it != config.end(); ++it) {
        // Iterate over nested keys and values
        if (it->second.IsMap()) {
            for (YAML::const_iterator subIt = it->second.begin(); subIt != it->second.end(); ++subIt) {
                // Add the key-value pairs to the map, converting to float
                try {
                    configMap[subIt->first.as<std::string>()] = subIt->second.as<float>();
                } catch (const YAML::Exception& e) {
                    std::cerr << "Error converting value to float: " << e.what() << std::endl;
                }
            }
        }
    } 
}
*/
/////////////////////////////////////////////////////////////////////////////////
std::map<std::string, float> ParamsManager::getConfigAsMap() const {
    std::map<std::string, float> configMap;

    if (config["drone"]) {
        configMap["mass"] = config["drone"]["mass"].as<float>();
        configMap["viscous_coefficient"] = config["drone"]["viscous_coefficient"].as<float>();
        configMap["max_acceleration"] = config["drone"]["max_acceleration"].as<float>();
        configMap["max_velocity"] = config["drone"]["max_velocity"].as<float>();
        configMap["force_step"] = config["drone"]["force_step"].as<float>();
        configMap["repulsion_distance"] = config["drone"]["repulsion_distance"].as<float>();
        configMap["repulsion_factor"] = config["drone"]["repulsion_factor"].as<float>();
        for (const auto& pair : configMap) {
    std::cout << pair.first << ": " << pair.second << std::endl;
}
    }
    

    return configMap;
}
