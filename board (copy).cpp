if (worldState.cmd == Command::reset) {
                
    // Reset the drone position (using default values, you can set it to any specific values)
    worldState.drone_position = {5.0, 5.0};  // Reset drone position to (5.0, 5.0)

    // Reset obstacles and targets positions by clearing their arrays (or setting to default points)
    for (int i = 0; i < obstacles_number; ++i) {
        worldState.obstacles_positions[i] = {0.0, 0.0};  // Reset obstacles positions (default to {0.0, 0.0})
    }

    for (int i = 0; i < target_number; ++i) {
        worldState.targets_positions[i] = {0.0, 0.0};  // Reset targets positions (default to {0.0, 0.0})
    }

    // Optional: You can regenerate obstacles and targets here if necessary, based on your logic
    // Example:
    // Generate new positions for obstacles
    ObjectsGenerator obstacles_obj_gen{worldState.getBorder().startX, 
                                       worldState.getBorder().startX + worldState.getBorder().width - 1,
                                       worldState.getBorder().startY, 
                                       worldState.getBorder().startY + worldState.getBorder().height - 1,
                                       obstacles_number};
    std::vector<Point> obstacles = obstacles_obj_gen.genrateObjects({worldState.drone_position});  // Exclude the drone position

    // Reassign the generated obstacles to the world state
    for (int i = 0; i < obstacles_number; ++i) {
        worldState.obstacles_positions[i] = obstacles[i];
    }

    // Generate new target positions
    ObjectsGenerator targets_obj_gen{worldState.getBorder().startX, 
                                     worldState.getBorder().startX + worldState.getBorder().width - 1,
                                     worldState.getBorder().startY, 
                                     worldState.getBorder().startY + worldState.getBorder().height - 1,
                                     target_number};
    std::vector<Point> targets = targets_obj_gen.genrateObjects({worldState.drone_position});  // Exclude the drone position

    // Reassign the generated targets to the world state
    for (int i = 0; i < target_number; ++i) {
        worldState.targets_positions[i] = targets[i];
    }

   /** // Optionally, you can notify other processes or systems about the reset by writing to the pipes
    write(board_to_targets_fd, &worldState, sizeof(worldState));  // Send updated world state to the targets generator
    write(board_to_obstacles_fd, &worldState, sizeof(worldState));  // Send updated world state to the obstacles generator

    // Send the reset world state to dynamics
    write(dynamics_to_board_fd, &worldState, sizeof(worldState));*/

            }