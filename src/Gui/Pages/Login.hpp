#pragma once
#include <Includes/Includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>
#include <cmath>

using namespace std;

namespace Login {

    static int iSubTabCount = 0;
    static float SubTabAlpha = 0.f;
    static int iSubTab = 0;

    ImVec2 window_center = ImVec2(
        g_MenuInfo.MenuSize.x * 0.5f,
        g_MenuInfo.MenuSize.y * 0.5f
    );

    void CentralizedText(
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

    void Render() {

        // Page de login simple avec juste un message
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

        // Message d'attente
        ImGui::SetCursorPos({
            window_center.x - 200,
            window_center.y
        });

        ImGui::TextColored(
            ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
            "Vérification en cours..."
        );

        // Animation de chargement
        static float spinnerAngle = 0.0f;

        spinnerAngle += ImGui::GetIO().DeltaTime * 5.0f;

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 center = {
            window_center.x,
            window_center.y + 50
        };

        const int segments = 12;
        const float radius = 12.0f;

        for (int i = 0; i < segments; i++) {

            float angle =
                spinnerAngle +
                (2.0f * IM_PI * i / segments);

            float alpha =
                static_cast<float>(i + 1) / segments;

            ImVec2 point = {
                center.x + cosf(angle) * radius,
                center.y + sinf(angle) * radius
            };

            drawList->AddCircleFilled(
                point,
                2.5f,
                ImGui::GetColorU32(
                    ImVec4(
                        1.0f,
                        1.0f,
                        1.0f,
                        alpha
                    )
                )
            );
        }

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