
#include "InputMapper.h"

#include "ButtonCombination.h"

InputMapper::InputMapper()
{
    // Key 0 needs to be set to match unused
    // keys in button combinations.
    m_key_states[0] = true;
}

InputMapper::~InputMapper() = default;

void InputMapper::LoadContext(KeyboardContext&& context)
{
    // TODO:
    // make sure that the callbacks are unique,
    // else the mapping ids will collide with each other
    m_contexts.emplace_back(context);
}

void InputMapper::ReceiveInput(const SDL_KeyboardEvent& e)
{
    // discard repeat inputs
    if (e.repeat != 0)
    {
        return;
    }

    // populate the callback info struct
    KeyboardEventInfo info;
    info.trigger = e.keysym.scancode;
    info.timestamp = e.timestamp;
    info.pressed = e.type == SDL_KEYDOWN;

    // Set the key state to true, so we can match
    // button combinations with this key.
    // The real state is set after the callbacks are run.
    m_key_states[info.trigger] = true;

    // for each context and button combination...
    for (auto &context : m_contexts)
    {
        int numCombinations = static_cast<int>(context.m_codes.size());
        for (int i = 0; i < numCombinations; ++i)
        {
            ButtonCombination& keys = context.m_codes[i];
            // If the input key is part of a button combo, 
            // and the rest of the buttons in that combo are set...
            if (ButtonCombination::has_button(keys, info.trigger) &&
                m_key_states[keys.m_buttons[0]] &&
                m_key_states[keys.m_buttons[1]] &&
                m_key_states[keys.m_buttons[2]] &&
                m_key_states[keys.m_buttons[3]])
            {
                // Set the mapping id and queue the callback.
                info.mappingId = i;
                m_callback_queue.emplace_back(info, context.m_callbacks[i]);
            }
        }
    }

    // set the key state to it's correct value
    m_key_states[info.trigger] = info.pressed;
}

void InputMapper::DispatchCallbacks()
{
    int num_events = SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_MOUSEWHEEL);
    std::vector<SDL_Event> events(num_events);

    SDL_PeepEvents(events.data(), num_events, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_MOUSEWHEEL);

    for (const SDL_Event &e : events)
    {
        switch (e.type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            ReceiveInput(e.key);
            break;
        default:
            break;
        }
    }
    for (auto& func : m_callback_queue)
    {
        func.second(func.first);
    }
    m_callback_queue.clear();
}
