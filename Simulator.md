# UNA-Watch Simulator

The simulator application works only on Windows OS. UNA Simulator works on the TouchGFX Designer and Visual Studio programs.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Prerequisites](#prerequisites)
3. [Opening SDK in VSCode](#opening-sdk-in-vscode)
4. [Compiling Projects](#compiling-projects)
5. [Simulation Sensors](#simulation-sensors) 

## Getting Started
### Need Components
1. Download and install [TouchGFXDesigner](https://www.st.com/en/development-tools/touchgfxdesigner.html#get-software).
2. If you want to work with UNA Simulator in Visual Studio, then:
   - Download and install [Visual Studio](https://visualstudio.microsoft.com).
   - In the Installing window, where you choose to install needed packages, go to Workloads -> Desktop & Mobile and here choose **Desktop development with C++**.
   - In Installation Details, **Desktop development with C++** choose MSVC V143 and then click to install.
3. Install UNA_SDK variable
   - Start **RunAddVariable.bat**, located: una-sdk\Utilities\Scripts\add-sdk-variable.
   - Enter 1 to install/update the UNA_SDK variable in Environment Variables.
### Start Simulator in TouchGFX Designer
1. TouchGFX Designer doesn't support Debug mode. If you want to debbuging project, use the Visual Studio program.
1. Open TouhcGFX project.
   - Examples: Go to 
   - Tutorials: Go to 
2. If you first open project click to **Generate Code(F4)**, it is need olny one.
3. Click to **Run Simulator(F5)**.
### Start Simulator in Visual Studio
1. This point need **only one**. Open the chosen application project in TouchGFX Designer and click to **Generate Code(F4)**.
2. Go to <app_name>\Software\Apps\TouchGFX-GUI\simulator\msvs and open the **Application.vcxproj** file.
3. Click to start **Debug (F5)**.
### Simulation Button
1. For the Simulation mechanicals button in the UNA Simulator, use keyboard keys.
2. You can see this message in Terminal Log, there how number keys are responsible for buttons on UNA watch.
   ```cpp
   ---------------------------------------------------
   |   For Simulation Button use keybaord Keys.      |
   |       Keys Keybaord:                            |
   |       1   L1,                                   |
   |       2   L2,                                   |
   |       3   R1,                                   |
   |       4   R2                                    |
   |                  /---------\                    |
   |                 /           \                   |
   | BUTTON UP   L1 |             | R1 BUTTON SELECT |
   |                |     UNA     |                  |
   |                |    WATCH    |                  |
   | BUTTON DOWN L2 |             | R2 BUTTON BACK   |
   |                 \           /                   |
   |                  \---------/                    |
   ---------------------------------------------------
   ```
3. Info: 
### Simulation Buzzer
1. Action Buzzer print to Terminal in the form of a log. For example:
   ```cpp
   I- Mock.Buzzer::play::39               : [0] time=150 ms, level=3
   I- Mock.Buzzer::play::39               : [1] time=100 ms, level=0
   I- Mock.Buzzer::play::39               : [2] time=150 ms, level=3
   ```
2. Number in **[]** is Note array, **time** - Duration in ms, **level** -  Sound level 1,2,3, 0 - no sound
3. For 
### Simulation Backlight
1. Action Backlight print to Terminal in the form of a log. For example:
   ```cpp
   I- Mock.Backlight::on::22              : on backlight, timeout = 5000
   I- Mock.Backlight::off::30             : off backlight
   ```
2. Only two log messages in the Backlight module, which is on the backlight, with a  time timeout in ms and off the backlight
### Simulation Vibro
1. Action Vibro print to Terminal in the form of a log. For example:
   ```cpp
   I- Mock.Vibro::play::31                : play(melody[1])
   I- Mock.Vibro::play::34                :   [0] effect=1, loop=0, pause=0
   ```
2. Number in **[]** is Note array, **time** - Duration in ms, **level** -  Sound level 1,2,3, 0 - no sound
### Simulation Sensors
- In File **ConfigurationSimulator.hpp**, you can enable/disable the simulation sensor and set simulation parameters.
- File **ConfigurationSimulator.hpp** located: <app-name>\Software\Libs\Header
#### GPS Sensor
- GPS Sensor simulation located, speed, altitude, and distance.
- Simulator simulation movement user on stadium.
- Have a timer for the simulation of each satellite.
- Simulate GPS signal loss.
- Noise for latitude/longitude.
- Possibilities: on/off Sensor, set min and max speed, value timer for search GPS satellites.
  ```cpp
   //GPS Sensor
   #define GSP_SIM_ENABLE               1  // 0 - Disable
   #define GSP_SIM_SPEED_MIN            20 // km/h
   #define GPS_SIM_SPEED_BASE           25 // km/h
   #define GPS_SIM_SPEED_MAX            30 // km/h
   #define GPS_SIM_TIME_SEACH_SATELLITE 7 // seconds
  ```
#### Heat Rate Sensor
- Heat Rate Sensor simulation HR, AHR, RHR, and trust level.
- Possibilities: on/off Sensor, set min and max Heat Rate value, type training(Cycling, Hiking, or Running).
  ```cpp
   //HeatRate Sensor
   #define HEAT_RATE_SIM_ENABLE        1 // 0 - Disable
   #define HEAT_RATE_SIM_MIN_HR        50
   #define HEAT_RATE_SIM_MAX_HR        140
   #define HEAT_RATE_SIM_TYPE_TRAINING 0// 0 - Cycling, 1 - Hiking, 2 - Running

  ```
#### Battery Level Sensor
- Battery Level Sensor simulation battery voltage drop.
- Possibilities: on/off Sensor, set initial value, and step value.
  ```cpp
   // Battery Level Sensor
   #define BATT_LEVEL_SIM_ENABLE      1 // 0 - Disable
   #define BATT_LEVEL_SIM_START_VALUE 100 // 10 - 100%
   #define BATT_LEVEL_SIM_STEP_VALUE  0.1 //percent 
  ```
#### IMU Sensor
- IMU Sensor simulation Wring detect. Wring detect event activate backlight for 5 seconds.
- Possibilities: on/off Sensor, change key to generate Wring detect.
- For simulate a Wring detect event, click the **5** key on the keyboard.
  ```cpp
   // IMU Sensor
   #define IMU_SIM_ENABLE           1 // 0 - Disable
   #define IMU_SIM_WRIST_DETECT_KEY 5 
  ```
### Include Header & Source file
1. TouchGFX Designer
   - Open **Application.vcxproj** in the text editor program, located: <app_name>\Software\Apps\TouchGFX-GUI\simulator\msvs.
   - Header file path add in **ADDITIONAL_INCLUDE_PATHS** variable.
   - Source file path add in **ADDITIONAL_SOURCES_UNA** variable.
3. Visual Studio
   - Open **MakeFile** in the text editor program, located: <app-name>\Software\Apps\TouchGFX-GUI\simulator\gcc
   - Header file path add in **ClInclude** variable.
   - Source file path add in **ClCompile** variable.
### Transfer Application
1. If you move the Application in other place, you need to update the Toucgfx lib path, for this:
   - Open **<name>.touchgfx** in the text editor program, located: <app_name>\Software\Apps\TouchGFX-GUI.
   - Find **"TouchGfxPath":** and update relative path to <una_sdk>/ThirdParty/touchgfx.
   ```cpp
    "SelectedStartupLanguage": "GB",
    "TouchGfxPath": "../../../../../../ThirdParty/touchgfx",
    "UIPath": ".",
   ```
2. If you changes located **una_sdk** folder, you need update UNA_SDK variable, for this do point [Install UNA_SDK variable](#install-una-sdk-varible) 
