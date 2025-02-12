#!/bin/bash
# Ensure the log folder exists
LOG_FOLDER="./log"
if [ ! -d "$LOG_FOLDER" ]; then
    echo "Log folder not found. Creating it..."
    mkdir -p "$LOG_FOLDER"
else
    echo "Log folder already exists."
fi








# Display the initial message
echo "Make sure the config.h variables TARGETS_TOPIC_NAME, OBSTACLES_TOPIC_NAME, START_X, START_Y, WIDTH, HEIGHT, DRONE_POS_X, DRONE_POS_Y, target_number, obstacles_number are the same on both sides."

# Prompt the user for their choice
echo "Do you want to run the target and obstacle generators or the main application?"
echo "Enter 1 for target and obstacle generators, 2 for the main application:"
read -r choice

# Execute logic based on user's choice
if [ "$choice" -eq 1 ]; then
    echo "Running target and obstacle generators..."
    sudo pkill obstacles_gen
    sudo pkill targets_gen
    ./build/obstacles_gen &
    ./build/targets_gen 
elif [ "$choice" -eq 2 ]; then

    echo "Running the main application..."

    
    rm /tmp/window
    rm /tmp/input_to_board
    sudo pkill board
    sudo pkill window
    ./build/board &
    konsole -e ./build/input &
    konsole -e ./build/window &
    # ./build/watchdog
else
    echo "Invalid choice. Please enter 1 or 2."
fi
