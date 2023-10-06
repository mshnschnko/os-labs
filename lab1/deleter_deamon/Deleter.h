#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread>


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

    void read_config();
    void start(std::string config_file_path);
    bool execute;

private:
    void do_delete();
    void set_config_file(std::string config_file_path);
    std::filesystem::path config_file_;
    deleting_folders folders_;
    Deleter() = default;
};
