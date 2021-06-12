#pragma once
#include "sodoku.h"
bool dfs_easy(int pos)
{
    if (pos == 81)
        return true;
    sum_node += 1;

    int x = pos / 9;
    int y = pos % 9;

    if (sudoku[x][y] != 0)
        return dfs_easy(pos + 1);

    for (int i = 1; i <= 9; i++)
    {
        bool can_set = easy_judge(x, y, i);
        if (can_set)
        {
            sudoku[x][y] = i;
            bool result = dfs_easy(pos + 1);
            if (result)
                return true;
            sudoku[x][y] = 0;
            easy_resume(x, y, i);
        }

    }
    return false;
}

bool easy_judge(int x, int y, int num)
{
    if (rows[x][num] == 1)
        return false;

    if (cows[y][num] == 1)
        return false;

    if (x == y)
        if (diag[0][num] == 1)
            return false;

    if ((x + y) == 8)
        if (diag[1][num] == 1)
            return false;

    if (grid[3 * (x / 3) + (y / 3)][num] == 1)
        return false;

    rows[x][num] = 1;
    cows[y][num] = 1;
    grid[3 * (x / 3) + (y / 3)][num] = 1;
    if (x == y)
        diag[0][num] = 1;
    if ((x + y) == 8)
        diag[1][num] = 1;

    return true;
}

void easy_resume(int x, int y, int num)
{
    rows[x][num] = 0;
    cows[y][num] = 0;
    grid[3 * (x / 3) + (y / 3)][num] = 0;
    if (x == y)
        diag[0][num] = 0;
    if ((x + y) == 8)
        diag[1][num] = 0;
}