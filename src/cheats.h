#pragma once
#include "memory_manager.h"
#include "csgo.hpp"
#include <ctime>

using namespace hazedumper;

class Cheats {
public:
    CMemoryManager* memoryManager;
    DWORD dwClientBase = 0;
    DWORD dwLocalPlayer = 0;
    int iCrosshairId = 0;
    int iTeamNum = 0;
    int playerTeamNum = 0;
    int dwGlowObjectManager = 0;
    float flFlashDuration = 0;
    time_t jumpTime = clock();

    void update();
    void bhop();
    void trigger();
    void esp();
    void noFlash();
    void aim_rage();
    Cheats(CMemoryManager* memoryManager);
};