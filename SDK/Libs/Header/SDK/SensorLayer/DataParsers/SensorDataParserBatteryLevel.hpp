/**
 ******************************************************************************
 * @file    SensorDataParserBatteryLevel.hpp
 * @date    23-October-2025
 * @author  Oleksandr Tymoshenko <oleksandr.tymoshenko@droid-technologies.com>
 * @brief   SensorData parser for the Battery Level sensor
 * 
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __SENSOR_DATA_PARSER_BATTERY_LEVEL_HPP
#define __SENSOR_DATA_PARSER_BATTERY_LEVEL_HPP

#include "SDK/Interfaces/ISensorData.hpp"

#include <cstdint>

namespace SDK {
namespace SensorDataParser {

/**
 * @brief   SensorData parser for the Battery Level sensor
 *
 * @details
 * The parser only reads fields and provides type-safe accessors.
 * It does not own the underlying storage.*/
class BatteryLevel
{
public:
    enum Field : uint8_t {
        LEVEL = 0,    ///< Raw charge value (units are device-specific)
        COUNT          ///< Number of fields (must be last)
    };

    /**
     * @brief   SensorData parser for the Battery Level sensor
     * @param data Reference to sensor data
     */
    explicit BatteryLevel(const Interface::ISensorData& data) : mData(&data) {}

    /**
     * @brief   SensorData parser for the Battery Level sensor
     * @param data Pointer to sensor data
     */
    explicit BatteryLevel(const Interface::ISensorData* data) : mData(data) {}

    /**
     * @brief   SensorData parser for the Battery Level sensor
     *
     * @details
 * The parser only reads fields and provides type-safe accessors.
 * It does not own the underlying storage.*/
    bool isDataValid() const
    {
        return (mData != nullptr) &&
               (mData->getLength() == static_cast<uint8_t>(Field::COUNT) &&
               (mData->getAsFloat(static_cast<uint8_t>(Field::LEVEL)) >= 0.0) &&
               (mData->getAsFloat(static_cast<uint8_t>(Field::LEVEL)) <= 100.0));
    }

    /**
     * @brief   SensorData parser for the Battery Level sensor
     *
     * @details
 * The parser only reads fields and provides type-safe accessors.
 * It does not own the underlying storage.*/
    float getCharge() const
    {
        if (!isDataValid()) {
            return -1.0f;
        }

        return mData->getAsFloat(static_cast<uint8_t>(Field::LEVEL));
    }

    /**
     * @brief  Get timestamp of the data
     * @return Data timestamp in ms (0 if data pointer is null).
     */
    uint32_t getTimestamp() const
    {
        return (mData != nullptr) ? mData->getTimestamp() : 0U;
    }

    /**
     * @brief Get data timestamp in us
     * @return Data timestamp in us (0 if invalid)
     */
    uint64_t getTimestampUs() const
    {
        return isDataValid() ? mData->getTimestampUs() : 0;
    }

    /**
     * @brief  Returns the number of fields
     * @return The number of fields
     */
    static constexpr uint8_t getFieldsNumber()
    {
        return static_cast<uint8_t>(Field::COUNT);
    }

private:
    const Interface::ISensorData* mData { nullptr };
}; /* class Power */

} /* namespace SensorDataParser */
} /* namespace SDK */

#endif /* __SENSOR_DATA_PARSER_BATTERY_LEVEL_HPP */


