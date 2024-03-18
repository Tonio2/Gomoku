#include "Timer.h"

std::map<Timer::CallStack, Timer::FunctionAccumulation> Timer::accumulatedFunctions;
std::stack<Timer::CallStack> Timer::callStacks;
std::map<std::string, int> Timer::activeFunctions;
