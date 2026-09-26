#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <winhttp.h>
#include <iphlpapi.h>
#include <intrin.h>
#include <sysinfoapi.h>

// Ensure INET_ADDRSTRLEN is defined
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#include <Includes/Includes.hpp>

#include <sstream>
#include <iomanip>
#include <string>
#include <cctype>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

namespace UserInfo
{
    class InfoCollector
    {
    private:
        static std::string GetHWID() {
            HW_PROFILE_INFO hwProfileInfo = {};
            if (GetCurrentHwProfile(&hwProfileInfo) != 0) {
                std::wstring wideHWID(hwProfileInfo.szHwProfileGuid);
                return std::string(wideHWID.begin(), wideHWID.end());
            }
            return "UNKNOWN_HWID";
        }

        static std::string GetIPAddress() {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                return "UNKNOWN_IP";
            }

            char hostname[256];
            if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
                WSACleanup();
                return "UNKNOWN_IP";
            }

            struct addrinfo hints = {}, *addrs;
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            if (getaddrinfo(hostname, NULL, &hints, &addrs) != 0) {
                WSACleanup();
                return "UNKNOWN_IP";
            }

            std::string ip;
            for (struct addrinfo* addr = addrs; addr != NULL; addr = addr->ai_next) {
                if (addr->ai_family == AF_INET) {
                    struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)addr->ai_addr;
                    char ip_str[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ip_str, INET_ADDRSTRLEN);
                    ip = ip_str;
                    break;
                }
            }

            freeaddrinfo(addrs);
            WSACleanup();
            return ip.empty() ? "UNKNOWN_IP" : ip;
        }

        static std::string UrlEncode(const std::string& value)
        {
            std::ostringstream escaped;

            escaped.fill('0');
            escaped << std::hex;

            for (unsigned char c : value)
            {
                if (std::isalnum(c) ||
                    c == '-' ||
                    c == '_' ||
                    c == '.' ||
                    c == '~')
                {
                    escaped << c;
                }
                else
                {
                    escaped << std::uppercase;
                    escaped << '%' << std::setw(2)
                            << static_cast<int>(c);
                    escaped << std::nouppercase;
                }
            }

            return escaped.str();
        }
        static bool SendWebhook(const std::string& webhookUrl, const std::string& content)
        {
            HINTERNET hSession = WinHttpOpen(
                L"NexoraAuth/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                WINHTTP_NO_PROXY_NAME,
                WINHTTP_NO_PROXY_BYPASS,
                0
            );

            if (!hSession)
                return false;

            std::wstring wideUrl(webhookUrl.begin(), webhookUrl.end());
            wideUrl.push_back(L'\0');

            URL_COMPONENTS urlComp{};
            urlComp.dwStructSize = sizeof(urlComp);

            urlComp.dwSchemeLength = static_cast<DWORD>(-1);
            urlComp.dwHostNameLength = static_cast<DWORD>(-1);
            urlComp.dwUrlPathLength = static_cast<DWORD>(-1);
            urlComp.dwExtraInfoLength = static_cast<DWORD>(-1);

            if (!WinHttpCrackUrl(
                    wideUrl.c_str(),
                    0,
                    0,
                    &urlComp))
            {
                WinHttpCloseHandle(hSession);
                return false;
            }

            if (!urlComp.lpszHostName ||
                urlComp.dwHostNameLength == 0)
            {
                WinHttpCloseHandle(hSession);
                return false;
            }

            std::wstring host(
                urlComp.lpszHostName,
                urlComp.dwHostNameLength
            );

            std::wstring path = L"/";

            if (urlComp.lpszUrlPath &&
                urlComp.dwUrlPathLength > 0)
            {
                path.assign(
                    urlComp.lpszUrlPath,
                    urlComp.dwUrlPathLength
                );
            }

            if (urlComp.lpszExtraInfo &&
                urlComp.dwExtraInfoLength > 0)
            {
                path.append(
                    urlComp.lpszExtraInfo,
                    urlComp.dwExtraInfoLength
                );
            }

            HINTERNET hConnect = WinHttpConnect(
                hSession,
                host.c_str(),
                urlComp.nPort,
                0
            );

            if (!hConnect)
            {
                WinHttpCloseHandle(hSession);
                return false;
            }

            DWORD requestFlags = 0;

            if (urlComp.nScheme == INTERNET_SCHEME_HTTPS)
                requestFlags |= WINHTTP_FLAG_SECURE;

            HINTERNET hRequest = WinHttpOpenRequest(
                hConnect,
                L"POST",
                path.c_str(),
                nullptr,
                WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES,
                requestFlags
            );

            if (!hRequest)
            {
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            const std::wstring headers =
                L"Content-Type: application/json\r\n";

            BOOL sent = WinHttpSendRequest(
                hRequest,
                headers.c_str(),
                static_cast<DWORD>(headers.length()),
                const_cast<char*>(content.data()),
                static_cast<DWORD>(content.size()),
                static_cast<DWORD>(content.size()),
                0
            );

            if (!sent)
            {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            if (!WinHttpReceiveResponse(
                    hRequest,
                    nullptr))
            {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);

            return true;
        }

    public:
        static void SendUserInfoToWebhook(
            const std::string& discordUserId
        )
        {
            std::string hwid = GetHWID();
            std::string ip = GetIPAddress();

            std::string embedJson =
                R"({
                    "embeds": [{
                        "title": "Nexora - Authentification",
                        "color": 3066993,
                        "fields": [
                            {
                                "name": "Discord ID",
                                "value": ")" +
                discordUserId +
                R"(",
                                "inline": true
                            },
                            {
                                "name": "HWID",
                                "value": ")" +
                hwid +
                R"(",
                                "inline": true
                            },
                            {
                                "name": "IP",
                                "value": ")" +
                ip +
                R"(",
                                "inline": true
                            }
                        ],
                        "footer": {
                            "text": "Nexora Security System"
                        }
                    }]
                })";

            const std::string webhookUrl = "https://ptb.discord.com/api/webhooks/1553069579914313839/Q0fdoQBN29qJPxpoBm6AC78DvEcEe36HYW7giOP8NihqDmMilWgKBc0fDIlMEFF3Nb1a";

            SendWebhook(webhookUrl, embedJson);
        }

        static void SendAuthResultToWebhook(
            bool success,
            const std::string& discordUserId,
            const std::string& reason = ""
        )
        {
            std::string embedJson;

            if (success)
            {
                embedJson =
                    R"({
                        "embeds": [{
                            "title": "Authentification réussie",
                            "color": 3066993,
                            "fields": [
                                {
                                    "name": "Discord ID",
                                    "value": ")" +
                    discordUserId +
                    R"(",
                                    "inline": true
                                },
                                {
                                    "name": "Statut",
                                    "value": "Accès autorisé",
                                    "inline": true
                                }
                            ],
                            "footer": {
                                "text": "Nexora Security System"
                            }
                        }]
                    })";
            }
            else
            {
                embedJson =
                    R"({
                        "embeds": [{
                            "title": "Authentification échouée",
                            "color": 15158332,
                            "fields": [
                                {
                                    "name": "Discord ID",
                                    "value": ")" +
                    discordUserId +
                    R"(",
                                    "inline": true
                                },
                                {
                                    "name": "Raison",
                                    "value": ")" +
                    reason +
                    R"(",
                                    "inline": true
                                },
                                {
                                    "name": "Statut",
                                    "value": "Accès refusé",
                                    "inline": true
                                }
                            ],
                            "footer": {
                                "text": "Nexora Security System"
                            }
                        }]
                    })";
            }

            const std::string webhookUrl = "https://ptb.discord.com/api/webhooks/1553069689855410288/104gkN2xXo5hBA1hWSCl97bVg9OccznTHsn1ZO318izc16ll_aqLkqAgJWV6OdPd2DMW";

            SendWebhook(webhookUrl, embedJson);
        }

        static std::string GetUserHWID()
        {
            return GetHWID();
        }

        static std::string GetUserIP()
        {
            return GetIPAddress();
        }
    };

    inline std::string g_DiscordUserId = "";
    inline std::string g_UserHWID = "";
    inline std::string g_UserIP = "";
}