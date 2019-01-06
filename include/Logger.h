#pragma once
#include "pch.h"

namespace Sudoku
{

class Logger
{
public:
  //! Write
  //  Formats the given message with the provided arguments and then writes it to the console. Does not add a newline to the end of the message.
  static void Write(const char* format, ...);
  //! WriteLine
  //  Formates the given message with the provided arguments, along with a newline at the end and then writes it to the console.
  static void WriteLine(const char* format, ...);

  //! NewLine
  //  Writes a line break to the console.
  static void NewLine();
  //! NewLine
  //  Writes the given amount of line breaks to the console.
  static void NewLine(int count);
};

} // namespace Sudoku
