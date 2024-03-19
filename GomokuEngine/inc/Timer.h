#include <iostream>
#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <iomanip>

std::string getColor(int depth);
std::string getIndent(int depth);

class FunctionAccumulation
{
public:
    double totalTime = 0;
    double otherTime = 0;
    int callCount = 0;

    void addTime(double time)
    {
        totalTime += time;
        callCount++;
    }

    double getAverageTime() const
    {
        return totalTime / std::max(callCount, 1);
    }
};

class Timer
{
private:
    using CallStack = std::vector<std::string>;
    static std::map<CallStack, FunctionAccumulation> accumulatedFunctions;
    static std::stack<CallStack> callStacks;
    static std::map<std::string, int> activeFunctions;

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    CallStack currentCallStack;
    std::string functionName;
    bool isRecursive;

    void updateParentSelfTime(double elapsedTime);
    void startTimer();
    void stopTimer();

public:
    Timer(const std::string &name);

    ~Timer();

    static double getAccumulatedTime(const std::string &name);

    static void printAccumulatedTimes();

    static void reset();
};