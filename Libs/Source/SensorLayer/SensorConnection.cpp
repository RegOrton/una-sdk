/**
 ******************************************************************************
 * @file    SensorConnection.cpp
 * @date    17-September-2025
 * @author  Oleksandr Tymoshenko <oleksandr.tymoshenko@droid-technologies.com>
 * @brief   Connection to the Sensor
 * 
 ******************************************************************************
 *
 ******************************************************************************
 */

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/SensorLayer/SensorConnection.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/Messages/MessageGuard.hpp"

#include <stdlib.h>

namespace SDK::Sensor {

/**
 * @brief Construct a connection wrapper for a sensor driver.
 *
 * Resolves the underlying sensor driver via
 * `kernel->sensorManager.getDefaultSensor(id)` and stores connection
 * parameters for subsequent @ref connect() calls.
 *
 * @param id       Sensor type identifier to resolve the default driver.
 * @param listener Pointer to a data listener (must remain valid while connected).
 * @param period   Desired sampling/update period (units as defined by the driver; commonly seconds).
 * @param latency  Maximum report latency/batching tolerance (units as defined by the driver; commonly milliseconds).
 * @pre `kernel` must be initialized.
 * @note Use @ref isValid() to check that an underlying driver was found.
 */
Connection::Connection(SDK::Sensor::Type                    id,
                       SDK::Interface::ISensorDataListener* listener,
                       float                                period,
                       uint32_t                             latency)
    : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
    , mID(id)
    , mHandle(0)
    , mListener(nullptr)
    , mPeriod(period)
    , mLatency(latency)
    , mIsConnected(false)
{}

/**
 * @brief Construct a connection wrapper using an explicit sensor driver.
 *
 * Stores the provided driver and connection parameters for subsequent @ref connect() calls.
 *
 * @param driver   Pointer to the sensor driver to use.
 * @param listener Pointer to a data listener (must remain valid while connected).
 * @param period   Desired sampling/update period (units as defined by the driver; commonly seconds).
 * @param latency  Maximum report latency/batching tolerance (units as defined by the driver; commonly milliseconds).
 *
 * @pre `driver` must be valid and compatible with the listener.
 */

Connection::Connection(uint8_t                              handle,
                       SDK::Interface::ISensorDataListener* listener,
                       float                                period,
                       uint32_t                             latency)
    : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
    , mID(SDK::Sensor::Type::UNKNOWN)
    , mHandle(handle)
    , mListener(nullptr)
    , mPeriod(period)
    , mLatency(latency)
    , mIsConnected(false)
{}

/**
 * @brief Check whether the underlying driver has been resolved.
 * @return `true` if a driver pointer is available, otherwise `false`.
 */
bool Connection::isValid()
{
    return (mHandle != 0);
}

/**
 * @brief Connect to the driver using the stored period and latency.
 *
 * Delegates to the driver's `connect(listener, userApp, period, latency)`.
 *
 * @return `true` on successful connection, `false` if no driver is available or the driver rejects the request.
 * @see connect(float,uint32_t)
 */
bool Connection::connect()
{
//    bool status;

    if (!isValid()) {
//        status = false;
//        auto* reqDefaultSensor = mKernel.comm.allocateMessage<SDK::Message::Sensor::RequestDefault>();
//        if (reqDefaultSensor) {
//            reqDefaultSensor->id = mID;
//
//            status = mKernel.comm.sendMessage(reqDefaultSensor, 100);
//
//            if (status && reqDefaultSensor->getResult() == SDK::MessageResult::SUCCESS) {
//                mHandle = reqDefaultSensor->handle;
//            } else {
//                status = false;
//            }
//
//            mKernel.comm.releaseMessage(reqDefaultSensor);
//        }
//
//        if (!status) {
//            return false;
//        }
        auto req = make_msg<SDK::Message::Sensor::RequestDefault>(mKernel.comm);
        if (!req) {
            return false;
        }

        req->id = mID;

        if (!req.send(100) || !req.ok()) {
            return false;
        }

        mHandle = req->handle;
    }

//    status = false;
//
//    auto* reqConnect = mKernel.comm.allocateMessage<SDK::Message::Sensor::RequestConnect>();
//    if (reqConnect) {
//        reqConnect->handle   = mHandle;
//        reqConnect->listener = mListener;
//        reqConnect->period   = mPeriod;
//        reqConnect->latency  = mLatency;
//
//        status = mKernel.comm.sendMessage(reqConnect, 100);
//
//        if (status && reqConnect->getResult() == SDK::MessageResult::SUCCESS) {
//            mIsConnected = true;
//        }
//
//        mKernel.comm.releaseMessage(reqConnect);
//    }

    auto reqConnect = make_msg<SDK::Message::Sensor::RequestConnect>(mKernel.comm);
    if (!reqConnect) {
        return false;
    }

    reqConnect->handle   = mHandle;
    reqConnect->listener = mListener;
    reqConnect->period   = mPeriod;
    reqConnect->latency  = mLatency;

    if (reqConnect.send(100) || reqConnect.ok()) {
        mIsConnected = true;
    }

    return mIsConnected;
}

/**
 * @brief Update period/latency and connect to the driver.
 *
 * Stores the provided parameters and then calls the parameterless @ref connect().
 *
 * @param period   Desired sampling/update period (units as defined by the driver).
 * @param latency  Maximum report latency/batching tolerance (units as defined by the driver).
 * @return `true` on successful connection, otherwise `false`.
 */
bool Connection::connect(float period, uint32_t latency)
{
    if (!isValid()) {
        return false;
    }

    if (mIsConnected) {
        return false;
    }

    mPeriod  = period;
    mLatency = latency;

    return connect();
}

bool Connection::isConnected()
{
    return mIsConnected;
}

/**
 * @brief Disconnect the listener from the underlying driver.
 *
 * Safe to call even if the connection is not valid or already disconnected.
 * If no driver is available, the call is a no-op.
 */
void Connection::disconnect()
{
    if (!isValid()) {
        return;
    }

    if (!mIsConnected) {
        return;
    }

    auto request = make_msg<SDK::Message::Sensor::RequestDisconnect>(mKernel.comm);
    if (!request) {
        return;
    }

    request->handle   = mHandle;
    request->listener = mListener;

    request.send();

    mIsConnected = false;
}

/**
 * @brief Check if the specified driver matches the connected one.
 *
 * @details
 * Compares the given driver pointer with the internal driver instance
 * associated with this connection. Returns true only if both pointers
 * are valid and refer to the same ISensorDriver object.
 *
 * @param  driver Pointer to a sensor driver to compare against.
 * @return true  If the specified driver is the same as the one managed by this connection.
 * @return false If either pointer is null or they do not match.
 */
bool Connection::matchesDriver(uint16_t handle)
{
    if (!isValid()) {
        return false;
    }

    if (handle == 0) {
        return false;
    }

    return mHandle == handle;
}

} // namespace SDK::Sensors
