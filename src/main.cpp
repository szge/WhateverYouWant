#include "memory_manager.h"
#include "csgo.hpp"
#include "cheats.h"
#include <iostream>
#include <ctime>
#include <thread>

using namespace hazedumper;

int main() {
    CMemoryManager* memoryManager;

    try {
        memoryManager = new CMemoryManager("csgo.exe");
    }
    catch (...) {
        std::cout << "CS:GO not found!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        return 0;
    }

    if (!memoryManager->GrabModule("client.dll")) {
        // client module not found, manage the failure
        std::cout << "Client module not found!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        return 0;
    }

    Cheats cheats(memoryManager);

    while (true) {
        cheats.update();
        cheats.bhop();
        cheats.trigger();
        cheats.esp();
        cheats.noFlash();

        // save cpu time before running next tick
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}