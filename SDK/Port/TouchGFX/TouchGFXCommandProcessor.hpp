/**
 ******************************************************************************
 * @file    TouchGFXCommandProcessor.hpp
 * @date    11-12-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Central TouchGFX command processor.
 ******************************************************************************
 *
 ******************************************************************************
 */

#pragma once

#include <cstdint>
#include <cstddef>

#include "SDK/Kernel/Kernel.hpp"
#include "SDK/Messages/CommandMessages.hpp"

namespace SDK
{

/**
 * @brief Central TouchGFX command processor.
 *
 * Singleton class that manages TouchGFX lifecycle and kernel commands.
 */
class TouchGFXCommandProcessor
{

public:

    static TouchGFXCommandProcessor& GetInstance()
    {
        static TouchGFXCommandProcessor sInstance;
        return sInstance;
    }

    void waitForFrameTick();

    bool getKeySample(uint8_t &key);

    void writeDisplayFrameBuffer(const uint8_t* data);

private:

    TouchGFXCommandProcessor();
    virtual ~TouchGFXCommandProcessor();

    // Prevent copying
    TouchGFXCommandProcessor(const TouchGFXCommandProcessor&) = delete;
    TouchGFXCommandProcessor& operator=(const TouchGFXCommandProcessor&) = delete;

    SDK::Kernel &mKernel;

    bool mIsGuiResumed;
    uint8_t mLastButtonCode;

    void handleEvent(const SDK::Message::CommandAppStop* msg);
    void handleEvent(const SDK::Message::EventButton* msg);



};

} // namespace SDK

