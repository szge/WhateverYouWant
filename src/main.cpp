#include "memory_manager.h"
#include <iostream>
#include <ctime>
#include <thread>

int main()
{
    CMemoryManager* MemoryManager;
    try {
        MemoryManager = new CMemoryManager("csgo.exe");
    }
    catch (...) {
        std::cout << "CS:GO not found!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        return 0;
    }

    if (!MemoryManager->GrabModule("client.dll"))
    {
        // client module not found, manage the failure
        std::cout << "Client module not found!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        return 0;
    }

    DWORD dwClientBase = 0; // This will hold the client base address
    // loop through modules
    for (auto m : MemoryManager->GetModules())
    {
        if (!strcmp(m.szModule, "client.dll"))
        {
            dwClientBase = reinterpret_cast<DWORD>(m.modBaseAddr);
            break;
        }
    }

    while (true) {
        DWORD dwLocalPlayer = 0;
        // set offset: dwLocalPlayer
        MemoryManager->Read<DWORD>(dwClientBase + 0xD3EC6C, dwLocalPlayer);

        
        /*
        BUNNYHOP
        holding space
        */
        if (GetAsyncKeyState(VK_SPACE)) {
            BYTE fFlags = 0; // bitfield
            // set offset: fFlags
            MemoryManager->Read<BYTE>(dwLocalPlayer + 0x104, fFlags);
            // check for FL_ONGROUND
            if (fFlags & (1 << 0)) {
                // jump for one tick
                // set offset: dwForceJump
                MemoryManager->Write(dwClientBase + 0x51FD20C, 6);
            }
        }

        /*
        TRIGGERBOT
        holding MB4
        */
        if (GetAsyncKeyState(VK_XBUTTON1)) {
            int iCrosshairId;
            // set offset: iCrosshairId
            MemoryManager->Read<int>(dwLocalPlayer + 0xB3E4, iCrosshairId);

            if (iCrosshairId > 0 && iCrosshairId <= 65) {
                DWORD entity;
                // set offset: dwEntityList
                MemoryManager->Read<DWORD>(dwClientBase + 0x4D534EC + 0x10 * iCrosshairId, entity);
                int iTeamNum;
                MemoryManager->Read<int>(entity + 0xF4, iTeamNum);
                int playerTeamNum;
                MemoryManager->Read<int>(dwLocalPlayer + 0xF4, playerTeamNum);
                if (playerTeamNum != iTeamNum) {
                    // set offset: dwForceAttack
                    MemoryManager->Write(dwClientBase + 0x3184A80, 6);
                }
            }
        }



        /*
        GLOW ESP
        */

        DWORD entity;
        int iTeamNum;
        int playerTeamNum;
        MemoryManager->Read<int>(dwLocalPlayer + 0xF4, playerTeamNum);
        int dwGlowObjectManager;
        // set offset: dwGlowObjectManager
        MemoryManager->Read<int>(dwClientBase + 0x529B3C8, dwGlowObjectManager);
        for (int id = 1; id <= 64; id++) {
            // set offset: dwEntityList
            MemoryManager->Read<DWORD>(dwClientBase + 0x4D534EC + 0x10 * id, entity);
            MemoryManager->Read<int>(entity + 0xF4, iTeamNum);
            if ((iTeamNum == 2 || iTeamNum == 3) && iTeamNum != playerTeamNum) {
                int iHealth;
                // set offset: iHealth
                MemoryManager->Read<int>(entity + 0x100, iHealth);
                float iHealthPercent = iHealth / 100.f;

                int iGlowIndex;
                // set offset: iGlowIndex
                MemoryManager->Read<int>(entity + 0xA438, iGlowIndex);

                // set size of a glow object
                int iGlowIndexOffset = iGlowIndex * 0x38;

                // set offset: iGlowIndex:red
                MemoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x04, 1.f);
                // set offset: iGlowIndex:green
                MemoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x08, iHealthPercent);
                // set offset: iGlowIndex:blue
                MemoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x0C, iHealthPercent);
                // set offset: iGlowIndex:alpha
                MemoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x10, 1.f);
                // set offset: iGlowIndex:renderWhenOccluded
                MemoryManager->Write(dwGlowObjectManager + iGlowIndexOffset + 0x24, true);
            }
        }

        /*
        NO FLASH
        */

        float flFlashDuration;
        // set offset: flFlashDuration
        MemoryManager->Read(dwClientBase + 0xA420, flFlashDuration);
        if (flFlashDuration > 0.f) {
            std::cout << "flash duration: " << flFlashDuration;
            MemoryManager->Write(dwClientBase + 0xA420, 0.f);
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}