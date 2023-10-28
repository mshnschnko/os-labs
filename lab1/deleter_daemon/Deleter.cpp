#include "Deleter.h"

namespace fs = std::filesystem;

std::string Deleter::config_file_;
deleting_folders Deleter::folders_;

void Deleter::start() const {
    close_running();
    do_fork();
    syslog(LOG_INFO, "Daemon started");
    while (true) {
        try {
            do_delete(std::ref(folders_));
            std::this_thread::sleep_for(std::chrono::seconds(interval_s_));
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            break;
        }
    }
}

void Deleter::do_delete(deleting_folders const & folders) const {
    for (auto& pair : folders) {
        if(fs::exists(pair.first)) {
            if(fs::exists(pair.first / pair.second))
                continue;
            else
                for (const auto &file : fs::directory_iterator(pair.first))
                        fs::remove(file);
        }
    }
}

Deleter &Deleter::getInstance(const std::string& config_filepath) {
        static Deleter deleter(config_filepath);
        return deleter;
}

void Deleter::read_config(const std::string &filepath) {
        folders_.clear();

        std::ifstream f(filepath);
        fs::path dir, ignfile;

        while (f >> dir >> ignfile) {
                folders_.push_back(std::make_pair(dir, ignfile));
        }
    }

Deleter::Deleter(const std::string &config_filepath) {
    if (fs::exists(config_filepath)) {
        config_file_ = fs::absolute(config_filepath);
        Deleter::read_config(config_file_);
    }
    else {
        openlog("Deleterdaemon", LOG_PID, LOG_DAEMON);
        syslog(LOG_NOTICE, "Config file does not exist. Daemon terminated");
        closelog();
        exit(EXIT_FAILURE);
    }
}

void Deleter::close_running() const {
    int pid;
    std::ifstream f(pid_file_path_);
    f >> pid;
    if (fs::exists(proc_dir_ + "/" + std::to_string(pid)))
        kill(pid, SIGTERM);
}

void Deleter::sighup_handler(int signum) {
    Deleter::read_config(config_file_);
}

void Deleter::sigterm_handler(int sigum) {
    syslog(LOG_NOTICE, "Daemon terminated");
    closelog();
    exit(EXIT_SUCCESS);
}

void Deleter::do_fork() const{
    pid_t pid = fork();
    if (pid != 0)
        exit(EXIT_FAILURE);

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    std::signal(SIGHUP, sighup_handler);
    std::signal(SIGTERM, sigterm_handler);

    pid = fork();
    if (pid != 0)
        exit(EXIT_FAILURE);
    umask(0);
    if (chdir("/") != 0)
        exit(EXIT_FAILURE);

    for (long x = sysconf(_SC_OPEN_MAX); x >= 0; --x)
        close(x);
    openlog("Deleterdaemon", LOG_PID, LOG_DAEMON);

    std::ofstream f(pid_file_path_, std::ios_base::trunc);
    f << getpid();
}
