#pragma once
#include <Includes/Includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>

using namespace std;

namespace Combat {

	static int iSubTabCount = 0;
	static float SubTabAlpha = 0.f;
	static int iSubTab = 0;

	void Render( ) {


		//ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2( 6, 0 ) );

		if ( Custom::SubTab( xorstr( "Aimbot" ), 0 == iSubTabCount ) ) {
			iSubTabCount = 0;
		}
		ImGui::SameLine( );
		if ( Custom::SubTab( xorstr( "TriggerBot" ), 1 == iSubTabCount ) ) {
			iSubTabCount = 1;
		}
		ImGui::SameLine( );
		if ( Custom::SubTab( xorstr( "SilentAim" ), 2 == iSubTabCount ) ) {
			iSubTabCount = 2;
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
			case 0: //Aimbot
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Globals" ), ImVec2( 230, 400 ), false, 0 );
					{
						Custom::CheckBox( xorstr( "Toggle" ), &g_Config.Aimbot->Enabled );
						Custom::CheckBox( xorstr( "Show Fov" ), &g_Config.Aimbot->ShowFov );
						//Custom::CheckBox( xorstr( "Only Visible" ), &g_Config.Aimbot->OnlyVisible );
						Custom::CheckBox( xorstr( "Ignore NPCs" ), &g_Config.Aimbot->IgnoreNPCs );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Settings" ), ImVec2( 230, 400 ), false, 0 );
					{
						ImGui::SliderInt( xorstr( "Field of View" ), &g_Config.Aimbot->FOV, 0, 400, xorstr( "%d" ), 0 );
						ImGui::SliderInt( xorstr( "Smooth" ), &g_Config.Aimbot->AimbotSpeed, 0, 100, xorstr( "%d" ), 0 );
						//ImGui::SliderInt( xorstr( "Smooth Vertical" ), &g_Config.Aimbot->SmoothVertical, 0, 100, xorstr( "%d" ), 0 );
						ImGui::SliderInt( xorstr( "Max Distance" ), &g_Config.Aimbot->MaxDistance, 0, 1000, xorstr( "%dm" ), 0 );
						static int KeyMode = 1;
						ImGui::Keybind( xorstr( "Bind" ), &g_Config.Aimbot->KeyBind, &KeyMode );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );

				break;
			case 1: //Trigger
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Globals" ), ImVec2( 230, 400 ), false, 0 );
					{
						Custom::CheckBox( xorstr( "Toggle" ), &g_Config.TriggerBot->Enabled );

						if (!g_Config.TriggerBot->SmartTrigger) {
						Custom::CheckBox( xorstr( "Show Fov" ), &g_Config.TriggerBot->ShowFov );
						}
						else
						{
							g_Config.TriggerBot->ShowFov = false;
						}

						Custom::CheckBox( xorstr( "Smart Triggerbot" ), &g_Config.TriggerBot->SmartTrigger );
						//Custom::CheckBox( xorstr( "Only Visible" ), &g_Config.TriggerBot->OnlyVisible );
						Custom::CheckBox( xorstr( "Ignore NPCs" ), &g_Config.TriggerBot->IgnoreNPCs );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Settings" ), ImVec2( 230, 400 ), false, 0 );
					{
						if (!g_Config.TriggerBot->SmartTrigger) {
							ImGui::SliderInt(xorstr("Field of View"), &g_Config.TriggerBot->FOV, 0, 400, xorstr("%d"), 0);
							ImGui::SliderInt(xorstr("Max Distance"), &g_Config.TriggerBot->MaxDistance, 0, 1000, xorstr("%dm"), 0);
						}
						
						ImGui::SliderInt( xorstr( "Delay" ), &g_Config.TriggerBot->Delay, 0, 10, xorstr( "%d" ), 0 );
						static int KeyMode = 1;
						ImGui::Keybind( xorstr( "Bind" ), &g_Config.TriggerBot->KeyBind, &KeyMode );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				break;
			case 2: //Silent
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Globals" ), ImVec2( 230, 400 ), false, 0 );
					{
						Custom::CheckBox( xorstr( "Toggle" ), &g_Config.SilentAim->Enabled );
						Custom::CheckBox( xorstr( "Show Fov" ), &g_Config.SilentAim->ShowFov );
						Custom::CheckBox( xorstr( "Magic Bullets" ), &g_Config.SilentAim->MagicBullets );
						//Custom::CheckBox( xorstr( "Only Visible" ), &g_Config.SilentAim->OnlyVisible );
						Custom::CheckBox( xorstr( "Ignore NPCs" ), &g_Config.SilentAim->IgnoreNPCs );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Settings" ), ImVec2( 230, 400 ), false, 0 );
					{
						ImGui::SliderInt( xorstr( "Field of View" ), &g_Config.SilentAim->FOV, 0, 400, xorstr( "%d" ), 0 );
						ImGui::SliderInt( xorstr( "Miss Chance" ), &g_Config.SilentAim->MissChance, 0, 100, xorstr( "%dx" ), 0 );
						ImGui::SliderInt( xorstr( "Max Distance" ), &g_Config.SilentAim->MaxDistance, 0, 1000, xorstr( "%dm" ), 0 );
						static int KeyMode = 1;
						ImGui::Keybind( xorstr( "Bind" ), &g_Config.SilentAim->KeyBind, &KeyMode );
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