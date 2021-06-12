#pragma once
#include <queue>
#include <set>
#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;

int rows[9][10];
int cows[9][10];
int grid[9][10];
int diag[2][10]; // 0 for \, 1 for /

typedef struct zero
{
    int x, y;
    int avaliable_num[10] = {0};
    int avaliable_sum = 0;
    friend bool operator < (zero a, zero b)
    {
        return a.avaliable_sum > b.avaliable_sum;
    }
}ZERO;

vector<ZERO> zero_list;

int sudoku[9][9];
long long sum_node = 0;
char* outputfile;
int use_h = 1;

bool dfs_easy(int pos);
bool easy_judge(int x, int y, int num);
void easy_resume(int x, int y, int num);

void read(char* filename)
{
	ifstream input(filename);
    int a;
    vector<ZERO> zero_num;

    for (int i = 0; i <= 9; i++)
    {
        diag[0][i] = 0;
        diag[1][i] = 0;
        for (int j = 0; j <= 9; j++)
        {
            rows[i][j] = 0;
            cows[i][j] = 0;
            grid[i][j] = 0;
        }
    }
        

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            input >> a;
           
            sudoku[i][j] = a;

            if (a != 0)
            {
                rows[i][a] = 1;
                cows[j][a] = 1;
                if (i == j)
                    diag[0][a] = 1;
                if ((i + j) == 8)
                    diag[1][a] = 1;
                grid[3 * (i / 3) + (j / 3)][a] = 1;
            }
            else
            {
                ZERO z;
                z.x = i;
                z.y = j;
                zero_num.push_back(z);
            }

        }
    }
    
    for (auto z : zero_num)
    {
        int x = z.x, y = z.y;
        for (int num = 1; num <= 9; num++)
        {
            bool flag = true;
            if (rows[x][num] == 1)
                flag = false;

            if (cows[y][num] == 1)
                flag = false;

            if (x == y)
                if (diag[0][num] == 1)
                    flag = false;

            if ((x + y) == 8)
                if (diag[1][num] == 1)
                    flag = false;

            if (grid[3 * (x / 3) + (y / 3)][num] == 1)
                flag = false;

            if (flag)
            {
                z.avaliable_num[num] = 1;
                z.avaliable_sum++;
            }    
        }
        zero_list.push_back(z);
    }

    make_heap(zero_list.begin(), zero_list.end());
   // cout << zero_list.front().avaliable_num << endl;
}

void write(char* filename)
{
    //ofstream output(filename);
    ofstream output(outputfile);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
            output << sudoku[i][j] << " ";
        output << endl;
    }
}