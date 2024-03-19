#include "Timer.h"

std::map<Timer::CallStack, FunctionAccumulation> Timer::accumulatedFunctions;
std::stack<Timer::CallStack> Timer::callStacks;
std::map<std::string, int> Timer::activeFunctions;

Timer::Timer(const std::string &name) : functionName(name), isRecursive(false)
{
  activeFunctions[name]++;
  isRecursive = activeFunctions[name] > 1;
  if (!isRecursive)
  {
    startTimer();
  }
}

Timer::~Timer()
{
  if (!isRecursive)
  {
    stopTimer();
  }
}

double Timer::getAccumulatedTime(const std::string &name)
{
  return accumulatedFunctions[{name}].totalTime;
}

void Timer::updateParentSelfTime(double elapsedTime)
{
  if (currentCallStack.size() > 1)
  {
    CallStack parentCallStack = currentCallStack;
    parentCallStack.pop_back();
    accumulatedFunctions[parentCallStack].otherTime -= elapsedTime;
  }
}

void Timer::startTimer()
{
  if (!callStacks.empty())
  {
    currentCallStack = callStacks.top();
  }
  currentCallStack.push_back(functionName);
  callStacks.push(currentCallStack);
  start = std::chrono::high_resolution_clock::now();
}

void Timer::stopTimer()
{
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end - start;
  activeFunctions[functionName]--;

  if (!currentCallStack.empty() && currentCallStack.back() == functionName)
  {
    auto &funcAccum = accumulatedFunctions[currentCallStack];
    funcAccum.addTime(elapsed.count());
    funcAccum.otherTime += elapsed.count();
    updateParentSelfTime(elapsed.count());
    callStacks.pop();
  }
}

std::string getColor(int depth)
{
  std::vector<std::string> colors = {
      "\033[1;31m", // Red
      "\033[1;34m", // Blue
      "\033[1;32m", // Green
      "\033[1;33m", // Yellow
      "\033[1;35m", // Magenta
      "\033[1;36m"  // Cyan
  };
  return colors[depth % colors.size()];
}

/*
 * Returns a string with the proper indentation for the given depth
 * @param depth callstack.size()
 */
std::string getIndent(int depth)
{
  std::string indent;
  for (int i = 0; i < depth - 1; ++i)
  {
    indent += (i < depth - 2) ? "|   " : "|-- ";
  }
  return indent;
}

void Timer::printAccumulatedTimes()
{
  // Then, print each entry with proper formatting
  for (auto it = accumulatedFunctions.begin(); it != accumulatedFunctions.end(); ++it)
  {
    const auto &entry = *it;
    const CallStack &callStack = entry.first;
    const FunctionAccumulation &accumulation = entry.second;

    std::string indent = getIndent(callStack.size());
    std::string color = getColor(callStack.size());

    // Print the current function with its color and time, but skip printing "other" here
    std::cout << indent << color << accumulation.callCount << " " << callStack.back() << ": "
              << static_cast<int>(accumulation.totalTime) << " ms\033[0m" << std::endl;

    if (accumulation.otherTime < accumulation.totalTime)
    {
      indent = getIndent(callStack.size() + 1);
      color = getColor(callStack.size() + 1);
      std::cout << indent << color << "other: "
                << static_cast<int>(accumulation.otherTime) << " ms\033[0m" << std::endl;
    }
  }
}

void Timer::reset()
{
  accumulatedFunctions.clear();
  while (!callStacks.empty())
  {
    callStacks.pop();
  }
}