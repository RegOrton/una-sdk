/**
 ******************************************************************************
 * @file    KernelBase.hpp
 * @date    05-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   The base class of the kernel simulator.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "SDK/Simulator/Kernel/KernelBase.hpp"
#include "SDK/Simulator/Kernel/Mock/ServiceControl.hpp"
#include "SDK/Simulator/Sensors/ISensorCore.hpp"
#include "SDK/Interfaces/IKernelIntfProvider.hpp"

const SDK::Interface::IKernel* gIKernel = nullptr;

// TODO: Move to common SDK file
#include "gui/common/GuiConfig.hpp"

static constexpr char sFsPath[] = "../../../../../Output/";

namespace SDK::Simulator
{
KernelBase::KernelBase(bool useMutex, Mock::ServiceControl& serviceControl, 
                        Sensors::ISensorCore* sensoreCore,
                        Mock::App* srvApp)
    : mIPower()
    , mISettings()
    , mIFilesystem(sFsPath)
    , mIAppMemAllocator()
    , mSynchManager()
    , mIApp(useMutex)
    , mServiceControl(serviceControl)
    , mBacklight()
    , mBuzer()
    , mVibro()
    , mSensoreCore(sensoreCore)
    , mKernel(new SDK::Kernel(mIPower,
                              mISettings,
                              mIFilesystem,
                              mIAppMemAllocator,
                              mSynchManager,
                              mSensorManager,
                              mIApp,
                              mServiceControl,
                              mServiceControl,
                              mBacklight,
                              mVibro,
                              mBuzer,
                              mILogger,
                              mIAppCapabilities))
    , mSrvApp(srvApp)
{
    if (gIKernel == nullptr) {
        gIKernel = createIKernel();
    }
}

void KernelBase::startApp()
{ 
    mIApp.create();
    mIApp.start();
    if (mSrvApp) {
        mSrvApp->guiState(true);
    }
    mIApp.resume();
}

void KernelBase::stopApp()
{
    mIApp.pause();
    if (mSrvApp) {
        mSrvApp->guiState(false);
    }
    mIApp.stop();
    mIApp.destroy();
}

void KernelBase::tick()
{
    if (mSensoreCore) {
        mSensoreCore->tick();
    }
}

bool KernelBase::keyFilter(uint8_t key)
{
    return (mIApp.getState() == Mock::App::State::RESUMED &&
            (Gui::Config::Button::L1 == key ||
             Gui::Config::Button::L2 == key ||
             Gui::Config::Button::R1 == key ||
             Gui::Config::Button::R2 == key ||
             Gui::Config::Button::L1R2 == key));
}

const SDK::Kernel* KernelBase::getKernel()
{
    return mKernel;
}

Mock::App& KernelBase::getApp()
{
    return mIApp;
}

std::string KernelBase::getFsPath()
{
    return mIFilesystem.getRootPath();
}

const SDK::Interface::IKernel* KernelBase::createIKernel()
{
    class FakeKIP : public SDK::Interface::IKIP
    {
    public:
        virtual void* queryInterface(IntfID iid) const { return nullptr; }
        virtual ~FakeKIP() = default;
    };

    static FakeKIP fakeKIP;

    static SDK::Interface::IKernel ikernel = SDK::Interface::IKernel(fakeKIP, mIApp, mIAppMemAllocator, mILogger);

    return &ikernel;
}

}
