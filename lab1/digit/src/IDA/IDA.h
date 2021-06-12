#pragma once
#include <set>
#include <map>
#include <queue>
#include <math.h>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include < unordered_map >
#include <memory>

using namespace std;
#define MAX_DEEPTH 1000

typedef struct oper
{
    int number;
    int dir; // 0:up 1:down 2:left 3:right
}OPER;

typedef struct node
{
    vector<int> status;
    OPER op;
    int zero_pos[2];
    int seven_pos; //only record pos of the first seven 
    double h;

    friend bool operator < (node a, node b)
    {
        return  a.h >  b.h;
    }

}NODE;

struct VectorHash {
    size_t operator()(const std::vector<int>& v) const {
        std::hash<int> hasher;
        size_t seed = 0;
        for (int i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

using Mymap = std::unordered_map<std::vector<int>, int, VectorHash>;
Mymap status_is_ex;
vector<int> des_status;
stack<OPER> path;

const int n = 5;
double w_h=1, w_7=1;
char* outputfile;
int des_status_hash[n * n];

void readfile(char* argv[], vector<int>& ori_status);
double cac_h(const vector<int> cur_status);
int get_direction(int i, int j, int* direction, int zero_pos, int seven_pos, vector<int> father_status,int dir);
OPER dfs(double limit, double& next_limit, int depth, NODE state, int dir,Mymap &is_ex);
void writefile(void);
vector<int> mask;//1 for mask, i.e. can't move
vector<int> non_mask_des; // 1 for cac h, 0 for not cac h



void readfile(char* argv[], vector<int>& ori_status)
{
    ifstream input(argv[1]);
    ifstream input_des(argv[2]);
    w_7 = atof(argv[3]);
    w_h = atof(argv[4]);
    outputfile = argv[5];

    int a;
    char b;

    int k = 22;
    //  7:22   7:23
    //         7:24

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            input >> a;
            if (a == 7)
                a = k++;
            ori_status.push_back(a);
            if (j != n - 1)
                input >> b;
        }
    }

    k = 22;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            input_des >> a;
            if (a == 7)
                a = k++;
            des_status.push_back(a);
            des_status_hash[a] = i * n + j;

            if (j != n - 1)
                input_des >> b;
        }
    }
}


double cac_h(const vector<int> cur_status)
{
    double sum = 0;
    int i_d, j_d;
    int cur_num;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cur_num = cur_status[i * n + j];
            i_d = des_status_hash[cur_num] / n;
            j_d = des_status_hash[cur_num] % n;

            if (cur_num && (cur_num!=22) && (cur_num != 23) && (cur_num != 24))
            {
                sum += abs(i_d - i);
                sum += abs(j_d - j);
            }
            if (cur_num == 22)
            {
                sum += w_7*abs(i_d - i);
                sum += w_7*abs(j_d - j);
            }

        }
    }
    return w_h*sum;
}


bool judge_finish(vector<int> now, int pos_7)
{
    bool finish = true;
    for (int i = 0; i < n * n && finish; i++)
    {
        if (now[i] != des_status[i])
            finish = false;
    }
    return finish;

}

inline int get_direction(int i, int j, int* direction, int zero_pos, int seven_pos, vector<int> father_status,int pre_dir)
{
    int x_7, y_7;
    x_7 = seven_pos / n;
    y_7 = seven_pos % n;
    int x_z, y_z;
    x_z = zero_pos / n;
    y_z = zero_pos % n;

    if (i == 0)
    {
        if (j == 0)
        {
            direction[1] = 1;
            direction[3] = 3;
        }
        else if (j == n - 1)
        {
            direction[1] = 1;
            direction[2] = 2;
        }
        else
        {
            direction[1] = 1;
            direction[2] = 2;
            direction[3] = 3;
        }
    }
    else if (i == n - 1)
    {
        if (j == 0)
        {
            direction[0] = 0;
            direction[3] = 3;
        }
        else if (j == n - 1)
        {
            direction[0] = 0;
            direction[2] = 2;
        }
        else
        {
            direction[0] = 0;
            direction[2] = 2;
            direction[3] = 3;
        }
    }
    else
    {
        if (j == 0)
        {
            direction[0] = 0;
            direction[1] = 1;
            direction[3] = 3;
        }
        else if (j == n - 1)
        {
            direction[0] = 0;
            direction[1] = 1;
            direction[2] = 2;
        }
        else
        {
            direction[0] = 0;
            direction[1] = 1;
            direction[2] = 2;
            direction[3] = 3;
        }
    }

    switch (pre_dir)
    {
    case 0:
        direction[1] = -1;
        break;
    case 1:
        direction[0] = -1;
        break;
    case 2:
        direction[3] = -1;
        break;
    case 3:
        direction[2] = -1;
        break;
    default:
        break;
    }


    
    bool is_mask_7 =false;
    int dir = -1;

    if (direction[0] != (-1))
    {
        // 7 7
        // 0 7
        if ((i - 1 == x_7) && (j == y_7))
        {
            if (x_z == (x_7 + 2) && (y_z == y_7 + 1) && (!is_mask_7))
                dir = 0;
            else
                direction[0] = -1;
        }
        //7 7
        //  7
        //  0
        else if ((i == (x_7 + 2)) && (j == y_7 + 1))
        {
            if ((x_z - 1 == x_7) && (y_z == y_7) && (!is_mask_7))
                dir = 0;
            else
                direction[0] = -1;
        }
    }

    if (direction[1] != (-1))
    {
        // 0 
        // 7 7
        //   7
        if ((i + 1 == x_7) && (j == y_7))
        {
            if ((x_z + 1 == x_7) && (y_z == y_7 + 1) && (!is_mask_7))
                  dir = 1;
            else
                direction[1] = -1;
        }
        //   0
        // 7 7
        //   7
        else if ((i + 1 == x_7) && (j == y_7 + 1))
        {
            if ((x_z + 1 == x_7) && (y_z == y_7) && (!is_mask_7))
                dir = 1;
            else
               direction[1] = -1;
        }
    }

    if (direction[2] != (-1))
    {
        // 7 7 0
        //   7
        if ((i == x_7) && (j == y_7 + 2))
        {
            if ((x_z == x_7 + 1) && (y_z == y_7 + 2) && (!is_mask_7))
                dir = 2;
            else
                direction[2] = -1;
        }
        // 7 7
        //   7 0
        else if ((i == x_7 + 1) && (j == y_7 + 2))
        {
            if ((x_z == x_7) && (y_z == y_7 + 2) && (!is_mask_7))
                dir = 2;
            else
                direction[2] = -1;
        }
    }

    if (direction[3] != (-1))
    {
        //0 7 7
        //    7
        if ((i == x_7) && (j == y_7 - 1))
        {
            if ((x_z == x_7 + 1) && (y_z == y_7) && (!is_mask_7))
                dir = 3;
            else
                direction[3] = -1;
        }
        // 7 7
        // 0 7
        else if ((i == x_7 + 1) && (j == y_7))
        {
            if ((x_z == x_7) && (y_z == y_7 - 1) && (!is_mask_7))
                dir = 3;
            else
                 direction[3] = -1;
        }
    }
    
    if ((x_z == i - 1) && (y_z == j))
        direction[0] = -1;
    if ((x_z == i + 1) && (y_z == j))
        direction[1] = -1;
    if ((x_z == i) && (y_z == j - 1))
        direction[2] = -1;
    if ((x_z == i) && (y_z == j + 1))
        direction[3] = -1;
    
    if ((direction[0] != -1) && mask[father_status[(i - 1) * n + j]])
        direction[0] = -1;
    if ((direction[1] != -1) && mask[father_status[(i + 1) * n + j]])
        direction[1] = -1;
    if ((direction[2] != -1) && mask[father_status[(i)*n + j - 1]])
        direction[2] = -1;
    if ((direction[3] != -1) && mask[father_status[(i)*n + j + 1]])
        direction[3] = -1;

    return dir;
}
