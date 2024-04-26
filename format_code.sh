#!/bin/bash

# Exit on first error
set -e

echo "Formatting TypeScript and React files..."
# Ensure Prettier is installed or install it
if ! [ -x "$(command -v prettier)" ]; then
  echo "Prettier is not installed. Installing..."
  npm install -g prettier
fi
# Format .ts and .tsx files
prettier --write "**/*.ts"
prettier --write "**/*.tsx"

echo "Formatting Python files..."
# Ensure Black is installed or install it
if ! [ -x "$(command -v black)" ]; then
  echo "Black is not installed. Installing..."
  pip install black
fi
# Format all Python files
black . --exclude '/GomokuEngine/external|/venv'

echo "Formatting C++ files..."
# Ensure clang-format is installed or install it
if ! [ -x "$(command -v clang-format)" ]; then
  echo "clang-format is not installed. Installing..."
  sudo apt-get install -y clang-format
fi
# Format all C++ files (.cpp, .hpp, .cc, .cxx)
find . \( -path './GomokuEngine/external' -prune -o -path './GomokuEngine/build' -prune \) -o -name 'pybinds.cpp' -prune -o -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -i -style=file {} +

echo "Formatting complete."

