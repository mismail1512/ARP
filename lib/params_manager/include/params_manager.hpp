#include <string>
#include <yaml-cpp/yaml.h>
#include <iostream>

#include <map> ////////////////////////////////




class ParamsManager{
    ParamsManager(const std::string& filename);
    void refresh();
    std::map<std::string, float> getConfigAsMap() const;  
    private:
        YAML::Node config;
        std::string configsFileName;
    
};