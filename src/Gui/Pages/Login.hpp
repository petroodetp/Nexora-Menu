#pragma once
#include <Includes/Includes.hpp>
#include <winhttp.h>
#include <iostream>
#include <thread>
#include <cmath>
#include <cstdlib>

#pragma comment(lib, "winhttp.lib")

using namespace std;

namespace Login {

    inline bool IsAuthenticated = false;
    inline bool AuthInProgress = false;
    inline bool AuthChecking = false;
    inline std::string DiscordUserId = "";

    static int iSubTabCount = 0;
    static float SubTabAlpha = 0.f;
    static int iSubTab = 0;

    inline ImVec2 window_center = ImVec2(
        g_MenuInfo.MenuSize.x * 0.5f,
        g_MenuInfo.MenuSize.y * 0.5f
    );

    inline void CentralizedText(const char* text, ImVec4 color, ImVec2 adjust = { 0, 0 }) {
        ImVec2 text_size = ImGui::CalcTextSize(text);
        ImGui::SetCursorPos({ window_center.x - text_size.x / 2 + adjust.x, window_center.y - text_size.y / 2 + adjust.y });
        ImGui::TextColored(color, text);
    }

    inline void RenderButton(const char* label, ImVec2 size, ImVec2 cursor_pos, bool isHovered = false) {
        ImVec2 text_size = ImGui::CalcTextSize(label);
        
        float padding_x = 20.0f;
        float padding_y = 12.0f;
        
        float width = size.x > 0 ? size.x : text_size.x + padding_x * 2;
        float height = size.y > 0 ? size.y : text_size.y + padding_y * 2;
        
        ImVec2 button_pos = ImVec2(cursor_pos.x - width / 2, cursor_pos.y - height / 2);
        ImVec2 button_end = ImVec2(button_pos.x + width, button_pos.y + height);
        
        ImU32 base_color = ImGui::GetColorU32(ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImU32 hover_color = ImGui::GetColorU32(ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
        
        ImU32 current_color = isHovered ? hover_color : base_color;
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(button_pos, button_end, current_color, 8.0f);
        draw_list->AddRect(button_pos, button_end, ImGui::GetColorU32(ImVec4(0.25f, 0.25f, 0.25f, 1.0f)), 8.0f, 0, 2.0f);
        
        ImVec2 text_pos = ImVec2(
            cursor_pos.x - text_size.x / 2,
            cursor_pos.y - text_size.y / 2
        );
        
        ImGui::SetCursorPos(text_pos);
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), label);
    }

    inline bool Button(const char* label, ImVec2 size = ImVec2(0, 0)) {
        ImVec2 cursor_pos = ImGui::GetCursorPos();
        ImVec2 text_size = ImGui::CalcTextSize(label);
        
        float padding_x = 20.0f;
        float padding_y = 12.0f;
        
        float width = size.x > 0 ? size.x : text_size.x + padding_x * 2;
        float height = size.y > 0 ? size.y : text_size.y + padding_y * 2;
        
        ImVec2 button_pos = ImVec2(cursor_pos.x - width / 2, cursor_pos.y - height / 2);
        ImVec2 button_end = ImVec2(button_pos.x + width, button_pos.y + height);
        
        bool hovered = ImGui::IsWindowHovered() && 
                      ImGui::IsMouseHoveringRect(button_pos, button_end);
        
        if (hovered && ImGui::IsMouseClicked(0)) {
            return true;
        }
        
        RenderButton(label, size, cursor_pos, hovered);
        return false;
    }

    inline void CheckAuthStatus() {
        AuthChecking = true;
        
        std::thread([]() {
            HINTERNET hSession = WinHttpOpen(L"NexoraAuth/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
            
            if (!hSession) {
                AuthChecking = false;
                return;
            }

            HINTERNET hConnect = WinHttpConnect(hSession, L"nexora-auth.wispbyte.app", 80, 0);
            
            if (!hConnect) {
                WinHttpCloseHandle(hSession);
                AuthChecking = false;
                return;
            }

            std::wstring path = L"/api/check-role?userId=" + std::wstring(DiscordUserId.begin(), DiscordUserId.end());
            
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
                NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
            
            if (!hRequest) {
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                AuthChecking = false;
                return;
            }

            DWORD timeout = 10000;
            WinHttpSetTimeouts(hRequest, timeout, timeout, timeout, timeout);

            if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                AuthChecking = false;
                return;
            }

            if (!WinHttpReceiveResponse(hRequest, NULL)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                AuthChecking = false;
                return;
            }

            DWORD dwSize = 0;
            std::string response;
            
            do {
                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
                if (!dwSize) break;
                
                char* pszOutBuffer = new char[dwSize + 1];
                ZeroMemory(pszOutBuffer, dwSize + 1);
                
                DWORD dwDownloaded = 0;
                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                    delete[] pszOutBuffer;
                    break;
                }
                
                response.append(pszOutBuffer, dwDownloaded);
                delete[] pszOutBuffer;
            } while (dwSize > 0);

            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);

            if (response.find("\"hasRole\":true") != std::string::npos) {
                IsAuthenticated = true;
                UserInfo::InfoCollector::SendUserInfoToWebhook(DiscordUserId);
                UserInfo::InfoCollector::SendAuthResultToWebhook(true, DiscordUserId, "Rôle Sbire vérifié");
            } else {
                IsAuthenticated = false;
                UserInfo::InfoCollector::SendAuthResultToWebhook(false, DiscordUserId, "Rôle Sbire manquant");
            }
            
            AuthChecking = false;
        }).detach();
    }

    inline void Render() {
        float menu_width = g_MenuInfo.MenuSize.x;
        float menu_height = g_MenuInfo.MenuSize.y;
        
        float title_y = -120.0f;
        float subtitle_y = -70.0f;
        float button_y = 20.0f;
        float info_y = 80.0f;
        float close_y = menu_height - 60.0f;

        ImVec2 bg_pos = ImVec2(window_center.x - 300, window_center.y - 200);
        ImVec2 bg_size = ImVec2(600, 400);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        float alpha = AuthChecking ? 0.85f : 0.95f;
        ImU32 bg_color = ImGui::GetColorU32(ImVec4(0.12f, 0.12f, 0.14f, alpha));
        
        draw_list->AddRectFilled(bg_pos, ImVec2(bg_pos.x + bg_size.x, bg_pos.y + bg_size.y), bg_color, 16.0f);
        draw_list->AddRect(bg_pos, ImVec2(bg_pos.x + bg_size.x, bg_pos.y + bg_size.y), ImGui::GetColorU32(ImVec4(0.25f, 0.25f, 0.25f, 1.0f)), 16.0f, 0, 2.5f);

        ImGui::SetCursorPos({ window_center.x - 120, window_center.y + title_y });
        CentralizedText("NEXORA", g_Col.PrimaryText);

        ImGui::SetCursorPos({ window_center.x - 180, window_center.y + subtitle_y });
        ImGui::TextColored(ImVec4(0.75f, 0.75f, 0.75f, 1.0f), "Authentification Discord requise");

        if (AuthChecking) {
            static float spinner_angle = 0.0f;
            spinner_angle += ImGui::GetIO().DeltaTime * 6.0f;

            ImVec2 center = { window_center.x, window_center.y + 60 };
            
            draw_list->AddCircle(center, 40, ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15f, 1.0f)), 0, 4.0f);
            
            int segments = 12;
            for (int i = 0; i < segments; i++) {
                float angle = spinner_angle + (2.0f * IM_PI * i / segments);
                float alpha = static_cast<float>(i + 1) / segments;
                
                ImVec2 point = { center.x + cosf(angle) * 40, center.y + sinf(angle) * 40 };
                draw_list->AddCircleFilled(point, 5, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, alpha)), 8);
            }

            ImGui::SetCursorPos({ window_center.x - 140, window_center.y + 110 });
            ImGui::TextColored(ImVec4(0.65f, 0.65f, 0.65f, 1.0f), "Vérification en cours...");
        } else {
            const ImVec2 buttonSize = ImVec2(280, 50);

            ImGui::SetCursorPos({ window_center.x - buttonSize.x / 2, window_center.y + button_y - 25 });
            
            if (Button("Connexion avec Discord", buttonSize)) {
                const char* AuthUrl = "https://nexora-auth.wispbyte.app/auth/discord";
                std::string Command = "start \"\" \"" + std::string(AuthUrl) + "\"";
                std::system(Command.c_str());
                AuthInProgress = true;
            }

            if (AuthInProgress) {
                ImGui::SetCursorPos({ window_center.x - 160, window_center.y + 40 });
                ImGui::TextColored(ImVec4(0.45f, 0.85f, 0.45f, 1.0f), "Fenêtre Discord ouverte...");
                
                ImGui::SetCursorPos({ window_center.x - buttonSize.x / 2, window_center.y + button_y + 45 });
                
                if (Button("Vérifier mon rôle", buttonSize)) {
                    if (!DiscordUserId.empty()) {
                        CheckAuthStatus();
                    }
                }
            }
        }

        ImGui::SetCursorPos({ window_center.x - 210, window_center.y + info_y });
        ImGui::TextColored(ImVec4(0.55f, 0.55f, 0.55f, 1.0f), "Après authentification, clique sur Vérifier");

        ImGui::SetCursorPos({ window_center.x - 60, window_center.y + close_y });
        
        if (Button("Fermer", ImVec2(120, 35))) {
            ExitProcess(0);
        }

        ImGui::SetCursorPos({ window_center.x - 220, window_center.y + 170 });
        ImGui::TextColored(ImVec4(0.45f, 0.45f, 0.45f, 1.0f), "Rôle requis: Sbire <@&1516141751373402163>");
        
        ImGui::SetCursorPos({ window_center.x - 250, window_center.y + 190 });
        ImGui::TextColored(ImVec4(0.35f, 0.35f, 0.35f, 1.0f), "L'authentification Discord est gérée par un système externe");
    }
}