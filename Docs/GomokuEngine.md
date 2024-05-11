## Update env before compiling

BUILD_TYPE=DEBUG * DEBUG Mode is for using gdb *
TIMER=OFF * TIMER ON is for having more information about time spent in each function *

## Compile gktool

```bash
cd GomokuEngine
make
```

## Test problems

Tell the desired depth and start the executable

```bash
DEPTH=3 ./gktool
```

## Test a specific problem

```bash
./gktool 5
```

## Test the heuristic evaluation for a certain position

```bash
./gktool eval "55,66,77..."
```

## Test reconizer for a certain structure

```bash
./gktool line " X 0 0"
```

## Start unit tests

```bash
docker compose up
```