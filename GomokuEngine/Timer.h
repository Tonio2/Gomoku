#include <iostream>
#include <chrono>
#include <map>
#include <string>

class Timer {
private:
    std::string functionName;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    static std::map<std::string, double> accumulatedTimes;

public:
    Timer(const std::string& name) : functionName(name), start(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        accumulatedTimes[functionName] += elapsed.count();
    }

    static void printAccumulatedTimes() {
        for (const auto& pair : accumulatedTimes) {
            std::cout << "Function " << pair.first << " took " << pair.second << " ms in total.\n";
        }
    }
};
