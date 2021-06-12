#include <iostream>
#include <fstream>
#include <math.h>
#include <stack>
#include <time.h>
#include <windows.h>
#include "Struct.h"
using namespace std;

int main(int argc, char* argv[])
{
    mask.resize(n * n);
    non_mask_des.resize(n * n);
    if (argc != 6)
    {
        cout << "Help:" << endl;
        cout << "First argument: origin status inputfile" << endl;
        cout << "Second argument: destinate status inputfile" << endl;
        cout << "Third argument: w to mutiply on 7 block (default 1)" << endl;
        cout << "Fourth argument: w to mutiply on h (default 1)" << endl;
        cout << "Fifth argument: outputfilie name (default result)" << endl;
        cout << "Quit!" << endl;
        exit(1);
    }


    vector<int> ori_status;

    double begin = GetTickCount();


    readfile(argv, ori_status);

    for (int i = 0; i < n * n; i++)
    {
        non_mask_des[i] = 1;
        mask[i] = 0;
    }

    NODE* root=new NODE;
    root->status = ori_status;
    root->g = 0;
    root->h = cac_h(root->status);
    root->father = nullptr;
    root->father_operate = { -1,-1 };
    root->itself = root;
    bool is_first_zero = true;
    for (int i = 0; i < n * n; i++)
    {
        if (ori_status[i] == 0)
        {
            if (is_first_zero)
            {
                root->zero_pos[0] = i;
                is_first_zero = false;
            }
            else
                root->zero_pos[1] = i;

        }
        else if (ori_status[i] == 22)
            root->seven_pos = i;
    }


    q.push(*root);
    status_is_explored.insert(pair<vector<int>,int>(root->status,root->h));
    NODE* result = nullptr;
    while (!q.empty())
    {
        NODE t = q.top();
        q.pop();
        result= expand_status(t);
        if (result != nullptr)
            break;
    }

    double end = GetTickCount();
    cout << "Time:" << (end - begin) << "ms" << endl;
    if (result != nullptr)
    {
        writefile(*result);
        cout << "Finish" << endl;
    }
        
    else
        cout << "No solution!";
    
    return 0;
}


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
    vector<int> zero_pos;
    int x_7, y_7;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cur_num = cur_status[i * n + j];
            i_d = des_status_hash[cur_num] / n;
            j_d = des_status_hash[cur_num] % n;

            if ((cur_num != 0) && (cur_num != 22) && (cur_num != 23)&& (cur_num != 24))
            {
                   sum += abs(i_d - i);
                   sum += abs(j_d - j);
            }
            if ((cur_num == 22))
            {
                sum += w_7 * abs(i_d - i);
                sum += w_7 * abs(j_d - j);
            }
        }
    }
 
    return  w_h*sum;
}

bool judge_finish(vector<int> now, int pos_7)
{
    bool finish = true;
    for (int i = 0; i < n * n && finish; i++)
    {
        if ((non_mask_des[now[i]] == 1) && (now[i] != des_status[i]))
            finish = false;
    }
    return finish;
}


//if find des, return the child node, else return nullptr
NODE* expand_status(NODE father_node)
{
    for (int k = 0; k < 2; k++)
    {
        int zero_i, zero_j;
        int direction_zero[4] = { -1,-1,-1,-1 };

        zero_i = father_node.zero_pos[k] / n;
        zero_j = father_node.zero_pos[k] % n;
        int can_move_7 = get_direction(zero_i, zero_j, direction_zero, father_node.zero_pos[(k + 1) % 2], father_node.seven_pos,father_node.status);
        int seven_i, seven_j;
        seven_i = father_node.seven_pos / n;
        seven_j = father_node.seven_pos % n;

        for (int m = 0; m < 4; m++)
        {
            vector<int> cur_status = father_node.status;
            NODE* next_node = new(NODE);
            next_node->itself = next_node;
            next_node->father = father_node.itself;
            next_node->g = father_node.g + 1;
            switch (direction_zero[m])
            {
            case 0:
                if (can_move_7 == direction_zero[m])
                {
                    cur_status[(seven_i + 1) * n + seven_j] = 22;
                    cur_status[(seven_i + 1) * n + seven_j + 1] = 23;
                    cur_status[(seven_i + 2) * n + seven_j + 1] = 24;
                    cur_status[(seven_i)*n + seven_j] = 0;
                    cur_status[(seven_i)*n + seven_j + 1] = 0;
                    next_node->h = cac_h(cur_status);
                    next_node->seven_pos = (seven_i + 1) * n + seven_j;
                    next_node->zero_pos[0] = (seven_i)*n + seven_j;
                    next_node->zero_pos[1] = (seven_i)*n + seven_j + 1;
                    next_node->father_operate = { 7,1 };
                    next_node->status = cur_status;
                    auto find_r = status_is_explored.find(cur_status);
                    if ((find_r == status_is_explored.end()))
                    {
                        status_is_explored.insert(pair<vector<int>, int>(cur_status, next_node->h + next_node->g));
                        if (judge_finish(cur_status, next_node->seven_pos))
                            return next_node;
                        else 
                            q.push(*next_node);
                    }
                    else
                    {
                        int f = next_node->h + next_node->g;
                        if (find_r->second > f)
                        {
                            find_r->second = f;
                            q.push(*next_node);
                        }
                        else
                            delete next_node;
                    }
                }
                else
                {
                    cur_status[zero_i * n + zero_j] = father_node.status[(zero_i - 1) * n + zero_j];
                    cur_status[(zero_i - 1)* n + zero_j] = 0;
                    next_node->h = cac_h(cur_status);
                    next_node->seven_pos = father_node.seven_pos;
                    next_node->zero_pos[k] = (zero_i - 1) * n + zero_j;
                    next_node->zero_pos[(k + 1) % 2] = father_node.zero_pos[(k + 1) % 2];
                    next_node->father_operate = { cur_status[zero_i * n + zero_j], 1 };
                    next_node->status = cur_status;
                    auto find_r = status_is_explored.find(cur_status);
                    if ((find_r == status_is_explored.end()))
                    {
                        status_is_explored.insert(pair<vector<int>, int>(cur_status, next_node->h + next_node->g));
                        
                        if (judge_finish(cur_status, next_node->seven_pos))
                            return next_node;
                        else
                            q.push(*next_node);
                    }
                    else
                    {
                        int f = next_node->h + next_node->g;
                        if (find_r->second > f)
                        {
                            find_r->second = f;
                            q.push(*next_node);
                        }
                        else
                            delete next_node;
                    }
                }
                break;
            case 1:
                if (can_move_7 == direction_zero[m])
                {
                    cur_status[(seven_i - 1) * n + seven_j] = 22;
                    cur_status[(seven_i - 1) * n + seven_j + 1] = 23;
                    cur_status[(seven_i) * n + seven_j + 1] = 24;
                    cur_status[(seven_i)*n + seven_j] = 0;
                    cur_status[(seven_i + 1)*n + seven_j + 1] = 0;
                    next_node->h = cac_h(cur_status);
                    next_node->seven_pos = (seven_i - 1) * n + seven_j;
                    next_node->zero_pos[0] = (seven_i)*n + seven_j;
                    next_node->zero_pos[1] = (seven_i + 1) * n + seven_j + 1;
                    next_node->father_operate = { 7,0 };
                    next_node->status = cur_status;
                    auto find_r = status_is_explored.find(cur_status);
                    if ((find_r == status_is_explored.end()))
                    {
                        status_is_explored.insert(pair<vector<int>, int>(cur_status, next_node->h + next_node->g));
                        if (judge_finish(cur_status, next_node->seven_pos))
                            return next_node;
                        else
                            q.push(*next_node);
                    }
                    else
                        delete next_node;
                }
                else
                {
                    cur_status[zero_i * n + zero_j] = father_node.status[(zero_i + 1) * n + zero_j];
                    cur_status[(zero_i + 1) * n + zero_j] = 0;
                    next_node->h = cac_h(cur_status);
                    auto find_r = status_is_explored.find(cur_status);
                    next_node->seven_pos = father_node.seven_pos;
                    next_node->zero_pos[k] = (zero_i + 1) * n + zero_j;
                    next_node->zero_pos[(k + 1) % 2] = father_node.zero_pos[(k + 1) % 2];
                    next_node->father_operate = { cur_status[zero_i * n + zero_j], 0 };
                    next_node->status = cur_status;
                    if ((find_r == status_is_explored.end()))
                    {
                        status_is_explored.insert(pair<vector<int>, int>(cur_status, next_node->h + next_node->g));
                        if (judge_finish(cur_status, next_node->seven_pos))
                            return next_node;
                        else
                            q.push(*next_node);
                    }
                    else
                    {
                        int f = next_node->h + next_node->g;
                        if (find_r->second > f)
                        {
                            find_r->second = f;
                            q.push(*next_node);
                        }
                        else
                            delete next_node;
                    }
                }
                break;
            case 2:
                if (can_move_7 == direction_zero[m])
                {
                    cur_status[(seven_i) * n + seven_j + 1] = 22;
                    cur_status[(seven_i) * n + seven_j + 2] = 23;
                    cur_status[(seven_i + 1)*n + seven_j + 2] = 24;
                    cur_status[(seven_i)*n + seven_j] = 0;
                    cur_status[(seven_i + 1) * n + seven_j + 1] = 0;
                    next_node->h = cac_h(cur_status);
                    next_node->seven_pos = (seven_i)*n + seven_j + 1;
                    next_node->zero_pos[0] = (seven_i)*n + seven_j;
                    next_node->zero_pos[1] = (seven_i + 1) * n + seven_j + 1;
                    next_node->father_operate = { 7,3 };
                    next_node->status = cur_status;
                    auto find_r = status_is_explored.find(cur_status);
                    if ((find_r == status_is_explored.end()))
                    {
                        status_is_explored.insert(pair<vector<int>, int>(cur_status, next_node->h + next_node->g));
                        if (judge_finish(cur_status, next_node->seven_pos))
                            return next_node;
                        else
                            q.push(*next_node);
                    }
                    else
                    {
                        int f = next_node->h + next_node->g;
                        if (find_r->second > f)
                        {
                            find_r->second = f;
                            q.push(*next_node);
                        }
                        else
                            delete next_node;
                    }
                }
                else
                {
                    cur_status[zero_i * n + zero_j] = father_node.status[(zero_i) * n + zero_j - 1];
                    cur_status[(zero_i) * n + zero_j - 1] = 0;
                    next_node->h = cac_h(cur_status);
                    next_node->seven_pos = father_node.seven_pos;
                    next_node->zero_pos[k] = (zero_i)*n + zero_j - 1;
                    next_node->zero_pos[(k + 1) % 2] = father_node.zero_pos[(k + 1) % 2];
                    next_node->father_operate = { cur_status[zero_i * n + zero_j], 3 };
                    next_node->status = cur_status;
                    auto find_r = status_is_explored.find(cur_status);
                    if (find_r == status_is_explored.end())
                    {
                        status_is_explored.insert(pair<vector<int>, int>(cur_status, next_node->h + next_node->g));
                        if (judge_finish(cur_status, next_node->seven_pos))
                            return next_node;
                        else
                            q.push(*next_node);
                    }
                    else
                    {
                        int f = next_node->h + next_node->g;
                        if (find_r->second > f)
                        {
                            find_r->second = f;
                            q.push(*next_node);
                        }
                        else
                            delete next_node;
                    }
                }
                break;
            case 3:
                if (can_move_7 == direction_zero[m])
                {
                    cur_status[(seven_i)*n + seven_j - 1] = 22;
                    cur_status[(seven_i)*n + seven_j] = 23;
                    cur_status[(seven_i + 1) * n + seven_j] = 24;
                    cur_status[(seven_i)*n + seven_j + 1] = 0;
                    cur_status[(seven_i + 1) * n + seven_j + 1] = 0;
                    next_node->h = cac_h(cur_status);
                    next_node->seven_pos = (seven_i)*n + seven_j - 1;
                    next_node->zero_pos[0] = (seven_i)*n + seven_j + 1;
                    next_node->zero_pos[1] = (seven_i + 1) * n + seven_j + 1;
                    next_node->father_operate = { 7,2 };
                    next_node->status = cur_status;
                    auto find_r = status_is_explored.find(cur_status);
                    if ((find_r == status_is_explored.end()))
                    {
                        status_is_explored.insert(pair<vector<int>, int>(cur_status, next_node->h + next_node->g));
                        if (judge_finish(cur_status, next_node->seven_pos))
                            return next_node;
                        else
                            q.push(*next_node);
                    }
                    else
                    {
                        int f = next_node->h + next_node->g;
                        if (find_r->second > f)
                        {
                            find_r->second = f;
                            q.push(*next_node);
                        }
                        else
                            delete next_node;
                    }
                }
                else
                {
                    cur_status[zero_i * n + zero_j] = father_node.status[(zero_i)*n + zero_j + 1];
                    cur_status[(zero_i)*n + zero_j + 1] = 0;
                    next_node->h = cac_h(cur_status);
                    next_node->seven_pos = father_node.seven_pos;
                    next_node->zero_pos[k] = (zero_i)*n + zero_j + 1;
                    next_node->zero_pos[(k + 1) % 2] = father_node.zero_pos[(k + 1) % 2];
                    next_node->father_operate = { cur_status[zero_i * n + zero_j], 2 };
                    next_node->status = cur_status;
                    auto find_r = status_is_explored.find(cur_status);
                    if ((find_r == status_is_explored.end()))
                    {
                        status_is_explored.insert(pair<vector<int>, int>(cur_status, next_node->h + next_node->g));
                        if (judge_finish(cur_status, next_node->seven_pos))
                            return next_node;
                        else
                            q.push(*next_node);
                    }
                    else
                    {
                        int f = next_node->h + next_node->g;
                        if (find_r->second > f)
                        {
                            find_r->second = f;
                            q.push(*next_node);
                        }
                        else
                            delete next_node;
                    }
                }
                break;
            default:
                delete next_node;
                break;
            }
        }
    }
    
    return nullptr;
}

//if can move 7, then return the dir, else return -1
int get_direction(int i, int j, int* direction, int zero_pos, int seven_pos, vector<int> father_status)
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
            direction[0] = 1;
            direction[1] = 3;
        }
        else if (j == n - 1)
        {
            direction[0] = 1;
            direction[1] = 2;
        }
        else
        {
            direction[0] = 1;
            direction[1] = 2;
            direction[2] = 3;
        }
    }
    else if (i == n - 1)
    {
        if (j == 0)
        {
            direction[0] = 0;
            direction[1] = 3;
        }
        else if (j == n - 1)
        {
            direction[0] = 0;
            direction[1] = 2;
        }
        else
        {
            direction[0] = 0;
            direction[1] = 2;
            direction[2] = 3;
        }
    }
    else
    {
        if (j == 0)
        {
            direction[0] = 0;
            direction[1] = 1;
            direction[2] = 3;
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

    bool is_mask_7 = (mask[22]);
    int dir = -1;
    for (int k = 0; k < 4; k++)
    {
        if (direction[k] != -1)
        {
            switch (direction[k])
            {
            case 0:
                // 7 7
                // 0 7
                if ((i - 1 == x_7) && (j == y_7))
                {
                    if (x_z == (x_7 + 2) && (y_z == y_7 + 1) && (!is_mask_7))
                    {
                        dir = 0;
                        break;
                    }     
                    direction[k] = -1;
                }
                //7 7
                //  7
                //  0
                else if ((i == (x_7 + 2) )&& (j == y_7 + 1))
                {
                    if ((x_z - 1 == x_7) && (y_z == y_7) && (!is_mask_7))
                    {
                        dir = 0;
                        break;
                    }
                    direction[k] = -1;
                }
                break;
            case 1:
                // 0 
                // 7 7
                //   7
                if ((i + 1 == x_7) && (j == y_7))
                {
                    if ((x_z + 1 == x_7) && (y_z == y_7 + 1) && (!is_mask_7))
                    {
                        dir = 1;
                        break;
                    }
                    direction[k] = -1;
                }
                //   0
                // 7 7
                //   7
                else if ((i + 1 == x_7) && (j == y_7 + 1))
                {
                    if ((x_z + 1 == x_7) && (y_z == y_7) && (!is_mask_7))
                    {
                        dir = 1;
                        break;
                    }
                    direction[k] = -1;
                }
                break;
            case 2:
                // 7 7 0
                //   7
                if ((i == x_7) && (j == y_7 + 2))
                {
                    if ((x_z == x_7 + 1) && (y_z == y_7 + 2) && (!is_mask_7))
                    {
                        dir = 2;
                        break;
                    }
                    direction[k] = -1;
                }
                // 7 7
                //   7 0
                else if ((i == x_7 + 1) && (j == y_7 + 2))
                {
                    if ((x_z == x_7) && (y_z == y_7 + 2) && (!is_mask_7))
                    {
                        dir = 2;
                        break;
                    }
                    direction[k] = -1;
                }
                break;
            case 3:
                //0 7 7
                //    7
                if ((i == x_7) && (j == y_7 - 1))
                {
                    if ((x_z == x_7 + 1) && (y_z == y_7) && (!is_mask_7))
                    {
                        dir = 3;
                        break;
                    }
                    direction[k] = -1;
                }
                // 7 7
                // 0 7
                else if ((i == x_7 + 1) && (j == y_7))
                {
                    if ((x_z == x_7) && (y_z == y_7 - 1) && (!is_mask_7))
                    {
                        dir = 3;
                        break;
                    }
                    direction[k] = -1;
                }
                break;
            default:
                break;
            }
        }
    }

    for (int k = 0; k < 4; k++)
    {
        switch (direction[k])
        {
        case 0:
            if ((x_z == i - 1) && (y_z == j))
                direction[k] = -1;
            break;
        case 1:
            if ((x_z == i + 1) && (y_z == j))
                direction[k] = -1;
            break;
        case 2:
            if ((x_z == i) && (y_z == j - 1))
                direction[k] = -1;
            break;
        case 3:
            if ((x_z == i) && (y_z == j + 1))
                direction[k] = -1;
            break;
        default:
            break;
        }
    }

    for (int k = 0; k < 4; k++)
    {
        switch (direction[k])
        {
        case 0:
            if (mask[father_status[(i - 1) * n + j]])
                direction[k] = -1;
            break;
        case 1:
            if (mask[father_status[(i + 1) * n + j]])
                direction[k] = -1;
            break;
        case 2:
            if (mask[father_status[(i) * n + j - 1]])
                direction[k] = -1;
            break;
        case 3:
            if (mask[father_status[(i) * n + j + 1]])
                direction[k] = -1;
            break;
        default:
            break;
        }
    }
   

    return dir;
}

void writefile(NODE result)
{
    NODE* p = &result;
    stack<OPER> out;

    while (p != nullptr)
    {
        out.push(p->father_operate);
        p = p->father;
    }

    ofstream output(outputfile);

    int step = 0;
    char d[4] = {'u','d','l','r'};
    out.pop();
    while (out.size())
    {
        OPER op = out.top();
        out.pop();
        step++;
        output << "("<<op.number<<"," << d[op.dir] << ")";
        if (out.size())
            output << ";";
        else
            output << ".";
    }
    cout << "steps:" << step << endl;
}
