//
// Created by 闵梦涛 on 2021/7/19.
//

#ifndef CRASHSIM_GRAPH_H
#define CRASHSIM_GRAPH_H
#include "iostream"
#include "cstdlib"
#include "cstdio"
#include "fstream"
#include "time.h"
#include "vector"

using namespace std;

class Graph {
public:
    int n;//图的顶点数
    int m;//图的边数
    int* indegree;//每个顶点入度数
    int* outdegree;//每个顶点出度数
    int** inAdjList;//每个顶点入度邻居集合
    int** outAdjList;//每个顶点出度邻居集合

    int maxN = 100000000;//由于可能出现顶点不连续的情况，直接初始化1kw
    int n_t = 0;//实际最大顶点序号


    Graph(){
        m = n = 0;
    }
    Graph(string file) {
        m=0;
        ifstream infile(file);
        infile >> n;
        cout << "# nodes= " << n << endl;

        //初始化顶点的度
        indegree = new int[maxN];
        outdegree = new int[maxN];
        for(int i = 0; i < maxN; i++) {
            indegree[i] = 0;
            outdegree[i] = 0;
        }
        //读图并且得到每个顶点的度
        int from, to;
        while(infile.good()) {
            infile >> from >> to;
            outdegree[from]++;
            indegree[to]++;
            n_t = max(n_t, max(from, to));
        }
        cout << "..." << endl;
        inAdjList = new int*[n_t + 1];
        outAdjList = new int*[n_t + 1];
        for(int i = 0; i <= n_t; i++) {
            inAdjList[i] = new int[indegree[i]];
            outAdjList[i] = new int[outdegree[i]];
        }
        int* pointer_in = new int[n_t + 1];
        int* pointer_out = new int[n_t + 1];
        for(int i = 0; i <= n_t; i++) {
            pointer_in[i] = 0;
            pointer_out[i] = 0;
        }
        infile.clear();
        infile.seekg(0);

        clock_t t1=clock();
        infile >> n;
        cout << "# nodes= " << n << endl;
        while(infile.good()) {
            infile >> from >> to;
            outAdjList[from][pointer_out[from]] = to;
            pointer_out[from]++;
            inAdjList[to][pointer_in[to]] = from;
            pointer_in[to]++;

            m++;
        }
        infile.close();
        clock_t t2=clock();
        cout << "# edges= " << m << endl;
        cout << "reading in graph takes " << (t2-t1)/(double)CLOCKS_PER_SEC << " s" << endl;

        delete[] pointer_in;
        delete[] pointer_out;
    }
    ~Graph() {
        /*if(indegree != nullptr)
            delete[] indegree;
        if(outdegree != nullptr)
            delete[] outdegree;
        for(int i=0; i<n; i++) {
            if(inAdjList[i] != nullptr)
                delete[] inAdjList[i];
            if(outAdjList[i] != nullptr)
                delete[] outAdjList[i];
        }
        if(inAdjList != nullptr)
            delete[] inAdjList;
        if(outAdjList != nullptr)
            delete[] outAdjList;*/
    }

    void outputGraph() {
        string prefix = "/Users/minmengtao/CLionProjects/CrashSim";
        string result = "/result/";
        string suffix = ".txt";
        string file = prefix + result + "1tx" + suffix;
        ofstream f(file);
        if(!f)
            return;

        f << "图G顶点数：" << n << " 图G边数：" << m << endl;
        for(int i = 0; i <= n_t; i++) {
            if(indegree[i] == 0 && outdegree[i] == 0)//无效顶点
                continue;

            f << "顶点" << i << "的出度集合为" << endl;
            for(int j = 0; j < outdegree[i]; j++) {
                f << outAdjList[i][j] << " ";
            }
            f << endl;
            f << "顶点" << i << "的入度集合为" << endl;
            for(int j = 0; j < indegree[i]; j++) {
                f << inAdjList[i][j] << " ";
            }
            f << endl;
        }
        f.close();
    }

};
#endif //CRASHSIM_GRAPH_H
