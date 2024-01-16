#!/bin/bash

# Compile client.c into executable client
gcc client.c -o client

# Check for compilation errors
if [ $? -ne 0 ]; then
    echo "Compilation of client.c failed."
    exit 1
fi

# Compile server.c into executable server
gcc server.c -o server

# Check for compilation errors
if [ $? -ne 0 ]; then
    echo "Compilation of server.c failed."
    exit 1
fi

pip3 install -r requirements.txt
echo "Compilation successful!"
