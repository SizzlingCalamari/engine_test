
#pragma once

#include "SDL_scancode.h"

//
// Holds a number of buttons grouped together
// to represent a button combination.
// The max number of buttons in a combination
// is ButtonCombination::MAX_BUTTON_COMBINATION
//
struct ButtonCombination
{
    ButtonCombination():
        m_allbuttons(0)
    {
    }

    static const uint MAX_BUTTON_COMBINATION = 4;
    union
    {
        uint64 m_allbuttons;
        uint16 m_buttons[MAX_BUTTON_COMBINATION];
    };

    static inline bool has_button(ButtonCombination buttons, SDL_Scancode button)
    {
        bool ret = false;
        ret |= (buttons.m_buttons[0] == button);
        ret |= (buttons.m_buttons[1] == button);
        ret |= (buttons.m_buttons[2] == button);
        ret |= (buttons.m_buttons[3] == button);
        return ret;
    }
};

inline bool operator==(ButtonCombination a, ButtonCombination b)
{
    return a.m_allbuttons == b.m_allbuttons;
}
