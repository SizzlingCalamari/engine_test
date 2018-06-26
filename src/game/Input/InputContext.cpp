
#include "InputContext.h"
#include "utils.h"

uint KeyboardContext::AddMapping(KeyCombination keys, KeyboardEventCallback c)
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
