#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread>
#include <csignal>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>


using deleting_folders = std::vector<std::pair<std::filesystem::path, std::filesystem::path>>;

class Deleter {
public:
    Deleter() = delete;
    Deleter(const Deleter&) = delete;
    Deleter(Deleter&&) = delete;
    
    Deleter& operator=(const Deleter&) = delete;
    Deleter& operator=(Deleter&&) = delete;

    static Deleter& getInstance(const std::string& config_filepath);
    void start() const;


private:
    static void sighup_handler(int signum);
    static void sigterm_handler(int sigum);

    void do_fork() const;
    void close_running() const;
    
    void do_delete(deleting_folders const &folders) const;
    Deleter(const std::string& config_filepath);

    static void read_config(const std::string &filepath);

    static std::string config_file_;

    const std::string pid_file_path_ = std::filesystem::absolute("Deleterdaemon.pid");
    const std::string proc_dir_ = "/proc";
    const std::string syslog_proc_name_ = "Deleterdaemon";
    const unsigned int interval_s_ = 20;

    bool execute_;
    static deleting_folders folders_;
};
