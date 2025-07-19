# SudokuSolver

A simple Sudoku Solver with a Windows API GUI, written in C++.

**Author:** Rohit Ahirwar

## Features
- 9x9 editable Sudoku grid
- Solve, Clear, and Exit buttons
- Shows solution or error if unsolvable

## How to Build (PowerShell)
1. Open PowerShell in this folder.
2. Run:
   ```powershell
   g++ -std=c++17 -mwindows -o SudokuSolver.exe main.cpp sudoku_solver.cpp
   ```
   (Requires MinGW g++ in your PATH)

## How to Use
- Enter your Sudoku puzzle (leave empty cells blank or 0).
- Click **Solve** to fill in the solution.
- Click **Clear** to reset the grid.
- Click **Exit** to close the app.

---
*This project is for educational purposes.* 