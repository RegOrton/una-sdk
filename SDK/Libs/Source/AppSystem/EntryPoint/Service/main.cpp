/**
 ******************************************************************************
 * @file    main.сpp
 * @date    25-September-2025
 * @author  Oleksandr Tymoshenko <oleksandr.tymoshenko@droid-technologies.com>
 * @brief   The application entry point
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/Kernel/KernelBuilder.hpp"
#include "SDK/AppSystem/SvcBootstrap.hpp"
#include "SDK/AppSystem/UserAppEntry.hpp"
#include "Service.hpp"

////////////////////////////////////////////////////////////////////////////////
//// Main
////////////////////////////////////////////////////////////////////////////////

int main()
{
    ///////////////////////////
    //// Build the kernel
    ///////////////////////////

    SDK::Kernel kernel = SDK::KernelBuilder::make();
    SDK::KernelProviderService::CreateInstance(&kernel);

    SDK::Service::Bootstrap bootstrap;
    bootstrap.run();

    return 0;
}

