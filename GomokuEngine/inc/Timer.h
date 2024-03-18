#include <iostream>
#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <iomanip>

class Timer
{
private:
    struct FunctionAccumulation
    {
        double totalTime = 0;
        int callCount = 0;

        void addTime(double time)
        {
            totalTime += time;
            callCount++;
        }

        double getAverage() const
        {
            return totalTime / std::max(callCount, 1);
        }
    };

    using FunctionNode = std::map<std::string, FunctionAccumulation>;
    using CallStack = std::vector<std::string>;

    static std::map<CallStack, FunctionAccumulation> accumulatedFunctions;
    static std::stack<CallStack> callStacks;
    static std::map<std::string, int> activeFunctions;

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    CallStack currentCallStack;
    std::string functionName;
    bool isRecursive;

public:
    Timer(const std::string &name) : functionName(name), isRecursive(false)
    {
        activeFunctions[name]++;
        if (activeFunctions[name] > 1)
        {
            isRecursive = true;
        }
        else
        {
            if (!callStacks.empty())
            {
                currentCallStack = callStacks.top();
            }

            currentCallStack.push_back(name);
            callStacks.push(currentCallStack);
            start = std::chrono::high_resolution_clock::now();
        }
    }

    ~Timer()
    {
        activeFunctions[functionName]--;
        if (!isRecursive)
        {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;

            if (!currentCallStack.empty() && currentCallStack.back() == functionName)
            {
                accumulatedFunctions[currentCallStack].addTime(elapsed.count());
            }
            callStacks.pop();
        }
    }

    static void printAccumulatedTimes()
    {
        std::vector<std::string> colors = {
            "\033[1;31m", // Red
            "\033[1;34m", // Blue
            "\033[1;32m", // Green
            "\033[1;33m", // Yellow
            "\033[1;35m", // Magenta
            "\033[1;36m"  // Cyan
        };

        // Then, print each entry with proper formatting
        for (auto it = accumulatedFunctions.begin(); it != accumulatedFunctions.end(); ++it)
        {
            const auto &entry = *it;
            const CallStack &callStack = entry.first;
            const FunctionAccumulation &accumulation = entry.second;

            std::string indent;
            for (size_t i = 0; i < callStack.size() - 1; ++i)
            {
                indent += (i < callStack.size() - 2) ? "|   " : "|-- ";
            }

            std::string color = colors[(callStack.size() - 1) % colors.size()];

            // Print the current function with its color and time, but skip printing "other" here
            std::cout << indent << color << callStack.back() << ": "
                      << static_cast<int>(accumulation.totalTime) << " ms\033[0m" << std::endl;

            // Calculate and print "other" time after printing all child functions
            auto nextIt = std::next(it);
            const CallStack &nextCallStack = nextIt->first;
            const FunctionAccumulation &nextAccumulation = nextIt->second;
            if (nextIt != accumulatedFunctions.end() && nextCallStack.size() > callStack.size())
            {
                double childrenTime = 0;
                while (nextIt != accumulatedFunctions.end() && nextCallStack.size() > callStack.size())
                {
                    if (nextCallStack.back() != "other")
                    {
                        childrenTime += nextAccumulation.totalTime;
                    }
                    ++nextIt;
                }

                double otherTime = accumulation.totalTime - childrenTime;
                if (otherTime > 0)
                {
                    indent = "";
                    for (size_t i = 0; i < callStack.size(); ++i)
                    {
                        indent += (i < callStack.size() - 1) ? "|   " : "|-- ";
                    }

                    color = colors[(callStack.size()) % colors.size()];
                    std::cout << indent << color << "other: "
                              << static_cast<int>(otherTime) << " ms\033[0m" << std::endl;
                }
            }
        }
    }

    static void reset()
    {
        accumulatedFunctions.clear();
        while (!callStacks.empty())
        {
            callStacks.pop();
        }
    }
};