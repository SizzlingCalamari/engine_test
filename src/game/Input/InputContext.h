
#pragma once

#include "SDL_scancode.h"
#include "ButtonCombination.h"
#include <vector>
#include "utils.h"
#include "stlutils.h"

// Stores info about a button combination
// event and what triggered it.
struct KeyboardEventInfo
{
    ButtonCombination buttons;
    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
    uint32 timestamp = 0;
    bool pressed = false;
};

typedef void (*KeyboardEventCallback)(const KeyboardEventInfo& info);

//
// Holds a set of button combinations
// mapped to callbacks for the input mapper.
//
class KeyboardContext
{
    friend class InputMapper;
public:
    bool AddMapping(std::vector<SDL_Scancode> keys, KeyboardEventCallback c)
    {
        // make sure the key combination doesn't have more than
        // the max number of keys
        assert(keys.size() <= ButtonCombination::MAX_BUTTON_COMBINATION);
        ButtonCombination b;

        // copy the buttons over
        int index = 0;
        for (auto key : keys)
        {
            b.m_buttons[index++] = key;
        }

        // fast sort them for memory access order
        utils::sort4(b.m_buttons);
        
        // check for duplicates
        if (stl_utils::find(m_codes, b))
        {
            return false;
        }
        
        // add the key combination and callback to the lists
        m_codes.emplace_back(b);
        m_callbacks.emplace_back(c);
        return true;
    }

private:
    // 1:1 mapping between buttons combinations and callbacks
    std::vector<ButtonCombination> m_codes;
    std::vector<KeyboardEventCallback> m_callbacks;
};
