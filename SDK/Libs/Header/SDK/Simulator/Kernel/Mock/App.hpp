/**
 ******************************************************************************
 * @file    App.hpp
 * @date    04-02-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Mock for IUserApp interface.
 ******************************************************************************
 *
 ******************************************************************************
 */

#pragma once

#include <cstdint>
#include <cstdarg>
#include <stdio.h>
#include <windows.h>

#include "SDK/Platform/OS/OS.hpp"
#include "SDK/Interfaces/IApp.hpp"
#include "SDK/Interfaces/IGlance.hpp"

#include <platform/hal/simulator/sdl2/HALSDL2.hpp>
#include "touchgfx/Utils.hpp"

namespace SDK::Simulator::Mock
{

class App : public SDK::Interface::IApp {

public:

    class FakeMutex : public SDK::Interface::IMutex
    {
    public:
        FakeMutex() = default;

        void lock()    override {}
        void unLock()  override {}
        bool tryLock() override { return true; }
    };

    App(bool useMutex = true)
        : mpCallback(nullptr)
        , mpGlance(nullptr)
        , mState(State::DESTROYED)
        , mMutex()
        , mFakeMutex()
        , mAppMutex(useMutex ? static_cast<SDK::Interface::IMutex*>(&mMutex)
                    : static_cast<SDK::Interface::IMutex*>(&mFakeMutex))
    {}

    virtual ~App() = default;

    virtual void registerApp(SDK::Interface::IApp::Callback *pCallback) override
    {
        mpCallback = pCallback;
    }

    void registerGlance(SDK::Interface::IGlance* glance) override
    {
        mpGlance = glance;
    }

    void getGlanceArea(int16_t& width, int16_t& height) override
    {
        width = 240;
        height = 60;
    }

    LaunchReason getLaunchReason() override
    {
        return LaunchReason::AUTO_START;
    }

    virtual void initialized() override
    {
        mAppMutex->lock();
    }

    virtual void waitForFrame() override { }
    virtual void pauseRequest() override 
    { 
        pause();
    }

    virtual void resumeRequest() override 
    {
        resume();
    }

    virtual void restartRequest() override
    {
        exit();
    }
    
    virtual void exit(int status = 0) override
    {
        static_cast<touchgfx::HALSDL2 *>(touchgfx::HAL::getInstance())->stopApplication();
    }

    virtual void getDisplayResolution(int16_t &width, int16_t &height) override
    {
        width = 240; 
        height = 240;
    }
    
    virtual uint8_t getDisplayColorDepth() override
    {
        return 6;
    }

    virtual void writeFrameBuffer(const uint8_t *pBuf) override { }
    virtual bool keySample(uint8_t &key) override
    {
        return false;
    }
    

    virtual uint32_t getTimeMs() override 
    { 
        return static_cast<uint32_t>(GetTickCount64());
    }
    
    virtual void delay(uint32_t ms) override
    {
        mAppMutex->unLock();
        Sleep(ms);
        mAppMutex->lock();
    }

    virtual void yield() override { }

    virtual void lock() override
    {
        mAppMutex->lock();
    }

    virtual void unLock() override
    {
        mAppMutex->unLock();
    }


    enum class State {
        DESTROYED,
        CREATED,
        STARTED,
        RESUMED,
    };

    State  getState()
    {
        return mState;
    }

    void create() 
    { 
        if (mState == State::DESTROYED) {
            mState = State::CREATED;
            if (mpCallback) {
                OS::MutexCS cs(*mAppMutex);
                mpCallback->onCreate();
            }
        }
    }


    void start()
    {
        if (mState == State::CREATED) {
            mState = State::STARTED;
            if (mpCallback) {
                OS::MutexCS cs(*mAppMutex);
                mpCallback->onStart();
            }
        }
    }


    void resume()
    {
        if (mState == State::STARTED) {
            mState = State::RESUMED;
            if (mpCallback) {
                OS::MutexCS cs(*mAppMutex);
                mpCallback->onResume();
            }
        }
    }


    void pause()
    {
        if (mState == State::RESUMED) {
            mState = State::STARTED;
            if (mpCallback) {
                OS::MutexCS cs(*mAppMutex);
                mpCallback->onPause();
            }
        }
    }

    void stop()
    {
        if (mState == State::STARTED) {
            mState = State::CREATED;
            if (mpCallback) {
                OS::MutexCS cs(*mAppMutex);
                mpCallback->onStop();
            }
        }
    }

    void destroy()
    {
        if (mState == State::CREATED) {
            mState = State::DESTROYED;
            OS::MutexCS cs(*mAppMutex);
            mpCallback->onDestroy();
        }
    }

    void guiState(bool isRun)
    {
        if (mState >= State::STARTED) {
            OS::MutexCS cs(*mAppMutex);
            if (isRun) {
                mpCallback->onStartGUI();
            } else {
                mpCallback->onStopGUI();
            }
            
        }
    }

private:
    SDK::Interface::IApp::Callback* mpCallback;
	SDK::Interface::IGlance*        mpGlance;
    State                           mState;
    OS::Mutex                       mMutex;
    FakeMutex                       mFakeMutex;
    SDK::Interface::IMutex*         mAppMutex;
    
};

} // namespace SDK::Simulator::Mock
