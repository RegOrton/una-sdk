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

#include "SDK/Kernel/KernelProviderGUI.hpp"
#include "SDK/Kernel/KernelBuilder.hpp"

extern "C" void touchgfx_init(void);
extern "C" void touchgfx_components_init(void);
extern "C" void touchgfx_taskEntry(void);


////////////////////////////////////////////////////////////////////////////////
//// Main
////////////////////////////////////////////////////////////////////////////////

int main()
{
    SDK::Kernel kernel = SDK::KernelBuilder::make();
    SDK::KernelProviderGUI::CreateInstance(&kernel);

    touchgfx_components_init();
    touchgfx_init();
    touchgfx_taskEntry();   // No return

    return 0;
}

