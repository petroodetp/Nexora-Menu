#pragma once

#include <Includes/Includes.hpp>
#include <iostream>
#include <thread>
#include <cmath>

using namespace std;

namespace Login {

    inline bool IsAuthenticated = false;
    inline bool AuthInProgress = false;

    static int iSubTabCount = 0;
    static float SubTabAlpha = 0.f;
    static int iSubTab = 0;

    inline ImVec2 window_center = ImVec2(
        g_MenuInfo.MenuSize.x * 0.5f,
        g_MenuInfo.MenuSize.y * 0.5f
    );

    inline void CentralizedText(
        const char* text,
        ImVec4 color,
        ImVec2 adjust = { 0, 0 }
    ) {
        ImVec2 text_size = ImGui::CalcTextSize(text);

        ImGui::SetCursorPos({
            window_center.x - text_size.x / 2 + adjust.x,
            window_center.y - text_size.y / 2 + adjust.y
        });

        ImGui::TextColored(color, text);
    }

    inline void Render() {

        // Titre
        CentralizedText(
            "NEXORA",
            g_Col.PrimaryText,
            { 0, -100 }
        );

        // Sous-titre
        ImGui::SetCursorPos({
            window_center.x - 150,
            window_center.y - 60
        });

        ImGui::TextColored(
            ImVec4(0.8f, 0.8f, 0.8f, 1.0f),
            "Système d'authentification"
        );

        // Bouton Discord
        const ImVec2 buttonSize = ImVec2(260, 45);

        ImGui::SetCursorPos({
            window_center.x - buttonSize.x / 2,
            window_center.y
        });

        if (ImGui::Button(
            AuthInProgress ? "Discord ouvert" : "Connexion avec Discord",
            buttonSize
        )) {
            const char* AuthUrl =
                "https://nexora-auth.wispbyte.app/auth/discord";

            ShellExecuteA(
                nullptr,
                "open",
                AuthUrl,
                nullptr,
                nullptr,
                SW_SHOWNORMAL
            );

            AuthInProgress = true;
        }

        // Information
        ImGui::SetCursorPos({
            window_center.x - 210,
            window_center.y + 75
        });

        ImGui::TextColored(
            ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
            "Une fenêtre de navigateur va s'ouvrir."
        );

        // Note
        ImGui::SetCursorPos({
            window_center.x - 250,
            g_MenuInfo.MenuSize.y - 50
        });

        ImGui::TextColored(
            ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "L'authentification Discord est gérée par un système externe"
        );
    }
}