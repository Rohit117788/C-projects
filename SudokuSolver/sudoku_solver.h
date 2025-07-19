#pragma once

class SudokuSolver {
public:
    static bool Solve(int grid[9][9]);
    static bool IsValid(int grid[9][9], int row, int col, int num);
}; 