
#pragma once

#include "InputContext.h"
#include <SDL_scancode.h>
#include <SDL_events.h>
#include <bitset>
#include <vector>

class KeyboardContext;

//
// Takes in keyboard input events and
// performs callbacks on relevant button
// combination listeners.
//
class InputMapper
{
    friend class Engine;
public:
    InputMapper();
    ~InputMapper();

    void LoadContext(KeyboardContext&& context);

private:
    void ReceiveInput(const SDL_KeyboardEvent& e);
    void DispatchCallbacks();

private:
    std::vector<KeyboardContext> m_contexts;
    std::bitset<SDL_NUM_SCANCODES> m_key_states;

    std::vector<std::pair<KeyboardEventInfo, KeyboardEventCallback>> m_callback_queue;
};
