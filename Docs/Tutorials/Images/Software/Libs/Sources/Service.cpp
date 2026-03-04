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
{
    LOG_INFO("Service constructor: Initializing service with kernel reference, message sender, GUI state set to false, file system access, and app folder path set to '/images'");
}

void Service::run()
{
    LOG_INFO("thread started\n");
    LOG_INFO("Service run method: Starting the main service loop to process messages and handle application lifecycle");

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
        LOG_DEBUG("Service loop iteration: Waiting for incoming messages with 1000ms timeout");
        SDK::MessageBase *msg;
        if (mKernel.comm.getMessage(msg, 1000)) {
            LOG_DEBUG("Message received, processing message type: %d", msg->getType());
            // Command handling
            switch (msg->getType()) {
                // Kernel messages
                case SDK::MessageType::COMMAND_APP_STOP:
                    LOG_INFO("Force exit from the application\n");
                    LOG_INFO("Handling app stop command: Preparing to exit service thread");
                    // mSensorHR.disconnect();  // Commented out: Disconnect HR sensor
                    // We must release message because this is the last event.
                    mKernel.comm.releaseMessage(msg);
                    return;

                case SDK::MessageType::COMMAND_APP_NOTIF_GUI_RUN:
                    LOG_INFO("GUI is now running\n");
                    LOG_INFO("GUI start notification received: Initializing GUI-related operations");
                    onStartGUI();
                    break;

                case SDK::MessageType::COMMAND_APP_NOTIF_GUI_STOP:
                    LOG_INFO("GUI has stopped\n");
                    LOG_INFO("GUI stop notification received: Cleaning up GUI-related resources");
                    onStopGUI();
                    break;

                // Sensors messages
                case SDK::MessageType::EVENT_SENSOR_LAYER_DATA: {
                    auto event = static_cast<SDK::Message::Sensor::EventData*>(msg);
                    SDK::Sensor::DataBatch data(event->data, event->count, event->stride);
                    LOG_DEBUG("Sensor data event received: Processing sensor data batch with handle %u", event->handle);
                    LOG_DEBUG("Data batch contains %u samples", data.size());
                    onSdlNewData(event->handle, data);
                } break;

                // GUI to Service messages
                case CustomMessage::REQUEST_IMAGE_LIST: {
                    LOG_DEBUG("Received REQUEST_IMAGE_LIST");
                    LOG_INFO("Image list request: Scanning directory for images and preparing to send list");
                    scanImageFolder();
                    sendImageList();
                } break;

                case CustomMessage::SELECT_IMAGE: {
                    LOG_DEBUG("Received SELECT_IMAGE");
                    auto* selectMsg = static_cast<CustomMessage::SelectImageMsg*>(msg);
                    LOG_DEBUG("Image selection request for file: %s", selectMsg->filename);
                    LOG_INFO("Loading selected image file");
                    loadImage(selectMsg->filename);
                } break;

                case CustomMessage::REQUEST_IMAGE_METADATA: {
                    LOG_DEBUG("Received REQUEST_IMAGE_METADATA");
                    auto* metadataMsg = static_cast<CustomMessage::RequestImageMetadataMsg*>(msg);
                    LOG_DEBUG("Metadata request for file: %s", metadataMsg->filename);
                    LOG_INFO("Retrieving image metadata");
                    getImageMetadata(metadataMsg->filename);
                } break;

                default:
                    break;
            }

            // Release message after processing
            mKernel.comm.releaseMessage(msg);
            LOG_DEBUG("Message processing complete, releasing message");
        }

        LOG_INFO("Note: FIT logging is commented out; uncomment to enable activity data recording");
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

    LOG_INFO("Service run method: Exiting main loop, thread stopping");
    LOG_INFO("thread stopped\n");
}

void Service::onStartGUI()
{
    LOG_INFO("GUI started\n");
    LOG_INFO("Setting GUI started flag to true, enabling GUI-dependent operations");
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
    LOG_INFO("Setting GUI started flag to false, disabling GUI-dependent operations");
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
    // LOG_INFO("Sensor data received: Checking if handle matches HR sensor");
    // if (mSensorHR.matchesDriver(handle)) {  // Verify data is from HR sensor
    //     LOG_DEBUG("HR sensor data matched, handle: %u", handle);
    //     if (mGUIStarted) {  // Only process if GUI is running
    //         LOG_DEBUG("GUI is active, parsing HR data");
    //         SDK::SensorDataParser::HeartRate parser(data[0]);  // Parse first batch
    //         if (parser.isDataValid()) {  // Check if data is valid
    //             LOG_DEBUG("HR data valid: BPM=%f, TrustLevel=%f", parser.getBpm(), parser.getTrustLevel());
    //             mHR   = parser.getBpm();  // Extract BPM
    //             mHRTL = parser.getTrustLevel();  // Extract trust level

    //             mSender.updateHeartRate(mHR, mHRTL);  // Send to GUI
    //             LOG_INFO("Updated GUI with new HR data");
    //         } else {
    //             LOG_DEBUG("HR data invalid, skipping update");
    //         }
    //     } else {
    //         LOG_DEBUG("GUI not active, ignoring HR data");
    //     }
    // }
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
    LOG_INFO("Starting image folder scan: Clearing previous image list");
    mImageList.clear();

    LOG_DEBUG("Attempting to open directory: %s", mAppFolderPath.c_str());
    auto dir = mFileSystem.dir(mAppFolderPath.c_str());
    if (!dir) {
        LOG_ERROR("Failed to open directory: %s", mAppFolderPath.c_str());
        return;
    }

    if (!dir->open()) {
        LOG_ERROR("Failed to open directory: %s", mAppFolderPath.c_str());
        return;
    }

    LOG_DEBUG("Directory opened successfully, reading contents");
    SDK::Interface::IFileSystem::ObjectInfo item;
    while (dir->readNext(item)) {
        if (!item.isDir) {
            LOG_DEBUG("File found: %s", item.name);
            std::string filename = item.name;
            // Check if it's a JPG file
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".jpg") {
                LOG_DEBUG("JPG file detected, adding to image list: %s", filename.c_str());
                mImageList.push_back(filename);
                LOG_DEBUG("Found image: %s", filename.c_str());
            }
        }
    }

    LOG_DEBUG("Directory scan complete, closing directory");
    dir->close();
    LOG_INFO("Scanned %d images", mImageList.size());
}

void Service::sendImageList()
{
    LOG_INFO("Sending image list to GUI: %d images available", mImageList.size());
    mSender.sendImageList(mImageList);
}

void Service::loadImage(const std::string& filename)
{
    LOG_INFO("Loading image: %s", filename.c_str());
    // Load JPG file using Bitmap::dynamicBitmapCreateFromJpegFile
    std::string fullPath = mAppFolderPath + "/" + filename;
    LOG_DEBUG("Constructing full file path: %s", fullPath.c_str());
    // Assume Bitmap::dynamicBitmapCreateFromJpegFile exists and returns a BitmapId
    // BitmapId bitmapId = Bitmap::dynamicBitmapCreateFromJpegFile(fullPath.c_str());
    // bool success = (bitmapId != BITMAP_INVALID);
    bool success = true; // Placeholder - assume success for now
    LOG_DEBUG("Placeholder: Assuming image load success (actual implementation would use Bitmap::dynamicBitmapCreateFromJpegFile)");
    LOG_INFO("Image load result: Success (placeholder), notifying GUI");
    mSender.sendImageLoaded(filename, success);
}

void Service::getImageMetadata(const std::string& filename)
{
    LOG_INFO("Retrieving metadata for image: %s", filename.c_str());
    std::string fullPath = mAppFolderPath + "/" + filename;
    LOG_DEBUG("Full file path: %s", fullPath.c_str());
    uint32_t width = 0, height = 0, fileSize = 0, renderTimeMs = 0;
    std::string lastModified = "unknown";

    LOG_DEBUG("Attempting to open file for metadata extraction");
    auto file = mFileSystem.file(fullPath.c_str());
    if (!file || !file->open()) {
        LOG_ERROR("Failed to open file: %s", fullPath.c_str());
        mSender.sendImageMetadata(filename, width, height, fileSize, lastModified, renderTimeMs);
        return;
    }

    fileSize = file->size();
    LOG_DEBUG("File opened successfully, size: %u bytes", fileSize);

    // Get file modification time
    LOG_DEBUG("Retrieving file modification time");
    SDK::Interface::IFileSystem::ObjectInfo info;
    if (mFileSystem.objectInfo(fullPath.c_str(), info)) {
        // Convert time_t to string
        char* timeStr = ctime(&info.utc);
        lastModified = timeStr;
        // Remove trailing newline
        if (!lastModified.empty() && lastModified.back() == '\n') {
            lastModified.pop_back();
        }
        LOG_DEBUG("Last modified time retrieved: %s", lastModified.c_str());
    }

    // Parse JPEG header for width/height
    LOG_DEBUG("Parsing JPEG header to extract dimensions");
    uint8_t buffer[1024];
    size_t bytesRead;
    if (file->read((char*)buffer, 2, bytesRead) && bytesRead == 2) {
        if (buffer[0] == 0xFF && buffer[1] == 0xD8) { // JPEG SOI
            LOG_DEBUG("JPEG file confirmed, parsing markers");
            // Read markers
            uint32_t offset = 2;
            while (offset < fileSize - 1) {
                LOG_DEBUG("Scanning for SOF0 marker at offset %u", offset);
                if (file->seek(offset) && file->read((char*)buffer, 4, bytesRead) && bytesRead == 4) {
                    if (buffer[0] == 0xFF) {
                        uint8_t marker = buffer[1];
                        uint16_t length = (buffer[2] << 8) | buffer[3];
                        if (marker == 0xC0) { // SOF0 - Start of Frame
                            LOG_DEBUG("SOF0 marker found, extracting width and height");
                            if (file->read((char*)buffer, 5, bytesRead) && bytesRead == 5) {
                                height = (buffer[1] << 8) | buffer[2];
                                width = (buffer[3] << 8) | buffer[4];
                                LOG_DEBUG("Dimensions extracted: %u x %u pixels", width, height);
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

    LOG_DEBUG("Closing file after metadata extraction");
    file->close();

    // Measure render time (placeholder - would need actual rendering)
    renderTimeMs = 0;

    LOG_INFO("Metadata retrieved successfully: File size %u bytes, Dimensions %u x %u, Last modified %s", fileSize, width, height, lastModified.c_str());
    mSender.sendImageMetadata(filename, width, height, fileSize, lastModified, renderTimeMs);
}

uint32_t Service::ParseVersion(const char* str)
{
    LOG_DEBUG("Parsing version string: '%s'", str ? str : "null");
    if (str == nullptr) {
        LOG_DEBUG("Input string is null, returning default version 0");
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
    LOG_DEBUG("Attempting to parse version components using sscanf");

    int major, minor, patch;

    if (sscanf(str, "%d.%d.%d", &major, &minor, &patch) == 3) {
        v.major = static_cast<uint8_t>(major);
        v.minor = static_cast<uint8_t>(minor);
        v.patch = static_cast<uint8_t>(patch);
        LOG_DEBUG("Version parsed successfully: Major %d, Minor %d, Patch %d", major, minor, patch);
        LOG_DEBUG("Packing version into uint32_t: 0x%08X", v.u32);
        return v.u32;
    }

    LOG_DEBUG("Failed to parse version string, returning 0");
    return 0;
}
