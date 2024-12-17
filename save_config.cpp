#include "save_config.hpp"
#include <fstream>
#include <iostream>

SaveConfig::SaveConfig(std::string filename) : _filename(filename) {}


void SaveConfig::save(std::string name, std::string value) {
    std::ofstream file(_filename, std::ios::out);
    if (!file) {
        std::cerr << "File error" << std::endl;
        return;
    }
    file << name << "=" << value << std::endl;
    file.close();
}

std::string SaveConfig::load(std::string name) {
    std::ifstream file(_filename);
    if (!file) {
        std::cerr << "Config file not found" << std::endl;
        return "";
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.find(name) != std::string::npos) {
            return line.substr(line.find("=") + 1);
        }
    }
    return "";
}
