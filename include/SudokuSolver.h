#pragma once

#include <vector>

namespace Sudoku
{

// Technology Defines
//! Turn this on to help visualize the data that gets setup and sorted
#define _DEBUG_DATA 0
//! Turn this on to help visualize how the data is being solved
#define _DEBUG_SOLVING 0

// --- Constants
#define kSudokuSize 9
#define kEmptySlot -1

class SudokuSolver
{
public:
  SudokuSolver() {};
  ~SudokuSolver() {};

  bool LoadFromFile(const char* file_path);
  bool Solve();

private:
  struct Slot
  {
#if _DEBUG_SOLVING
    int _grid_row;
    int _grid_column;
#endif
    int _value = kEmptySlot;
    std::vector<int> _possible_values;
    std::vector<Slot*>* _group;
    std::vector<Slot*>* _column;
    std::vector<Slot*>* _row;
  };

  void DisplayPuzzle() const;
  void SetupDataGroups();
  void VisualizeDataGroups() const;
  void DeterminePossibleValues();
  void VisualizePossibleValues(int row, int column) const;
  void VisualizePossibleValueGrid() const;
  void FindSolution();

  //! The Sudoku Puzzle in the form of a 2D array that matches how it gets displayed (rows and columns)
  Slot _puzzle_slots[kSudokuSize][kSudokuSize];
  //! The Sudoku Puzzle broken into the 3x3 groups (Only the solved slots)
  std::vector<Slot*> _groups[kSudokuSize];
  //! The Sudoku Puzzle broken into the individual vertical columns (Only the solved slots)
  std::vector<Slot*> _columns[kSudokuSize];
  //! The Sudoku Puzzle broken into the individual horizontal rows (Only the solved slots)
  std::vector<Slot*> _rows[kSudokuSize];
};

} // namespace Sudoku
