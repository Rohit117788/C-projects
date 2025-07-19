#include <windows.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include "sudoku_solver.h"

#define IDC_BASE 1000
#define IDC_SOLVE 2000
#define IDC_CLEAR 2001
#define IDC_EXIT 2002

HINSTANCE hInst;
HWND hGrid[9][9];
int sudoku[9][9];

void LoadGridToUI() {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            char buf[2] = {0};
            if (sudoku[r][c] > 0 && sudoku[r][c] <= 9)
                sprintf(buf, "%d", sudoku[r][c]);
            SetWindowTextA(hGrid[r][c], buf);
        }
    }
}

void ReadUIToGrid() {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            char buf[4] = {0};
            GetWindowTextA(hGrid[r][c], buf, 4);
            int v = atoi(buf);
            sudoku[r][c] = (v >= 1 && v <= 9) ? v : 0;
        }
    }
}

void ClearGrid() {
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            sudoku[r][c] = 0;
    LoadGridToUI();
}

void ShowMessage(HWND hwnd, const char* msg, const char* title) {
    MessageBoxA(hwnd, msg, title, MB_OK | MB_ICONINFORMATION);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Title
        CreateWindowA("STATIC", "Sudoku Solver", WS_CHILD | WS_VISIBLE | SS_CENTER,
            10, 10, 340, 30, hwnd, NULL, hInst, NULL);
        // Grid
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                hGrid[r][c] = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
                    10 + c * 35, 50 + r * 35, 32, 32, hwnd, (HMENU)(IDC_BASE + r * 9 + c), hInst, NULL);
            }
        }
        // Buttons
        CreateWindowA("BUTTON", "Solve", WS_CHILD | WS_VISIBLE,
            10, 380, 80, 30, hwnd, (HMENU)IDC_SOLVE, hInst, NULL);
        CreateWindowA("BUTTON", "Clear", WS_CHILD | WS_VISIBLE,
            100, 380, 80, 30, hwnd, (HMENU)IDC_CLEAR, hInst, NULL);
        CreateWindowA("BUTTON", "Exit", WS_CHILD | WS_VISIBLE,
            190, 380, 80, 30, hwnd, (HMENU)IDC_EXIT, hInst, NULL);
        // Initial grid
        ClearGrid();
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == IDC_SOLVE) {
            ReadUIToGrid();
            int temp[9][9];
            memcpy(temp, sudoku, sizeof(sudoku));
            if (SudokuSolver::Solve(temp)) {
                memcpy(sudoku, temp, sizeof(sudoku));
                LoadGridToUI();
                ShowMessage(hwnd, "Solved!", "Sudoku Solver");
            } else {
                ShowMessage(hwnd, "No solution exists for this puzzle.", "Sudoku Solver");
            }
        } else if (id == IDC_CLEAR) {
            ClearGrid();
        } else if (id == IDC_EXIT) {
            PostQuitMessage(0);
        }
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    hInst = hInstance;
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SudokuSolverWnd";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(wc.lpszClassName, "Sudoku Solver - YOHO", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 460, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
} 