
#pragma once

#include "SDL_scancode.h"
#include "SDL_events.h"
#include "ButtonCombination.h"
#include <vector>

class KeyboardContext;

//
// Takes in keyboard input events and
// performs callbacks on relevant button
// combination listeners.
//
class InputMapper
{
public:
    InputMapper():
        m_key_states(SDL_NUM_SCANCODES, false)
    {
        // Key 0 needs to be set to match unused
        // keys in button combinations.
        m_key_states[0] = true;
    }

    void LoadContext(KeyboardContext&& context)
    {
        m_contexts.emplace_back(context);
    }

    void ReceiveInput(const SDL_KeyboardEvent& e)
    {
        // discard repeat inputs
        if (e.repeat != 0)
        {
            return;
        }

        // populate the callback info struct
        KeyboardEventInfo info;
        info.scancode = e.keysym.scancode;
        info.timestamp = e.timestamp;
        info.pressed = e.type == SDL_KEYDOWN;

        // Set the key state to true, so we can match
        // button combinations with this key.
        // The real state is set after the callbacks are run.
        m_key_states[info.scancode] = true;

        // for each context and button combination...
        for (auto &context : m_contexts)
        {
            int index = 0;
            for (auto keys : context.m_codes)
            {
                // If the input key is part of a button combo, 
                // and the rest of the buttons in that combo are set...
                if (ButtonCombination::has_button(keys, info.scancode) &&
                    m_key_states[keys.m_buttons[0]] &&
                    m_key_states[keys.m_buttons[1]] &&
                    m_key_states[keys.m_buttons[2]] &&
                    m_key_states[keys.m_buttons[3]])
                {
                    // Set the key combination and run the callback.
                    info.buttons = keys;
                    context.m_callbacks[index](info);
                }
                ++index;
            }
        }

        // set the key state to it's correct value
        m_key_states[info.scancode] = info.pressed;
    }

private:
    std::vector<KeyboardContext> m_contexts;
    std::vector<bool> m_key_states;
};
