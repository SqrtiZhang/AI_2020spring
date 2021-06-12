#include <iostream>
#include "IDA.h"
#include <windows.h>
using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 6)
    {
        cout << argv[2] << endl;
        cout << "Help:" << endl;
        cout << "First argument: origin status inputfile" << endl;
        cout << "Second argument: destinate status inputfile" << endl;
        cout << "Third argument: w to mutiply on 7 block (default 1)" << endl;
        cout << "Fourth argument: w to mutiply on h (default 1)" << endl;
        cout << "Fifth argument: outputfilie name (default result)" << endl;
        cout << "Quit!" << endl;
        exit(1);
    }
    mask.resize(n * n);
    non_mask_des.resize(n * n);

    for (int i = 0; i < n * n; i++)
    {
        non_mask_des[i] = 1;
        mask[i] = 0;
    }
    //non_mask_des[6] = 1;
    //mask[22] = mask[23] = mask[24] = 1;

    vector<int> ori_status;
    
    readfile(argv, ori_status);

    NODE root;
    root.status = ori_status;
    root.h = cac_h(root.status);

    bool is_first_zero = true;
    for (int i = 0; i < n * n; i++)
    {
        if (ori_status[i] == 0)
        {
            if (is_first_zero)
            {
                root.zero_pos[0] = i;
                is_first_zero = false;
            }
            else
                root.zero_pos[1] = i;

        }
        else if (ori_status[i] == 22)
            root.seven_pos = i;
    }
    //int limit = 10*root.h;
    double limit = root.h;
    double next_limit;
    double begin = GetTickCount();
    while (limit < MAX_DEEPTH)
    {
        next_limit = MAX_DEEPTH;
        status_is_ex.clear();
        stack<OPER>().swap(path);
        status_is_ex.insert(pair<vector<int>, int>(root.status, root.h + 0));

        OPER result = dfs(limit, next_limit, 0, root, -1, status_is_ex);
        if (result.dir != -1)
        {
            path.push(result);
            break;
        }
        limit = next_limit;
    }

    double end = GetTickCount();
    cout << "Time:" << (end - begin) << "ms" << endl;
    writefile();

    
    return 0;
}

int w = 1;

OPER dfs(double limit, double& next_limit, int depth, NODE state,int dir, Mymap &is_ex)
{
    if ((w*depth + state.h) > limit)
    {
        if (next_limit > (w * depth + state.h))
            next_limit = (w * depth + state.h);
        return { -1,-1 };
    }

    

    priority_queue<NODE> q;
    //stack<node> q;
    for (int k = 0; k < 2; k++)
    {
        int direction_zero[4] = { -1,-1,-1,-1 };
        int zero_i = state.zero_pos[k] / n;
        int zero_j = state.zero_pos[k] % n;
        int can_move_7 = get_direction(zero_i, zero_j, direction_zero, 
            state.zero_pos[(k + 1) % 2], state.seven_pos, state.status,dir);
        int seven_i, seven_j;
        seven_i = state.seven_pos / n;
        seven_j = state.seven_pos % n;

        for (int m = 0; m < 4; m++)
        {
            vector<int> cur_status = state.status;
            NODE next_state;
            switch (direction_zero[m])
            {
            case 0:
                if (can_move_7 == direction_zero[m])
                {
                    cur_status[(seven_i + 1) * n + seven_j] = 22;
                    cur_status[(seven_i + 1) * n + seven_j + 1] = 23;
                    //cur_status[(seven_i + 2) * n + seven_j] = 6;
                    cur_status[(seven_i + 2) * n + seven_j + 1] = 24;
                    cur_status[(seven_i)*n + seven_j] = 0;
                    cur_status[(seven_i)*n + seven_j + 1] = 0;
                    next_state.h = cac_h(cur_status);
                    next_state.seven_pos = (seven_i + 1) * n + seven_j;
                    next_state.zero_pos[0] = (seven_i)*n + seven_j;
                    next_state.zero_pos[1] = (seven_i)*n + seven_j + 1;
                    next_state.status = cur_status;
                    next_state.op = { 7,1 };
                }
                else
                {
                    cur_status[zero_i * n + zero_j] = state.status[(zero_i - 1) * n + zero_j];
                    cur_status[(zero_i - 1) * n + zero_j] = 0;
                    next_state.h = cac_h(cur_status);
                    next_state.seven_pos = state.seven_pos;
                    next_state.zero_pos[k] = (zero_i - 1) * n + zero_j;
                    next_state.zero_pos[(k + 1) % 2] = state.zero_pos[(k + 1) % 2];
                    //next_state.father_operate = { cur_status[zero_i * n + zero_j], 1 };
                    next_state.status = cur_status;
                    next_state.op = { cur_status[zero_i * n + zero_j],1 };
                   
                }
                break;
            case 1:
                if (can_move_7 == direction_zero[m])
                {
                    cur_status[(seven_i - 1) * n + seven_j] = 22;
                    cur_status[(seven_i - 1) * n + seven_j + 1] = 23;
                    //cur_status[(seven_i)*n + seven_j] = 6;
                    cur_status[(seven_i)*n + seven_j + 1] = 24;
                    //cur_status[(seven_i + 1) * n + seven_j] = 0;
                    cur_status[(seven_i) * n + seven_j] = 0;
                    cur_status[(seven_i + 1) * n + seven_j + 1] = 0;
                    next_state.h = cac_h(cur_status);
                    next_state.seven_pos = (seven_i - 1) * n + seven_j;
                    //next_state.zero_pos[0] = (seven_i + 1)*n + seven_j;
                    next_state.zero_pos[0] = (seven_i) * n + seven_j;
                    next_state.zero_pos[1] = (seven_i + 1) * n + seven_j + 1;
                    next_state.status = cur_status;
                    next_state.op = { 7,0 };
                }
                else
                {
                    cur_status[zero_i * n + zero_j] = state.status[(zero_i + 1) * n + zero_j];
                    cur_status[(zero_i + 1) * n + zero_j] = 0;
                    next_state.h = cac_h(cur_status);
                    next_state.seven_pos = state.seven_pos;
                    next_state.zero_pos[k] = (zero_i + 1) * n + zero_j;
                    next_state.zero_pos[(k + 1) % 2] = state.zero_pos[(k + 1) % 2];
                    next_state.status = cur_status;
                    next_state.op= { cur_status[zero_i * n + zero_j], 0 };
                }
                break;
            case 2:
                if (can_move_7 == direction_zero[m])
                {
                    cur_status[(seven_i)*n + seven_j + 1] = 22;
                    cur_status[(seven_i)*n + seven_j + 2] = 23;
                    //cur_status[(seven_i + 1) * n + seven_j + 1] = 6;
                    cur_status[(seven_i + 1) * n + seven_j + 2] = 24;
                    cur_status[(seven_i)*n + seven_j] = 0;
                    //cur_status[(seven_i + 1) * n + seven_j ] = 0;
                    cur_status[(seven_i + 1) * n + seven_j + 1] = 0;
                    next_state.h = cac_h(cur_status);
                    next_state.seven_pos = (seven_i)*n + seven_j + 1;
                    next_state.zero_pos[0] = (seven_i)*n + seven_j;
                    next_state.zero_pos[1] = (seven_i + 1) * n + seven_j + 1;
                    next_state.status = cur_status;
                    next_state.op = { 7,3 };
                }
                else
                {
                    cur_status[zero_i * n + zero_j] = state.status[(zero_i)*n + zero_j - 1];
                    cur_status[(zero_i)*n + zero_j - 1] = 0;
                    next_state.h = cac_h(cur_status);
                    next_state.seven_pos = state.seven_pos;
                    next_state.zero_pos[k] = (zero_i)*n + zero_j - 1;
                    next_state.zero_pos[(k + 1) % 2] = state.zero_pos[(k + 1) % 2];
                    next_state.status = cur_status;
                    next_state.op= { cur_status[zero_i * n + zero_j], 3 };

                }
                break;
            case 3:
                if (can_move_7 == direction_zero[m])
                {
                    cur_status[(seven_i)*n + seven_j - 1] = 22;
                    cur_status[(seven_i)*n + seven_j] = 23;
                    //cur_status[(seven_i + 1) * n + seven_j - 1] = 6;
                    cur_status[(seven_i + 1) * n + seven_j] = 24;
                    cur_status[(seven_i)*n + seven_j + 1] = 0;
                    cur_status[(seven_i + 1) * n + seven_j + 1] = 0;
                    next_state.h = cac_h(cur_status);
                    next_state.seven_pos = (seven_i)*n + seven_j - 1;
                    next_state.zero_pos[0] = (seven_i)*n + seven_j + 1;
                    next_state.zero_pos[1] = (seven_i + 1) * n + seven_j + 1;
                    next_state.status = cur_status;
                    next_state.op = { 7,2 };
                }
                else
                {
                    cur_status[zero_i * n + zero_j] = state.status[(zero_i)*n + zero_j + 1];
                    cur_status[(zero_i)*n + zero_j + 1] = 0;
                    next_state.h = cac_h(cur_status);
                    next_state.seven_pos = state.seven_pos;
                    next_state.zero_pos[k] = (zero_i)*n + zero_j + 1;
                    next_state.zero_pos[(k + 1) % 2] = state.zero_pos[(k + 1) % 2];
                    next_state.status = cur_status;
                    next_state.op = { cur_status[zero_i * n + zero_j], 2 };
                }
                break;
            default:
                break;
            }
            if (direction_zero[m] != -1)
            {
                if (judge_finish(cur_status, next_state.seven_pos))
                    return next_state.op;
                q.push(next_state);
            }
        }
    }

    while (!q.empty())
    {
        NODE next_state = q.top();
        q.pop();

        auto iter = is_ex.find(next_state.status);
        if (iter == is_ex.end())
        {
            is_ex.insert(pair<vector<int>, int>(next_state.status, depth + next_state.h + 1));
            OPER road = dfs(limit, next_limit, depth + 1, next_state, next_state.op.dir, is_ex);
            if (road.dir != -1)
            {
                path.push(road);
                return next_state.op;
            }
        }
        else
        {
            if ((depth + next_state.h + 1) < (iter->second))
            {
                iter->second = (depth + next_state.h + 1);
                OPER road = dfs(limit, next_limit, depth + 1, next_state, next_state.op.dir, is_ex);
                if (road.dir != -1)
                {
                    path.push(road);
                    return next_state.op;
                }
            }
        }

        

        
    }
   

    return { -1,-1 };
    
}

void writefile(void)
{
    int step = 0;
    ofstream output("result.txt");

    char d[4] = { 'u','d','l','r' };

    while (!path.empty())
    {
        OPER op = path.top();
        path.pop();
        step++;
        if (op.number == 7)
        {
            output << "(7," << d[op.dir] << ");";
            output << "(7," << d[op.dir] << ");";
            output << "(7," << d[op.dir] << ")";
        }
        else
            output << "(" << op.number << "," << d[op.dir] << ")";
        if (path.size())
            output << ";";
        else
            output << ".";
    }
    cout << step << endl;
}