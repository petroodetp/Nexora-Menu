#pragma once
#include <Includes/includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>

#include <Core/Core.hpp>
#include <Core/Features/Exploits/Exploits.hpp>
#include <Core/Features/Exploits/HandlingEditor.hpp>

using namespace std;

namespace Local {

	static int iSubTabCount = 0;
	static float SubTabAlpha = 0.f;
	static int iSubTab = 0;

	void Render( )
	{
		if ( Custom::SubTab( xorstr( "Main" ), 0 == iSubTabCount ) ) {
			iSubTabCount = 0;
		}
		ImGui::SameLine( );
		if ( Custom::SubTab( xorstr( "Vehicles" ), 1 == iSubTabCount ) ) {
			iSubTabCount = 1;
		}
		ImGui::SameLine( );
		if ( Custom::SubTab( xorstr( "Teleports" ), 2 == iSubTabCount ) ) {
			iSubTabCount = 2;
		}

		SubTabAlpha = ImClamp( SubTabAlpha + ( 5.f * ImGui::GetIO( ).DeltaTime * ( iSubTabCount == iSubTab ? 1.f : -1.f ) ), 0.f, 1.f );
		if ( SubTabAlpha == 0.f ) iSubTab = iSubTabCount;

		ImGui::PushStyleVar( ImGuiStyleVar_Alpha, SubTabAlpha * ImGui::GetStyle( ).Alpha );

		ImGui::SetCursorPos( ImVec2( 184, 76 ) );
		ImGui::BeginGroup( );
		{
			switch ( iSubTab )
			{
			case 0: // Main
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Globals" ), ImVec2( 230, 400 ), false, 0 );
					{
						if ( Custom::CheckBoxCfg( xorstr( "GodMode" ), &g_Config.Player->EnableGodMode,
							[ & ] ( ) { static int KeyMode = 1; ImGui::Keybind( xorstr( "Bind" ), &g_Config.Player->GodModeKey, &KeyMode ); },
							true, xorstr( "Can be detected by server AntiCheat" ), ICON_FA_TRIANGLE_EXCLAMATION ) )
						{
							Core::SDK::Pointers::pLocalPlayer->SetGodMode( g_Config.Player->EnableGodMode );
						}

						if ( Custom::CheckBoxCfg( xorstr( "NoClip" ), &g_Config.Player->NoClipEnabled,
							[ & ] ( ) {
								static int KeyMode = 1;
								ImGui::Keybind( xorstr( "Bind" ), &g_Config.Player->NoClipKey, &KeyMode );
								ImGui::SliderFloat( xorstr( "Speed" ), &g_Config.Player->NoClipSpeed, 0.1f, 20.f, xorstr( "%1.2fm/s" ) );
							},
							true, xorstr( "Can be detected by server AntiCheat" ), ICON_FA_TRIANGLE_EXCLAMATION ) )
						{
							Core::SDK::Pointers::pLocalPlayer->FreezePed( g_Config.Player->NoClipEnabled );
						}

						if ( Custom::CheckBoxCfg( xorstr( "Fast Run" ), &g_Config.Player->FastRun,
							[ & ] ( ) {
								if ( ImGui::SliderFloat( xorstr( "Speed" ), &g_Config.Player->RunSpeed, 1.f, 10.f, xorstr( "%1.2fm/s" ) ) )
									if ( g_Config.Player->FastRun )
										Core::SDK::Pointers::pLocalPlayer->SetSpeed( g_Config.Player->RunSpeed );
							} ) )
						{
							if ( !g_Config.Player->FastRun )
								Core::SDK::Pointers::pLocalPlayer->SetSpeed( 1.f );
						}

						if ( Custom::CheckBox( xorstr( "Infinite Stamina" ), &g_Config.Player->InfiniteStamina,
							true, xorstr( "Can be detected by server AntiCheat" ), ICON_FA_TRIANGLE_EXCLAMATION ) )
						{
							Core::SDK::Pointers::pLocalPlayer->SetInfStamina( g_Config.Player->InfiniteStamina );
						}

						if ( Custom::CheckBox( xorstr( "No RagDoll" ), &g_Config.Player->NoRagDollEnabled ) )
						{
							Core::SDK::Pointers::pLocalPlayer->NoRagDoll( g_Config.Player->NoRagDollEnabled );
						}

						if ( Custom::CheckBox( xorstr( "Anti Headshot" ), &g_Config.Player->AntiHSEnabled,
							true, xorstr( "Can be detected by server AntiCheat" ), ICON_FA_TRIANGLE_EXCLAMATION ) )
						{
							Core::SDK::Pointers::pLocalPlayer->SetConfigFlag( ePedConfigFlag::NoCriticalHits, g_Config.Player->AntiHSEnabled );
						}

						if ( Custom::CheckBox( xorstr( "Infinite CombatRoll" ), &g_Config.Player->InfiniteCombatRoll ) )
						{
							std::thread( [ ] ( ) { Core::SDK::Pointers::pLocalPlayer->SetInfCombatRoll( g_Config.Player->InfiniteCombatRoll ); } ).detach( );
						}

						if ( Custom::CheckBox( xorstr( "Steal Car" ), &g_Config.Player->StealCarEnabled ) )
						{
							if ( g_Config.Player->StealCarEnabled )
							{
								Core::SDK::Pointers::pLocalPlayer->SetConfigFlag( ePedConfigFlag::NotAllowedToJackAnyPlayers, false );
								Core::SDK::Pointers::pLocalPlayer->SetConfigFlag( ePedConfigFlag::PlayerCanJackFriendlyPlayers, true );
								Core::SDK::Pointers::pLocalPlayer->SetConfigFlag( ePedConfigFlag::WillJackAnyPlayer, true );
							}
							else
							{
								Core::SDK::Pointers::pLocalPlayer->SetConfigFlag( ePedConfigFlag::NotAllowedToJackAnyPlayers, true );
								Core::SDK::Pointers::pLocalPlayer->SetConfigFlag( ePedConfigFlag::PlayerCanJackFriendlyPlayers, false );
								Core::SDK::Pointers::pLocalPlayer->SetConfigFlag( ePedConfigFlag::WillJackAnyPlayer, false );
							}
						}
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Stats" ), ImVec2( 230, 400 ), false, 0 );
					{
						g_Config.Player->CurrentHealthValue = Core::SDK::Pointers::pLocalPlayer->GetHealth( ) - 100.f;
						g_Config.Player->CurrentArmorValue  = Core::SDK::Pointers::pLocalPlayer->GetArmor( );

						if ( ImGui::SliderFloat( xorstr( "Health" ), &g_Config.Player->CurrentHealthValue, -1, Core::SDK::Pointers::pLocalPlayer->GetMaxHealth( ), xorstr( "%1.f" ) ) )
							Core::SDK::Pointers::pLocalPlayer->SetHealth( g_Config.Player->CurrentHealthValue + 100.f );

						if ( ImGui::SliderFloat( xorstr( "Armor" ), &g_Config.Player->CurrentArmorValue, 0, 100, xorstr( "%1.f" ) ) )
							Core::SDK::Pointers::pLocalPlayer->SetArmor( g_Config.Player->CurrentArmorValue );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				break;

			case 1: // Vehicles
			{
				bool InVehicle = Core::SDK::Pointers::pLocalPlayer->InVehicle( );
				auto CurrentVehicle = Core::SDK::Pointers::pLocalPlayer->GetLastVehicle( );

				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "VehOptions" ), ImVec2( 230, 400 ), false, 0 );
					{
						if ( Custom::CheckBox( xorstr( "GodMode" ), &g_Config.Player->VehicleGodMode ) )
						{
							if ( !InVehicle )
							{
								g_Config.Player->VehicleGodMode = false;
								std::thread( [ ] ( ) { NotifyManager::Send( xorstr( "You must be in a vehicle." ), 4000 ); } ).detach( );
							}
							else
								CurrentVehicle->SetGodMode( g_Config.Player->VehicleGodMode );
						}

						static bool Locked = false;
						if ( Custom::CheckBox( xorstr( "Doors Locked" ), &Locked ) )
							CurrentVehicle->DoorState( !Locked );

						if ( Custom::CheckBox( xorstr( "SeatBelt" ), &g_Config.Player->SeatBelt ) )
						{
							if ( !InVehicle )
							{
								g_Config.Player->SeatBelt = false;
								std::thread( [ ] ( ) { NotifyManager::Send( xorstr( "You must be in a vehicle." ), 4000 ); } ).detach( );
							}
							else
								std::thread( [ ] ( ) { Core::SDK::Pointers::pLocalPlayer->SeatBealt( g_Config.Player->SeatBelt ); } ).detach( );
						}

						if ( Custom::CheckBoxPage( xorstr( "Handling Editor" ), &g_Config.Player->HandlingEditor,
							[ & ] ( ) {
								if ( !InVehicle )
									std::thread( [ ] ( ) { NotifyManager::Send( xorstr( "You must be in a vehicle." ), 4000 ); } ).detach( );
								else
									iSubTabCount = 3;
							}, true, xorstr( "Click to open Handling Editor" ) ) )
						{
							if ( !InVehicle )
							{
								g_Config.Player->HandlingEditor = false;
								std::thread( [ ] ( ) { NotifyManager::Send( xorstr( "You must be in a vehicle." ), 4000 ); } ).detach( );
							}
							if ( g_Config.Player->HandlingEditor )
								Features::Exploits::g_HandlingEditor.SaveHandlingValues( );
							else
								Features::Exploits::g_HandlingEditor.RestoreHandlingValues( );
						}
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "VehActions" ), ImVec2( 230, 400 ), false, 0 );
					{
						ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 14, 8 ) );
						if ( Custom::Button( xorstr( "Repair Vehicle" ), ImVec2( -1, 33 ), 0 ) )
							CurrentVehicle->Fix( );
						ImGui::PopStyleVar( );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
			}
			break;

			case 2: // Teleports
				ImGui::SetCursorPos( ImVec2( 184, 76 ) );
				ImGui::BeginGroup( );
				{
					struct Locations_t {
						std::string Name;
						D3DXVECTOR3 Coords;
					};

					std::vector<Locations_t> Locations = {
						Locations_t( xorstr( "Waypoint" ),          D3DXVECTOR3( 0, 0, 0 ) ),
						Locations_t( xorstr( "Square" ),            D3DXVECTOR3( 156.184f, -1043.17f, 29.3236f ) ),
						Locations_t( xorstr( "Pier" ),              D3DXVECTOR3( -1847.72f, -1223.36f, 13.8745f ) ),
						Locations_t( xorstr( "Paleto Bay" ),        D3DXVECTOR3( -397.605f, 6047.57f, 32.1797f ) ),
						Locations_t( xorstr( "Central Bank" ),      D3DXVECTOR3( 221.781f, 217.278f, 106.705f ) ),
						Locations_t( xorstr( "Cassino" ),           D3DXVECTOR3( 885.322f, 16.8489f, 80.65f ) ),
						Locations_t( xorstr( "Airport" ),           D3DXVECTOR3( -975.532f, -2880.89f, 16.2665f ) ),
						Locations_t( xorstr( "Sandy Shores" ),      D3DXVECTOR3( 1681.48f, 3251.91f, 40.809f ) ),
					};

					static int SeletedIndex = 0;
					bool IsSelected;

					ImGui::BeginChild( xorstr( "Locations" ), ImVec2( g_MenuInfo.MenuSize.x - ( 184 + 24 ), 250 ), false, 0 );
					{
						ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 14, 8 ) );
						for ( int i = 0; i < ( int )Locations.size( ); i++ )
						{
							IsSelected = SeletedIndex == i;
							if ( ImGui::ListSelectable( Locations[ i ].Name.c_str( ), &IsSelected ) ) SeletedIndex = i;
						}
						ImGui::PopStyleVar( );
					}
					ImGui::EndChild( );

					static ImVec2 ContentSize = ImVec2( g_MenuInfo.MenuSize.x - ( 184 + 24 ), 120 );
					ImGui::BeginContent( xorstr( "LocationsInfo" ), ContentSize, false, 0 );
					{
						auto Location = Locations[ SeletedIndex ];
						std::string Loc = xorstr( "Teleport to " ) + Location.Name;
						if ( Custom::Button( Loc.c_str( ), ImVec2( ContentSize.x, 32 ), 0 ) )
						{
							if ( SeletedIndex == 0 )
								Core::Features::Exploits::TpToWaypoint( );
							else
								Core::SDK::Pointers::pLocalPlayer->SetPos( Location.Coords );
						}
					}
					ImGui::EndContent( );
				}
				ImGui::EndGroup( );
				break;

			case 3: // Handling Editor (sub-page from Vehicles)
				ImGui::BeginGroup( );
				{
					bool InVehicle = Core::SDK::Pointers::pLocalPlayer->InVehicle( );
					if ( !InVehicle ) iSubTabCount = 1;

					ImGui::SetCursorPos( ImVec2( 184, 60 ) );
					ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 10, 4 ) );
					{
						ImGui::TextColored( g_Col.FeaturesText, xorstr( "Handling Editor" ) );
						ImGui::PushFont( g_Variables.FontAwesomeSolidSmall );
						{
							ImGui::SameLine( );
							ImGui::TextColored( g_Col.Base, ICON_FA_REPLY );
							if ( ImGui::IsItemClicked( ) ) iSubTabCount = 1;
						}
						ImGui::PopFont( );
					}
					ImGui::PopStyleVar( );

					ImGui::Spacing( );

					ImGui::BeginChild( xorstr( "Handling" ), ImVec2( g_MenuInfo.MenuSize.x - ( 184 + 24 ), 360 ), false, 0 );
					{
						if ( ImGui::SliderFloat( xorstr( "Acceleration" ), &Features::Exploits::g_HandlingEditor.fAcceleration, 0.0f, 400.f, xorstr( "%1.1f" ) ) )
							Features::Exploits::g_HandlingEditor.ApplyHandlingValues( );

						if ( ImGui::SliderFloat( xorstr( "Break Force" ), &Features::Exploits::g_HandlingEditor.fBreakForce, 0.0f, 100.f, xorstr( "%1.1f" ) ) )
							Features::Exploits::g_HandlingEditor.ApplyHandlingValues( );

						if ( ImGui::SliderFloat( xorstr( "Traction Curve Min" ), &Features::Exploits::g_HandlingEditor.fTractionCurveMin, 0.0f, 100.f, xorstr( "%1.1f" ) ) )
							Features::Exploits::g_HandlingEditor.ApplyHandlingValues( );
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
