#include "cheats.h"
#include <iostream>

using namespace hazedumper;

void Cheats::bhop() {
    if (GetAsyncKeyState(VK_SPACE)) {
        BYTE fFlags = 0; // bitfield
        memoryManager->Read<BYTE>(dwLocalPlayer + netvars::m_fFlags, fFlags);
        // check for FL_ONGROUND and wait 15ms since last jump attempt
        // (this is to circumvent the csgo hyperscroll prevention thing)
        if (fFlags & (1 << 0) && clock() - jumpTime > 15) {
            jumpTime = clock();
            // jump for one tick
            memoryManager->Write(dwClientBase + signatures::dwForceJump, 6);
        }
    }
}

void Cheats::trigger() {
    if (GetAsyncKeyState(VK_XBUTTON1)) {
        if (iCrosshairId > 0 && iCrosshairId <= 65) {
            DWORD entity;
            memoryManager->Read<DWORD>(dwClientBase + signatures::dwEntityList + 0x10 * iCrosshairId, entity);
            int iTeamNum; // local player's team num
            memoryManager->Read<int>(entity + netvars::m_iTeamNum, iTeamNum);
            int playerTeamNum; // entity's team num
            memoryManager->Read<int>(dwLocalPlayer + netvars::m_iTeamNum, playerTeamNum);
            if (playerTeamNum != iTeamNum) { // check if entity is on enemy team
                memoryManager->Write(dwClientBase + signatures::dwForceAttack, 6);
            }
        }
    }
}

void Cheats::esp() {
    DWORD entity;
    for (int id = 1; id <= 64; id++) {
        memoryManager->Read<DWORD>(dwClientBase + signatures::dwEntityList + 0x10 * id, entity);
        memoryManager->Read<int>(entity + netvars::m_iTeamNum, iTeamNum);
        if ((iTeamNum == 2 || iTeamNum == 3) && iTeamNum != playerTeamNum) {
            int iHealth;
            memoryManager->Read<int>(entity + netvars::m_iHealth, iHealth);
            float iHealthPercent = iHealth / 100.f;

            int iGlowIndex;
            memoryManager->Read<int>(entity + netvars::m_iGlowIndex, iGlowIndex);

            // set size of a glow object
            int iGlowIndexOffset = iGlowIndex * 0x38;

            memoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x04, 1.f);
            memoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x08, iHealthPercent);
            memoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x0C, iHealthPercent);
            memoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x10, 1.f);
            memoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x24, true);
        }
    }
}

void Cheats::noFlash() {
    if (flFlashDuration > 0.f) {
        // change flash duration to zero
        memoryManager->Write(dwLocalPlayer + netvars::m_flFlashDuration, 0.f);
    }
}

void Cheats::update() {
    // stuff that needs to update every frame
    memoryManager->Read<int>(dwLocalPlayer + netvars::m_iTeamNum, playerTeamNum);
    memoryManager->Read(dwLocalPlayer + netvars::m_flFlashDuration, flFlashDuration);
    memoryManager->Read<int>(dwLocalPlayer + netvars::m_iCrosshairId, iCrosshairId);
}

void Cheats::aim_rage() {
    DWORD targetEntity;
    // find target closest to crosshair
    // targetEntity = findClosestEnemyToFOV(fov, boneId);
    // Vector3 aimAngles = aimAnglesTo(localPlayer, target.getBonePosition(boneId));
    // client->setViewAngles(aimAngles);
}

Cheats::Cheats(CMemoryManager* memoryManager) {
    this->memoryManager = memoryManager;
    for (auto m : memoryManager->GetModules())
    {
        if (!strcmp(m.szModule, "client.dll"))
        {
            dwClientBase = reinterpret_cast<DWORD>(m.modBaseAddr);
            break;
        }
    }

    // stuff that doesn't change
    memoryManager->Read<DWORD>(dwClientBase + signatures::dwLocalPlayer, dwLocalPlayer);
    memoryManager->Read<int>(dwClientBase + signatures::dwGlowObjectManager, dwGlowObjectManager);
}
