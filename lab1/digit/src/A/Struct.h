#pragma once
#include <set>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
#include <memory>
using namespace std;

typedef struct oper
{
    int number;
    int dir; // 0:up 1:down 2:left 3:right
}OPER;

typedef struct node
{
    OPER father_operate;
    node* father;
    node* itself;
    vector<int> status;
    int zero_pos[2];
    int seven_pos; //only record pos of the first seven 
    int g;
    double h;

    friend bool operator < (node a, node b)
    {
        return (a.g + a.h) > (b.g + b.h);
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

using Mymap = std::unordered_map<std::vector<int>,int, VectorHash>;

priority_queue<NODE> q;
//map <vector<int>,int> status_is_explored;
Mymap status_is_explored;
vector<int> des_status;
vector<int> mask;//1 for mask, i.e. can't move
vector<int> non_mask_des; // 1 for cac h, 0 for not cac h
const int n = 5;
double w_h = 1, w_7 = 1;
int des_status_hash[n * n];
char* outputfile;

void readfile(char* argv[], vector<int>& ori_status);
double cac_h(const vector<int> cur_status);
NODE* expand_status(NODE father_node);
int get_direction(int i, int j, int* direction, int zero_pos, int seven_pos,  vector<int> father_status);
void writefile(NODE result);