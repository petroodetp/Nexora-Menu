#pragma once
#include <Includes/Includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>

using namespace std;

namespace Settings {

	static int iSubTabCount = 0;
	static float SubTabAlpha = 0.f;
	static int iSubTab = 0;


	void Render( ) {


		//ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2( 6, 0 ) );

		if ( Custom::SubTab( xorstr( "Main" ), 0 == iSubTabCount ) ) {
			iSubTabCount = 0;
		}
		ImGui::SameLine( );
		if ( Custom::SubTab( xorstr( "Colors" ), 1 == iSubTabCount ) ) {
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
			case 0: //Settings
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Globals" ), ImVec2( 230, 400 ), false, 0 );
					{
						Custom::CheckBox( xorstr( "VSync" ), &g_Config.General->VSync );
						if ( Custom::CheckBox( xorstr( "Stream Proof" ), &g_Config.General->StreamProof ) )
						{
							if ( g_Config.General->StreamProof )
							{
								SetWindowDisplayAffinity( g_Variables.g_hCheatWindow, WDA_EXCLUDEFROMCAPTURE );
							}
							else
							{
								SetWindowDisplayAffinity( g_Variables.g_hCheatWindow, WDA_NONE );
							}
						}

						//Custom::CheckBox( xorstr( "WaterMark" ), &g_Config.General->WaterMark );
						//Custom::CheckBox( xorstr( "ArrayList" ), &g_Config.General->ArrayList );

						//Custom::CheckBoxCfg( xorstr( "WaterMark" ), &g_Config.General->WaterMark, [ & ] ( ) {
						//	static bool Rainbow = false;
						//	Custom::CheckBox( xorstr( "Rainbow" ), &Rainbow );
						//	} );

						//Custom::CheckBoxCfg( xorstr( "ArrayList" ), &g_Config.General->ArrayList, [ & ] ( ) {
						//	static bool Rainbow = false;
						//	Custom::CheckBox( xorstr( "Rainbow" ), &Rainbow );
						//} );


						static int KeyMode = 1;
						ImGui::Keybind( xorstr( "Menu Key" ), &g_Config.General->MenuKey, &KeyMode );

						if ( Custom::Button( xorstr( "Unload" ), ImVec2( -1, 30 ), 0 ) ) 
						{
							exit( 0 );
						}
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Configs" ), ImVec2( 230, 400 ), false, 0 );
					{
						ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 14, 8 ) );
						if ( Custom::Button( xorstr( "Export Config" ), ImVec2( -1, 30 ), 0 ) ) 
						{
							std::thread cfgExport( [ ] {
								std::string CfgMsg = g_Config.SaveCurrentConfig( xorstr( "..." ) );
								NotifyManager::Send( CfgMsg.c_str( ), 3000 );
								} );

							cfgExport.detach( );
						}
						if ( Custom::Button( xorstr( "Import Config" ), ImVec2( -1, 30 ), 0 ) ) 
						{
							std::thread cfgImport( [ ] { NotifyManager::Send( g_Config.LoadCfg( xorstr( "..." ), Utils::GetClipboard( ) ).c_str( ), 3000 ); } );

							cfgImport.detach( );
						}
						ImGui::PopStyleVar( );
					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );

				break;
			case 1: //Colors
				ImGui::BeginGroup( );
				{
					ImGui::BeginChild( xorstr( "Colors" ), ImVec2( g_MenuInfo.MenuSize.x - ( 184 + 24 ), 300 ), false, 0 );
					{
						static float AimbotFovCol[ 4 ] = { g_Config.Aimbot->FovColor.Value.x, g_Config.Aimbot->FovColor.Value.y, g_Config.Aimbot->FovColor.Value.z,g_Config.Aimbot->FovColor.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Aimbot Fov" ), AimbotFovCol, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.Aimbot->FovColor.Value.x = AimbotFovCol[ 0 ];
							g_Config.Aimbot->FovColor.Value.y = AimbotFovCol[ 1 ];
							g_Config.Aimbot->FovColor.Value.z = AimbotFovCol[ 2 ];
							g_Config.Aimbot->FovColor.Value.w = AimbotFovCol[ 3 ];
						}


						static float TriggerFovCol[ 4 ] = { g_Config.TriggerBot->FovColor.Value.x, g_Config.TriggerBot->FovColor.Value.y, g_Config.TriggerBot->FovColor.Value.z,g_Config.TriggerBot->FovColor.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Triggerbot Fov" ), TriggerFovCol, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.TriggerBot->FovColor.Value.x = TriggerFovCol[ 0 ];
							g_Config.TriggerBot->FovColor.Value.y = TriggerFovCol[ 1 ];
							g_Config.TriggerBot->FovColor.Value.z = TriggerFovCol[ 2 ];
							g_Config.TriggerBot->FovColor.Value.w = TriggerFovCol[ 3 ];
						}

						static float SilentAimFovCol[ 4 ] = { g_Config.SilentAim->FovColor.Value.x, g_Config.SilentAim->FovColor.Value.y, g_Config.SilentAim->FovColor.Value.z,g_Config.SilentAim->FovColor.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "SilentAim Fov" ), SilentAimFovCol, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.SilentAim->FovColor.Value.x = SilentAimFovCol[ 0 ];
							g_Config.SilentAim->FovColor.Value.y = SilentAimFovCol[ 1 ];
							g_Config.SilentAim->FovColor.Value.z = SilentAimFovCol[ 2 ];
							g_Config.SilentAim->FovColor.Value.w = SilentAimFovCol[ 3 ];
						}

						static float box_col[ 4 ] = { g_Config.ESP->BoxCol.Value.x, g_Config.ESP->BoxCol.Value.y, g_Config.ESP->BoxCol.Value.z,g_Config.ESP->BoxCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Box" ), box_col, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.ESP->BoxCol.Value.x = box_col[ 0 ];
							g_Config.ESP->BoxCol.Value.y = box_col[ 1 ];
							g_Config.ESP->BoxCol.Value.z = box_col[ 2 ];
							g_Config.ESP->BoxCol.Value.w = box_col[ 3 ];
						}

						static float filled_box_col[ 4 ] = { g_Config.ESP->FilledBoxCol.Value.x, g_Config.ESP->FilledBoxCol.Value.y, g_Config.ESP->FilledBoxCol.Value.z,g_Config.ESP->FilledBoxCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Filled Box" ), filled_box_col, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.ESP->FilledBoxCol.Value.x = filled_box_col[ 0 ];
							g_Config.ESP->FilledBoxCol.Value.y = filled_box_col[ 1 ];
							g_Config.ESP->FilledBoxCol.Value.z = filled_box_col[ 2 ];
							g_Config.ESP->FilledBoxCol.Value.w = filled_box_col[ 3 ];
						}


						static float skeleton_col[ 4 ] = { g_Config.ESP->SkeletonCol.Value.x, g_Config.ESP->SkeletonCol.Value.y, g_Config.ESP->SkeletonCol.Value.z,g_Config.ESP->SkeletonCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Skeleton" ), skeleton_col, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.ESP->SkeletonCol.Value.x = skeleton_col[ 0 ];
							g_Config.ESP->SkeletonCol.Value.y = skeleton_col[ 1 ];
							g_Config.ESP->SkeletonCol.Value.z = skeleton_col[ 2 ];
							g_Config.ESP->SkeletonCol.Value.w = skeleton_col[ 3 ];
						}

						static float lines_col[ 4 ] = { g_Config.ESP->SnapLinesCol.Value.x, g_Config.ESP->SnapLinesCol.Value.y, g_Config.ESP->SnapLinesCol.Value.z,g_Config.ESP->SnapLinesCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "SnapLines" ), lines_col, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.ESP->SnapLinesCol.Value.x = lines_col[ 0 ];
							g_Config.ESP->SnapLinesCol.Value.y = lines_col[ 1 ];
							g_Config.ESP->SnapLinesCol.Value.z = lines_col[ 2 ];
							g_Config.ESP->SnapLinesCol.Value.w = lines_col[ 3 ];
						}

						static float names_col[ 4 ] = { g_Config.ESP->UserNamesCol.Value.x, g_Config.ESP->UserNamesCol.Value.y, g_Config.ESP->UserNamesCol.Value.z,g_Config.ESP->UserNamesCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Names" ), names_col, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.ESP->UserNamesCol.Value.x = names_col[ 0 ];
							g_Config.ESP->UserNamesCol.Value.y = names_col[ 1 ];
							g_Config.ESP->UserNamesCol.Value.z = names_col[ 2 ];
							g_Config.ESP->UserNamesCol.Value.w = names_col[ 3 ];
						}

						static float weapon_names_col[ 4 ] = { g_Config.ESP->WeaponNameCol.Value.x, g_Config.ESP->WeaponNameCol.Value.y, g_Config.ESP->WeaponNameCol.Value.z,g_Config.ESP->WeaponNameCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Weapon Names" ), weapon_names_col, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.ESP->WeaponNameCol.Value.x = weapon_names_col[ 0 ];
							g_Config.ESP->WeaponNameCol.Value.y = weapon_names_col[ 1 ];
							g_Config.ESP->WeaponNameCol.Value.z = weapon_names_col[ 2 ];
							g_Config.ESP->WeaponNameCol.Value.w = weapon_names_col[ 3 ];
						}

						static float dist_col[ 4 ] = { g_Config.ESP->DistanceCol.Value.x, g_Config.ESP->DistanceCol.Value.y, g_Config.ESP->DistanceCol.Value.z,g_Config.ESP->DistanceCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Distance" ), dist_col, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.ESP->DistanceCol.Value.x = dist_col[ 0 ];
							g_Config.ESP->DistanceCol.Value.y = dist_col[ 1 ];
							g_Config.ESP->DistanceCol.Value.z = dist_col[ 2 ];
							g_Config.ESP->DistanceCol.Value.w = dist_col[ 3 ];
						}

					}
					ImGui::EndChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
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