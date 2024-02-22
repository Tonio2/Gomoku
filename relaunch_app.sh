#!/bin/bash

# Navigate to the build directory
cd build

# Run the make command to build the application
make

# Move and rename the compiled file to the expected name
mv pygomoku.cpython-310-x86_64-linux-gnu.so pygomoku.so

# Navigate back to the original directory
cd ..

# Run the Python script
python gomoku.py
