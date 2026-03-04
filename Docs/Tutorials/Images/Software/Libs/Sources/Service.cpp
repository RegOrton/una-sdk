// #include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"  // Commented out: HR parser include
#include "SDK/Messages/SensorLayerMessages.hpp"

#include "Service.hpp"
#include <ctime>

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

Service::Service(SDK::Kernel& kernel)
    : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
    , mSender(mKernel)
    , mGUIStarted(false)
    , mFileSystem(mKernel.fs)  // Initialize file system from kernel
    , mAppFolderPath("/images")  // Set correct path
    // Uncomment below to enable HR sensor and FIT logging:
    // , mSensorHR(SDK::Sensor::Type::HEART_RATE, 1000, 2000)  // Initialize HR sensor (1s sample, 2s timeout)
    // , mHR(0)  // Initialize HR value to 0
    // , mHRTL(0)  // Initialize HR trust level to 0
    // , mActivityWriter(mKernel, "Activity")  // Initialize FIT writer with "Activity" directory
{}

void Service::run()
{
    LOG_INFO("thread started\n");

    /*
     * To enable HR sensor and FIT logging:
     * 1. Uncomment sensor connection below
     * 2. Uncomment FIT file initialization
     * 3. Ensure member variables are uncommented in Service.hpp
     * 4. Uncomment sensor disconnect in COMMAND_APP_STOP and final cleanup
     */
    // mSensorHR.connect();  // Connect to heart rate sensor

    // // Initialize FIT file with app metadata
    // ActivityWriter::AppInfo info{};
    // info.timestamp  = std::time(nullptr);  // Current UTC time
    // info.appVersion = ParseVersion(BUILD_VERSION);  // Parsed version number
    // info.devID      = DEV_ID;  // Developer ID string
    // info.appID      = APP_ID;  // Application ID string
    // mActivityWriter.start(info);  // Create and initialize FIT file

    // time_t startTime    = time(nullptr);
    // time_t utcTimestamp = 0;

    // float    hrAvgSum   = 0;
    // uint32_t hrAvgCount = 0;
    // float    hrMax      = 0;

    // uint32_t startTimeMs = mKernel.sys.getTimeMs();

    while (true) {
        SDK::MessageBase *msg;
        if (mKernel.comm.getMessage(msg, 1000)) {
            // Command handling
            switch (msg->getType()) {
                // Kernel messages
                case SDK::MessageType::COMMAND_APP_STOP:
                    LOG_INFO("Force exit from the application\n");
                    // mSensorHR.disconnect();  // Commented out: Disconnect HR sensor
                    // We must release message because this is the last event.
                    mKernel.comm.releaseMessage(msg);
                    return;

                case SDK::MessageType::COMMAND_APP_NOTIF_GUI_RUN:
                    LOG_INFO("GUI is now running\n");
                    onStartGUI();
                    break;

                case SDK::MessageType::COMMAND_APP_NOTIF_GUI_STOP:
                    LOG_INFO("GUI has stopped\n");
                    onStopGUI();
                    break;

                // Sensors messages
                case SDK::MessageType::EVENT_SENSOR_LAYER_DATA: {
                    auto event = static_cast<SDK::Message::Sensor::EventData*>(msg);
                    SDK::Sensor::DataBatch data(event->data, event->count, event->stride);
                    onSdlNewData(event->handle, data);
                } break;

                // GUI to Service messages
                case CustomMessage::REQUEST_IMAGE_LIST: {
                    LOG_DEBUG("Received REQUEST_IMAGE_LIST");
                    scanImageFolder();
                    sendImageList();
                } break;

                case CustomMessage::SELECT_IMAGE: {
                    LOG_DEBUG("Received SELECT_IMAGE");
                    auto* selectMsg = static_cast<CustomMessage::SelectImageMsg*>(msg);
                    loadImage(selectMsg->filename);
                } break;

                case CustomMessage::REQUEST_IMAGE_METADATA: {
                    LOG_DEBUG("Received REQUEST_IMAGE_METADATA");
                    auto* metadataMsg = static_cast<CustomMessage::RequestImageMetadataMsg*>(msg);
                    getImageMetadata(metadataMsg->filename);
                } break;

                default:
                    break;
            }

            // Release message after processing
            mKernel.comm.releaseMessage(msg);
        }

        /*
         * FIT Record Logging (every second while GUI is active):
         * - Checks if GUI is started (mGUIStarted)
         * - Logs HR data every second to FIT file
         * - Maintains aggregates for lap/track summaries
         * To enable: Uncomment the block below and ensure mActivityWriter is initialized
         */
        // if (mGUIStarted) {
        //     // Save record to the FIT file every second
        //     time_t utc = time(nullptr);
        //     if (utcTimestamp != utc) {  // Check if a second has passed
        //         utcTimestamp = utc;

        //         ActivityWriter::RecordData fitRecord {};
        //         fitRecord.timestamp  = utc;  // UTC timestamp
        //         fitRecord.heartRate  = static_cast<uint8_t>(mHR);  // Current HR (BPM)
        //         fitRecord.trustLevel = static_cast<uint8_t>(mHRTL);  // Trust level
        //         mActivityWriter.addRecord(fitRecord);  // Add record to FIT file

        //         // Update session aggregates (skip invalid readings)
        //         if (mHR > 1) {
        //             hrAvgSum += mHR;
        //             ++hrAvgCount;
        //             hrMax = std::max(hrMax, mHR);
        //         }
        //     }
        // } else {
        // Just wait some time to see if GUI starts
        // if (mKernel.sys.getTimeMs() - startTimeMs > 5000) {
        //     LOG_DEBUG("start GUI timeout\n");
        //     break;
        // }
            // mKernel.sys.delay(100);
        // }
    }

    /*
     * FIT File Finalization on App Exit:
     * - Calculate session duration and aggregates
     * - Add lap data (full session as one lap)
     * - Add track summary with final stats
     * - Close FIT file and save to storage
     * To enable: Uncomment below and ensure ActivityWriter is initialized
     */
    // time_t utc = time(nullptr);  // Get final timestamp

    // // Add lap data for the entire session
    // ActivityWriter::LapData fitLap {};
    // fitLap.timeStart = utc - startTime;  // Session start time
    // fitLap.duration = utc - startTime;   // Total duration
    // fitLap.elapsed = utc - startTime;    // Elapsed time
    // fitLap.hrAvg = static_cast<uint8_t>(hrAvgSum / hrAvgCount);  // Average HR
    // fitLap.hrMax = static_cast<uint8_t>(hrMax);  // Max HR
    // mActivityWriter.addLap(fitLap);

    // // Add track summary
    // ActivityWriter::TrackData fitTrack{};
    // fitTrack.timeStart = utc;  // Final timestamp
    // fitTrack.duration = utc - startTime;
    // fitTrack.elapsed = utc - startTime;
    // fitTrack.hrAvg = static_cast<uint8_t>(hrAvgSum / hrAvgCount);
    // fitTrack.hrMax = static_cast<uint8_t>(hrMax);

    // mActivityWriter.stop(fitTrack);  // Finalize and save FIT file

    // mSensorHR.disconnect();  // Disconnect HR sensor

    LOG_INFO("thread stopped\n");
}

void Service::onStartGUI()
{
    LOG_INFO("GUI started\n");
    mGUIStarted = true;
    /*
     * Send initial HR values to GUI on startup.
     * To enable: Uncomment below and ensure HR message types are defined in Commands.hpp
     * This initializes the GUI display with default values before real sensor data arrives.
     */
    // mSender.updateHeartRate(0.0f, 0.0f);  // Send initial HR (0.0) and trust level (0.0) to GUI
}

void Service::onStopGUI()
{
    LOG_INFO("GUI stopped\n");
    mGUIStarted = false;
}

void Service::onSdlNewData(uint16_t handle, SDK::Sensor::DataBatch& data)
{
    /*
     * Handle incoming sensor data from kernel.
     * - Check if handle matches our HR sensor
     * - Parse HR data if GUI is active
     * - Send updates to GUI via custom message
     * To enable: Uncomment below, ensure sensor is connected, and message types defined
     */
    // if (mSensorHR.matchesDriver(handle)) {  // Verify data is from HR sensor
    //     if (mGUIStarted) {  // Only process if GUI is running
    //         SDK::SensorDataParser::HeartRate parser(data[0]);  // Parse first batch
    //         if (parser.isDataValid()) {  // Check if data is valid
    //             mHR   = parser.getBpm();  // Extract BPM
    //             mHRTL = parser.getTrustLevel();  // Extract trust level

    //             mSender.updateHeartRate(mHR, mHRTL);  // Send to GUI
    //         }
    //     }
    // }
}

void Service::scanImageFolder()
{
    mImageList.clear();

    auto dir = mFileSystem.dir(mAppFolderPath.c_str());
    if (!dir) {
        LOG_ERROR("Failed to open directory: %s", mAppFolderPath.c_str());
        return;
    }

    if (!dir->open()) {
        LOG_ERROR("Failed to open directory: %s", mAppFolderPath.c_str());
        return;
    }

    SDK::Interface::IFileSystem::ObjectInfo item;
    while (dir->readNext(item)) {
        if (!item.isDir) {
            std::string filename = item.name;
            // Check if it's a JPG file
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".jpg") {
                mImageList.push_back(filename);
                LOG_DEBUG("Found image: %s", filename.c_str());
            }
        }
    }

    dir->close();
    LOG_INFO("Scanned %d images", mImageList.size());
}

void Service::sendImageList()
{
    mSender.sendImageList(mImageList);
}

void Service::loadImage(const std::string& filename)
{
    // Load JPG file using Bitmap::dynamicBitmapCreateFromJpegFile
    std::string fullPath = mAppFolderPath + "/" + filename;
    // Assume Bitmap::dynamicBitmapCreateFromJpegFile exists and returns a BitmapId
    // BitmapId bitmapId = Bitmap::dynamicBitmapCreateFromJpegFile(fullPath.c_str());
    // bool success = (bitmapId != BITMAP_INVALID);
    bool success = true; // Placeholder - assume success for now
    mSender.sendImageLoaded(filename, success);
}

void Service::getImageMetadata(const std::string& filename)
{
    std::string fullPath = mAppFolderPath + "/" + filename;
    uint32_t width = 0, height = 0, fileSize = 0, renderTimeMs = 0;
    std::string lastModified = "unknown";

    auto file = mFileSystem.file(fullPath.c_str());
    if (!file || !file->open()) {
        LOG_ERROR("Failed to open file: %s", fullPath.c_str());
        mSender.sendImageMetadata(filename, width, height, fileSize, lastModified, renderTimeMs);
        return;
    }

    fileSize = file->size();

    // Get file modification time
    SDK::Interface::IFileSystem::ObjectInfo info;
    if (mFileSystem.objectInfo(fullPath.c_str(), info)) {
        // Convert time_t to string
        char* timeStr = ctime(&info.utc);
        lastModified = timeStr;
        // Remove trailing newline
        if (!lastModified.empty() && lastModified.back() == '\n') {
            lastModified.pop_back();
        }
    }

    // Parse JPEG header for width/height
    uint8_t buffer[1024];
    size_t bytesRead;
    if (file->read((char*)buffer, 2, bytesRead) && bytesRead == 2) {
        if (buffer[0] == 0xFF && buffer[1] == 0xD8) { // JPEG SOI
            // Read markers
            uint32_t offset = 2;
            while (offset < fileSize - 1) {
                if (file->seek(offset) && file->read((char*)buffer, 4, bytesRead) && bytesRead == 4) {
                    if (buffer[0] == 0xFF) {
                        uint8_t marker = buffer[1];
                        uint16_t length = (buffer[2] << 8) | buffer[3];
                        if (marker == 0xC0) { // SOF0 - Start of Frame
                            if (file->read((char*)buffer, 5, bytesRead) && bytesRead == 5) {
                                height = (buffer[1] << 8) | buffer[2];
                                width = (buffer[3] << 8) | buffer[4];
                            }
                            break;
                        }
                        offset += 2 + length;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
        }
    }

    file->close();

    // Measure render time (placeholder - would need actual rendering)
    renderTimeMs = 0;

    mSender.sendImageMetadata(filename, width, height, fileSize, lastModified, renderTimeMs);
}

uint32_t Service::ParseVersion(const char* str)
{
    if (str == nullptr) {
        return 0;
    }

    typedef union {
        struct {
            uint8_t patch;
            uint8_t minor;
            uint8_t major;
        };
        uint32_t u32;
    } FirmwareVersion_t;

    FirmwareVersion_t v{};

    int major, minor, patch;

    if (sscanf(str, "%d.%d.%d", &major, &minor, &patch) == 3) {
        v.major = static_cast<uint8_t>(major);
        v.minor = static_cast<uint8_t>(minor);
        v.patch = static_cast<uint8_t>(patch);
        return v.u32;
    }

    return 0;
}
