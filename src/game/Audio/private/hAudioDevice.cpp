
#include "../public/hAudioDevice.h"
#include "hAudioCommon.h"
#include "hAudioDeviceImpl.h"

namespace SDLAudio
{
    const AudioDeviceDesc& AudioDevice::GetDeviceDesc() const
    {
        return ToImpl(this)->GetDeviceDesc();
    }

    void AudioDevice::StartPlayback()
    {
        ToImpl(this)->StartPlayback();
    }

    void AudioDevice::PushAudio(const void* data, int numBytes)
    {
        return ToImpl(this)->PushAudio(data, numBytes);
    }
}
