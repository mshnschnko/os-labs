#include <iostream>
#include "deleter_deamon/Deleter.h"

int main(int argc, char* argv[]) {
    std::cout << __cplusplus << std::endl;
    Deleter& deleter = Deleter::getInstance();
    deleter.read_config("config.txt");
    deleter.do_delete();
    return 0;
}
