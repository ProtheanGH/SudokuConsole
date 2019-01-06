#include "Logger.h"

#include <cstdarg>
#include <iostream>

namespace Sudoku
{
// --- Constants
#define BUFFER_SIZE 1024

// --- Macros
#define LogToConsole(message) std::cout << message;

#define LogFormatToConsole(format)\
        va_list arguments;\
        va_start(arguments, format);\
        vprintf_s(format, arguments);\
        va_end(arguments);

void Logger::Write(const char* format, ...)
{
  LogFormatToConsole(format);
}

void Logger::WriteLine(const char* format, ...)
{
  LogFormatToConsole(format);

  LogToConsole(std::endl);
}

void Logger::NewLine()
{
  LogToConsole(std::endl);
}

void Logger::NewLine(int count)
{
  for(int i = 0; i < count; ++i)
  {
    LogToConsole(std::endl);
  }
}

} // namepsace Sudoku
