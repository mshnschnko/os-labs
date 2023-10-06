#include <iostream>
#include <string>
#include <csignal>
#include "deleter_deamon/Deleter.h"
#include "signal_handlers/Handlers.h"

int main(int argc, char* argv[]) {
    Deleter& deleter = Deleter::getInstance();
    std::string config_file = argc > 1 ? argv[1] : "config.conf"; 
    std::signal(SIGHUP, signalhandlers::sighup_handler);
    std::signal(SIGTERM, signalhandlers::sigterm_handler);
    deleter.start(config_file);    
    return 0;
}
