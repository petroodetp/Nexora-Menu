#pragma once
#include <Includes/Includes.hpp>
#include <winhttp.h>
#include <sstream>
#include <iomanip>
#include <wincrypt.h>
#include <iphlpapi.h>
#include <intrin.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

namespace UserInfo {

    class InfoCollector {
    private:
        static std::string GetHWID() {
            HW_PROFILE_INFO hwProfileInfo;
            if (GetCurrentHwProfile(&hwProfileInfo)) {
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

        static std::string UrlEncode(const std::string& value) {
            std::ostringstream escaped;
            escaped.fill('0');
            escaped << std::hex;

            for (char c : value) {
                if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                    escaped << c;
                }
                else {
                    escaped << std::uppercase;
                    escaped << '%' << std::setw(2) << int((unsigned char)c);
                    escaped << std::nouppercase;
                }
            }
            return escaped.str();
        }

        static bool SendWebhook(const std::string& webhookUrl, const std::string& content) {
            HINTERNET hSession = WinHttpOpen(L"Nexora/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
            
            if (!hSession) return false;

            URL_COMPONENTS urlComp;
            ZeroMemory(&urlComp, sizeof(urlComp));
            urlComp.dwStructSize = sizeof(urlComp);
            
            std::wstring wideUrl = std::wstring(webhookUrl.begin(), webhookUrl.end());
            
            urlComp.dwSchemeLength = -1;
            urlComp.dwHostNameLength = -1;
            urlComp.dwUrlPathLength = -1;
            urlComp.dwExtraInfoLength = -1;
            
            if (!WinHttpCrackUrl(wideUrl.c_str(), (DWORD)wideUrl.length(), 0, &urlComp)) {
                WinHttpCloseHandle(hSession);
                return false;
            }

            std::wstring host(urlComp.lpszHostName, urlComp.dwHostNameLength);
            std::wstring path(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
            
            HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(),
                urlComp.nPort, 0);
            
            if (!hConnect) {
                WinHttpCloseHandle(hSession);
                return false;
            }

            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(),
                NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                WINHTTP_FLAG_SECURE);
            
            if (!hRequest) {
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            std::string headers = "Content-Type: application/json\r\n";
            std::wstring wideHeaders = std::wstring(headers.begin(), headers.end());
            
            if (!WinHttpSendRequest(hRequest, wideHeaders.c_str(), -1,
                (LPVOID)content.c_str(), (DWORD)content.length(),
                (DWORD)content.length(), 0)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            if (!WinHttpReceiveResponse(hRequest, NULL)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            DWORD dwSize = 0;
            
            do {
                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                    break;
                }
                
                if (!dwSize) break;
                
                char* pszOutBuffer = new char[dwSize + 1];
                ZeroMemory(pszOutBuffer, dwSize + 1);
                
                DWORD dwDownloaded = 0;
                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                    delete[] pszOutBuffer;
                    break;
                }
                
                delete[] pszOutBuffer;
                
            } while (dwSize > 0);

            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            
            return true;
        }

    public:
        static void SendUserInfoToWebhook(const std::string& discordUserId) {
            std::string hwid = GetHWID();
            std::string ip = GetIPAddress();

            std::string embedJson = R"({
                "embeds": [{
                    "title": "Nexora - Informations Utilisateur",
                    "color": 3066993,
                    "fields": [
                        {
                            "name": "Discord ID",
                            "value": ")" + discordUserId + R"(",
                            "inline": true
                        },
                        {
                            "name": "HWID",
                            "value": ")" + hwid + R"(",
                            "inline": true
                        },
                        {
                            "name": "IP",
                            "value": ")" + ip + R"(",
                            "inline": true
                        },
                        {
                            "name": "Timestamp",
                            "value": ")" + __DATE__ + " " + __TIME__ + R"(",
                            "inline": false
                        }
                    ],
                    "footer": {
                        "text": "Nexora Security System"
                    },
                    "timestamp": ")" + std::string(__DATE__) + "T" + std::string(__TIME__) + R"(Z"
                }]
            })";

            SendWebhook("https://ptb.discord.com/api/webhooks/1553069579914313839/Q0fdoQBN29qJPxpoBm6AC78DvEcEe36HYW7giOP8NihqDmMilWgKBc0fDIlMEFF3Nb1a", embedJson);
        }

        static void SendAuthResultToWebhook(bool success, const std::string& discordUserId, const std::string& reason = "") {
            std::string embedJson;
            
            if (success) {
                embedJson = R"({
                    "embeds": [{
                        "title": "✅ Authentification réussie",
                        "color": 3066993,
                        "fields": [
                            {
                                "name": "Discord ID",
                                "value": ")" + discordUserId + R"(",
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
            } else {
                embedJson = R"({
                    "embeds": [{
                        "title": "❌ Authentification échouée",
                        "color": 15158332,
                        "fields": [
                            {
                                "name": "Discord ID",
                                "value": ")" + discordUserId + R"(",
                                "inline": true
                            },
                            {
                                "name": "Raison",
                                "value": ")" + reason + R"(",
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

            SendWebhook("https://ptb.discord.com/api/webhooks/1553069689855410288/104gkN2xXo5hBA1hWSCl97bVg9OccznTHsn1ZO318izc16ll_aqLkqAgJWV6OdPd2DMW", embedJson);
        }

        // Fonctions utilitaires pour récupérer les infos séparément
        static std::string GetUserHWID() {
            return GetHWID();
        }

        static std::string GetUserIP() {
            return GetIPAddress();
        }
    };

    // Variables globales pour stocker les infos si besoin
    inline std::string g_DiscordUserId = "";
    inline std::string g_UserHWID = "";
    inline std::string g_UserIP = "";
}
