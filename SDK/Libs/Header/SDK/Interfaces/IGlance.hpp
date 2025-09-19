/**
 ******************************************************************************
 * @file    IGlance.hpp
 * @date    19-September-2025
 * @author  Oleksandr Tymoshenko <oleksandr.tymoshenko@droid-technologies.com>
 * @brief   Glance application's interface
 * 
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __INTERFACE_GLANCE_HPP
#define __INTERFACE_GLANCE_HPP

#include "SDK/Glance/GlanceControl.h"

#include <cstdint>
#include <cstdbool>

namespace SDK::Interface {

    class IGlance {
    public:
        virtual ~IGlance() = default;

        virtual GlanceControl_t& glanceGetControls(uint8_t& count) = 0;
        virtual void             glanceUpdateControls()            = 0;
        virtual void             glanceClose()                     = 0;
    };

}

#endif  /* __INTERFACE_GLANCE_HPP */
