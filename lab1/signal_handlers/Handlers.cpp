#include "Handlers.h"
#include "../deleter_deamon/Deleter.h"

void signalhandlers::sighup_handler(int signum) {
    Deleter& deleter = Deleter::getInstance();
    deleter.read_config();
}

void signalhandlers::sigterm_handler(int sigum) {
    openlog("Deleter deamon", LOG_NDELAY | LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Deleter stopped. Exit...");
    closelog();
    Deleter& deleter = Deleter::getInstance();
    deleter.execute = false;
}