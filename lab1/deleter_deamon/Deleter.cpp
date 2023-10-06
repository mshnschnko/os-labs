#include "Deleter.h"

void Deleter::read_config() {
    std::ifstream config_file(config_file_);
    if(!config_file)
        throw std::runtime_error("File does not exist or you have not access to open it.");
    std::filesystem::path folder, ignfile;
    // int i = 1;
    while (config_file >> folder >> ignfile) {
        folders_.push_back(std::make_pair(folder, ignfile));
        // std::cout << i << " FOLDER = " << folder << " " << i << " FILE = " << ignfile << std::endl;
    }
}

void Deleter::do_delete() {
    for (auto& pair : folders_) {
        if(std::filesystem::exists(pair.first / pair.second))
            continue;
        else
            std::filesystem::remove_all(pair.first);
    }
}

void Deleter::set_config_file(std::string config_file_path) {
    if (std::filesystem::exists(config_file_path))
        config_file_ = config_file_path;
    else
        throw std::runtime_error("File does not exist or you have not access to open it.");
    
}

void Deleter::start(std::string config_file_path) {
    while (execute) {
        try {
            set_config_file(config_file_path);
            read_config();
            do_delete();
            std::this_thread::sleep_for(std::chrono::seconds(20));
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            break;
        }
    }
}