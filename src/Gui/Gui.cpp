#include <winsock2.h>
#include <ws2tcpip.h>

#include "Gui.hpp"

#include <Includes/CustomWidgets/Custom.hpp>
#include <Includes/CustomWidgets/WaterMarks.hpp>
#include <Includes/CustomWidgets/Notify.hpp>

#include <Gui/Pages/Combat.hpp>
#include <Gui/Pages/Visuals.hpp>
#include <Gui/Pages/Local.hpp>
#include <Gui/Pages/Exploits.hpp>
#include <Gui/Pages/World.hpp>
#include <Gui/Pages/Settings.hpp>
#include <Gui/Pages/Login.hpp>
#include <Security/UserInfo.hpp>

#include <Core/Features/Exploits/Exploits.hpp>
#include <Includes/CustomWidgets/Notify.hpp>
#include <Core/Features/Esp.hpp>

int PauseLoop;
inline std::mutex DrawMtx;

void Gui::Rendering( )
{
	ImGui::SetNextWindowSize( g_MenuInfo.MenuSize );

	if ( !PauseLoop ) { ImGui::SetNextWindowPos( g_Variables.g_vGameWindowSize / 2 - g_MenuInfo.MenuSize / 2 ); PauseLoop++; }

	ImGui::Begin( " ", nullptr, ImGuiWindowFlags );
	ImGui::PushFont( g_Variables.m_FontNormal );
	{
		Custom::DrawBackground( true );
		if ( !Login::IsAuthenticated )
        {
            Login::Render();

            ImGui::PopFont();
            ImGui::End();

            return;
        }

		ImGui::SetCursorPos( ImVec2( 12, 120 ) );

		ImGui::BeginGroup( );
		ImGui::PushFont( g_Variables.m_FontSecundary );
		{
			ImGui::TextColored( ImColor( 80, 80, 80 ), xorstr( "Aim Assist" ) );
			if ( Custom::Tab( ICON_FA_PERSON_RIFLE, xorstr( "Combat" ), g_MenuInfo.Combat == g_MenuInfo.iTabCount ) )
				g_MenuInfo.iTabCount = g_MenuInfo.Combat;

			ImGui::TextColored( ImColor( 80, 80, 80 ), xorstr( "Game" ) );
			if ( Custom::Tab( ICON_FA_BINOCULARS, xorstr( "Visuals" ), g_MenuInfo.Visuals == g_MenuInfo.iTabCount ) )
				g_MenuInfo.iTabCount = g_MenuInfo.Visuals;

			if ( Custom::Tab( ICON_FA_USER, xorstr( "Local" ), g_MenuInfo.Local == g_MenuInfo.iTabCount ) )
				g_MenuInfo.iTabCount = g_MenuInfo.Local;

			if ( Custom::Tab( ICON_FA_GLOBE, xorstr( "World" ), g_MenuInfo.World == g_MenuInfo.iTabCount ) )
				g_MenuInfo.iTabCount = g_MenuInfo.World;

			if ( Custom::Tab( ICON_FA_BARS, xorstr( "Exploits" ), g_MenuInfo.Exploits == g_MenuInfo.iTabCount ) )
				g_MenuInfo.iTabCount = g_MenuInfo.Exploits;

			ImGui::TextColored( ImColor( 80, 80, 80 ), xorstr( "Others" ) );
			if ( Custom::Tab( ICON_FA_GEAR, xorstr( "Settings" ), g_MenuInfo.Settings == g_MenuInfo.iTabCount ) )
				g_MenuInfo.iTabCount = g_MenuInfo.Settings;
		}
		ImGui::PopFont( );
		ImGui::EndGroup( );

		g_MenuInfo.TabAlpha = ImClamp( g_MenuInfo.TabAlpha + ( 5.f * ImGui::GetIO( ).DeltaTime * ( g_MenuInfo.iTabCount == g_MenuInfo.iCurrentPage ? 1.f : -1.f ) ), 0.f, 1.f );
		if ( g_MenuInfo.TabAlpha == 0.f )
			g_MenuInfo.iCurrentPage = g_MenuInfo.iTabCount;

		ImGuiStyle * style = &ImGui::GetStyle( );
		ImGui::PushStyleVar( ImGuiStyleVar_Alpha, g_MenuInfo.TabAlpha * style->Alpha );

		ImGui::SetCursorPos( ImVec2( 176, 16 ) );
		ImGui::BeginGroup( );

		// Ici, tu peux ajouter ta propre logique d'authentification
		// Pour l'instant, on affiche toujours le menu
		// Exemple: if (g_IsAuthenticated) { ... } else { Login::Render(); }
		
		switch (g_MenuInfo.iCurrentPage)
		{
		case g_MenuInfo.Combat:   Combat::Render();   break;
		case g_MenuInfo.Visuals:  Visuals::Render();  break;
		case g_MenuInfo.Local:    Local::Render();    break;
		case g_MenuInfo.World:    World::Render();    break;
		case g_MenuInfo.Exploits: Exploits::Render(); break;
		case g_MenuInfo.Settings: Settings::Render(); break;
		}

		ImGui::EndGroup();

		HWND ActiveWindow = GetForegroundWindow( );

		{
			std::lock_guard<std::mutex> Lock( DrawMtx );

			NotifyManager::Render( );

			if ( ActiveWindow == g_Variables.g_hGameWindow )
			{
				if ( GetAsyncKeyState( g_Config.Player->GodModeKey ) & 1 )
				{
					g_Config.Player->EnableGodMode = !g_Config.Player->EnableGodMode;
					Core::SDK::Pointers::pLocalPlayer->SetGodMode( g_Config.Player->EnableGodMode );
					std::thread( [ & ] ( ) {
						NotifyManager::Send( xorstr( "GodMode " ) + ( std::string )( g_Config.Player->EnableGodMode ? xorstr( "enabled" ) : xorstr( "disabled" ) ), 2000 );
					} ).detach( );
				}

				if ( GetAsyncKeyState( g_Config.Player->NoClipKey ) & 1 )
				{
					g_Config.Player->NoClipEnabled = !g_Config.Player->NoClipEnabled;
					Core::SDK::Pointers::pLocalPlayer->FreezePed( g_Config.Player->NoClipEnabled );
					std::thread( [ & ] ( ) {
						NotifyManager::Send( xorstr( "NoClip " ) + ( std::string )( g_Config.Player->NoClipEnabled ? xorstr( "enabled" ) : xorstr( "disabled" ) ), 2000 );
					} ).detach( );
				}

				if ( g_Config.Player->NoClipEnabled )
					Features::Exploits::NoClip( );

				struct FovFuncs_t {
					bool * Enabled;
					int * FovSize;
					ImVec4 FovColor;
				};

				std::vector<FovFuncs_t> FovDrawList = {
					FovFuncs_t( &g_Config.Aimbot->ShowFov,    &g_Config.Aimbot->FOV,    g_Config.Aimbot->FovColor    ),
					FovFuncs_t( &g_Config.SilentAim->ShowFov, &g_Config.SilentAim->FOV, g_Config.SilentAim->FovColor ),
					FovFuncs_t( &g_Config.TriggerBot->ShowFov,&g_Config.TriggerBot->FOV,g_Config.TriggerBot->FovColor),
				};

				static std::vector<float> Alphas( FovDrawList.size( ), 0.0f );
				static std::vector<float> Sizes( FovDrawList.size( ), 0.0f );

				for ( int i = 0; i < ( int )FovDrawList.size( ); ++i )
				{
					auto & Fov = FovDrawList[ i ];
					Alphas[ i ] = ImClamp( ImLerp( Alphas[ i ], *Fov.Enabled ? 1.f : 0.f, ImGui::GetIO( ).DeltaTime * 10.f ), 0.f, 1.f );
					Sizes[ i ]  = ImLerp( Sizes[ i ], ( float )*Fov.FovSize, ImGui::GetIO( ).DeltaTime * 12.f );
					ImGui::PushStyleVar( ImGuiStyleVar_Alpha, Alphas[ i ] );
					ImGui::GetBackgroundDrawList( )->AddCircle( ImVec2( g_Variables.g_vGameWindowCenter.x, g_Variables.g_vGameWindowCenter.y ), Sizes[ i ], ImGui::GetColorU32( Fov.FovColor ), 999 );
					ImGui::PopStyleVar( );
				}

				if ( g_Config.General->WaterMark )
					Custom::WaterMark::Render( );

				ImGui::PushFont( g_Variables.m_DrawFont );
				Features::g_Esp.Draw( );
				Features::g_Esp.DrawVehicle( );
				ImGui::PopFont( );
			}
			else
			{
				if ( ImGui::GetStyle( ).Alpha >= 0.9f )
				{
					g_MenuInfo.IsOpen = false;
					SetWindowLong( g_Variables.g_hCheatWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT );
				}
			}
		}

		ImGui::PopStyleVar( );
		ImGui::PopFont( );
	}
	ImGui::End( );
}
