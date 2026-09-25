#pragma once
#include <Includes/Includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>

using namespace std;

namespace Login {

    static int iSubTabCount = 0;
    static float SubTabAlpha = 0.f;
    static int iSubTab = 0;

    ImVec2 window_center = ImVec2(g_MenuInfo.MenuSize.x * 0.5f, g_MenuInfo.MenuSize.y * 0.5f);

    void CentralizedText(const char* text, ImVec4 color, ImVec2 adjust = { 0, 0 }) {
        ImVec2 text_size = ImGui::CalcTextSize(text);
        ImGui::SetCursorPos({ window_center.x - text_size.x / 2 + adjust.x, window_center.y - text_size.y / 2 + adjust.y });
        ImU32 col_u32 = ImGui::GetColorU32(color);
        ImGui::TextColored(color, text);
    }

    void Render() {
        // Page de login simple avec juste un message
        CentralizedText("NEXORA", g_Col.PrimaryText, { 0, -100 });
        
        // Sous-titre
        ImGui::SetCursorPos({ window_center.x - 150, window_center.y - 60 });
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Système d'authentification");
        
        // Message d'attente
        ImGui::SetCursorPos({ window_center.x - 200, window_center.y });
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Vérification en cours...");
        
        // Spinner animation
        static float spinnerAngle = 0.f;
        spinnerAngle += ImGui::GetIO().DeltaTime * 360.f;
        
        ImGui::SetCursorPos({ window_center.x - 10, window_center.y + 40 });
        ImGui::Spinner("##spinner", 15, 3, ImColor(255, 255, 255));
        
        // Note
        ImGui::SetCursorPos({ window_center.x - 250, g_MenuInfo.MenuSize.y - 50 });
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "L'authentification Discord est gérée par un système externe");
    }
}
