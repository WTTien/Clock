#include "clock_core/system.hpp"

int main() {
    System system;
    if (system.init() != PICO_OK) {
        return -1;
    }
    system.run();
    return 0;
}