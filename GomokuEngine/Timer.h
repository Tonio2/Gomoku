#include <iostream>
#include <chrono>
#include <map>
#include <string>
#include <set>

class Timer {
private:
    std::string functionName;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    static std::map<std::string, double> accumulatedTimes;
    static std::map<std::string, int> count;
    static std::set<std::string> activeFunctions;
    bool isActive;

public:
    Timer(const std::string& name) : functionName(name), isActive(false) {
        // Check if the function is already being timed
        if (activeFunctions.find(name) == activeFunctions.end()) {
            std::cout << "Starting timer for function " << name << ".\n";
            start = std::chrono::high_resolution_clock::now();
            activeFunctions.insert(name);
            isActive = true;
            count[name]++;
        }
    }

    ~Timer() {
        if (isActive) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            accumulatedTimes[functionName] += elapsed.count();
            activeFunctions.erase(functionName);
        }
    }

    static void printAccumulatedTimes() {
        for (const auto& pair : accumulatedTimes) {
            std::cout << "Function " << pair.first << " took " << pair.second << " ms in total.\n";
        }
    }
};
