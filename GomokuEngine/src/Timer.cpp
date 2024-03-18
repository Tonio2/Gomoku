#include "Timer.h"

std::map<std::string, Timer::FunctionAccumulation> Timer::accumulatedFunctions;
std::set<std::string> Timer::activeFunctions;
