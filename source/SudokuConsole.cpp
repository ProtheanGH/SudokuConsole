// SudokuConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Logger.h"
#include "PerformanceScopeTimer.h"
#include "SudokuSolver.h"

#include <iostream>

int main()
{
    Sudoku::Logger::WriteLine("--- Sudoku Solver Console App ---");
    Sudoku::Logger::NewLine();

    {
      Sudoku::SudokuSolver solver;
      Sudoku::Logger::WriteLine("Loading puzzle data from: \"..\\runtime\\Sudoku.txt\"");
      solver.LoadFromFile("..\\runtime\\Sudoku.txt");

      Utility::PerformanceScopeTimer scope_timer("Sudoku Solve");
      solver.Solve();
      scope_timer.ScopeComplete();
    }

    // Current Impl doesn't currently work for the world's hardest Sudoku
    /*{
      Sudoku::SudokuSolver solver;
      Sudoku::Logger::WriteLine("Loading puzzle data from: \"..\\runtime\\Sudoku.txt\"");
      solver.LoadFromFile("..\\runtime\\Hardest_Sudoku.txt");

      Utility::PerformanceScopeTimer scope_timer("Sudoku Solve");
      solver.Solve();
      scope_timer.ScopeComplete();
    }*/
}
