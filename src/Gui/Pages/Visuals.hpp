#pragma once
#include <Includes/includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>
#include <Includes/CustomWidgets/Preview.hpp>

#include <Core/Core.hpp>

using namespace std;

namespace Visuals {

	static int iSubTabCount = 0;
	static float SubTabAlpha = 0.f;
	static int iSubTab = 0;

	void Render( ) {


		//ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2( 6, 0 ) );

		if ( Custom::SubTab( xorstr( "Players" ), 0 == iSubTabCount ) ) {
			iSubTabCount = 0;
		}
		ImGui::SameLine( );
		if ( Custom::SubTab( xorstr( "Vehicles" ), 1 == iSubTabCount ) ) {
			iSubTabCount = 1;
		}


		SubTabAlpha = ImClamp( SubTabAlpha + ( 5.f * ImGui::GetIO( ).DeltaTime * ( iSubTabCount == iSubTab ? 1.f : -1.f ) ), 0.f, 1.f );

		if ( SubTabAlpha == 0.f )
			iSubTab = iSubTabCount;


		ImGui::PushStyleVar( ImGuiStyleVar_Alpha, SubTabAlpha * ImGui::GetStyle( ).Alpha );

		ImGui::SetCursorPos( ImVec2( 184, 76 ) );
		ImGui::BeginGroup( );
		{
			switch ( iSubTab )
			{
			case 0: //Players
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Globals#ESP" ), ImVec2( 230, 400 ), false, 0 );
					{
						Custom::CheckBox( xorstr( "Toggle" ), &g_Config.ESP->Enabled );
						if ( Custom::CheckBox( xorstr( "Box" ), &g_Config.ESP->Box ) ) {
							if ( !g_Config.ESP->Box ) {
								g_Config.ESP->FilledBox = false;
							}
						}
						if ( Custom::CheckBox( xorstr( "Filled Box" ), &g_Config.ESP->FilledBox ) ) {
							if ( !g_Config.ESP->Box ) {
								g_Config.ESP->Box = true;
							}
						}

						Custom::CheckBox( xorstr( "Player Names" ), &g_Config.ESP->UserNames );
						Custom::CheckBox( xorstr( "Weapon Names" ), &g_Config.ESP->WeaponName );
						Custom::CheckBox( xorstr( "Distance" ), &g_Config.ESP->DistanceFromMe );


						if ( Custom::CheckBox( xorstr( "Skeleton" ), &g_Config.ESP->Skeleton ) )
						{
							Core::SDK::Pointers::pLocalPlayer->RemoveKinematics( );
						}

						Custom::CheckBox( xorstr( "Head Circle" ), &g_Config.ESP->HeadCircle );
						Custom::CheckBox( xorstr( "Health Bar" ), &g_Config.ESP->HealthBar );
						Custom::CheckBox( xorstr( "Armor Bar" ), &g_Config.ESP->ArmorBar );
						Custom::CheckBox( xorstr( "SnapLines" ), &g_Config.ESP->SnapLines );
						//Custom::CheckBox( xorstr( "Highlight Visible" ), &g_Config.ESP->HighlightVisible );
						Custom::CheckBoxCfg( xorstr( "Friend Marker" ), &g_Config.ESP->FriendsMarker, [ ]
							{
								static int KeyMode = 1;
								ImGui::Keybind( xorstr( "Bind" ), &g_Config.ESP->FriendsMarkerBind, &KeyMode );
							}
						);
						Custom::CheckBox( xorstr( "Show LocalPlayer" ), &g_Config.ESP->ShowLocalPlayer );
						Custom::CheckBox( xorstr( "Ignore Dead" ), &g_Config.ESP->IgnoreDead );
						Custom::CheckBox( xorstr( "Ignore NPCs" ), &g_Config.ESP->IgnoreNPCs );

						ImGui::SliderInt( xorstr( "Max Distance" ), &g_Config.ESP->MaxDistance, 0, 1000, xorstr( "%dm" ), 0 );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Preview" ), ImVec2( 230, 400 ), false, ImGuiWindowFlags_NoScrollbar );
					{
						Custom::g_EspPreview.DragDropHandler( );
						Custom::g_EspPreview.Draw( );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );

				break;
			case 1: //Vehicles
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Globals" ), ImVec2( 230, 400 ), false, 0 );
					{
						Custom::CheckBox( xorstr( "Toggle" ), &g_Config.VehicleESP->Enabled );
						Custom::CheckBox( xorstr( "Vehicle Names" ), &g_Config.VehicleESP->VehName );
						Custom::CheckBox( xorstr( "Locked/Unlocked" ), &g_Config.VehicleESP->ShowLockUnlock );
						Custom::CheckBox( xorstr( "SnapLines" ), &g_Config.VehicleESP->SnapLines );
						Custom::CheckBox( xorstr( "Distance" ), &g_Config.VehicleESP->DistanceFromMe );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Settings" ), ImVec2( 230, 400 ), false, 0 );
					{
						ImGui::SliderInt( xorstr( "Max Distance" ), &g_Config.VehicleESP->MaxDistance, 0, 1000, xorstr( "%dm" ), 0 );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				break;
			default:
				break;
			}

		}
		ImGui::EndGroup( );
		ImGui::PopStyleVar( );
	}
}