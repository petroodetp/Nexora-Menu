#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Includes/Includes.hpp>

using namespace std;

namespace Gui {

    void Rendering();

    inline DWORD ImGuiWindowFlags =
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

}