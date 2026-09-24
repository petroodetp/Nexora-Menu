#include "Fivem.hpp"

#include <fstream>
#include <filesystem>
#include <thread>
#include "../Options.hpp"
#include "D:\NMET\Fivem-External\FrameWork\Math\CustomMath.hpp"
#include <TlHelp32.h>
#include <Psapi.h>
#include <string>

// ImGui includes
#include "../../FrameWork/Dependencies/ImGui/imgui.h"
#include "../../FrameWork/Dependencies/ImGui/imgui_internal.h"

// FrameWork includes
#include "../../FrameWork/Utilities/Memory.hpp"
#include "../../FrameWork/Utilities/Misc.hpp"
#include "../../FrameWork/Math/Vectors/Vector3D.hpp"
#include "../../FrameWork/Math/Vectors/Vector4D.hpp"
#include "../../FrameWork/Math/Matrix4x4.hpp"
#include "../../FrameWork/Security/LazyImporter.hpp"

typedef long NTSTATUS;
#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif
typedef NTSTATUS (NTAPI* pNtQueryInformationThread)(HANDLE ThreadHandle, unsigned long ThreadInformationClass, PVOID ThreadInformation, unsigned long ThreadInformationLength, PULONG ReturnLength);


typedef HRESULT (WINAPI* pGetThreadDescription)(HANDLE hThread, PWSTR* ppszThreadDescription);

namespace Cheat
{
    static std::vector<DWORD> g_blockedLuaThreadIds;

    static pNtQueryInformationThread ResolveNtQueryInformationThread()
    {
        static pNtQueryInformationThread fn = nullptr;
        if (!fn)
        {
            HMODULE ntdll = GetModuleHandleA("ntdll.dll");
            if (ntdll)
                fn = reinterpret_cast<pNtQueryInformationThread>(GetProcAddress(ntdll, "NtQueryInformationThread"));
        }
        return fn;
    }

    static pGetThreadDescription ResolveGetThreadDescription()
    {
        static pGetThreadDescription fn = nullptr;
        static bool tried = false;
        if (!tried)
        {
            tried = true;
            HMODULE k32 = GetModuleHandleA("Kernel32.dll");
            if (k32)
                fn = reinterpret_cast<pGetThreadDescription>(GetProcAddress(k32, "GetThreadDescription"));
        }
        return fn;
    }

    static bool GetRemoteThreadStartAddress(HANDLE hThread, void** outStart)
    {
        auto NtQueryInformationThread = ResolveNtQueryInformationThread();
        if (!NtQueryInformationThread)
            return false;

        PVOID startAddr = nullptr;
        NTSTATUS status = NtQueryInformationThread(hThread, 9 /*ThreadQuerySetWin32StartAddress*/, &startAddr, sizeof(startAddr), nullptr);
        if (!NT_SUCCESS(status))
            return false;
        *outStart = startAddr;
        return (startAddr != nullptr);
    }

    static bool GetModulePathByAddress(HANDLE hProcess, void* address, std::string& outPath)
    {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQueryEx(hProcess, address, &mbi, sizeof(mbi)))
            return false;

        char modulePath[MAX_PATH] = { 0 };
        if (GetMappedFileNameA(hProcess, reinterpret_cast<LPVOID>(mbi.AllocationBase), modulePath, MAX_PATH) == 0)
            return false;

        outPath.assign(modulePath);
        return true;
    }

    static std::wstring GetThreadDescIfAny(HANDLE hThread)
    {
        auto fn = ResolveGetThreadDescription();
        if (!fn)
            return L"";
        PWSTR desc = nullptr;
        if (SUCCEEDED(fn(hThread, &desc)) && desc)
        {
            std::wstring res(desc);
            LocalFree(desc);
            return res;
        }
        return L"";
    }

    static bool IsLikelyLuaResourceThread(HANDLE hProcess, HANDLE hThread)
    {
        void* start = nullptr;
        if (GetRemoteThreadStartAddress(hThread, &start) && start)
        {
            std::string path;
            if (GetModulePathByAddress(hProcess, start, path))
            {
                std::string lower = path;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                if (lower.find("citizen-scripting-lua") != std::string::npos)
                    return true;
                if (lower.find("citizen-scripting-core") != std::string::npos)
                    return true;
                if (lower.find("electronac") != std::string::npos)
                    return true;
            }
        }

        std::wstring desc = GetThreadDescIfAny(hThread);
        if (!desc.empty())
        {
            std::wstring lower = desc;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
            if (lower.find(L"lua") != std::wstring::npos)
                return true;
            if (lower.find(L"fr3kless") != std::wstring::npos)
                return true;
            if (lower.find(L"silentaim") != std::wstring::npos)
                return true;
            if (lower.find(L"heartbeat") != std::wstring::npos)
                return false;
        }

        return false;
    }

    static void ResumeBlockedLuaThreads()
    {
        for (DWORD tid : g_blockedLuaThreadIds)
        {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, tid);
            if (!hThread)
                continue;
            for (;;)
            {
                DWORD prev = ResumeThread(hThread);
                if (prev == (DWORD)-1 || prev == 0)
                    break;
            }
            CloseHandle(hThread);
        }
        g_blockedLuaThreadIds.clear();
    }

	uint64_t EntityType;
	uint64_t Armor;
	uint64_t BoneManager;
	uint64_t PlayerInfo;
	uint64_t PlayerNetID;
	uint64_t WeaponManager;
	uint64_t FragInsNmGTA;
	uint64_t ConfigFlags;
	uint64_t MaxHealth;
	uint64_t LastVehicle;
	uint64_t Driver;
	uint64_t DoorLock;
	uint64_t Stamina;
	uint64_t VisibleFlag;
	uint64_t LocalPlayer;
	uint64_t PlayerSpeed;
	uint64_t FrameFlag;
	uint64_t Weapon;
	uint64_t RunSpeed;
	uint64_t VehicleBodyHealth;
	uint64_t VehicleTankHealth;
	uint64_t VehicleDirt;
	uint64_t PlayerName;
	uint64_t PlayerAppearance;


	void FivemSDK::Intialize()
	{
		if (bIsIntialized)
			return;

		if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b2372_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b2372_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2372_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b2372;
				ModuleName = ("FiveM_b2372_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2372_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b2372;
				ModuleName = ("FiveM_b2372_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x252DCD8;
				ReplayInterface = ModuleBase + 0x1F05208;
				ViewPort = ModuleBase + 0x1F9E9F0;
				Camera = ModuleBase + 0x1F9F898;
				bIsPlayerAiming = ModuleBase + 0x2BF7CF0;
				PlayerAimingAt = ModuleBase + 0x1FACDE0;
				HandleBullet = ModuleBase + 0xFE7EF8;
				BlipList = ModuleBase + 0x1F9FFA0;

				EntityType = 0x10B8;
				Armor = 0x14E0;
				BoneManager = 0x430;
				PlayerInfo = 0x10C8;
				PlayerAppearance = 0x10B8;
				PlayerSpeed = 0x0CF0;
				PlayerNetID = 0x88;
				WeaponManager = 0x10D8;
				FragInsNmGTA = 0x1400;
				ConfigFlags = 0x1414;
				MaxHealth = 0x2A0;
				LastVehicle = 0xD30;
				Driver = 0xC68;
				DoorLock = 0x1390;
				Stamina = 0xCD4;
				VisibleFlag = 0x142C;
				LocalPlayer = FrameWork::Memory::ReadMemory<float>(World + 0x8);

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b2612_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b2612_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2612_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b2612;
				ModuleName = ("FiveM_b2612_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2612_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b2612;
				ModuleName = ("FiveM_b2612_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x2567DB0;
				ReplayInterface = ModuleBase + 0x1F77EF0;
				ViewPort = ModuleBase + 0x1FD8570;
				Camera = ModuleBase + 0x1FD9418;
				bIsPlayerAiming = ModuleBase + 0x2C33DB0;
				PlayerAimingAt = ModuleBase + 0x1FE6960;
				HandleBullet = ModuleBase + 0xFF340C;
				BlipList = ModuleBase + 0x1FDDD20;

				EntityType = 0x10B8;
				Armor = 0x1530;
				BoneManager = 0x430;
				PlayerInfo = 0x10C8;
				PlayerAppearance = 0x10B8;
				PlayerNetID = 0x88;
				WeaponManager = 0x10D8;
				PlayerSpeed = 0x0CF0;
				FragInsNmGTA = 0x1450;
				ConfigFlags = 0x1464;
				MaxHealth = 0x2A0;
				LastVehicle = 0xD30;
				Driver = 0xC68;
				DoorLock = 0x1390;
				Stamina = 0xCF4;
				VisibleFlag = 0x147C;

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b2699_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b2699_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2699_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b2699;
				ModuleName = ("FiveM_b2699_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2699_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b2699;
				ModuleName = ("FiveM_b2699_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x26684D8;
				ReplayInterface = ModuleBase + 0x20304C8;
				ViewPort = ModuleBase + 0x20D8C90;
				Camera = ModuleBase + 0x20D9B38;
				bIsPlayerAiming = ModuleBase + 0x2DA1950;
				PlayerAimingAt = ModuleBase + 0x20E6CC0;
				HandleBullet = ModuleBase + 0xFF9D90;
				BlipList = ModuleBase + 0x20E1420;

				EntityType = 0x10B8;
				Armor = 0x1530;
				BoneManager = 0x430;
				PlayerInfo = 0x10C8;
				PlayerAppearance = 0x10B8;
				PlayerNetID = 0x88;
				WeaponManager = 0x10D8;
				PlayerSpeed = 0x0CF0;
				FragInsNmGTA = 0x1450;
				ConfigFlags = 0x1464;
				MaxHealth = 0x2A0;
				LastVehicle = 0xD30;
				Driver = 0xC68;
				DoorLock = 0x1390;
				Stamina = 0xCF4;
				VisibleFlag = 0x147C;
				LocalPlayer = FrameWork::Memory::ReadMemory<float>(World + 0x8);


				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b2189_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b2189_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2189_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b2189;
				ModuleName = ("FiveM_b2189_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2189_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b2189;
				ModuleName = ("FiveM_b2189_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x24E6D90;
				ReplayInterface = ModuleBase + 0x1EE18A8;
				ViewPort = ModuleBase + 0x1F888C0;
				Camera = ModuleBase + 0x1F89768;
				bIsPlayerAiming = ModuleBase + 0x2BC0FCC;
				PlayerAimingAt = ModuleBase + 0x1F7CBD0;
				HandleBullet = ModuleBase + 0xFE2154;
				BlipList = ModuleBase + 0x1F6EF80;

				PlayerSpeed = 0x0CF0;
				EntityType = 0x10B8;
				Armor = 0x14E0;
				BoneManager = 0x430;
				PlayerInfo = 0x10C8;
				PlayerAppearance = 0x10B8;
				PlayerNetID = 0x78;
				WeaponManager = 0x10D8;
				FragInsNmGTA = 0x1400;
				ConfigFlags = 0x1414;
				MaxHealth = 0x2A0;
				LastVehicle = 0xD30;
				Driver = 0xC68;
				DoorLock = 0x1390;
				Stamina = 0xCD4;
				VisibleFlag = 0x142C;
				LocalPlayer = FrameWork::Memory::ReadMemory<float>(World + 0x8);

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b2802_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b2802_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2802_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b2802;
				ModuleName = ("FiveM_b2802_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2802_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b2802;
				ModuleName = ("FiveM_b2802_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x254D448;
				ReplayInterface = ModuleBase + 0x1F5B820;
				ViewPort = ModuleBase + 0x1FBC100;
				Camera = ModuleBase + 0x1FBCFA8;
				bIsPlayerAiming = ModuleBase + 0x2CA3594;
				PlayerAimingAt = ModuleBase + 0x1FCA160;
				HandleBullet = ModuleBase + 0xFF716C;
				BlipList = ModuleBase + 0x1FBD6E0;

				EntityType = 0x1098;
				PlayerSpeed = 0x0CF0;
				Armor = 0x1530;
				BoneManager = 0x410;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x1098;
				PlayerNetID = 0x88;
				WeaponManager = 0x10B8;
				FragInsNmGTA = 0x1430;
				ConfigFlags = 0x1444;
				MaxHealth = 0x284;
				LastVehicle = 0xD10;
				Driver = 0xC48;
				DoorLock = 0x1370;
				Stamina = 0xCF4;
				VisibleFlag = 0x145C;
				LocalPlayer = FrameWork::Memory::ReadMemory<float>(World + 0x8);

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b2060_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b2060_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2060_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b2060;
				ModuleName = ("FiveM_b2060_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2060_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b2060;
				ModuleName = ("FiveM_b2060_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x24C8858;
				ReplayInterface = ModuleBase + 0x1EC3828;
				ViewPort = ModuleBase + 0x1F6A7E0;
				Camera = ModuleBase + 0x1F6B940;
				bIsPlayerAiming = ModuleBase + 0x2B958CC;
				PlayerAimingAt = ModuleBase + 0x1F5EB20;
				HandleBullet = ModuleBase + 0xFCE6EC;
				BlipList = ModuleBase + 0x1F4F940;

				EntityType = 0x10B8;
				Armor = 0x14E0;
				BoneManager = 0x430;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x10B8;
				PlayerSpeed = 0x0CF0;
				PlayerNetID = 0x78;
				WeaponManager = 0x10D8;
				FragInsNmGTA = 0x1400;
				ConfigFlags = 0x1414;
				MaxHealth = 0x2A0;
				LastVehicle = 0xD28;
				Driver = 0xC68;
				DoorLock = 0x1390;
				Stamina = 0xCD4;
				VisibleFlag = 0x142C;
				LocalPlayer = FrameWork::Memory::ReadMemory<float>(World + 0x8);

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b2545_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b2545_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2545_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b2545;
				ModuleName = ("FiveM_b2545_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2545_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b2545;
				ModuleName = ("FiveM_b2545_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x25667E8;
				ReplayInterface = ModuleBase + 0x1F2E7A8;
				ViewPort = ModuleBase + 0x1FD6F70;
				Camera = ModuleBase + 0x1FD7E18;
				bIsPlayerAiming = ModuleBase + 0x2C32630;
				PlayerAimingAt = ModuleBase + 0x1FE5360;
				HandleBullet = ModuleBase + 0xFF1B40;
				BlipList = ModuleBase + 0x1F4F940;

				EntityType = 0x10B8;
				Armor = 0x1530;
				BoneManager = 0x430;
				PlayerSpeed = 0x0CF0;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x10B8;
				PlayerNetID = 0x88;
				WeaponManager = 0x10D8;
				FragInsNmGTA = 0x1450;
				ConfigFlags = 0x1464;
				MaxHealth = 0x2A0;
				LastVehicle = 0xD30;
				Driver = 0xC68;
				DoorLock = 0x1390;
				Stamina = 0xCF4;
				VisibleFlag = 0x147C;

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b2944_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b2944_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2944_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b2944;
				ModuleName = ("FiveM_b2944_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b2944_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b2944;
				ModuleName = ("FiveM_b2944_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x257BEA0;
				ReplayInterface = ModuleBase + 0x1F42068;
				ViewPort = ModuleBase + 0x1FEAAC0;
				Camera = ModuleBase + 0x1FEB968;
				bIsPlayerAiming = ModuleBase + 0x2CE2A94;
				PlayerAimingAt = ModuleBase + 0x1FF8AF0;
				HandleBullet = ModuleBase + 0x1003F80;
				BlipList = ModuleBase + 0x1FF3130;

				EntityType = 0x1098;
				PlayerSpeed = 0x0D40;
				Armor = 0x150C;
				BoneManager = 0x430;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x1098;
				PlayerNetID = 0xE8;
				WeaponManager = 0x10B8;
				FragInsNmGTA = 0x1430;
				ConfigFlags = 0x1444;
				MaxHealth = 0x284;
				LastVehicle = 0xD10;
				Driver = 0xC48;
				DoorLock = 0x1370;
				Stamina = 0xCF4;
				VisibleFlag = 0x145C;

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b3095_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b3095_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b3095_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b3095;
				ModuleName = ("FiveM_b3095_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b3095_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b3095;
				ModuleName = ("FiveM_b3095_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x2593320;
				ReplayInterface = ModuleBase + 0x1F58B58;
				ViewPort = ModuleBase + 0x20019E0;
				Camera = ModuleBase + 0x2002888;
				bIsPlayerAiming = ModuleBase + 0x2D3839C;
				PlayerAimingAt = ModuleBase + 0x200FA10;
				HandleBullet = ModuleBase + 0x100F5A4;
				BlipList = ModuleBase + 0x2002FA0;

				EntityType = 0x1098;
				PlayerSpeed = 0x0D40;
				Armor = 0x150C;
				BoneManager = 0x430;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x1098;
				PlayerNetID = 0xE8;
				WeaponManager = 0x10B8;
				FragInsNmGTA = 0x1430;
				ConfigFlags = 0x1444;
				MaxHealth = 0x284;
				LastVehicle = 0xD10;
				Driver = 0xC90;
				DoorLock = 0x13C0;
				Stamina = 0xCF4;
				VisibleFlag = 0x145C;
				PlayerName = 0x10A8;

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b3258;
				ModuleName = ("FiveM_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b3258;
				ModuleName = ("FiveM_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x25B14B0;
				ReplayInterface = ModuleBase + 0x1FBD4F0;
				ViewPort = ModuleBase + 0x201DBA0;
				Camera = ModuleBase + 0x201ED50;
				bIsPlayerAiming = ModuleBase + 0x2D3839C;
				PlayerAimingAt = ModuleBase + 0x200FA10;
				HandleBullet = ModuleBase + 0x101A660;
				BlipList = ModuleBase + 0x2023400;

				EntityType = 0x1098;
				Armor = 0x150C;
				BoneManager = 0x430;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x1098;
				PlayerNetID = 0xE8;
				WeaponManager = 0x10B8;
				FragInsNmGTA = 0x1430;
				ConfigFlags = 0x1444;
				MaxHealth = 0x284;
				LastVehicle = 0xD10;
				Driver = 0xC90;
				DoorLock = 0x13C0;
				Stamina = 0xD44;
				VisibleFlag = 0x145C;
				FrameFlag = 0x0270;
				Weapon = 0x320;
				RunSpeed = 0x0CF0;
				VehicleBodyHealth = 0x820;
				VehicleTankHealth = 0x824;
				VehicleDirt = 0x9D8;
				PlayerName = 0x10A8;

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b3323_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b3323_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b3323_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b3323;
				ModuleName = ("FiveM_b3323_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b3323_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b3323;
				ModuleName = ("FiveM_b3323_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x25C15B0;
				ReplayInterface = ModuleBase + 0x1F85458;
				ViewPort = ModuleBase + 0x202DC50;
				Camera = ModuleBase + 0x202EB48;
				bIsPlayerAiming = ModuleBase + 0x0;
				PlayerAimingAt = ModuleBase + 0x0;
				HandleBullet = ModuleBase + 0x0;
				BlipList = ModuleBase + 0x20333E0;

				EntityType = 0x1098;
				Armor = 0x150C;
				BoneManager = 0x430;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x1098;
				PlayerNetID = 0xE8;
				WeaponManager = 0x10B8;
				FragInsNmGTA = 0x1430;
				ConfigFlags = 0x1444;
				MaxHealth = 0x284;
				LastVehicle = 0xD10;
				Driver = 0xC90;
				DoorLock = 0x13C0;
				Stamina = 0xD44;
				VisibleFlag = 0x145C;
				FrameFlag = 0x0270;
				Weapon = 0x320;
				RunSpeed = 0x0CF0;
				VehicleBodyHealth = 0x820;
				VehicleTankHealth = 0x824;
				VehicleDirt = 0x9D8;

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b3407_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b3407_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b3407_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b3407;
				ModuleName = ("FiveM_b3407_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b3407_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b3407;
				ModuleName = ("FiveM_b3407_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x25D7108;
				ReplayInterface = ModuleBase + 0x1F9A9D8;
				ViewPort = ModuleBase + 0x20431C0;
				Camera = ModuleBase + 0x20440C8;
				bIsPlayerAiming = ModuleBase + 0x2D3839C;
				PlayerAimingAt = ModuleBase + 0x200FA10;
				HandleBullet = ModuleBase + 0x102FF8C;
				BlipList = ModuleBase + 0x2047D50;

				EntityType = 0x1098;
				PlayerSpeed = 0x0D40;
				Armor = 0x150C;
				BoneManager = 0x430;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x1098;
				PlayerNetID = 0xE8;
				WeaponManager = 0x10B8;
				FragInsNmGTA = 0x1430;
				ConfigFlags = 0x1444;
				MaxHealth = 0x284;
				LastVehicle = 0xD10;
				Driver = 0xC48;
				DoorLock = 0x1370;
				Stamina = 0xCF4;
				VisibleFlag = 0x145C;

				bIsIntialized = true;
			}
		}
		else if (FrameWork::Memory::GetProcessPidByName((L"FiveM_b3570_GameProcess.exe")) || FrameWork::Memory::GetProcessPidByName((L"FiveM_b3570_GTAProcess.exe")))
		{
			Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b3570_GameProcess.exe"));
			if (Pid)
			{
				GameVersion = GAME_VERSION_GAME_b3570;
				ModuleName = ("FiveM_b3570_GameProcess.exe");
			}
			else
			{
				Pid = FrameWork::Memory::GetProcessPidByName((L"FiveM_b3570_GTAProcess.exe"));
				GameVersion = GAME_VERSION_GTA_b3570;
				ModuleName = ("FiveM_b3570_GTAProcess.exe");
			}

			ModuleBase = FrameWork::Memory::GetModuleBaseByName(Pid, FrameWork::Misc::String2WString(ModuleName));
			if (ModuleBase)
			{
				World = ModuleBase + 0x25EC580;
				ReplayInterface = ModuleBase + 0x1FB0418;
				ViewPort = ModuleBase + 0x2058BA0;
				Camera = ModuleBase + 0x2059778;
				bIsPlayerAiming = ModuleBase + 0x0;
				PlayerAimingAt = ModuleBase + 0x0;
				HandleBullet = ModuleBase + 0x102D550;
				BlipList = ModuleBase + 0x0;

				EntityType = 0x1098;
				PlayerSpeed = 0x0D40;
				Armor = 0x150C;
				BoneManager = 0x430;
				PlayerInfo = 0x10A8;
				PlayerAppearance = 0x1098;
				PlayerNetID = 0xE8;
				WeaponManager = 0x10B8;
				FragInsNmGTA = 0x1430;
				ConfigFlags = 0x1444;
				MaxHealth = 0x284;
				LastVehicle = 0xD10;
				Driver = 0xC90;
				DoorLock = 0x13C0;
				Stamina = 0xD44;
				VisibleFlag = 0x145C;

				bIsIntialized = true;
			}
		}

		if (bIsIntialized)
			FrameWork::Memory::AttachProces(Pid);
	}

	bool FivemSDK::UpdateEntities()
	{
		if (!pWorld)
		{
			pWorld = (CWorld*)FrameWork::Memory::ReadMemory<uint64_t>(World);

			if (!pWorld)
				return false;
		}

		pLocalPlayer = pWorld->LocalPlayer();

		if (pReplayInterface && !pLocalPlayer)
		{
			g_Options.General.ShutDown = true;
			return false;
		}

		if (!pLocalPlayer)
			return false;

#ifdef _DEBUG
		static bool Logged = false;
		if (!Logged)
		{
			std::cout << ("[FivemSDK] LocalPlayer: 0x") << std::hex << std::uppercase << pLocalPlayer << std::endl;
			Logged = true;
		}
#endif // _DEBUG

		if (!pReplayInterface)
		{
			pReplayInterface = (CReplayInterface*)FrameWork::Memory::ReadMemory<uint64_t>(ReplayInterface);

			if (!pReplayInterface)
				return false;
		}

		if (!pPedInterface)
		{
			pPedInterface = pReplayInterface->PedInterface();

			if (!pPedInterface)
				return false;
		}

		if (!pCamGameplayDirector)
		{
			pCamGameplayDirector = (CCamGameplayDirector*)FrameWork::Memory::ReadMemory<uint64_t>(Camera);

			if (!pCamGameplayDirector)
				return false;
		}

		LockLists.lock();

		EntityList.clear();
		EntityList.shrink_to_fit();

		for (size_t i = 0; i < pPedInterface->PedMaximum(); i++)
		{
			CPed* Ped = pPedInterface->PedList()->Ped(i);
			if (!Ped)
				continue;

				if ((uint64_t)Ped > 0xCCCCCCCCCCCCC)
				continue;

			if (Ped->IsAnimal())
				continue;
			// if (Ped->GetHealth() <= 0 || Ped->GetHealth() <= 101.f)
			//     continue;

			PedStaticInfo StaticInfo;

			{
				StaticInfo.Ped = Ped;
				StaticInfo.iIndex = i;
				StaticInfo.bIsLocalPlayer = (Ped == pLocalPlayer);
				StaticInfo.bIsNPC = Ped->IsNPC();

				if (FriendList.find(Ped) != FriendList.end())
					StaticInfo.IsFriend = true;
				else
					StaticInfo.IsFriend = false;

				if (!StaticInfo.bIsNPC)
				{
					StaticInfo.NetId = Ped->GetPlayerInfo()->GetPlayerID();
				}
				else
				{
					StaticInfo.NetId = -1;
				}

				StaticInfo.crSkeletonData = FrameWork::Memory::ReadMemory<uint64_t>(FrameWork::Memory::ReadMemory<uint64_t>(FrameWork::Memory::ReadMemory<uint64_t>(Ped + FragInsNmGTA) + 0x68) + 0x178);
			}

			Entity CurrentEntity;
			CurrentEntity.StaticInfo = StaticInfo;

			if (CurrentEntity.StaticInfo.Name.empty())
			{
				if (!PlayersInfo.empty() && !PlayerIdToName.empty())
				{
					auto it = PlayerIdToName.find(CurrentEntity.StaticInfo.NetId);
					if (it != PlayerIdToName.end() && CurrentEntity.StaticInfo.NetId != -1)
					{
						CurrentEntity.StaticInfo.Name = std::to_string(CurrentEntity.StaticInfo.NetId);
					}
					else
					{
						CurrentEntity.StaticInfo.Name = std::to_string(CurrentEntity.StaticInfo.NetId);
					}
				}
				else
				{
					CurrentEntity.StaticInfo.Name = std::to_string(CurrentEntity.StaticInfo.NetId);
				}
			}

			CurrentEntity.Cordinates = Ped->GetCoordinate();
			CurrentEntity.Velocity = Ped->GetVelocity();
			CurrentEntity.HeadPos = WorldToScreen(GetBonePosVec3(CurrentEntity, SKEL_Head));
			CurrentEntity.Visible = Ped->IsVisible();

			if (CurrentEntity.StaticInfo.bIsLocalPlayer)
			{
				LocalPlayerInfo.Ped = Ped;
				LanGame = StaticInfo.bIsNPC;
				LocalPlayerInfo.iIndex = i;
				LocalPlayerInfo.WorldPos = CurrentEntity.Cordinates;
				LocalPlayerInfo.ScreenPos = WorldToScreen(CurrentEntity.Cordinates);
			}

			EntityList.push_back(CurrentEntity);
		}

		LockLists.unlock();

		if (!LanGame)
		{
			if (ServerIp.size() < 2)
			{
				if (FivemFolder.empty() || CrashoMetryLocation.empty())
				{
					HKEY hKey;
					WCHAR Buffer[MAX_PATH];
					DWORD BufferSize = sizeof(Buffer);
					if (SafeCall(RegOpenKeyEx)(HKEY_CURRENT_USER, (L"SOFTWARE\\CitizenFX\\FiveM"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
					{
						if (RegQueryValueEx(hKey, (L"Last Run Location"), NULL, NULL, (LPBYTE)Buffer, &BufferSize) == ERROR_SUCCESS)
						{
							std::string AAA = FrameWork::Misc::Wstring2String(std::wstring(Buffer)) + ("data\\cache\\crashometry");
							if (std::filesystem::exists(AAA))
							{
								FivemFolder = FrameWork::Misc::Wstring2String(std::wstring(Buffer));
								CrashoMetryLocation = FrameWork::Misc::Wstring2String(std::wstring(Buffer)) + ("data\\cache\\crashometry");
							}
						}
						SafeCall(RegCloseKey)(hKey);
					}
				}

				if (!CrashoMetryLocation.empty() || ServerIp.empty())
				{
					std::ifstream Crashometry(CrashoMetryLocation, std::ios::binary);
					std::string RawText;
					if (Crashometry.is_open())
					{
						std::vector<char> Buffer((std::istreambuf_iterator<char>(Crashometry)), std::istreambuf_iterator<char>());
						RawText = std::string(Buffer.begin(), Buffer.end());
						if (RawText.size() > 0)
						{
							size_t StartPos = RawText.find(("last_server_url"));
							if (StartPos != std::string::npos)
							{
								std::string TempServerIp = RawText.substr(StartPos + 15);
								if (TempServerIp.find(("last_server_url")))
								{
									TempServerIp = TempServerIp.substr(TempServerIp.find(("last_server_url")) + 15);
								}
								StartPos = TempServerIp.find(("last_server"));
								TempServerIp = TempServerIp.substr(StartPos + 11);
								StartPos = TempServerIp.find(":" );
								ServerIp = TempServerIp.substr(0, StartPos);
								ServerPort = TempServerIp.substr(StartPos + 1, StartPos + 5);
							}
						}
						Crashometry.close();
					}
				}

				if (ServerIp.size() < 5 || ServerPort.size() < 2)
				{
					LanGame = true;
				}
				else
				{
#ifdef _DEBUG
					std::cout << ("[FivemSDK] Server IP: ") << ServerIp << (":") << ServerPort << std::endl;
#endif // _DEBUG
				}
			}
		}

		static bool isOnSpecificServer = false;
		static bool isOnElectronACServer = false;
		static bool blocked = false;
		static ULONGLONG lastScanMs = 0;

		{
			std::string lower = ServerIp;
			std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
			isOnSpecificServer = (lower.find("cfx.re/join/bp534b") != std::string::npos) ||
							   (lower.find("/bp534b") != std::string::npos) ||
							   (lower == "bp534b");

			isOnElectronACServer = (lower.find("cfx.re/join/9kpyay") != std::string::npos) ||
								 (lower.find("/9kpyay") != std::string::npos) ||
								 (lower == "9kpyay");
		}

		if ((!isOnSpecificServer && !isOnElectronACServer) && blocked)
		{
			ResumeBlockedLuaThreads();
			blocked = false;
		}

		if (isOnSpecificServer || isOnElectronACServer)
		{
			ULONGLONG now = GetTickCount64();
			if (!blocked || now - lastScanMs > 3000)
			{
				lastScanMs = now;
				HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Pid);
				if (hProc)
				{
					DWORD mainThreadId = 0;
					FILETIME earliestCreateTime{ 0xFFFFFFFF, 0xFFFFFFFF };
					HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
					if (hSnapshot != INVALID_HANDLE_VALUE)
					{
						THREADENTRY32 te{}; te.dwSize = sizeof(te);
						if (Thread32First(hSnapshot, &te))
						{
							do
							{
								if (te.th32OwnerProcessID != Pid) { te.dwSize = sizeof(te); continue; }
								HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te.th32ThreadID);
								if (hThread)
								{
									FILETIME createTime{}, exitTime{}, kernelTime{}, userTime{};
									if (GetThreadTimes(hThread, &createTime, &exitTime, &kernelTime, &userTime))
									{
										if (CompareFileTime(&createTime, &earliestCreateTime) < 0)
										{
											earliestCreateTime = createTime;
											mainThreadId = te.th32ThreadID;
										}
									}
									CloseHandle(hThread);
								}
								te.dwSize = sizeof(te);
							} while (Thread32Next(hSnapshot, &te));
						}
						CloseHandle(hSnapshot);

						int specificallyBlocked = 0;
						hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
						if (hSnapshot != INVALID_HANDLE_VALUE)
						{
							THREADENTRY32 te2{}; te2.dwSize = sizeof(te2);
							if (Thread32First(hSnapshot, &te2))
							{
								do
								{
									if (te2.th32OwnerProcessID != Pid || te2.th32ThreadID == mainThreadId) { te2.dwSize = sizeof(te2); continue; }
									HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, FALSE, te2.th32ThreadID);
									if (!hThread) { te2.dwSize = sizeof(te2); continue; }

									bool isLua = IsLikelyLuaResourceThread(hProc, hThread);
									bool isTarget = false;
									std::wstring desc = GetThreadDescIfAny(hThread);
									if (!desc.empty())
									{
										std::wstring lowerDesc = desc;
										std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::towlower);
										if (lowerDesc.find(L"fr3kless") != std::wstring::npos || lowerDesc.find(L"silentaim") != std::wstring::npos)
											isTarget = true;
									}

									if (isLua && isTarget)
									{
										if (SuspendThread(hThread) != (DWORD)-1)
										{
											g_blockedLuaThreadIds.push_back(te2.th32ThreadID);
											specificallyBlocked++;
										}
									}

									CloseHandle(hThread);
									te2.dwSize = sizeof(te2);
								} while (Thread32Next(hSnapshot, &te2));
							}
							CloseHandle(hSnapshot);
						}

						if (specificallyBlocked == 0)
						{
							HANDLE hSnapshot2 = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
							if (hSnapshot2 != INVALID_HANDLE_VALUE)
							{
								THREADENTRY32 te3{}; te3.dwSize = sizeof(te3);
								if (Thread32First(hSnapshot2, &te3))
								{
									do
									{
										if (te3.th32OwnerProcessID != Pid || te3.th32ThreadID == mainThreadId) { te3.dwSize = sizeof(te3); continue; }
										HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, FALSE, te3.th32ThreadID);
										if (!hThread) { te3.dwSize = sizeof(te3); continue; }

										if (IsLikelyLuaResourceThread(hProc, hThread))
										{
											if (SuspendThread(hThread) != (DWORD)-1)
												g_blockedLuaThreadIds.push_back(te3.th32ThreadID);
										}

										CloseHandle(hThread);
										te3.dwSize = sizeof(te3);
									} while (Thread32Next(hSnapshot2, &te3));
								}
								CloseHandle(hSnapshot2);
							}
						}

						CloseHandle(hProc);
						blocked = true;
					}
				}
			}
		}

		return true;
	}

	bool FivemSDK::UpdateVehicles()
	{
		if (!pReplayInterface)
			return false;

		if (!pVehicleInterface)
		{
			pVehicleInterface = pReplayInterface->VehicleInterface();
			if (!pVehicleInterface)
				return false;
		}

		LockLists2.lock();
		VehicleList.clear();
		VehicleList.shrink_to_fit();

		for (size_t i = 0; i < pVehicleInterface->VehicleMaximum(); i++)
		{
			CVehicle* Vehicle = pVehicleInterface->VehicleList()->Vehicle(i);
			if (!Vehicle)
				continue;

			if ((uint64_t)Vehicle > 0xCCCCCCCCCCCCC)
				continue;

			VehicleInfo CurrentVehicle;
			CurrentVehicle.Vehicle = Vehicle;
			CurrentVehicle.iIndex = i;
			CurrentVehicle.Name = "Vehicle_" + std::to_string(i);
			CurrentVehicle.ModelInfo = FrameWork::Memory::ReadMemory<uint64_t>(Vehicle + 0x20);

			VehicleList.push_back(CurrentVehicle);
		}

		LockLists2.unlock();
		return true;
	}

	Vector3D GetBonePosByInstFragAndID(uint64_t crSkeletonData, unsigned int BoneID)
	{
		Matrix4x4 v4 = FrameWork::Memory::ReadMemory<Matrix4x4>(FrameWork::Memory::ReadMemory<uint64_t>(crSkeletonData + 0x8));
		Matrix4x4 Result = FrameWork::Memory::ReadMemory<Matrix4x4>(FrameWork::Memory::ReadMemory<uint64_t>(crSkeletonData + 0x18) + (BoneID << 6));

		Vector3D vec1(v4._11, v4._12, v4._13);
		Vector3D vec2(v4._21, v4._22, v4._23);
		Vector3D vec3(v4._31, v4._32, v4._33);
		Vector3D vec4(v4._41, v4._42, v4._43);
		Vector3D vec5(Result._41, Result._42, Result._43);

		return Vector3D(
			vec1.x * vec5.x + vec4.x + vec2.x * vec5.y + vec3.x * vec5.z,
			vec1.y * vec5.x + vec4.y + vec2.y * vec5.y + vec3.y * vec5.z,
			vec1.z * vec5.x + vec4.z + vec2.z * vec5.y + vec3.z * vec5.z
		);
	}

	Vector3D FivemSDK::GetBonePosVec3(Entity Ped, unsigned int Mask)
	{
		uint64_t FragInstNMGta = FrameWork::Memory::ReadMemory<uint64_t>(Ped.StaticInfo.Ped + FragInsNmGTA);
		if (FragInstNMGta)
		{
			Ped.StaticInfo.crSkeletonData = FrameWork::Memory::ReadMemory<uint64_t>(FrameWork::Memory::ReadMemory<uint64_t>(FragInstNMGta + 0x68) + 0x178);

			auto it = Ped.StaticInfo.MaskToBoneId.find(Mask);
			if (it == Ped.StaticInfo.MaskToBoneId.end())
			{
				unsigned int BoneId = 0;
				if (GetPedBoneIndex(Ped, Mask, BoneId))
				{
					if (BoneId)
					{
						Ped.StaticInfo.MaskToBoneId[Mask] = BoneId;
						return GetBonePosByInstFragAndID(Ped.StaticInfo.crSkeletonData, BoneId);
					}
				}
			}
			else
			{
				return GetBonePosByInstFragAndID(Ped.StaticInfo.crSkeletonData, it->second);
			}
		}

		return Vector3D(0, 0, 0);
	}

	CPed* FivemSDK::GetAimingEntity()
	{
		return (CPed*)FrameWork::Memory::ReadMemory<uint64_t>(PlayerAimingAt);
	}

	bool FivemSDK::IsPlayerAiming()
	{
		return FrameWork::Memory::ReadMemory<bool>(bIsPlayerAiming);
	}

	bool FivemSDK::GetPedBoneIndex(Entity Ped, unsigned int Mask, unsigned int& newIdx)
	{
		uint64_t crSkeletonData = FrameWork::Memory::ReadMemory<uint64_t>(Ped.StaticInfo.crSkeletonData);

		if (FrameWork::Memory::ReadMemory<int16_t>(crSkeletonData + 0x1A))
		{
			uint16_t v1 = FrameWork::Memory::ReadMemory<uint16_t>(crSkeletonData + 0x18);
			if (v1)
			{
				int64_t v2 = FrameWork::Memory::ReadMemory<int64_t>(crSkeletonData + 0x10);
				int Count = 0;
				for (int64_t i = FrameWork::Memory::ReadMemory<int64_t>(v2 + 0x8 * (Mask % v1)); ; i = FrameWork::Memory::ReadMemory<int64_t>(i + 0x8))
				{
					Count++;
					if (!i || i >= 0xCCCCCCCCCCCCCC || Count > 3)
						return false;

					int v5 = FrameWork::Memory::ReadMemory<int>(i);
					if (Mask == v5)
					{
						int v6 = FrameWork::Memory::ReadMemory<int>(i + 0x4);
						newIdx = v6;
						return true;
					}
				}
			}
		}
		else if (Mask < FrameWork::Memory::ReadMemory<uint64_t>(crSkeletonData + 0x5E))
		{
			newIdx = Mask;
			return true;
		}

		return false;
	}

	ImVec2 FivemSDK::GetClosestHitBox(Entity Ped)
	{
		ImVec2 Result = ImVec2(0, 0);

		ImVec2 Head = Ped.HeadPos;
		if (!g_Fivem.IsOnScreen(Head))
			return Result;

		ImVec2 Neck = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Ped, SKEL_Neck_1));
		if (!g_Fivem.IsOnScreen(Neck))
			return Result;

		ImVec2 Chest = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Ped, SKEL_Spine3));
		if (!g_Fivem.IsOnScreen(Chest))
			return Result;

		ImVec2 Center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);

		float HeadDistance = DistTo(Head, Center);
		float NeckDistance = DistTo(Neck, Center);
		float ChestDistance = DistTo(Chest, Center);

		float DistancesArray[] = { HeadDistance, NeckDistance, ChestDistance };

		float Closest = DistancesArray[0];
		int ClosestBone = 0;

		for (int i = 0; i < 5; ++i)
		{
			if (DistancesArray[i] < Closest)
			{
				Closest = DistancesArray[i];
				ClosestBone = i;
			}
		}

		switch (ClosestBone)
		{
		case 0:
			return Head;
			break;
		case 1:
			return Neck;
			break;
		case 2:
			return Chest;
			break;
		default:
			return Head;
			break;
		}

	}

	bool FivemSDK::FindClosestEntity(float Fov, int MaxDistance, bool NPC, Entity* Output)
	{
		Entity Closest;
		float ClosestWorldDistance = FLT_MAX;
		float ClosestScreenDistance = FLT_MAX;

		std::vector<Entity> ClosestPeds;

		bool Found = false;

		LockLists.lock();
		for (Entity Current : EntityList)
		{
			if (Current.StaticInfo.bIsLocalPlayer)
				continue;
		
			if (Current.StaticInfo.bIsNPC && !NPC)
				continue;
		
			float WorldDistance = Current.Cordinates.DistTo(GetLocalPlayerInfo().WorldPos);
		
			if (WorldDistance > MaxDistance)
				continue;
		
			ImVec2 ClosestBone = GetClosestHitBox(Current);
			if (ClosestBone.x == 0 && ClosestBone.y == 0)
				continue;
		
			ImVec2 Center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		
			float ScreenDistance = DistTo(ClosestBone, Center);
		
			if (ScreenDistance > Fov)
				continue;
		
			if (ScreenDistance < ClosestScreenDistance)
			{
				ClosestScreenDistance = ScreenDistance;
				ClosestPeds.push_back(Current);
			}
		}
		LockLists.unlock();
		
		for (Entity Current : ClosestPeds)
		{
			float WorldDistance = Current.Cordinates.DistTo(GetLocalPlayerInfo().WorldPos);
		
			if (WorldDistance < ClosestWorldDistance)
			{
				ClosestWorldDistance = WorldDistance;
				Closest = Current;
				Found = true;
			}
		}

		*Output = Closest;
		return Found;
	}

	void FivemSDK::ProcessCameraMovement(Vector3D WorldPosition, int SmoothHorizontal, int SmoothVertical)
	{
		if (!pCamGameplayDirector)
			return;

		auto FollowPedCamera = pCamGameplayDirector->GetFollowPedCamera();

		Vector3D CrosshairPosition = FollowPedCamera->GetCrosshairPosition();
		Vector3D ViewAngles = FollowPedCamera->GetViewAngles();

		float Distance = CrosshairPosition.DistTo(WorldPosition);

		Vector3D AimAngles = Vector3D((WorldPosition.x - CrosshairPosition.x) / Distance, (WorldPosition.y - CrosshairPosition.y) / Distance, (WorldPosition.z - CrosshairPosition.z) / Distance);

		Vector3D FinalAngles = AimAngles;

		Vector3D CameraDelta = Vector3D(AimAngles.x - ViewAngles.x, AimAngles.y - ViewAngles.y, AimAngles.z - ViewAngles.z);

		if (SmoothHorizontal > 1)
		{
			FinalAngles.x = ViewAngles.x + CameraDelta.x / (float)SmoothHorizontal;
			FinalAngles.y = ViewAngles.y + CameraDelta.y / (float)SmoothHorizontal;
		}

		if (SmoothVertical > 1)
			FinalAngles.z = ViewAngles.z + CameraDelta.z / (float)SmoothVertical;

		float AimbotFixZ = ViewAngles.z - FollowPedCamera->GetThirdpersonViewAngles().z;

		Vector3D ThirdPersonAngles = FinalAngles;
		ThirdPersonAngles.z = ThirdPersonAngles.z - AimbotFixZ;

		FollowPedCamera->SetThirdpersonViewAngles(ThirdPersonAngles);
		FollowPedCamera->SetViewAngles(FinalAngles);
	}

	void FivemSDK::TeleportObject(uintptr_t Object, uintptr_t Navigation, uintptr_t ModelInfo, Vector3D Position, Vector3D VisualPosition, bool Stop)
	{
		float BackupMagic = 0.f;
		if (Stop)
		{
			BackupMagic = FrameWork::Memory::ReadMemory<float>(ModelInfo + 0x2C);
			FrameWork::Memory::WriteMemory(ModelInfo + 0x2C, 0.f);
		}

		FrameWork::Memory::WriteMemory(Object + 0x90, VisualPosition);
		FrameWork::Memory::WriteMemory(Navigation + 0x50, Position);

		if (Stop)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
			FrameWork::Memory::WriteMemory(ModelInfo + 0x2C, BackupMagic);
		}
	}

	ImVec2 FivemSDK::WorldToScreen(Vector3D Pos)
	{
		if (!pViewPort)
		{
			pViewPort = FrameWork::Memory::ReadMemory<uint64_t>(ViewPort);
			if (!pViewPort)
				return ImVec2(0, 0);
		}

		Matrix4x4 ViewMatrix = FrameWork::Memory::ReadMemory<Matrix4x4>(pViewPort + 0x24C);

		ViewMatrix.TransposeThisMatrix();

		Vector4D VecX(ViewMatrix._21, ViewMatrix._22, ViewMatrix._23, ViewMatrix._24);
		Vector4D VecY(ViewMatrix._31, ViewMatrix._32, ViewMatrix._33, ViewMatrix._34);
		Vector4D VecZ(ViewMatrix._41, ViewMatrix._42, ViewMatrix._43, ViewMatrix._44);

		Vector3D ScreenPos;
		ScreenPos.x = (VecX.x * Pos.x) + (VecX.y * Pos.y) + (VecX.z * Pos.z) + VecX.w;
		ScreenPos.y = (VecY.x * Pos.x) + (VecY.y * Pos.y) + (VecY.z * Pos.z) + VecY.w;
		ScreenPos.z = (VecZ.x * Pos.x) + (VecZ.y * Pos.y) + (VecZ.z * Pos.z) + VecZ.w;

		if (ScreenPos.z <= 0.1f)
			return ImVec2(0, 0);

		ScreenPos.z = 1.0f / ScreenPos.z;
		ScreenPos.x *= ScreenPos.z;
		ScreenPos.y *= ScreenPos.z;

		ScreenPos.x += ImGui::GetIO().DisplaySize.x / 2 + float(0.5f * ScreenPos.x * ImGui::GetIO().DisplaySize.x + 0.5f);
		ScreenPos.y = ImGui::GetIO().DisplaySize.y / 2 - float(0.5f * ScreenPos.y * ImGui::GetIO().DisplaySize.y + 0.5f);

		return ImVec2(ScreenPos.x, ScreenPos.y);
	}

	bool FivemSDK::IsOnScreen(ImVec2 Pos)
	{
		if (Pos.x < 0.1f || Pos.y < 0.1 || Pos.x > ImGui::GetIO().DisplaySize.x || Pos.y > ImGui::GetIO().DisplaySize.y)
			return false;
		return true;
	}

	void FivemSDK::AddFriendByNetId(int netId)
	{
		std::lock_guard<std::mutex> lock(LockLists);
		for (const auto& entity : EntityList)
		{
			if (entity.StaticInfo.NetId == netId && !entity.StaticInfo.bIsNPC)
			{
				FriendList[entity.StaticInfo.Ped] = entity.StaticInfo;
				break;
			}
		}
	}

	void FivemSDK::RemoveFriendByNetId(int netId)
	{
		std::lock_guard<std::mutex> lock(LockLists);
		for (auto it = FriendList.begin(); it != FriendList.end(); ++it)
		{
			if (it->second.NetId == netId)
			{
				FriendList.erase(it);
				break;
			}
		}
	}

	bool FivemSDK::IsFriendByNetId(int netId)
	{
		std::lock_guard<std::mutex> lock(LockLists);
		for (const auto& friendEntry : FriendList)
		{
			if (friendEntry.second.NetId == netId)
				return true;
		}
		return false;
	}

	void FivemSDK::UpdateFriendsFromGUI(const std::set<int>& guiFriendsList)
	{
		std::lock_guard<std::mutex> lock(LockLists);
		
		std::set<int> currentFriends;
		for (const auto& friendEntry : FriendList)
		{
			currentFriends.insert(friendEntry.second.NetId);
		}

		for (int netId : guiFriendsList)
		{
			if (currentFriends.find(netId) == currentFriends.end())
			{
				for (const auto& entity : EntityList)
				{
					if (entity.StaticInfo.NetId == netId && !entity.StaticInfo.bIsNPC)
					{
						FriendList[entity.StaticInfo.Ped] = entity.StaticInfo;
						break;
					}
				}
			}
		}

		for (auto it = FriendList.begin(); it != FriendList.end();)
		{
			if (guiFriendsList.find(it->second.NetId) == guiFriendsList.end())
			{
				it = FriendList.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
