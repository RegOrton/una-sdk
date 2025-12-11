/**
 ******************************************************************************
 * @file    TouchGFXCommandProcessor.cpp
 * @date    11-12-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Central TouchGFX command processor.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "SDK/Port/TouchGFX/TouchGFXCommandProcessor.hpp"

#define LOG_MODULE_PRX      "TouchGFXCommandProcessor"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Kernel/KernelProviderGUI.hpp"



namespace SDK
{

TouchGFXCommandProcessor::TouchGFXCommandProcessor()
          : mKernel(SDK::KernelProviderGUI::GetInstance().getKernel())
          , mIsGuiResumed(false)
          , mLastButtonCode(0)
{
}

TouchGFXCommandProcessor::~TouchGFXCommandProcessor()
{

}

void TouchGFXCommandProcessor::waitForFrameTick()
{
    mLastButtonCode = 0;

        SDK::MessageBase *msg;

        while (true) {

            // Wait for command (blocks until available)
            if(!mKernel.comm.getMessage(msg)) {
                continue;
            }

            switch (msg->getType()) {

                case SDK::MessageType::COMMAND_APP_STOP: {
                    handleEvent(static_cast<SDK::Message::CommandAppStop*>(msg));
                } break;

                case SDK::MessageType::EVENT_GUI_TICK: {
                    mKernel.comm.releaseMessage(msg);
                    return; // Allow TouchGFX make frame
                } break;

                case SDK::MessageType::EVENT_BUTTON: {
                    handleEvent(static_cast<SDK::Message::EventButton*>(msg));
                }

                case SDK::MessageType::COMMAND_APP_GUI_RESUME: {
                    mIsGuiResumed = true;
                    // onResumeCallback

                } break;

                case SDK::MessageType::COMMAND_APP_GUI_SUSPEND: {
                    mIsGuiResumed = false;
                    // onSuspendCallback
                } break;


                default:
                    // onUserCallback
                    break;
            }

            // Release message after processing
            kernel.comm.releaseMessage(msg);

        }

}



bool TouchGFXCommandProcessor::getKeySample(uint8_t &key)
{
    key = mLastButtonCode;
}

void TouchGFXCommandProcessor::writeDisplayFrameBuffer(const uint8_t* data)
{
    if (!data || !mIsGuiResumed) {
        return;
    }

    auto* msg = mKernel.comm.allocateMessage<SDK::Message::RequestDisplayUpdate>();
    if (msg) {
        msg->pBuffer = data;
        mKernel.comm.sendMessage(msg);
        mKernel.comm.releaseMessage(msg);
    }
}

void TouchGFXCommandProcessor::handleEvent(SDK::Message::CommandAppStop* msg)
{
    // We must release message because this is the last event.
    mKernel.comm.releaseMessage(msg);

    // onStopCallback

    // Waiting for the kernel to kill this app
    mKernel.sys.exit(0); // no return
}

void TouchGFXCommandProcessor::handleEvent(SDK::Message::EventButton* msg)
{
    if (msg->event == SDK::Message::EventButton::Event::CLICK) {
        switch (msg->id) {
            case SDK::Message::EventButton::Id::SW1: mLastButtonCode = '1'; break;
            case SDK::Message::EventButton::Id::SW2: mLastButtonCode = '3'; break;
            case SDK::Message::EventButton::Id::SW3: mLastButtonCode = '2'; break;
            case SDK::Message::EventButton::Id::SW4: mLastButtonCode = '4'; break;
            default: break;
        }
    }
}

} // namespace SDK
