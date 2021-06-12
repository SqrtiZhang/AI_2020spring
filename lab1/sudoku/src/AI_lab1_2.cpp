#include <iostream>
#include <fstream>
#include <math.h>
#include <stack>
#include <time.h>
#include <windows.h>
#include "sodoku.h"
#include "easy.h"
#include "improve.h"
using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        cout << "Help:" << endl;
        cout << "Argument 1: origin status inputfile" << endl;
        cout << "Argument 2: outputfile" << endl;
        cout << "Argument 3: whether to use heuristic: 1 for use and other for not use" << endl;
        cout << "Quit!" << endl;
        exit(1);
    }
    read(argv[1]);
    outputfile = argv[2];
    use_h = atof(argv[3]);

    double begin = GetTickCount();
    if(use_h == 1)
        dfs_imp();
    else
        dfs_easy(0);
    double end = GetTickCount();

    write(argv[0]);

    cout << "Time:" << (end - begin) << "ms" << endl;
    cout << "Number of nodes:" << sum_node << endl;
}

