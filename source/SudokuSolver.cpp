#include "Logger.h"
#include "SudokuSolver.h"

#include <algorithm>
#include <fstream>
#include <string>

namespace Sudoku
{

// --- Macros
//! Expects a single char in the range of '0'-'9'
#define SimpleCharToInt(c) static_cast<int>(c) - 48

//! Expects a vector<Slot*>*, an integer to check against, and a bool to set if it was found
#define SearchSlotsForNumber(slots_vector, number, was_found)\
        for(auto iter = slots_vector->begin(); iter != slots_vector->end(); ++iter)\
        {\
          if((*iter)->_value == number)\
          {\
            was_found = true;\
            break;\
          }\
        }

//! Expects a vector<int>, an integer to check against, and a bool to set if it was found
#define SearchVectorForNumber(vector, number, was_found)\
        for(auto iter = vector.begin(); iter != vector.end(); ++iter)\
        {\
          if(*iter == number)\
          {\
            was_found = true;\
            break;\
          }\
        }

//! Expects a vector<Slot*>, an integer to check for, and a vector<Slot*> to store any updated slots that now only have one value
#define RemovePossibleValue(vector, number, ready_to_solve_vector)\
        for(auto slot_iter = vector->begin(); slot_iter != vector->end(); ++slot_iter)\
        {\
          for(auto possible_value_iter = (*slot_iter)->_possible_values.begin(); possible_value_iter != (*slot_iter)->_possible_values.end(); ++possible_value_iter)\
          {\
            if(*possible_value_iter == number)\
            {\
              (*slot_iter)->_possible_values.erase(possible_value_iter);\
              if((*slot_iter)->_possible_values.size() == 1)\
              {\
                ready_to_solve_vector.push_back(*slot_iter);\
              }\
              break;\
            }\
          }\
        }\

//! Expects a Slot* to represent the current Slot, vector<int> for the slots possible numbers, a vector<Slot*>* for the group to check, and a bool to set if it was found
#define CheckGroupForPossibleNumbers(self, possible_values_vector, slots_vector, was_found)\
        for(auto values_iter = possible_values_vector.begin(); values_iter != possible_values_vector.end(); ++values_iter)\
        {\
          was_found = false;\
          for(auto slot_iter = slots_vector->begin(); slot_iter != slots_vector->end(); ++slot_iter)\
          {\
            if(*slot_iter == self)\
            {\
              continue;\
            }\
            \
            SearchVectorForNumber((*slot_iter)->_possible_values, *values_iter, was_found);\
            if(was_found == true)\
            {\
              break;\
            }\
          }\
          \
          if(was_found == false)\
          {\
            self->_value = *values_iter;\
            self->_possible_values.clear();\
            break;\
          }\
        }

// --- Pubic Interface --- //
bool SudokuSolver::LoadFromFile(const char* file_path)
{
  std::ifstream file;

  file.open(file_path, std::ios::in);

  if(file.is_open() == false)
  {
    // Couldn't open the file
    return false;
  }

  std::string file_input;
  int input_length = -1;
  for(int line = 0; line < kSudokuSize; ++line)
  {
    std::getline(file, file_input);
    input_length = static_cast<int>(file_input.length());
    for(int index = 0; index < kSudokuSize; ++index)
    {
#if _DEBUG_SOLVING
      _puzzle_slots[line][index]._grid_row = line;
      _puzzle_slots[line][index]._grid_column = index;
#endif
      _puzzle_slots[line][index]._value = (index < input_length && file_input[index] >= '0' && file_input[index] <= '9') ? SimpleCharToInt(file_input[index]) : kEmptySlot;
    }
  }

  DisplayPuzzle();

  return true;
}

bool SudokuSolver::Solve()
{
  SetupDataGroups();

  DeterminePossibleValues();

  FindSolution();

  Logger::NewLine();
  Logger::WriteLine("Puzzle Solved:");
  Logger::NewLine();
  DisplayPuzzle();

  return true;
}

// --- Private Interface --- //
void SudokuSolver::DisplayPuzzle() const
{
  for(int row = 0; row < kSudokuSize; ++row)
  {
    if(row != 0 && row % 3 == 0)
    {
      Logger::WriteLine(" -  -  -  |  -  -  -  |  -  -  - ");
    }

    for(int col = 0; col < kSudokuSize; ++col)
    {
      if(col != 0 && col % 3 == 0)
      {
        Logger::Write(" | ");
      }

      if(_puzzle_slots[row][col]._value >= 0)
      {
        // Solved
        Logger::Write("[%d]", _puzzle_slots[row][col]._value);
      }
      else
      {
        // Empty
        Logger::Write("[ ]");
      }
    }

    Logger::NewLine();
  }
}

void SudokuSolver::SetupDataGroups()
{
  int row_group_index = 0;
  int col_group_index = 0;
  for(int row = 0; row < kSudokuSize; ++row)
  {
    row_group_index = (row / 3) * 3;

    for(int col = 0; col < kSudokuSize; ++col)
    {
      int col_group_index = (col / 3);

      // TODO-OPT: Should probably separate the empty slots from the solved ones, to save time on iterating
      // Add to group
      _groups[row_group_index + col_group_index].push_back(&_puzzle_slots[row][col]);
      _puzzle_slots[row][col]._group = &_groups[row_group_index + col_group_index];

      // Add to the columns
      _columns[col].push_back(&_puzzle_slots[row][col]);
      _puzzle_slots[row][col]._column = &_columns[col];

      // Add to the rows
      _rows[row].push_back(&_puzzle_slots[row][col]);
      _puzzle_slots[row][col]._row = &_rows[row];
    }
  }

#if _DEBUG_DATA
  VisualizeDataGroups();
#endif
}

void SudokuSolver::VisualizeDataGroups() const
{
  Logger::NewLine();

  // Visualize the Groups
  for(int i = 0; i < kSudokuSize; ++i)
  {
    Logger::WriteLine("Group #%d:", i);
    for(auto slot : _groups[i])
    {
      Logger::Write("%d  ", slot->_value);
    }
    Logger::NewLine(2);
  }

  // Visualize the Columns
  Logger::NewLine(2);
  for(int i = 0; i < kSudokuSize; ++i)
  {
    Logger::WriteLine("Column #%d:", i);
    for(auto slot : _columns[i])
    {
      Logger::Write("%d  ", slot->_value);
    }
    Logger::NewLine(2);
  }

  // Visualize the Rows
  Logger::NewLine(2);
  for(int i = 0; i < kSudokuSize; ++i)
  {
    Logger::WriteLine("Row #%d:", i);
    for(auto slot : _rows[i])
    {
      Logger::Write("%d  ", slot->_value);
    }
    Logger::NewLine(2);
  }
}

void SudokuSolver::DeterminePossibleValues()
{
  bool number_found = false;

  for(int row = 0; row < kSudokuSize; ++row)
  {
    for(int col = 0; col < kSudokuSize; ++col)
    {
      if(_puzzle_slots[row][col]._value != kEmptySlot)
      {
        continue;
      }

      for(int possible_number = 1; possible_number <= kSudokuSize; ++possible_number)
      {
        number_found = false;

        // Search the Group
        SearchSlotsForNumber(_puzzle_slots[row][col]._group, possible_number, number_found);
        if(number_found == true)
        {
          // Early out
          continue;
        }

        // Search the Column
        SearchSlotsForNumber(_puzzle_slots[row][col]._column, possible_number, number_found);
        if(number_found == true)
        {
          // Early out
          continue;
        }

        // Search the Row
        SearchSlotsForNumber(_puzzle_slots[row][col]._row, possible_number, number_found);
        if(number_found == true)
        {
          // Early out
          continue;
        }

        // Add the possible number
        _puzzle_slots[row][col]._possible_values.push_back(possible_number);
      }
    }
  }

#if _DEBUG_DATA
  //! Change the parameters to visualize whatever slot you want to see
  VisualizePossibleValues(0, 2);
#endif
}

void SudokuSolver::VisualizePossibleValues(int row, int column) const
{
  Logger::NewLine();

  Logger::WriteLine("Possible Values for Slot [%d][%d]:", row, column);
  for(auto possible_value : _puzzle_slots[row][column]._possible_values)
  {
    Logger::Write("%d ", possible_value);
  }

  Logger::NewLine();
}

void SudokuSolver::FindSolution()
{
  // A vector with all of the empty slots
  std::vector<Slot*> empty_slots;
  empty_slots.reserve(kSudokuSize * kSudokuSize); // The maximum we'll ever need
  // A vector for any slots that were updated, and now only have 1 possible value
  std::vector<Slot*> newly_single_slots;
  newly_single_slots.reserve(8); // Probably won't have more than this at one time?

  // Find all the empty slots that need to be solved
  for(int row = 0; row < kSudokuSize; ++row)
  {
    for(int col = 0; col < kSudokuSize; ++col)
    {
      if(_puzzle_slots[row][col]._value == kEmptySlot)
      {
        empty_slots.push_back(&_puzzle_slots[row][col]);
      }
    }
  }

  // Sort the vector by number of possible values
  struct {
    bool operator()(Slot* a, Slot* b)
    {
      // TODO-OPT: Might be more efficient to sort the other way, so the less possibilities are at the end, so removing is cheaper?
      return a->_possible_values.size() < b->_possible_values.size();
    }
  } SlotPossibleValuesSort;
  std::sort(empty_slots.begin(), empty_slots.end(), SlotPossibleValuesSort);

  // Start solving away
#if _DEBUG_SOLVING
  Logger::NewLine();
  Logger::WriteLine("Beginning to solve the puzzle ...");
#endif

  int back_slot_offset = 0;
  while(empty_slots.empty() == false)
  {
    if(empty_slots[0]->_possible_values.size() == 1)
    {
      // Only one possible value
      empty_slots[0]->_value = empty_slots[0]->_possible_values[0];
      empty_slots[0]->_possible_values.clear();

      // Remove that value from any associated spots that may also share that number
      RemovePossibleValue(empty_slots[0]->_group, empty_slots[0]->_value, newly_single_slots);
      RemovePossibleValue(empty_slots[0]->_column, empty_slots[0]->_value, newly_single_slots);
      RemovePossibleValue(empty_slots[0]->_row, empty_slots[0]->_value, newly_single_slots);

#if _DEBUG_SOLVING
      Logger::WriteLine("Solved Slot[%d,%d] with value: %d", empty_slots[0]->_grid_row, empty_slots[0]->_grid_column, empty_slots[0]->_value);
#endif

      // Remove this slot from the empty slots
      empty_slots.erase(empty_slots.begin());
    }
    else if(newly_single_slots.empty() == false)
    {
#if _DEBUG_SOLVING
      Logger::WriteLine("Handling the Newly Single Slots ...");
#endif
      do {
        // In rare cases, this slot could have been solved after it was added to the newly_single_slots vector, if so, just remove this slot
        if(newly_single_slots[0]->_value == kEmptySlot)
        {
          // Only one possible value
          newly_single_slots[0]->_value = newly_single_slots[0]->_possible_values[0];
          newly_single_slots[0]->_possible_values.clear();

          // Remove that value from any associated spots that may also share that number
          RemovePossibleValue(newly_single_slots[0]->_group, newly_single_slots[0]->_value, newly_single_slots);
          RemovePossibleValue(newly_single_slots[0]->_column, newly_single_slots[0]->_value, newly_single_slots);
          RemovePossibleValue(newly_single_slots[0]->_row, newly_single_slots[0]->_value, newly_single_slots);

#if _DEBUG_SOLVING
          Logger::WriteLine("Solved Slot[%d,%d] with value: %d", newly_single_slots[0]->_grid_row, newly_single_slots[0]->_grid_column, newly_single_slots[0]->_value);
#endif

          // Remove it from the empty slots vector
          // TODO-OPT: there's gotta be a better way than doing this
          for(size_t i = 0; i < empty_slots.size(); ++i)
          {
            if(empty_slots[i] == newly_single_slots[0])
            {
              empty_slots.erase(empty_slots.begin() + i);
              --i;
            }
          }
        }

        // Remove this slot from the vector
        newly_single_slots.erase(newly_single_slots.begin());
      } while(newly_single_slots.empty() == false);
    }
    else
    {
#if _DEBUG_SOLVING
      Logger::WriteLine("Beginning slot has more than 1 possible value, checking slots against their groups ...");
#endif
      
      bool was_found = false;
      do {
#if _DEBUG_SOLVING
        //Logger::WriteLine("Checking Slot[%d,%d] for possible solutions ...", empty_slots[0]->_grid_row, empty_slots[0]->_grid_column);
#endif

        was_found = false;
        // Check the 3x3 group
        CheckGroupForPossibleNumbers(empty_slots[0], empty_slots[0]->_possible_values, empty_slots[0]->_group, was_found);

        if(was_found == false)
        {
          // Remove that value from any associated spots that may also share that number
          RemovePossibleValue(empty_slots[0]->_column, empty_slots[0]->_value, newly_single_slots);
          RemovePossibleValue(empty_slots[0]->_row, empty_slots[0]->_value, newly_single_slots);

#if _DEBUG_SOLVING
          Logger::WriteLine("Solved Slot[%d,%d] with value: %d", empty_slots[0]->_grid_row, empty_slots[0]->_grid_column, empty_slots[0]->_value);
#endif

          // Remove this slot from the empty slots
          empty_slots.erase(empty_slots.begin());
          continue;
        }

        // Check the Column group
        CheckGroupForPossibleNumbers(empty_slots[0], empty_slots[0]->_possible_values, empty_slots[0]->_column, was_found);
        if(was_found == false)
        {
          // Remove that value from any associated spots that may also share that number
          RemovePossibleValue(empty_slots[0]->_group, empty_slots[0]->_value, newly_single_slots);
          RemovePossibleValue(empty_slots[0]->_row, empty_slots[0]->_value, newly_single_slots);

#if _DEBUG_SOLVING
          Logger::WriteLine("Solved Slot[%d,%d] with value: %d", empty_slots[0]->_grid_row, empty_slots[0]->_grid_column, empty_slots[0]->_value);
#endif

          // Remove this slot from the empty slots
          empty_slots.erase(empty_slots.begin());
          continue;
        }

        // Check the Row group
        CheckGroupForPossibleNumbers(empty_slots[0], empty_slots[0]->_possible_values, empty_slots[0]->_row, was_found);
        if(was_found == false)
        {
          // Remove that value from any associated spots that may also share that number
          RemovePossibleValue(empty_slots[0]->_group, empty_slots[0]->_value, newly_single_slots);
          RemovePossibleValue(empty_slots[0]->_column, empty_slots[0]->_value, newly_single_slots);

#if _DEBUG_SOLVING
          Logger::WriteLine("Solved Slot[%d,%d] with value: %d", empty_slots[0]->_grid_row, empty_slots[0]->_grid_column, empty_slots[0]->_value);
#endif

          // Remove this slot from the empty slots
          empty_slots.erase(empty_slots.begin());
          continue;
        }
        else
        {
          // That slot's not ready to be solved yet, swap it to the end of the vector
          if(back_slot_offset == empty_slots.size() - 2)
          {
            back_slot_offset = 0;
          }
          std::iter_swap(empty_slots.begin(), empty_slots.begin() + (empty_slots.size() - 1 - back_slot_offset));
          ++back_slot_offset;
        }

      } while(newly_single_slots.empty() == true && empty_slots.empty() == false);

      if(newly_single_slots.empty() == true && empty_slots.empty() == false && empty_slots[0]->_possible_values.size() > 1)
      {
        // TODO - still no single possible value slots
        int debug = 0;
        debug = 0;
      }
    }
  }
}

} // namespace Sudoku
