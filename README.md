﻿# UWB_RTLS

## Overview

This project implements a UWB Real-Time Location System (RTLS) with powered anchors and tags.

![Hardware](images/ESP32_UWB_DW1000.png)

The [hardware](https://www.makerfabs.com/esp32-uwb-ultra-wideband.html) is a module based on DW1000.

Location of tag is published through micro ROS for easy interfacing and visualization.

The anchor calibration is based on this [repo](https://github.com/jremington/UWB-Indoor-Localization_Arduino?tab=readme-ov-file).

The linked list method of storing variable number of anchors is based on this [repo](https://github.com/playfultechnology/UWBRTLPS).

## Table of Contents
- [UWB\_RTLPS](#uwb_rtlps)
  - [Overview](#overview)
  - [Table of Contents](#table-of-contents)
  - [Multilateration](#multilateration)
    - [2D Multilateration](#2d-multilateration)
    - [3D Multilateration](#3d-multilateration)
  - [PlatformIO Configuration](#platformio-configuration)
    - [Anchor Calibration](#anchor-calibration)
    - [Anchor Coordinates](#anchor-coordinates)
  - [Visualization on RViz2](#visualization-on-rviz2)
    - [Building micro-ROS agent, uwb\_viz and micro\_ros\_interfaces](#building-micro-ros-agent-uwb_viz-and-micro_ros_interfaces)
    - [Running micro-ros agent](#running-micro-ros-agent)
    - [Running uwb\_viz](#running-uwb_viz)
    - [Running RViz2](#running-rviz2)
    - [.bashrc alias for QoL](#bashrc-alias-for-qol)
  - [Testing](#testing)
  - [List of Known Bugs and Issues](#list-of-known-bugs-and-issues)
    - [TODO](#todo)

## Multilateration 

The multilateration implemented is based on the technique described in [this paper](https://www.th-luebeck.de/fileadmin/media_cosa/Dateien/Veroeffentlichungen/Sammlung/TR-2-2015-least-sqaures-with-ToA.pdf), where the locations of anchors and the distances between the tag and anchors are known.

For 2D multilateration, a minimum of 3 anchors is needed. For 3D multilateration, a minimum of 4 anchors is needed. One of the anchors is used to linearize the system so it can be solved more easily through linear algebra.

### 2D Multilateration

In 2D multilateration, the coordinates of the tag are determined using the distances from at least 3 anchors. The Z coordinate is set to 0.

<span style="color: red;">**Important:** To ensure invertability of matrix during 2D multilateration, anchors cannot be collinear.

### 3D Multilateration

In 3D multilateration, the coordinates of the tag are determined using the distances from at least 4 anchors. This allows for the calculation of the X, Y, and Z coordinates.

<span style="color: red;">**Important:** To ensure invertability  of matrix during 3D multilateration, anchors cannot be coplanar.

## PlatformIO Configuration

The `platformio.ini` file includes the configuration for anchor calibration (different from hardware to hardware) and anchor coordinates used for multilateration.

### Anchor Calibration

The reconmended receive power for calibration is different for different channels and pulse repetition frequency (PRF). Currenly, this implementation uses channel 4 and PRF of 16

![Calibration_Reconmendation](images/Calibration_Reconmendation.png)
Above picture is from the [DW1000 user manual](https://www.sunnywale.com/uploadfile/2021/1230/DW1000%20User%20Manual_Awin.pdf).

To perform anchor calibration, follow these steps:

1. **Flash the Tag with Normal Operation Tag Code**: Ensure the tag is running the normal tag code.
2. **Edit the Anchor Calibration Code**: Update the code to set the target distance to the current distance apart.
3. **Flash the Anchor to be Calibrated with Anchor Calibration Code**: Ensure the anchor is running the calibration code, it will run until it converges onto the best antenna delay value.
4. **Update the `platformio.ini`**: Update the corresponding anchor in the `platformio.ini` file with the best antenna delay value.
5. **Flash the Anchor with Normal Operation Code**: Flash the anchor with the normal operation code to double-check the distance estimated by UWB.

### Anchor Coordinates
With a reference anchor at (0, 0, 0), measure the other tags in a Cartesian coordinate system. The unit of measurement does not matter as long as it is consistent for all measurements. The coordinates (0, 0, 0) can be arbitrarily chosen for convenience.

For setups with only 3 anchors, only 2D trilateration is possible. In this case, fill the Z coordinate with 0.

Update the `platformio.ini` file with the corresponding anchor coordinates.

## Visualization on RViz2

### Building micro-ROS agent, uwb_viz and micro_ros_interfaces

The micro-ROS agent is the bridge running on a PC that connects the MCU running micro-ROS to ROS 2. It is already included in the repository as a submodule.

uwb_viz subscribes to topic from UWB device and publish the coordinates as markers to be visualised on RViz2

micro_ros_interfaces is the custom message types for UWB

To build them, follow these steps:

1. **Navigate to the ROS2 Workspace**:
    ```sh
    cd uwb_ws
    ```

2. **Build the Workspace**:
    ```sh
    colcon build
    ```

3. **Source the Environment**:
    ```sh
    source install/local_setup.bash
    ```
### Running micro-ros agent
Make sure that the PC and UWB device are connected to the same network and have the same `ROS_DOMAIN_ID`.

1. **Navigate to the Workspace**:
    ```sh
    cd uwb_ws
    ```

2. **Source the Environment**:
    ```sh
    source install/local_setup.bash
    ```
    
3. **Set the ROS Domain ID**:
    ```sh
    export ROS_DOMAIN_ID=57
    ```

4. **Running the Agent for WiFi**:
    ```sh
    ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
    ```

5. **Running the Agent for Serial (currently not used)**:
    ```sh
    ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0
    ```

### Running uwb_viz
To run the UWB visualization tool, follow these steps:

1. **Navigate to the Workspace**:
    ```sh
    cd uwb_ws
    ```

2. **Source the Environment**:
    ```sh
    source install/local_setup.bash
    ```

3.  **Set the ROS Domain ID**:
    ```sh
    export ROS_DOMAIN_ID=57
    ```
    
4. **Run the UWB Visualization**:
    ```sh
    ros2 run uwb_viz uwb_viz_node
    ```

### Running RViz2
To visualize the data using RViz2, follow these steps:

1. **Run `rviz2`**:
    ```sh
    rviz2
    ```

2. **Configure `rviz2`**:
    - Add marker from the topic `/visualization_marker` 

### .bashrc alias for QoL
```sh
alias uwburos='export ROS_DOMAIN_ID=57 && cd ~/{YOUR_PATH}/UWB_RTLPS/uwb_ws && source install/setup.bash && ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888'
alias uwbviz='export ROS_DOMAIN_ID=57 && cd ~/{YOUR_PATH}/UWB_RTLPS/uwb_ws && source install/setup.bash && ros2 run uwb_viz uwb_viz_node'
alias uwbecho='export ROS_DOMAIN_ID=57 && cd ~/{YOUR_PATH}/UWB_RTLPS/uwb_ws && source install/setup.bash && ros2 topic echo uwb_pub'
alias uwbrviz='export ROS_DOMAIN_ID=57 && rviz2'
```

## Testing
For this particular test, the anchors are placed as shown with the tag at (-2m, -2m, 0m) from the origin at anchor 1.

Since only 3 anchors are available, this is a 2D trilateration.

![Test Setup](images/2m_2m_Test_Setup.png)

Below shows the ideal distances from Geogebra

![Geogebra](images/2m_2m_Geogebra.png)

Below shows the picture of measured distances and estimated tag coordinates.

![RViz and Terminal](images/2m_2m_Test_RViz_and_Terminal.png)

The estimated distances for anchor more than 3 m away are within 20 cm, while for anchor less than 3 m away are within 30 cm and has very large variance.

The tag coordinates are within ±30cm of the actual coordinates, with error mainly contributed by anchor less than 3 m away. With more anchors and some filtering this may be improved.

## List of Known Bugs and Issues
1. microROS agent IP is hardcoded, if it changes it will not be able to connect to microROS agent even if both are connected to the same network
2. Sometimes when changing branch PIO Full Clean doesnt clear everything, have to delete contents in the .pio directory and rebuild
3. tag debug and release both does serial debug print 

### TODO

- [ ] Support multiple tags
- [ ] Test 3D with more anchors
- [ ] Fix timing issues causing invalid distance values due to ROS publishing 
  - [ ] Splitting dual core doesnt solve it fully, need to check that it is not waiting for range to return before running other functions
  - [ ] filtering range values before performing mulitlateration helps (this is done)
- [ ] Implement a loop in the setup to avoid needing a reset if the window to connect to micro-ROS agent is missed, or allow tag to run even if agent is not connected for debugging
- [ ] Update `platformio.ini` to include tag and anchor numbers for node naming and addressing
- [ ] Update `platformio.ini` to include WiFi or serial option.
- [ ] Remove hardcoded wifi credential
- [ ] Implement low power mode
