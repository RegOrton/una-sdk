
#include "SDK/Simulator/Kernel/Mock/System.hpp"

// Fix: GetTickCount64() and Sleep() are Windows-only. Provide portable replacements.
#ifndef _WIN32
#include <cstdint>
#include <time.h>
#include <unistd.h>
static inline uint64_t GetTickCount64()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1000ULL +
           static_cast<uint64_t>(ts.tv_nsec) / 1000000ULL;
}
static inline void Sleep(uint32_t ms)
{
    usleep(ms * 1000U);
}
#endif

#define LOG_MODULE_PRX      "Mock.System"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

namespace SDK::Simulator::Mock
{
    ////////////////////////////////////
	//// SystemGUI implementation
    ////////////////////////////////////

    bool SystemGUI::isAppRunning() const
    {
        return mAppRunning;
	}

    void SystemGUI::exit(int status)
    {
        LOG_DEBUG("status = %d\n", status);

        mAppRunning = false;

        static_cast<touchgfx::HALSDL2*>(touchgfx::HAL::getInstance())->stopApplication();
    }

    uint32_t SystemGUI::getTimeMs()
    {
        return static_cast<uint32_t>(GetTickCount64());
    }

    void SystemGUI::delay(uint32_t ms)
    {
        Sleep(ms);
    }

    void SystemGUI::yield()
    {}

    ////////////////////////////////////
    //// SystemService implementation
    ////////////////////////////////////

    bool SystemService::isAppRunning() const
    {
        return mAppRunning;
    }

    void SystemService::exit(int status)
    {
        LOG_DEBUG("status = %d\n", status);

        mAppRunning = false;
    }

    uint32_t SystemService::getTimeMs()
    {
        return static_cast<uint32_t>(GetTickCount64());
    }

    void SystemService::delay(uint32_t ms)
    {
        Sleep(ms);
    }

    void SystemService::yield()
    {}

} // namespace SDK::Simulator::Mock