#include "PerformanceScopeTimer.h"

#include "Logger.h"

#include <Windows.h>

namespace Utility
{

PerformanceScopeTimer::PerformanceScopeTimer(const char* scope_name)
{
  _scope_name = scope_name;
  _start_time = GetTickCount64();
}

void PerformanceScopeTimer::ScopeComplete()
{
  uint64_t end_time = GetTickCount64();

  Sudoku::Logger::WriteLine("Start Tick: %u", _start_time);
  Sudoku::Logger::WriteLine("End Tick: %u", end_time);
  Sudoku::Logger::WriteLine("Scope - %s has finished (%u ms)", _scope_name.c_str(), end_time - _start_time);
}

} // namespace Utility
