# Ensure the log folder exists
LOG_FOLDER="./log"
if [ ! -d "$LOG_FOLDER" ]; then
    echo "Log folder not found. Creating it..."
    mkdir -p "$LOG_FOLDER"
else
    echo "Log folder already exists."
fi
./build/obstacles_gen &
./build/targets_gen &
./build/board &
konsole -e ./build/input &
konsole -e ./build/window &
./build/watchdog 
