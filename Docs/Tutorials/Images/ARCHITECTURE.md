(tutorials/images/architecture)=

# Images Tutorial: Building a JPEG Image Viewer

Welcome to the UNA SDK Images tutorial! This comprehensive guide walks you through implementing a complete JPEG image viewer application with four screens: menu navigation, image list, image viewer, and image properties. You'll learn how to integrate file system operations, handle image loading, extract metadata, and create a responsive TouchGFX GUI.

## What You'll Learn

- How to implement multi-screen TouchGFX applications
- File system integration using IFileSystem interface
- JPEG image loading and metadata extraction
- GUI-backend communication patterns
- TouchGFX MVP (Model-View-Presenter) architecture
- Hardware button event handling for navigation

## Application Overview

The Images app provides a complete JPEG viewer with:
- **ImageMenu**: Navigation menu to access different screens
- **ImageList**: Scrollable list of JPG files from the app folder
- **ImageViewer**: Full-screen JPEG display with scaling
- **ImageProp**: JPEG metadata display (dimensions, file size, modification time, render time)

## Prerequisites

- UNA SDK environment set up (see [SDK setup](../../sdk-setup.md))
- TouchGFX Designer installed
- ARM GCC toolchain and build tools
- Basic understanding of C++ and TouchGFX

## Step-by-Step Implementation

### Step 1: Project Setup and Architecture

1. **Create the project structure**:
   - Copy an existing TouchGFX project (like ScrollMenu) as a starting point
   - Rename directories and files to reflect the Images app
   - Update CMakeLists.txt with new project name and APP_ID

2. **Set up the four screens in TouchGFX Designer**:
   - Create screens: ImageMenu, ImageList, ImageViewer, ImageProp
   - Add basic containers and backgrounds to each screen
   - Generate initial code

3. **Establish the MVP architecture**:
   - Each screen has Presenter, View, and Presenter base classes
   - Model handles communication between GUI and Service
   - Service manages file system operations and image processing

### Step 2: Define Communication Interfaces

1. **Add event types to AppTypes.hpp**:
   ```cpp
   // GUI to Backend events
   struct RequestImageList {};
   struct SelectImage { std::string filename; };
   struct RequestMetadata { std::string filename; };

   // Backend to GUI events
   struct ImageList { std::vector<std::string> filenames; };
   struct ImageLoaded { std::string filename; bool success; };
   struct Metadata {
       std::string filename;
       uint32_t width, height, fileSize;
       std::string lastModified;
       uint32_t renderTimeMs;
   };
   ```

2. **Create message structures in Commands.hpp**:
   ```cpp
   struct ImageListMsg : public SDK::MessageBase {
       std::vector<std::string> filenames;
   };
   struct ImageLoadedMsg : public SDK::MessageBase {
       std::string filename; bool success;
   };
   struct ImageMetadataMsg : public SDK::MessageBase {
       std::string filename; uint32_t width, height, fileSize;
       std::string lastModified; uint32_t renderTimeMs;
   };
   ```

3. **Add virtual methods to ModelListener.hpp**:
   ```cpp
   virtual void updateImageList(const std::vector<std::string>& filenames) {}
   virtual void updateImageLoaded(const std::string& filename, bool success) {}
   virtual void updateImageMetadata(const std::string& filename, uint32_t width,
                                   uint32_t height, uint32_t fileSize,
                                   const std::string& lastModified, uint32_t renderTimeMs) {}
   ```

### Step 3: Implement Backend Service

1. **Update Service.hpp**:
   ```cpp
   class Service {
   private:
       std::unique_ptr<SDK::Interface::IFileSystem> mFileSystem;
       std::vector<std::string> mImageList;
       std::string mAppFolderPath;

       // Image methods
       void scanImageFolder();
       void sendImageList();
       void loadImage(const std::string& filename);
       void getImageMetadata(const std::string& filename);
   };
   ```

2. **Initialize file system in Service.cpp constructor**:
   ```cpp
   Service::Service(SDK::Kernel& kernel)
       : mKernel(kernel), mSender(mKernel), mGUIStarted(false),
         mFileSystem(nullptr), mAppFolderPath("/images")
   {
       // Initialize file system from kernel
       // mFileSystem = kernel.getFileSystem(); // Implementation specific
   }
   ```

3. **Implement image scanning**:
   ```cpp
   void Service::scanImageFolder() {
       mImageList.clear();
       if (!mFileSystem) return;

       auto dir = mFileSystem->dir(mAppFolderPath.c_str());
       if (!dir || !dir->open()) return;

       SDK::Interface::IFileSystem::ObjectInfo item;
       while (dir->readNext(item)) {
           if (!item.isDir) {
               std::string name = item.name;
               if (name.size() > 4 && name.substr(name.size() - 4) == ".jpg") {
                   mImageList.push_back(name);
               }
           }
       }
       dir->close();
   }
   ```

4. **Implement metadata extraction**:
   ```cpp
   void Service::getImageMetadata(const std::string& filename) {
       uint32_t width = 0, height = 0, fileSize = 0, renderTimeMs = 0;
       std::string lastModified = "unknown";

       // Open file and read JPEG header
       auto file = mFileSystem->file((mAppFolderPath + "/" + filename).c_str());
       if (file && file->open()) {
           // Read SOF0 marker to get dimensions
           // Implementation would parse JPEG headers here
           file->close();
       }

       // Get file info
       SDK::Interface::IFileSystem::ObjectInfo info;
       if (mFileSystem->objectInfo((mAppFolderPath + "/" + filename).c_str(), info)) {
           fileSize = info.size;
           // Convert time_t to string
       }

       mSender.sendImageMetadata(filename, width, height, fileSize, lastModified, renderTimeMs);
   }
   ```

5. **Add message handling in Service run loop**:
   ```cpp
   // In Service::run(), add handling for GUI messages:
   case CUSTOM_GUI_TO_SERVICE_MESSAGE_TYPE: {
       // Handle RequestImageList, SelectImage, RequestMetadata
       // Call appropriate methods
   } break;
   ```

### Step 4: Implement GUI Components

1. **Update Model.cpp for message handling**:
   ```cpp
   bool Model::customMessageHandler(SDK::MessageBase *msg) {
       switch (msg->getType()) {
           case IMAGE_LIST: {
               auto* m = static_cast<ImageListMsg*>(msg);
               modelListener->updateImageList(m->filenames);
           } break;
           // Handle other message types
       }
       return true;
   }

   bool Model::sendEventToBackend(const AppType::G2BEvent::Data &data) {
       // Send events to Service via kernel messages
       // Implementation routes events to Service
   }
   ```

2. **Implement ImageList screen**:
   - **Presenter**: Requests image list on activation
   - **View**: Populates ScrollList with filenames, handles selection

3. **Implement ImageViewer screen**:
   - **Presenter**: Loads selected image on activation
   - **View**: Displays ScalableImage, handles zoom/pan if needed

4. **Implement ImageProp screen**:
   - **Presenter**: Requests metadata for current image
   - **View**: Displays metadata in TextArea widgets

5. **Implement ImageMenu screen**:
   - **View**: Sets up menu with 4 items (List, Viewer, Properties, Back)
   - Handles navigation between screens

### Step 5: Add UI Widgets and Text Resources

1. **Update Images.touchgfx JSON**:
   ```json
   {
     "Name": "ImageList",
     "Components": [
       {
         "Type": "ScrollList",
         "Name": "scrollList1",
         "Width": 240, "Height": 240,
         "ItemTemplate": "TextArea"
       }
     ]
   }
   ```

2. **Add text resources to texts.xml**:
   ```xml
   <Text Id="T_MENU" Alignment="Center" TypographyId="Poppins_SemiBold_25">
     <Translation Language="GB">Image Menu</Translation>
   </Text>
   <Text Id="T_LIST" Alignment="Center" TypographyId="Poppins_SemiBold_18">
     <Translation Language="GB">Image List</Translation>
   </Text>
   <!-- Add other menu texts -->
   ```

### Step 6: Implement Navigation and Event Handling

1. **Menu navigation in ImageMenuView**:
   ```cpp
   void ImageMenuView::wheelUpdateItem(MenuItemNotSelected &item, int16_t index) {
       switch (index) {
           case 0: item.setText(T_LIST); break;
           case 1: item.setText(T_VIEWER); break;
           case 2: item.setText(T_PROPS); break;
           case 3: item.setText(T_BACK); break;
       }
   }

   void ImageMenuView::handleKeyEvent(uint8_t key) {
       if (key == R1) {  // Select
           int selected = menu1.getSelectedItem();
           // Navigate to appropriate screen
           switch (selected) {
               case 0: application().gotoImageListScreen(); break;
               case 1: application().gotoImageViewerScreen(); break;
               case 2: application().gotoImagePropScreen(); break;
               case 3: application().gotoMainScreen(); break;
           }
       }
       // Handle L1/L2 for navigation
   }
   ```

2. **Screen transitions**:
   - Update FrontendApplication with new screen transition methods
   - Ensure proper cleanup and state management

### Step 7: Testing and Debugging

1. **Build the application**:
   ```bash
   cd $UNA_SDK/Docs/Tutorials/Images/Software/Apps/TouchGFX-GUI
   mkdir build && cd build
   cmake -G "Unix Makefiles" ../
   make
   ```

2. **Test in simulator**:
   - Verify menu navigation works
   - Test image list population
   - Check image loading and display
   - Validate metadata extraction

3. **Debug common issues**:
   - File system access permissions
   - JPEG parsing errors
   - Memory management for large images
   - TouchGFX widget invalidation

## Key Architecture Insights

### MVP Pattern Implementation
- **Model**: Handles communication and state management
- **View**: Manages UI widgets and user interaction
- **Presenter**: Coordinates between Model and View

### File System Integration
- Uses IFileSystem interface for platform-independent file operations
- Handles directory scanning and file metadata
- Proper error handling for file access failures

### Image Processing
- JPEG header parsing for metadata extraction
- Dynamic bitmap creation for image display
- Memory management for large image files

### Event-Driven Communication
- Clean separation between GUI and backend
- Asynchronous message passing via kernel
- Proper error handling and timeouts

## Common Patterns and Best Practices

### Memory Management
- Use smart pointers for file system objects
- Clean up resources in destructors
- Handle out-of-memory conditions gracefully

### Error Handling
- Check return values from file operations
- Provide user feedback for failures
- Log errors for debugging

### Performance Considerations
- Cache frequently accessed data
- Minimize file I/O operations
- Use efficient image decoding

### Code Organization
- Separate concerns between layers
- Use clear naming conventions
- Document complex algorithms

## Troubleshooting

### File System Issues
- Verify file system initialization
- Check path permissions
- Ensure proper cleanup of file handles

### Image Loading Problems
- Validate JPEG file format
- Check available memory
- Handle corrupted files gracefully

### GUI Update Issues
- Ensure proper invalidation after changes
- Check widget visibility and positioning
- Verify text encoding and fonts

## Next Steps

1. **Add image manipulation features** (zoom, pan, rotate)
2. **Implement image caching** for better performance
3. **Add thumbnail generation** for the list view
4. **Support additional image formats** (PNG, BMP)
5. **Add image editing capabilities** (crop, filter)

This tutorial provides a complete foundation for building image viewing applications on the UNA platform. The patterns and techniques demonstrated here can be extended to create rich, interactive media applications.