# **Part2: FastDDS Communication Application**

## **Overview**

The goal of Part2 is to utilize **FastDDS** for seamless communication across multiple devices using TCP. The application is divided into two main components:

1. **Obstacles and Targets Generators**: These generate and continuously publish random obstacles and targets.
2. **Main Application**: This manages the board, drone, and interactions with the generated obstacles and targets.

The communication protocol used is **UDTCP**, which ensures accurate message delivery. Since the architecture is simple and small, **auto-discovery** is enabled, and occasional message loss is acceptable due to the continuous nature of message flow.

### **Important Note**

Before running the application:

1. Ensure both devices are on the **same LAN network**.
2. Verify that the following variables in `include/config.h` are identical on both sides:
   - `TARGETS_TOPIC_NAME`
   - `OBSTACLES_TOPIC_NAME`
   - `START_X`, `START_Y`
   - `WIDTH`, `HEIGHT`
   - `DRONE_POS_X`, `DRONE_POS_Y`
   - `target_number`
   - `obstacles_number`

These variables are crucial for ensuring proper communication and functionality between the devices.
### The architecture for the communication 

![System Communication Architecture](DDS.png)


### Test Video 

Here you will find a test video for the  DDS --> the test on two computers on is publisher and the other is subscriber and vise versa

https://drive.google.com/file/d/1DJTD1fpjzmkX70z_RnElX_fs9wNN3nM6/view?usp=sharing

---

## **How to Build**

1. Create a build directory:

2. Run the build script:

```bash
mkdir build
./build.sh
```

## **How to Run**

Run the application by executing the following script:

```bash
./run.sh
```


## **Implementation Details**

### **DDS Helper Library**

The **DDS Helper Library** is located in `lib/dds_helper` and contains two classes:

- **Publisher**: Handles publishing of targets and obstacles.
- **Subscriber**: Handles receiving data for targets and obstacles.

These classes manage the **FastDDS communication layer** for the application.

---

### **Main Components**

1. **Board**:
   - Acts as the central manager for obstacles, targets, and drone interactions.
   - Handles scoring by removing a target when the drone steps on it.

2. **Obstacles and Targets Generators**:
   - Continuously generate and publish random obstacles and targets.
   - **Obstacles**: Change randomly over time.
   - **Targets**: Remain static until a drone interacts with them.
---

### **Managing Targets by the Board**

When the drone steps on a target (starting with the smallest number), the **board**:

- Removes the target.
- Updates the score based on predefined rules.

---


