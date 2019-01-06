#pragma once

#include <string>

namespace Utility
{

typedef unsigned long long uint64_t;

//! Simple class for tracking how long certain scopes take to execute
class PerformanceScopeTimer
{
public:
  PerformanceScopeTimer(const char* scope_name);

  void ScopeComplete();

private:
  std::string _scope_name;
  uint64_t _start_time;
};

} // namespace Utility
