
#pragma once

#include "ButtonCombination.h"
#include "utils.h"
#include <SDL_scancode.h>
#include <functional>
#include <array>

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
    uint AddMapping(KeyCombination keys, KeyboardEventCallback c)
    {
        // make sure the key combination doesn't have more than
        // the max number of keys
        static_assert(keys.size() <= ButtonCombination::MAX_BUTTON_COMBINATION, "");

        // copy the key combination over
        ButtonCombination b;
        b.m_buttons[0] = keys[0];
        b.m_buttons[1] = keys[1];
        b.m_buttons[2] = keys[2];
        b.m_buttons[3] = keys[3];

        // fast sort them for memory access order
        utils::sort4(b.m_buttons);

        // add the key combination and callback to the lists
        m_codes.emplace_back(b);
        m_callbacks.emplace_back(std::move(c));
        return static_cast<uint>(m_callbacks.size() - 1);
    }

private:
    // 1:1 mapping between buttons combinations and callbacks
    std::vector<ButtonCombination> m_codes;
    std::vector<KeyboardEventCallback> m_callbacks;
};
