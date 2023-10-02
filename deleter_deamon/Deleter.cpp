#include "Deleter.h"

void Deleter::read_config(std::string filepath) {
    std::ifstream config_file(filepath);
    if(!config_file)
        throw std::runtime_error("File does not exist or you have not access to open it.");
    std::filesystem::path folder, ignfile;
    // int i = 1;
    while (config_file >> folder >> ignfile) {
        folders.push_back(std::make_pair(folder, ignfile));
        // std::cout << i << " FOLDER = " << folder << " " << i << " FILE = " << ignfile << std::endl;
    }
}

void Deleter::do_delete() {
    for (auto& pair : folders) {
        if(std::filesystem::exists(pair.first / pair.second))
            continue;
        else
            std::filesystem::remove_all(pair.first);
    }
}