#include "host.h"

int main(int argc, char* argv[]) {
    Host::GetInstance();
    while (true);
    return 0;
}