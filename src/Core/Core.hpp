#pragma once
#include <Includes/Includes.hpp>
#include <Core/Offsets.hpp>
#include <Core/SDK/SDK.hpp>

// Threads
#include "Threads/ScreenResolution.hpp"
#include "Threads/UpdatePointers.hpp"
#include "Threads/UpdateNames.hpp"
#include "Threads/VehicleList.hpp"
#include "Threads/EntityList.hpp"


// Features
#include "Features/MagicBullets.hpp"
#include "Features/Triggerbot.hpp"
#include "Features/SilentAim.hpp"
#include "Features/Aimbot.hpp"
#include "Features/ESP.hpp"

//Exploits
#include "Features/Exploits/ResourceList.hpp"

namespace Core {

	inline static bool ThreadsStarted = false;

	inline void StartThreads( ) {
		ThreadsStarted = false;

		//Info
		std::thread( &Threads::cScreenResolution::Update, Threads::g_ScreenResolution ).detach( );
		std::thread( &Threads::cUpdatePtrs::Update, Threads::g_UpdatePtrs ).detach( );
		std::thread( &Threads::cEntityList::Update, &Threads::g_EntityList ).detach( );
		std::thread( &Threads::cVehicleList::Update, &Threads::g_VehicleList ).detach( );
		std::thread( &Threads::cUpdateNames::Update, &Threads::g_UpdateNames ).detach( );

		//Exploits
		std::thread( &Features::Exploits::cResourceList::List, Features::Exploits::g_ResourceList ).detach( );

		//Features
		std::thread( &Features::cAimbot::Start, Features::g_Aimbot ).detach( );
		std::thread( &Features::cSilentAim::HookSilent, Features::g_SilentAim ).detach( );
		std::thread( &Features::cTriggerbot::Start, Features::g_Triggerbot ).detach( );
		std::thread( &Features::cMagicBullets::Start, Features::g_MagicBullets ).detach( );

		ThreadsStarted = true;
	}

	inline void GetOffsets( )
	{
		g_Offsets.m_WeaponManager = 0x10D8;
		g_Offsets.m_WeaponInfo = 0x20;
		g_Offsets.m_LastVehicle = 0xD30;
		g_Offsets.m_PlayerInfo = 0x10C8;
		g_Offsets.m_PlayerId = 0x88;
		g_Offsets.m_FragInst = 0x1400;
		g_Offsets.m_Armor = 0x14E0;
		g_Offsets.m_PedFlag = 0x1414;
		
		// Initialize new offsets with default values
		g_Offsets.m_PlayerSpeed = 0x0CF0;
		g_Offsets.m_RunSpeed = 0x0CF0;
		g_Offsets.m_VehicleBodyHealth = 0x820;
		g_Offsets.m_VehicleTankHealth = 0x824;
		g_Offsets.m_VehicleDirt = 0x9D8;
		g_Offsets.m_PlayerName = 0x10A8;
		g_Offsets.m_PlayerAppearance = 0x10B8;
		g_Offsets.m_FrameFlag = 0x0270;
		g_Offsets.m_Weapon = 0x320;
		g_Offsets.m_Stamina = 0xCD4;
		g_Offsets.m_VisibleFlag = 0x142C;
		g_Offsets.m_DoorLock = 0x1390;
		g_Offsets.m_Driver = 0xC68;

		if ( g_Offsets.CurrentBuild == 2060 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x24C8858;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1EC3828;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x1F6A7E0;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x1F6B940;
			g_Offsets.m_BlipList = Mem.ModBase + 0x1F4F940;

			g_Offsets.m_MaxHealth = 0x2A0;
			g_Offsets.m_EntityType = 0x10B8;
			g_Offsets.m_PlayerId = 0x68;
			g_Offsets.m_Speed = 0xCD0;

			g_Offsets.CurrentBuild = 2060;
		}
		else if ( g_Offsets.CurrentBuild == 2189 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x24E6D90;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1EE18A8;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x1F888C0;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x1F89768;
			g_Offsets.m_BlipList = Mem.ModBase + 0x1F6EF80;

			g_Offsets.m_MaxHealth = 0x2A0;
			g_Offsets.m_EntityType = 0x10B8;
			g_Offsets.m_PlayerId = 0x68;
			g_Offsets.m_Speed = 0xCD0;

			g_Offsets.CurrentBuild = 2189;
		}
		else if ( g_Offsets.CurrentBuild == 2372 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x252DCD8;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1F05208;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x1F9E9F0;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x1F9F898;
			g_Offsets.m_BlipList = Mem.ModBase + 0x1F9FFA0;

			g_Offsets.m_MaxHealth = 0x2A0;
			g_Offsets.m_EntityType = 0x10B8;
			g_Offsets.CurrentBuild = 2372;
		}
		else if ( g_Offsets.CurrentBuild == 2545 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x25667E8;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1F2E7A8;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x1FD6F70;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x1FD7E18;
			g_Offsets.m_BlipList = Mem.ModBase + 0x1FDF560;

			g_Offsets.m_MaxHealth = 0x2A0;
			g_Offsets.m_EntityType = 0x10B8;
			g_Offsets.m_FragInst = 0x1450;
			g_Offsets.m_Armor = 0x1530;
			g_Offsets.m_PedFlag = 0x1464;

			g_Offsets.CurrentBuild = 2545;
		}
		else if ( g_Offsets.CurrentBuild == 2612 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x2567DB0;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1F77EF0;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x1FD8570;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x1FD9418;
			g_Offsets.m_BlipList = Mem.ModBase + 0x1FDDD20;

			g_Offsets.m_MaxHealth = 0x284;
			g_Offsets.m_EntityType = 0x10B8;
			g_Offsets.m_FragInst = 0x1450;
			g_Offsets.m_Armor = 0x1530;
			g_Offsets.m_PedFlag = 0x1464;

			g_Offsets.CurrentBuild = 2612;
		}
		else if ( g_Offsets.CurrentBuild == 2699 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x26684D8;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x20304C8;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x20D8C90;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x20D9B38;
			g_Offsets.m_BlipList = Mem.ModBase + 0x20E1420;

			g_Offsets.m_MaxHealth = 0x284;
			g_Offsets.m_EntityType = 0x10B8;
			g_Offsets.m_FragInst = 0x1450;
			g_Offsets.m_Armor = 0x1530;
			g_Offsets.m_PedFlag = 0x1464;

			g_Offsets.CurrentBuild = 2699;
		}
		else if ( g_Offsets.CurrentBuild == 2802 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x254D448;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1F5B820;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x1FBC100;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x1FBCFA8;
			g_Offsets.m_BlipList = Mem.ModBase + 0x1FBD6E0;

			g_Offsets.m_MaxHealth = 0x284;

			g_Offsets.m_WeaponManager = 0x10B8;
			g_Offsets.m_EntityType = 0x1098;
			g_Offsets.m_LastVehicle = 0xD10;
			g_Offsets.m_PlayerInfo = 0x10A8;
			g_Offsets.m_PlayerAppearance = 0x1098;
			g_Offsets.m_FragInst = 0x1430;
			g_Offsets.m_Armor = 0x1530;
			g_Offsets.m_PedFlag = 0x1444;
			g_Offsets.m_Driver = 0xC48;
			g_Offsets.m_DoorLock = 0x1370;

			g_Offsets.CurrentBuild = 2802;
		}
		else if ( g_Offsets.CurrentBuild == 2944 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x257BEA0;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1F42068;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x1FEAAC0;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x1FEB968;
			g_Offsets.m_BlipList = Mem.ModBase + 0x1FF3130;

			g_Offsets.m_MaxHealth = 0x284;

			g_Offsets.m_WeaponManager = 0x10B8;
			g_Offsets.m_EntityType = 0x1098;
			g_Offsets.m_LastVehicle = 0xD10;
			g_Offsets.m_PlayerInfo = 0x10A8;
			g_Offsets.m_FragInst = 0x1430;
			g_Offsets.m_PlayerId = 0xE8;
			g_Offsets.m_Armor = 0x150C;
			g_Offsets.m_PedFlag = 0x1444;

			g_Offsets.CurrentBuild = 2944;

		}
		else if ( g_Offsets.CurrentBuild == 3095 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x2593320;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1F58B58;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x20019E0;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x2002888;
			g_Offsets.m_BlipList = Mem.ModBase + 0x2002FA0;

			g_Offsets.m_MaxHealth = 0x284;

			g_Offsets.m_WeaponManager = 0x10B8;
			g_Offsets.m_EntityType = 0x1098;
			g_Offsets.m_LastVehicle = 0xD10;
			g_Offsets.m_PlayerInfo = 0x10A8;
			g_Offsets.m_FragInst = 0x1430;
			g_Offsets.m_PlayerId = 0xE8;
			g_Offsets.m_Armor = 0x150C;
			g_Offsets.m_PedFlag = 0x1444;

			g_Offsets.CurrentBuild = 3095;
		}
		else if ( g_Offsets.CurrentBuild == 3258 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x25B14B0;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1FBD4F0;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x201DBA0;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x201ED50;
			g_Offsets.m_BlipList = Mem.ModBase + 0x2023400;

			g_Offsets.m_MaxHealth = 0x284;

			g_Offsets.m_WeaponManager = 0x10B8;
			g_Offsets.m_EntityType = 0x1098;
			g_Offsets.m_LastVehicle = 0xD10;
			g_Offsets.m_PlayerInfo = 0x10A8;
			g_Offsets.m_PlayerAppearance = 0x1098;
			g_Offsets.m_FragInst = 0x1430;
			g_Offsets.m_PlayerId = 0xE8;
			g_Offsets.m_Armor = 0x150C;
			g_Offsets.m_PedFlag = 0x1444;
			g_Offsets.m_Driver = 0xC90;
			g_Offsets.m_DoorLock = 0x13C0;
			g_Offsets.m_Stamina = 0xD44;
			g_Offsets.m_VisibleFlag = 0x145C;
			g_Offsets.m_FrameFlag = 0x0270;
			g_Offsets.m_Weapon = 0x320;
			g_Offsets.m_RunSpeed = 0x0CF0;
			g_Offsets.m_VehicleBodyHealth = 0x820;
			g_Offsets.m_VehicleTankHealth = 0x824;
			g_Offsets.m_VehicleDirt = 0x9D8;
			g_Offsets.m_PlayerName = 0x10A8;

			g_Offsets.CurrentBuild = 3258;
		}
		else if ( g_Offsets.CurrentBuild == 3323 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x25C15B0;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1F85458;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x202DC50;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x202EB48;
			g_Offsets.m_BlipList = Mem.ModBase + 0x20333E0;

			g_Offsets.m_MaxHealth = 0x284;

			g_Offsets.m_WeaponManager = 0x10B8;
			g_Offsets.m_EntityType = 0x1098;
			g_Offsets.m_LastVehicle = 0xD10;
			g_Offsets.m_PlayerInfo = 0x10A8;
			g_Offsets.m_PlayerAppearance = 0x1098;
			g_Offsets.m_FragInst = 0x1430;
			g_Offsets.m_PlayerId = 0xE8;
			g_Offsets.m_Armor = 0x150C;
			g_Offsets.m_PedFlag = 0x1444;
			g_Offsets.m_Driver = 0xC90;
			g_Offsets.m_DoorLock = 0x13C0;
			g_Offsets.m_Stamina = 0xD44;
			g_Offsets.m_VisibleFlag = 0x145C;
			g_Offsets.m_FrameFlag = 0x0270;
			g_Offsets.m_Weapon = 0x320;
			g_Offsets.m_RunSpeed = 0x0CF0;
			g_Offsets.m_VehicleBodyHealth = 0x820;
			g_Offsets.m_VehicleTankHealth = 0x824;
			g_Offsets.m_VehicleDirt = 0x9D8;

			g_Offsets.CurrentBuild = 3323;
		}
		else if ( g_Offsets.CurrentBuild == 3407 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x25D7108;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1F9A9D8;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x20431C0;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x20440C8;
			g_Offsets.m_BlipList = Mem.ModBase + 0x2047D50;

			g_Offsets.m_MaxHealth = 0x284;

			g_Offsets.m_WeaponManager = 0x10B8;
			g_Offsets.m_EntityType = 0x1098;
			g_Offsets.m_LastVehicle = 0xD10;
			g_Offsets.m_PlayerInfo = 0x10A8;
			g_Offsets.m_PlayerAppearance = 0x1098;
			g_Offsets.m_FragInst = 0x1430;
			g_Offsets.m_PlayerId = 0xE8;
			g_Offsets.m_Armor = 0x150C;
			g_Offsets.m_PedFlag = 0x1444;
			g_Offsets.m_Driver = 0xC48;
			g_Offsets.m_DoorLock = 0x1370;
			g_Offsets.m_Stamina = 0xCF4;
			g_Offsets.m_VisibleFlag = 0x145C;

			g_Offsets.CurrentBuild = 3407;
		}
		else if ( g_Offsets.CurrentBuild == 3570 )
		{
			g_Offsets.m_World = Mem.ModBase + 0x25EC580;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1FB0418;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x2058BA0;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x2059778;
			g_Offsets.m_BlipList = Mem.ModBase + 0x0;

			g_Offsets.m_MaxHealth = 0x284;

			g_Offsets.m_WeaponManager = 0x10B8;
			g_Offsets.m_EntityType = 0x1098;
			g_Offsets.m_LastVehicle = 0xD10;
			g_Offsets.m_PlayerInfo = 0x10A8;
			g_Offsets.m_PlayerAppearance = 0x1098;
			g_Offsets.m_FragInst = 0x1430;
			g_Offsets.m_PlayerId = 0xE8;
			g_Offsets.m_Armor = 0x150C;
			g_Offsets.m_PedFlag = 0x1444;
			g_Offsets.m_Driver = 0xC90;
			g_Offsets.m_DoorLock = 0x13C0;
			g_Offsets.m_Stamina = 0xD44;
			g_Offsets.m_VisibleFlag = 0x145C;

			g_Offsets.CurrentBuild = 3570;
		}
		else {
			g_Offsets.m_World = Mem.ModBase + 0x247F840;
			g_Offsets.m_ReplayInterFace = Mem.ModBase + 0x1EFD4C8;
			g_Offsets.m_ViewPort = Mem.ModBase + 0x2087780;
			g_Offsets.m_CamGameplayDirector = Mem.ModBase + 0x20888E0;
			g_Offsets.m_BlipList = Mem.ModBase + 0x206B4E0;

			g_Offsets.m_MaxHealth = 0x284;

			g_Offsets.m_WeaponManager = 0x10C8;
			g_Offsets.m_LastVehicle = 0xD28;
			g_Offsets.m_PlayerInfo = 0x10B8;
			g_Offsets.m_PlayerId = 0x68;
			g_Offsets.m_FragInst = 0x13E0;

			g_Offsets.CurrentBuild = 1604;
		}

		if ( g_Offsets.CurrentBuild != 0 )
		{
			//Functions Signatures
			{
				//48 89 5c 24 ? 48 89 6c 24 ? 48 89 74 24 ? 57 48 83 ec ? 41 8b f0 8b fa 48 8b d9 e8
				g_Offsets.m_GiveWeapon = Mem.FindSignature(
					{ 0x48, 0x89, 0x5c, 0x24, 0x00 , 0x48, 0x89, 0x6c, 0x24, 0x00 , 0x48, 0x89, 0x74, 0x24, 0x00 , 0x57, 0x48, 0x83, 0xec, 0x00 , 0x41, 0x8b, 0xf0, 0x8b, 0xfa, 0x48, 0x8b, 0xd9, 0xe8 }
				);

				//0F 29 4F ?? 83 8F ?? ?? ?? ?? ?? 48 8B 4F
				g_Offsets.m_MagicBulletsPatch = Mem.FindSignature(
					{ 0x0F, 0x29, 0x4F, 0x00, 0x83, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x4F }
				);

				//E8 ?? ?? ?? ?? 48 83 C3 60 48 FF CF 75 E6 48 8B 5C 24
				g_Offsets.m_ArmsKinematics = Mem.FindSignature(
					{ 0xE8, 0x00, 0x00, 0x00, 0x00, 0x48, 0x83, 0xC3, 0x60, 0x48, 0xFF, 0xCF, 0x75, 0xE6, 0x48, 0x8B, 0x5C, 0x24 }
				);

				//E8 ?? ?? ?? ?? 48 83 C3 60 48 FF CF 75 DF 48 8B 5C 24 ?? 48 8B 6C 24 ?? 48 8B 74 24
				g_Offsets.m_LegsKinematics = Mem.FindSignature(
					{ 0xE8, 0x00, 0x00, 0x00, 0x00, 0x48, 0x83, 0xC3, 0x60, 0x48, 0xFF, 0xCF, 0x75, 0xDF, 0x48, 0x8B, 0x5C, 0x24, 0x00, 0x48, 0x8B, 0x6C, 0x24, 0x00, 0x48, 0x8B, 0x74, 0x24 }
				);

				//48 8D 45 ?? F3 0F 10 00 F3 0F 10 48 ?? F3 0F 11 45
				g_Offsets.m_SilentAim = Mem.FindSignature(
					{ 0x48, 0x8D, 0x45, 0x00, 0xF3, 0x0F, 0x10, 0x00, 0xF3, 0x0F, 0x10, 0x48, 0x00, 0xF3, 0x0F, 0x11, 0x45 }
				);

				//89 81 ?? ?? ?? ?? 8B 87 ?? ?? ?? ?? F7 D0
				g_Offsets.m_InfiniteCombatRoll = Mem.FindSignature(
					{ 0x89, 0x81, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x87, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xD0 }
				);

				//41 2B C9 3B C8 0F 4D C8
				g_Offsets.m_InfiniteAmmo0 = Mem.FindSignature(
					{ 0x41, 0x2B, 0xC9, 0x3B, 0xC8, 0x0F, 0x4D, 0xC8 }
				);

				//41 2B D1 E8
				g_Offsets.m_InfiniteAmmo1 = Mem.FindSignature(
					{ 0x41, 0x2B, 0xD1, 0xE8 }
				);


				//48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? 48 85 C9 74 05 E8 ?? ?? ?? ?? 8A CB
				g_Offsets.m_AimCPedPatternResult = Mem.FindSignature(
					{ 0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x48, 0x85, 0xC9, 0x74, 0x05, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x8A, 0xCB }
				);


			}

			//Offsets Update
			{
				//0f 2f c2 77 ? f3 0f 10 83 ? ? ? ? 48 83 c4
				uintptr_t SpeedSigAddr = Mem.FindSignature( { 0x0f, 0x2f, 0xc2, 0x77, 0x00, 0xf3, 0x0f, 0x10, 0x83, 0x00, 0x00, 0x00, 0x00, 0x48, 0x83, 0xc4 } );
				g_Offsets.m_Speed = Mem.Read<int>( SpeedSigAddr + 9 );

				//f3 0f 11 80 ? ? ? ? 48 8b 87 ? ? ? ? f3 0f 10 05
				uintptr_t VehicleEngineSigAddr = Mem.FindSignature( { 0xf3, 0x0f, 0x11, 0x80, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8b, 0x87, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x0f, 0x10, 0x05 } );
				g_Offsets.m_VehicleEngineHealth = Mem.Read<int>( VehicleEngineSigAddr + 4 );

				//f3 41 0f 10 b0 ? ? ? ? 48 8b ca
				uintptr_t SpreadSigAddr = Mem.FindSignature( { 0xf3, 0x41, 0x0f, 0x10, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8b, 0xca } );
				g_Offsets.m_Spread = Mem.Read<int>( SpreadSigAddr + 5 );

				//f3 0f 10 b0 ? ? ? ? 49 8b 87
				uintptr_t RecoilSigAddr = Mem.FindSignature( { 0xf3, 0x0f, 0x10, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x49, 0x8b, 0x87 } );
				g_Offsets.m_Recoil = Mem.Read<int>( RecoilSigAddr + 4 );

				//4c 8b 41 ? 4d 85 c9
				uintptr_t ObjectSigAddr = Mem.FindSignature( { 0x4c, 0x8b, 0x41, 0x00, 0x4d, 0x85, 0xc9 } );
				g_Offsets.m_CObject = ( int ) Mem.Read<BYTE>( ObjectSigAddr + 3 );


				//48 8b 8b ? ? ? ? 48 8b d7 e8 ? ? ? ? 48 8b 8b ? ? ? ? 48 8b d7 e8
				uintptr_t CWeaponSigAddr = Mem.FindSignature( { 0x48, 0x8b, 0x8b, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8b, 0xd7, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8b, 0x8b, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8b, 0xd7, 0xe8 } );
				g_Offsets.m_CWeapon = Mem.Read<int>( CWeaponSigAddr + 3 );

				//4c 8b 94 c7 ? ? ? ? eb ? 4d 8b d6 41 8a 82
				uintptr_t VehicleSigAddr = Mem.FindSignature( { 0x4c, 0x8b, 0x94, 0xc7, 0x00, 0x00, 0x00, 0x00, 0xeb, 0x00, 0x4d, 0x8b, 0xd6, 0x41, 0x8a, 0x82 } );
				g_Offsets.m_VehicleDriver = Mem.Read<int>( VehicleSigAddr + 4 );

				//8b 81 ? ? ? ? 83 e0 ? c1 e0 ? 3d ? ? ? ? 0f 8f
				uintptr_t RagDollSigAddr = Mem.FindSignature( { 0x8b, 0x81, 0x00, 0x00, 0x00, 0x00, 0x83, 0xe0, 0x00, 0xc1, 0xe0, 0x00, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x8f } );
				g_Offsets.m_NoRagDoll = Mem.Read<int>( RagDollSigAddr + 2 );

				//f6 81 ? ? ? ? ? 75 ? 8b 83
				uintptr_t SeatBealtAddr = Mem.FindSignature( { 0xf6, 0x81,0x00 ,0x00 ,0x00 ,0x00 ,0x00 , 0x75,0x00 , 0x8b, 0x83 } );
				g_Offsets.m_SeatBealt = Mem.Read<int>( SeatBealtAddr + 2 );

				//74 ? 83 b9 ? ? ? ? ? 75 ? 45 84 f6
				uintptr_t VehicleDoorsLockAddr = Mem.FindSignature( { 0x74, 0x00, 0x83, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x00, 0x45, 0x84, 0xf6 } );
				g_Offsets.m_VehicleDoorsLockState = Mem.Read<int>( VehicleDoorsLockAddr + 4 );

				//48 8b 83 ? ? ? ? 8b d5 48 89 44 24 ? 4c 89 74 24
				uintptr_t HandlingSigAddr = Mem.FindSignature( { 0x48, 0x8b, 0x83,0x00,0x00,0x00,0x00, 0x8b, 0xd5, 0x48, 0x89, 0x44, 0x24,0x00, 0x4c, 0x89, 0x74, 0x24 } );
				g_Offsets.m_Handling = Mem.Read<int>( HandlingSigAddr + 3 );

			}
		}
	}

}


