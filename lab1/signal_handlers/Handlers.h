#include <syslog.h>
#include <iostream>

namespace signalhandlers {
    void sighup_handler(int signum);
    void sigterm_handler(int signum);
}