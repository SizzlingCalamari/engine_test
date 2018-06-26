
#pragma once

#include "ButtonCombination.h"
#include <SDL_scancode.h>
#include <functional>
#include <array>
#include <vector>

// Stores info about a button combination
// event and what triggered it.
struct KeyboardEventInfo
{
    uint32 mappingId = static_cast<uint>(-1);
    uint32 timestamp = 0;
    SDL_Scancode trigger = SDL_SCANCODE_UNKNOWN;
    bool pressed = false;
};

using KeyboardEventCallback = std::function<void(const KeyboardEventInfo& info)>;
using KeyCombination = std::array<SDL_Scancode, 4>;

//
// Holds a set of button combinations
// mapped to callbacks for the input mapper.
//
class KeyboardContext
{
    friend class InputMapper;
public:
    uint AddMapping(KeyCombination keys, KeyboardEventCallback c);

private:
    // 1:1 mapping between buttons combinations and callbacks
    std::vector<ButtonCombination> m_codes;
    std::vector<KeyboardEventCallback> m_callbacks;
};
