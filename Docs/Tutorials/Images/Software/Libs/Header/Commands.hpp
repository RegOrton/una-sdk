
#pragma once

#include "SDK/Messages/MessageBase.hpp"
#include "SDK/Messages/MessageTypes.hpp"
#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/MessageGuard.hpp"
#include "SDK/Kernel/Kernel.hpp"

#include <array>
#include <string>
#include <vector>

// Force 4-byte alignment for all message structures
#pragma pack(push, 4)

namespace CustomMessage {

    ///////////////////////////////////////
    //// Application custom commands
    ///////////////////////////////////////

    // Service --> GUI
    /*
     * HR_VALUES Message Type:
     * - Unique identifier for heart rate update messages
     * - Must be unique across all custom message types in the app
     * - Used to route messages to correct handler in GUI
     */
    // constexpr SDK::MessageType::Type HR_VALUES = 0x00000001;

    // Service --> GUI
    constexpr SDK::MessageType::Type IMAGE_LIST = 0x00000002;
    constexpr SDK::MessageType::Type IMAGE_LOADED = 0x00000003;
    constexpr SDK::MessageType::Type IMAGE_METADATA = 0x00000004;

    // GUI --> Service
    constexpr SDK::MessageType::Type REQUEST_IMAGE_LIST = 0x00000005;
    constexpr SDK::MessageType::Type SELECT_IMAGE = 0x00000006;
    constexpr SDK::MessageType::Type REQUEST_IMAGE_METADATA = 0x00000007;

    ///////////////////////////////////////
    //// Application custom structures
    ///////////////////////////////////////

    // Service --> GUI
    /*
     * HRValues Message Struct:
     * - Inherits from SDK::MessageBase for SDK messaging
     * - Constructor initializes message type to HR_VALUES
     * - Contains HR data: heartRate (BPM) and trustLevel (0.0-1.0)
     */
    // struct HRValues : public SDK::MessageBase {
    //     float heartRate;
    //     float trustLevel;

    //     HRValues()
    //         : SDK::MessageBase(HR_VALUES)
    //         , heartRate()
    //         , trustLevel()
    //     {}
    // };

    struct ImageListMsg : public SDK::MessageBase {
        std::vector<std::string> filenames;

        ImageListMsg()
            : SDK::MessageBase(IMAGE_LIST)
            , filenames()
        {}
    };

    struct ImageLoadedMsg : public SDK::MessageBase {
        std::string filename;
        bool success;

        ImageLoadedMsg()
            : SDK::MessageBase(IMAGE_LOADED)
            , filename()
            , success(false)
        {}
    };

    struct ImageMetadataMsg : public SDK::MessageBase {
        std::string filename;
        uint32_t width;
        uint32_t height;
        uint32_t fileSize;
        std::string lastModified;
        uint32_t renderTimeMs;

        ImageMetadataMsg()
            : SDK::MessageBase(IMAGE_METADATA)
            , filename()
            , width(0)
            , height(0)
            , fileSize(0)
            , lastModified()
            , renderTimeMs(0)
        {}
    };

    // GUI --> Service
    struct RequestImageListMsg : public SDK::MessageBase {
        RequestImageListMsg()
            : SDK::MessageBase(REQUEST_IMAGE_LIST)
        {}
    };

    struct SelectImageMsg : public SDK::MessageBase {
        std::string filename;

        SelectImageMsg()
            : SDK::MessageBase(SELECT_IMAGE)
            , filename()
        {}
    };

    struct RequestImageMetadataMsg : public SDK::MessageBase {
        std::string filename;

        RequestImageMetadataMsg()
            : SDK::MessageBase(REQUEST_IMAGE_METADATA)
            , filename()
        {}
    };


    ///////////////////////////////////////
    //// Wrappers
    ///////////////////////////////////////

    class GUISender {
    public:
        GUISender(const SDK::Kernel& kernel) : mKernel(kernel)
        {}

        virtual ~GUISender() = default;

        // Service --> GUI
        /*
         * updateHeartRate Method:
         * - Creates and sends HR update message to GUI
         * - Uses SDK::make_msg to allocate message from kernel pool
         * - Returns true if message sent successfully
         * - Automatically cleaned up by SDK after delivery
         */
        // bool updateHeartRate(float value, float trustLevel)
        // {
        //     if (auto req = SDK::make_msg<CustomMessage::HRValues>(mKernel)) {
        //         req->heartRate  = value;
        //         req->trustLevel = trustLevel;

        //         return req.send();  // Send to GUI via SDK messaging
        //     }

        //     return false;
        // }

        bool sendImageList(const std::vector<std::string>& filenames)
        {
            if (auto req = SDK::make_msg<CustomMessage::ImageListMsg>(mKernel)) {
                req->filenames = filenames;
                return req.send();
            }
            return false;
        }

        bool sendImageLoaded(const std::string& filename, bool success)
        {
            if (auto req = SDK::make_msg<CustomMessage::ImageLoadedMsg>(mKernel)) {
                req->filename = filename;
                req->success = success;
                return req.send();
            }
            return false;
        }

        bool sendImageMetadata(const std::string& filename, uint32_t width, uint32_t height, uint32_t fileSize, const std::string& lastModified, uint32_t renderTimeMs)
        {
            if (auto req = SDK::make_msg<CustomMessage::ImageMetadataMsg>(mKernel)) {
                req->filename = filename;
                req->width = width;
                req->height = height;
                req->fileSize = fileSize;
                req->lastModified = lastModified;
                req->renderTimeMs = renderTimeMs;
                return req.send();
            }
            return false;
        }

    private:
        const SDK::Kernel& mKernel;
    };

} // namespace CustomMessage

#pragma pack(pop)
