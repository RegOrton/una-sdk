# UNA-Watch Simulator

The simulator application works only on Windows OS. UNA Simulator works on the TouchGFX Designer and Visual Studio programs.

## Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Opening SDK in VSCode](#opening-sdk-in-vscode)
4. [Compiling Projects](#compiling-projects)
5. [Compiling Examples](#compiling-examples) 

## Getting Started
### Need Components
1. Download and install [TouchGFXDesigner](https://www.st.com/en/development-tools/touchgfxdesigner.html#get-software).
2. If you want to work with UNA Simulator in Visual Studio, then:
   - Download and install [Visual Studio](https://visualstudio.microsoft.com).
   - In the Installing window, where you choose to install needed packages, go to Workloads -> Desktop & Mobile and here choose **Desktop development with C++**.
   - In Installation Details, **Desktop development with C++** choose MSVC V143 and then click to install.
3. Install UNA_SDK variables
   -
   -
   -
### Start Simulator in TouchGFX Designer
1. TouchGFX Designer doesn't support Debug mode. If you want to debbuging project, use the Visual Studio program.
1. Open TouhcGFX project.
   - Examples: Go to 
   - Tutorials: Go to 
2. If you first open project click to **Generate Code(F4)**, it is need olny one.
3. Click to **Run Simulator(F5)**.
### Start Simulator in Visual Studio
1. This point need **olny one**. Open the chosen application project in TouchGFX Designer and click to **Generate Code(F4)**.
2. Go to and open the file.
3. Click to start **Debug (F5)**.
### Simation Button
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
### Simation Buzzer
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
2. Only two log messages in the Backlight module, which is on the backlight, with a  time timeout in ms and off backlight
### Simulation Vibro
1. Action Vibro print to Terminal in the form of a log. For example:
   ```cpp
   I- Mock.Vibro::play::31                : play(melody[1])
   I- Mock.Vibro::play::34                :   [0] effect=1, loop=0, pause=0
   ```
2. Number in **[]** is Note array, **time** - Duration in ms, **level** -  Sound level 1,2,3, 0 - no sound
### Simulation Sensors
- In File **ConfigurationSimulator.hpp** you can enable/disable simulate sensor, set sensor paramaters.
- File **ConfigurationSimulator.hpp** located:
#### GPS Sensor
- Gps Sensor simulation located, speed, altitude and distance.
- Simulator simulation movement user on stadium.
- Have timer for simulation seach satelite.
- Simulate GPS signal loss.
- Noise for latitude/longitude.
- Possibilities: on/off Sensor, set min and max speed, value timer for valid GPS data.
  ```cpp
  ```
#### Heat Rate Sensor
- Heat Rate Sensor simulation HR, AHR, RHR and trust level.
- Possibilities: on/off Sensor, set min and max Heat Rate value, type training(Cycling, Hiking or Running).
#### Battery Level Sensor
- Battery Level Sensor simulation battery voltage drop.
- Possibilities: on/off Sensor, set initial value and 
#### IMU Sensor
### Include Header & Source file

### Transfer Application
