#pragma once
#include <Includes/Includes.hpp>
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <fstream>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")

namespace AuthChecker {

    static bool CheckAuthStatus() {
        // Ouvre une fenêtre pour l'utilisateur pour qu'il copie son token
        // En attendant une vraie intégration HTTP, c'est manuel
        
        HINTERNET hSession = WinHttpOpen(L"NexoraAuth/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        
        if (!hSession) return false;

        HINTERNET hConnect = WinHttpConnect(hSession, L"nexora-auth.wispbyte.app", 80, 0);
        
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return false;
        }

        std::wstring path = L"/api/check-auth";
        
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
            NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
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
        std::string response;
        
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
            
            response.append(pszOutBuffer, dwDownloaded);
            delete[] pszOutBuffer;
            
        } while (dwSize > 0);

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        // Vérifie la réponse (simplifié)
        return response.find("\"authenticated\":true") != std::string::npos;
    }

    static bool IsAuthenticated() {
        // Pour l'instant, on considère que si l'utilisateur a cliqué sur "Vérifier"
        // et que le token a été vérifié, il est authentifié
        // À modifier selon ton système
        return false;
    }
}