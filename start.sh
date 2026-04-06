#!/bin/bash

# Start the Open Stop Motion Studio application.
# This script assumes it is located in the root directory of the extracted application.

# Find the executable. It should have the same name as the project file without extension.
EXECUTABLE_NAME="OpenStopMotionStudio"

# Check if the executable exists
if [ ! -f "./$EXECUTABLE_NAME" ]; then
    echo "Error: Executable '$EXECUTABLE_NAME' not found in the current directory."
    echo "Please ensure this script is in the same directory as the application executable."
    exit 1
fi

# Make sure it's executable (might not be necessary if tar preserves permissions, but good practice)
chmod +x "./$EXECUTABLE_NAME"

# Run the application
./"$EXECUTABLE_NAME"
