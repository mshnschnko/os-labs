#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <filesystem>


using deleting_folders = std::vector<std::pair<std::filesystem::path, std::filesystem::path>>;

class Deleter {
public:
    Deleter(const Deleter&) = delete;
    Deleter(Deleter&&) = delete;
    
    Deleter& operator=(const Deleter&) = delete;
    Deleter& operator=(Deleter&&) = delete;

    static Deleter& getInstance() {
        static Deleter deleter;
        return deleter;
    }

    // void read_config(std::string filepath) {
    //     std::ifstream config_file(filepath);
    //     if(!config_file)
    //         throw std::runtime_error("File does not exist or you have not access to open it.");
    //     std::filesystem::path folder, ignfile;
    //     // int i = 1;
    //     while (config_file >> folder >> ignfile) {
    //         folders.push_back(std::make_pair(folder, ignfile));
    //         // std::cout << i << " FOLDER = " << folder << " " << i << " FILE = " << ignfile << std::endl;
    //     }
    // }

    void read_config(std::string filepath);

    // void do_delete() {
    //     for (auto& pair : folders) {
    //         if(std::filesystem::exists(pair.first / pair.second))
    //             continue;
    //         else
    //             std::filesystem::remove_all(pair.first);
    //     }
    // }

    void do_delete();

private:
    deleting_folders folders;
    Deleter() {
        std::cout << "created" << std::endl;
    };
};