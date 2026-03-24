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
  -I- Mock.Buzzer::play::39               : [0] time=150 ms, level=3
  -I- Mock.Buzzer::play::39               : [1] time=100 ms, level=0
  -I- Mock.Buzzer::play::39               : [2] time=150 ms, level=3
   ```
2. 
### Simulation Backlight

### Simulation Sensor

### Include Header & Source file

### Transfer Application
