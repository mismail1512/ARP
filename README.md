# Drone Operation Interactive Simulator

## Table of Contents

Project Overview

Directory Structure

Active Components

Installation and Running Instructions



## Project Overview
This project is an interactive drone operation simulator implemented using the ncurses library. The simulator features:

A drone controlled via keyboard inputs.

Targets that appear randomly and must be reached in sequence.

Obstacles that appear randomly and repel the drone.

A watchdog to monitor system health.

A blackboard server to manage the shared state of the world.

Real-time logging and parameter management via a YAML file.

The drone's movement is governed by a 2-degree-of-freedom dynamic model with inertia and viscous resistance. Obstacles and targets exert forces on the drone using the Latombe/Kathib model.

Directory Structure
The system is divided into multiple processes communicating via pipes and shared memory. Below is a high-level architecture diagram:


```
ARP Project/
├── board.cpp
├── obstacles_gen.cpp
├── params.yaml
├── README.md
├── targets_gen.cpp
├── watchdog.cpp
├── window.cpp
├── input.cpp
├── build/
│   └── cbuild.sh
├── CMakeLists.txt
├── dynamics_server.cpp
├── exec.sh
├── exec1.sh
├── include/
│   └── configs.hpp
├── lib/
│   ├── data_formats/
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   ├── commands.hpp
│   │   │   ├── drone_state.hpp
│   │   │   ├── object_order.hpp
│   │   │   ├── point.hpp
│   │   │   └── world_state.hpp
│   ├── dynamics/
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   └── dynamics.hpp
│   │   ├── src/
│   │       └── dynamics.cpp
│   ├── logger/
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   └── logger.hpp
│   │   ├── src/
│   │       └── logger.cpp
│   ├── objects_generator/
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   └── objects_generator.hpp
│   │   ├── src/
│   │       └── objects_generator.cpp
│   ├── params_manager/
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   └── params_manager.hpp
│   │   ├── src/
│   │       └── params_manager.cpp
│   ├── visualizer/
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   └── visualizer.hpp
│   │   ├── src/
│   │       └── visualizer.cpp
│   ├── CMakeLists.txt
├── logs/
│   ├── board.log
│   ├── dynamics_server.log
│   ├── obstacles_gen.log
│   ├── targets_gen.log
│   └── window.log
```

##  Active Components

### Blackboard Server (B)
**Purpose:**
The Blackboard Server acts as the central hub for managing the shared state of the simulation. It maintains the positions of the drone, targets, and obstacles, and ensures that all processes are synchronized.

**Key Responsibilities:**  

- Manages the WorldState object, which contains:

- Drone position.

- Target positions.

- Obstacle positions.

- Simulation borders (geofence).

- Communicates with other processes via pipes:

- Receives drone dynamics updates from the Dynamics Server.

- Receives user input from the Input Manager.

- Receives obstacle and target updates from the Obstacle Generator and Target Generator.

- Sends the current state to the Visualizer for rendering.

- *Handles signals:*

SIGUSR1: Pause/resume the simulation.

SIGUSR2: Reset the simulation.


- Calculates the score:

Based on the number of targets reached, time taken, and distance traveled.

**Interactions:**
- Dynamics Server: Receives drone position updates and sends the current state.

- Input Manager: Receives user commands (e.g., movement, pause, reset).

- Obstacle Generator: Receives obstacle positions and sends the current state.
 
- Target Generator: Receives target positions and sends the current state.

- Visualizer: Sends the current state for rendering.


### Drone Dynamics (D)
**Purpose:**
The Drone Dynamics module simulates the drone's movement based on the forces applied by user input, obstacles, and targets. It uses a 2-degree-of-freedom (2-DOF) dynamic model to calculate the drone's position and velocity.

**Key Responsibilities:**
- Implements the dynamic model:

- Uses Euler's method to numerically solve the motion equations.

- Applies forces to the drone:

Command forces: Generated from user input (keyboard commands).

Repulsion forces: Generated from obstacles using the Latombe/Kathib model.

Attraction forces: (Optional) Generated from targets using the same model.

- Updates the drone's position:

- Calculates the new position and velocity based on the applied forces.

- Sends the updated position to the Blackboard Server.

**Interactions:**

- Blackboard Server: Receives the current state and sends updated drone positions.

- Obstacle Generator: Uses obstacle positions to calculate repulsion forces.

- Target Generator: Uses target positions to calculate attraction forces.


###  Input Process (I)
**Purpose:**
The Input Process handles user input for controlling the drone. It translates keyboard inputs into commands that are sent to the Blackboard Server to update the drone's movement.

**Key Responsibilities:**
- Maps keyboard keys to forces in 8 directions:

The following keys are used to control the drone:

```
w e r
s d f
x c v

ps to pause\start and reset to reset
```

The center key (d) acts as a brake.

- Sends commands to the Blackboard Server via a pipe.

- Handles signals:

ps: Pause the simulation / Start the simulation.

reset: Reset the simulation.

**Interactions:**

- Blackboard Server: Sends user commands (e.g., movement, pause, reset).

- Other Processes: Sends signals to pause, resume, or reset the simulation.




### Obstacle Generator Process (O)

**Purpose:**
The Obstacle Generator randomly generates and removes obstacles within the simulation environment. It ensures that obstacles do not overlap with the drone or targets.

**Key Responsibilities:**
- Generates obstacles:

- Randomly places obstacles within the simulation borders.

- Ensures obstacles do not overlap with the drone or targets.

- Updates obstacle positions:

- Periodically updates obstacle positions and sends them to the Blackboard Server.

- Removes obstacles:

- Randomly removes obstacles after a certain period or when the drone interacts with them.

**Interactions:**

- Blackboard Server: Receives the current state and sends updated obstacle positions.


### 5. Target Generator (T)
**Purpose:**
The Target Generator randomly generates targets for the drone to reach. It ensures that targets are placed in valid locations and removes them once the drone reaches them.

**Key Responsibilities:**
- Generates targets:

- Randomly places targets within the simulation borders.

- Ensures targets do not overlap with the drone or obstacles.

- Removes targets:

- Removes a target once the drone reaches it.

- Updates target positions:

- Periodically generates new targets and sends their positions to the Blackboard Server.

**Interactions:**

- Blackboard Server: Receives the current state and sends updated target positions.




### 6. Watchdog (W)
**Purpose:**
The Watchdog monitors the health of all processes in the system. It ensures that all processes are running correctly and terminates the system if any process fails or becomes unresponsive.

**Key Responsibilities:**
- Monitors log files:

- Checks for heartbeat messages from each process in the logs/ directory.

-Detects inactivity:

If a process fails to send a heartbeat within a specified timeout, the Watchdog detects it as inactive.

- Terminates the system:

If a process fails or becomes unresponsive, the Watchdog terminates the entire system to prevent further issues.

**Interactions:** 

- All Processes: Monitors their activity via log files.

- System Control: Terminates the system if a process fails.



### 7. Visualizer
**Purpose:**
The Visualizer implements the ncurses window for real-time visualization of the simulation. It provides a graphical representation of the drone, targets, obstacles, and other relevant information.

**Key Responsibilities:**
- Displays the simulation:

- Renders the drone, targets, and obstacles in a full-screen window.

- Updates the display in real-time based on the current state received from the Blackboard Server.

- Shows additional information:

- Displays the score, drone position, and other relevant details in an inspection window.

- Refreshes the display:

- Continuously updates the window to reflect changes in the simulation state.

**Interactions:** 

- Blackboard Server: Receives the current state for rendering.




### 8. Logger
**Purpose:**
The Logger is responsible for logging system events to files in the logs/ directory. It ensures that each process has its own log file for debugging and monitoring purposes.

**Key Responsibilities:**
- Logs system events:

- Writes heartbeat messages, errors, and other events to log files.

- Supports multiple log files:

- Each process (e.g., Blackboard Server, Dynamics Server, etc.) has its own dedicated log file.

- Ensures traceability

- Logs include timestamps and process IDs for easy debugging and monitoring.

Interactions:

- All Processes: Logs events from each process to their respective log files.

- Watchdog: Provides heartbeat messages for monitoring process health.



### 9. Params Manager
**Purpose:**
The Params Manager reads and manages simulation parameters from the params.yaml file. It allows for real-time updates to parameters, ensuring flexibility and adaptability during the simulation.

**Key Responsibilities:**
- Reads parameters:

- Loads simulation parameters (e.g., drone mass, viscous coefficient, repulsion distance) from the params.yaml file.

- Manages parameters:

- Provides a map of parameters for other modules to use.

- Supports real-time updates:

- Allows parameters to be updated dynamically during the simulation without restarting the system.

**Interactions:**

- All Processes: Provides access to simulation parameters.

- Dynamics Server: Uses parameters for calculating forces and updating drone dynamics.


## Installation and Running Instructions
 
1- After downloading the repo, create the logs and build directories

```
mkdir logs
mkdir buld
```

2- build and run
```
./build.sh
./run.sh
```
Note: you have to install Konsole in your device beacusewe use konsole application in the display of window and input process

