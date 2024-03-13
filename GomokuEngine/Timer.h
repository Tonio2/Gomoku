#include <iostream>
#include <chrono>
#include <map>
#include <string>
#include <set>

class Timer {
private:
    std::string functionName;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    struct FunctionAccumulation {
        double totalTime = 0;
        int callCount = 0;

        void operator+=(double count) {
            totalTime += count;
            callCount++;
        }

        double getAverage() const {
            return totalTime / std::max(callCount, 1);
        }
    };

    static std::map<std::string, FunctionAccumulation> accumulatedFunctions;
    static std::set<std::string> activeFunctions;
    bool isActive;

public:
    Timer(const std::string& name) : functionName(name), isActive(false) {
        // Check if the function is already being timed
        if (activeFunctions.find(name) == activeFunctions.end()) {
            start = std::chrono::high_resolution_clock::now();
            activeFunctions.insert(name);
            isActive = true;
        }
    }

    ~Timer() {
        if (isActive) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            accumulatedFunctions[functionName] += elapsed.count();
            activeFunctions.erase(functionName);
        }
    }

    static void printAccumulatedTimes() {
        for (const auto& pair : accumulatedFunctions) {
            std::cout << "Function " << pair.first << " used " << pair.second.callCount << " times took " << pair.second.totalTime
                << " ms in total for an average of "
                << pair.second.getAverage() << ".\n";
        }
    }
};
