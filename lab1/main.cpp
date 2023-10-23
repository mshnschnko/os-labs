#include <iostream>
#include <string>
#include <csignal>
#include "deleter_daemon/Deleter.h"


int main(int argc, char* argv[]) {
    std::string config_file = argc > 1 ? argv[1] : "config.conf"; 
    Deleter const& deleter = Deleter::getInstance(config_file);
    deleter.start();    
    return 0;
}
