#include "Aimbot.hpp"

void Core::Features::cAimbot::SetViewAngles( CPed * Ped, D3DXVECTOR3 BonePos )
{
	uintptr_t CamFollowPedCamera = Mem.Read<uintptr_t>( Core::SDK::Pointers::pCamGamePlayDirector + 0x2C0 );
	D3DXVECTOR3 CamPos = Mem.Read<D3DXVECTOR3>( CamFollowPedCamera + 0x60 );

	D3DXVECTOR3 CurrentViewAngles = Mem.Read<D3DXVECTOR3>( CamFollowPedCamera + 0x40 );
	D3DXVec3Normalize( &CurrentViewAngles, &CurrentViewAngles );

	uintptr_t CamFollowVehicle = Mem.Read<uintptr_t>( CamFollowPedCamera + 0x10 );

	if ( !CamFollowPedCamera || !CamFollowVehicle ) return;

	if ( Core::SDK::Pointers::pLocalPlayer->InVehicle( ) )
	{
		if ( Mem.Read<float>( CamFollowVehicle + 0x2AC ) == -2.f )
		{
			Mem.Write<float>( CamFollowVehicle + 0x2AC, 0.f );
			Mem.Write<float>( CamFollowVehicle + 0x2C0, 111.f );
			Mem.Write<float>( CamFollowVehicle + 0x2C4, 111.f );
		}
	}

	D3DXVECTOR3 TargetViewAngles = BonePos - CamPos;
	D3DXVec3Normalize( &TargetViewAngles, &TargetViewAngles );	
	
	D3DXVECTOR3 FinalAngles;


	if ( g_Config.Aimbot->AimbotSpeed > 1 )
	{

		auto Delta = TargetViewAngles - CurrentViewAngles;
		FinalAngles.x = CurrentViewAngles.x + Delta.x / g_Config.Aimbot->AimbotSpeed;
		FinalAngles.y = CurrentViewAngles.y + Delta.y / g_Config.Aimbot->AimbotSpeed;
		FinalAngles.z = CurrentViewAngles.z + Delta.z / g_Config.Aimbot->AimbotSpeed;

		D3DXVec3Normalize( &FinalAngles, &FinalAngles );
		Mem.Write<D3DXVECTOR3>( CamFollowPedCamera + 0x40, FinalAngles );
		Mem.Write<D3DXVECTOR3>( CamFollowPedCamera + 0x3D0, FinalAngles );
	}
	else {

		Mem.Write<D3DXVECTOR3>( CamFollowPedCamera + 0x40, TargetViewAngles );
		Mem.Write<D3DXVECTOR3>( CamFollowPedCamera + 0x3D0, TargetViewAngles );
	}
}

void Core::Features::cAimbot::Start( )
{
	while ( true )
	{

		if ( g_Config.Aimbot->Enabled && g_Config.Aimbot->KeyBind && GetAsyncKeyState( g_Config.Aimbot->KeyBind ) & 0x8000 && GetForegroundWindow( ) != g_Variables.g_hCheatWindow )
		{
			CPed * Ped = Core::SDK::Game::GetClosestPed( g_Config.Aimbot->MaxDistance, g_Config.Aimbot->IgnoreNPCs, g_Config.Aimbot->OnlyVisible );
			if ( !Ped ) continue;

			D3DXVECTOR3 HeadPos = Ped->GetBonePosDefault( 0 /*Head*/ );
			D3DXVECTOR2 ScreenHeadPos = Core::SDK::Game::WorldToScreen( HeadPos );

			if ( Core::SDK::Game::IsOnScreen( ScreenHeadPos ) )
			{
				int Fov = std::hypot( ScreenHeadPos.x - g_Variables.g_vGameWindowCenter.x, ScreenHeadPos.y - g_Variables.g_vGameWindowCenter.y );
				if ( Fov < g_Config.Aimbot->FOV )
				{
					SetViewAngles( Ped, HeadPos + D3DXVECTOR3( 0, 0, 0.08 ) );
				}
			}
		}
		std::this_thread::sleep_for( std::chrono::nanoseconds( 1 ) );
	}
}