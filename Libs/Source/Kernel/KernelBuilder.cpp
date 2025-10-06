/**
 ******************************************************************************
 * @file    KernelBuilder.сpp
 * @date    24-September-2025
 * @author  Oleksandr Tymoshenko <oleksandr.tymoshenko@droid-technologies.com>
 * @brief   Helper to construct an SDK::Kernel facade from an IKernel provider.
 * @details Declares a lightweight builder that resolves all required subsystem
 *          interfaces via @c IKernel::queryInterface and returns an initialized
 *          @c SDK::Kernel facade. The @c require<T>() helper asserts that each
 *          requested interface is available and safely casts the returned pointer.
 *
 * @note    This header only declares the builder; the actual construction logic
 *          (e.g., the @c build() definition and how the @c IKernel instance is
 *          obtained) is expected to be provided in the corresponding source file.
 *
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "SDK/Kernel/KernelBuilder.hpp"

/**
 * @brief Global kernel provider pointer (non-owning).
 * @details Supplied by the platform/loader; must point to a valid provider that
 *          implements the required interfaces for this application.
 */
extern const SDK::Interface::IKernel* gIKernel;

/**
 * @brief Build a @ref SDK::Kernel façade by querying the underlying provider.
 * @details Fetches each required sub-interface by its @ref SDK::Interface::IKIP::IntfID
 *          and binds them into a single convenience object.
 *
 * @return A value instance of @ref SDK::Kernel holding references to all sub-interfaces.
 *
 * @pre @c kernel is non-null and provides all interfaces used below.
 * @note The façade is returned by value; the contained references remain valid only
 *       as long as the underlying kernel provider remains alive.
 */
SDK::Kernel SDK::KernelBuilder::make()
{
    SDK::Kernel k(require<SDK::Interface::IPower>(gIKernel,             SDK::Interface::IKIP::IntfID::IID_POWER),
                  require<SDK::Interface::ISettings>(gIKernel,          SDK::Interface::IKIP::IntfID::IID_SETTINGS),
                  require<SDK::Interface::IFileSystem>(gIKernel,        SDK::Interface::IKIP::IntfID::IID_FILESYSTEM),
                  gIKernel->mem,
                  require<SDK::Interface::ISynchManager>(gIKernel,      SDK::Interface::IKIP::IntfID::IID_SYNCH_MANAGER),
                  require<SDK::Interface::ISensorManager>(gIKernel,     SDK::Interface::IKIP::IntfID::IID_SENSOR_MANAGER),
                  gIKernel->app,
                  require<SDK::Interface::IServiceControl>(gIKernel,    SDK::Interface::IKIP::IntfID::IID_SERVICE_CONTROL),
                  require<SDK::Interface::IGUIControl>(gIKernel,        SDK::Interface::IKIP::IntfID::IID_GUI_CONTROL),
                  require<SDK::Interface::IBacklight>(gIKernel,         SDK::Interface::IKIP::IntfID::IID_BACKLIGHT),
                  require<SDK::Interface::IVibro>(gIKernel,             SDK::Interface::IKIP::IntfID::IID_VIBRO),
                  require<SDK::Interface::IBuzzer>(gIKernel,            SDK::Interface::IKIP::IntfID::IID_BUZZER),
                  gIKernel->log,
                  require<SDK::Interface::IAppCapabilities>(gIKernel,   SDK::Interface::IKIP::IntfID::IID_APP_CAPABILITIES));

    return k;
}
