# Gomoku Terminal Game

A fast-paced, high-performance Gomoku game that runs directly in your terminal. This project combines Python for user interactions and C++ for the game engine, with pybind11 used to bridge the two languages.

## Prerequisites

Before you start, make sure you have the following installed on your system:
- Python 3.6 or higher
- CMake 3.4 or higher
- A C++ compiler (GCC or Clang)
- Git

## Installation

Follow these steps to install and run the Gomoku game after cloning the repository:

### 1. Clone the Repository

```bash
git clone https://your-repository-url/gomoku.git
cd gomoku
```

### 2. Create a Python Virtual Environment (Optional)

```bash
python3 -m virtualenv venv
source venv/bin/activate
```
### 3. Install Python Dependencies

```bash
pip install -r requirements.txt
```

### 4. Initialize Submodules

```bash
git submodule update --init --recursive
```

### 5. Build the Engine and the Display

```bash
make
```
### 6. Run the Game

```bash
./Gomoku
```

## LINTER

### 1. Copy pre-commit scrip

```bash
cp ./pre-commit.example .git/hooks/pre-commit
chmod u+x .git/hooks/pre-commit
```

### 2. Format code

```bash
./format_code.sh
```

