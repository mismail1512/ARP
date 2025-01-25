# Drone Operation Interactive Simulator
Table of Contents
Project Overview

Architecture Sketch

Active Components

Directory Structure

Installation and Running Instructions

Operational Instructions

Makefile and Build Process

Logging and Debugging



## Project Overview
This project is an interactive drone operation simulator implemented using the ncurses library. The simulator features:

A drone controlled via keyboard inputs.

Targets that appear randomly and must be reached in sequence.

Obstacles that appear randomly and repel the drone.

A watchdog to monitor system health.

A blackboard server to manage the shared state of the world.

Real-time logging and parameter management via a YAML file.

The drone's movement is governed by a 2-degree-of-freedom dynamic model with inertia and viscous resistance. Obstacles and targets exert forces on the drone using the Latombe/Kathib model.




## Architecture Sketch
The system is divided into multiple processes communicating via pipes and shared memory. Below is a high-level architecture diagram:

+-------------------+       +-------------------+       +-------------------+

|  Keyboard Manager |       |  Obstacle Generator|       |  Target Generator  |

|       (I)         |       |       (O)          |       |       (T)          |

+-------------------+       +-------------------+       +-------------------+

        |                           |                           |
        
        |                           |                           |
        
        v                           v                           v
        
+-------------------+       +-------------------+       +-------------------+
|  Blackboard Server|       |  Drone Dynamics   |       |  Watchdog (W)     |
|       (B)         |       |       (D)         |       |                   |
+-------------------+       +-------------------+       +-------------------+
        |                           |                           |
        |                           |                           |
        v                           v                           v
+-------------------+       +-------------------+       +-------------------+
|  Visualizer       |       |  Logger           |       |  Params Manager   |
|  (ncurses Window) |       |                   |       |                   |
+-------------------+       +-------------------+       +-------------------+





ARP Project/

├── board.cpp                # Blackboard Server

├── obstacles_gen.cpp        # Obstacle Generator

├── params.yaml              # Simulation Parameters

├── README.md                # Project Documentation

├── run.sh                   # Execution Script

├── targets_gen.cpp          # Target Generator

├── watchdog.cpp             # Watchdog Process

├── window.cpp               # Visualizer (ncurses Window)

├── input.cpp                # Keyboard Manager

├── build/                   # Build Directory

│     └── cbuild.sh            # Build Script

├── CMakeLists.txt           # CMake Configuration

├── dynamics_server.cpp      # Drone Dynamics

├── exec.sh                  # Execution Script (Alternative)

├── exec1.sh                 # Execution Script (Alternative)

├── include/                 # Header Files

│     └── configs.hpp          # Global Configurations

├── lib/                     # Library Modules

│     ├── data_formats/        # Data Structures

│     ├── dynamics/            # Drone Dynamics

│     ├── logger/              # Logging Module

│     ├── objects_generator/   # Obstacle and Target Generators

│     ├── params_manager/      # Parameter Management

│     └── visualizer/          # ncurses Visualizer

├── logs/                    # Log Files

│     ├── board.log

│     ├── dynamics_server.log

│     ├── obstacles_gen.log

│     ├── targets_gen.log

│     └── window.log





## Active Components
### Blackboard Server (B):

Manages the shared state of the world (drone position, targets, obstacles).

Uses pipes and shared memory for inter-process communication.

Implements a select() loop to handle multiple clients.

### Drone Dynamics (D):

Simulates the drone's movement using a 2-DOF dynamic model.

Solves the motion equation using Euler's method.

Applies forces from keyboard inputs, obstacles, and targets.

### Keyboard Manager (I):

Handles user input for controlling the drone.

Maps keyboard keys to forces in 8 directions.

### Obstacle Generator (O):

Randomly generates and removes obstacles.

Uses the Latombe/Kathib model to calculate repulsive forces.

### Target Generator (T):

Randomly generates targets for the drone to reach.

Removes targets once the drone reaches them.

### Watchdog (W):

Monitors the health of all processes.

Logs errors and shuts down the system if a process fails.

### Visualizer:

Implements the ncurses window for real-time visualization.

Displays the drone, targets, obstacles, and an inspection window.

### Logger:

Logs system events to files in the logs/ directory.

Supports multiple log files for different processes.

### Params Manager:

Reads and manages simulation parameters from params.yaml.

Allows real-time updates to parameters.























ARP/
├── board.cpp
├── Master_Process.cpp
├── obstacles_gen.cpp
├── params.yaml
├── README.md
├── run.sh
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





















































