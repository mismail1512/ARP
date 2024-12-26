#ifndef PARAMS_MANAGER_HPP
#define PARAMS_MANAGER_HPP

#include <string>
#include <map>
#include <yaml-cpp/yaml.h>

class params_manager {
public:
    YAML::Node config_;  // YAML node to store the loaded config
    std::string filePath= "params.yaml";
    // Constructor that accepts the file path
    explicit params_manager();
     //explicit params_manager();
    // Method to get the configuration as a map
    std::map<std::string, float> getConfigAsMap() ;

    // Static method to print the configuration map
    static void printConfigMap(const std::map<std::string, float>& configMap);

//private:
   
};

#endif // PARAMS_MANAGER_HPP