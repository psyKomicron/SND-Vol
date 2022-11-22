﻿#include "pch.h"
#include "AudioProfile.h"
#if __has_include("AudioProfile.g.cpp")
#include "AudioProfile.g.cpp"
#endif

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;

namespace winrt::SND_Vol::implementation
{
    void AudioProfile::Save(const ApplicationDataContainer& container)
    {
        ApplicationDataContainer props = container.Containers().TryLookup(profileName);
        if (!props)
        {
            props = container.CreateContainer(profileName, ApplicationDataCreateDisposition::Always);
        }

        props.Values().Insert(L"IsDefaultProfile", IReference(isDefaultProfile));
        props.Values().Insert(L"DisableAnimations", IReference(disableAnimations));
        props.Values().Insert(L"ShowAdditionalButtons", IReference(showAdditionalButtons));
        props.Values().Insert(L"KeepOnTop", IReference(keepOnTop));
        props.Values().Insert(L"ShowMenu", IReference(showMenu));
        props.Values().Insert(L"SystemVolume", IReference(systemVolume));
        props.Values().Insert(L"Layout", IReference(layout));

        ApplicationDataCompositeValue audioLevelsContainer{};
        for (auto&& pair : audioStates)
        {
            hstring name = pair.Key();
            float level = pair.Value();
            audioLevelsContainer.Insert(name, IReference(level));
        }
        props.Values().Insert(L"AudioLevels", audioLevelsContainer);

        ApplicationDataCompositeValue audioStatesContainer{};
        for (auto&& pair : audioStates)
        {
            hstring name = pair.Key();
            bool muted = pair.Value();
            audioStatesContainer.Insert(name, IReference(muted));
        }
        props.Values().Insert(L"AudioStates", audioStatesContainer);
    }

    void AudioProfile::Restore(const ApplicationDataContainer& container)
    {
        profileName = container.Name();
        isDefaultProfile = unbox_value<bool>(container.Values().Lookup(L"IsDefaultProfile"));
        disableAnimations = unbox_value<bool>(container.Values().Lookup(L"DisableAnimations"));
        showAdditionalButtons = unbox_value<bool>(container.Values().Lookup(L"ShowAdditionalButtons"));
        keepOnTop = unbox_value<bool>(container.Values().Lookup(L"KeepOnTop"));
        showMenu = unbox_value<bool>(container.Values().Lookup(L"ShowMenu"));
        systemVolume = unbox_value<float>(container.Values().Lookup(L"SystemVolume"));
        layout = unbox_value<uint32_t>(container.Values().Lookup(L"Layout"));

        auto audioLevelsContainer = container.Values().Lookup(L"AudioLevels").as<ApplicationDataCompositeValue>();
        auto audioStatesContainer = container.Values().Lookup(L"AudioStates").as<ApplicationDataCompositeValue>();

        for (auto value : audioLevelsContainer)
        {
            hstring name = value.Key();
            float level = value.Value().as<float>();

            audioLevels.Insert(name, level);
        }

        for (auto value : audioStatesContainer)
        {
            hstring name = value.Key();
            bool muted = value.Value().as<bool>();

            audioLevels.Insert(name, muted);
        }
    }
}
