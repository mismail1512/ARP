# Ensure the log folder exists
LOG_FOLDER="./log"
if [ ! -d "$LOG_FOLDER" ]; then
    echo "Log folder not found. Creating it..."
    mkdir -p "$LOG_FOLDER"
else
    echo "Log folder already exists."
fi


rm /tmp/board_to_obstacles
rm /tmp/targets_to_board
rm /tmp/window
rm /tmp/obstacles_to_board
rm /tmp/board_to_targets
rm /tmp/input_to_board





pkill obstacles_gen
pkill targets_gen
pkill board
pkill window



./build/obstacles_gen &
./build/targets_gen &
./build/board &
konsole -e ./build/input &
konsole -e ./build/window &
./build/watchdog 
