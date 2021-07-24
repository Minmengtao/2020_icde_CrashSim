//
// Created by 闵梦涛 on 2021/7/19.
//

#ifndef CRASHSIM_CRASHSIM_H
#define CRASHSIM_CRASHSIM_H
#include "iostream"
#include "cstdlib"
#include "cstdio"
#include "fstream"
#include "Graph.h"
#include "Random.h"
#include "queue"
#include "unordered_map"

using namespace std;

class CrashSim {
public:
    Graph g;
    double c_val;//衰减因子
    int lmax;//游走的最大步长
    int nr;//需要进行nr次实验得出模拟结果
    Random R;//蒙特卡洛随机函数
    double *sim;//模拟的simrank结果

    //自定义(pair<int, int>, int)的unordered_map
    struct HashFunc {
        std::size_t operator()(const pair<int,int> &key) const {
            using std::size_t;
            using std::hash;
            return ((hash<int>()(key.first)
                     ^ (hash<int>()(key.second) << 1)) >> 1);
        }
    };
    struct EqualKey {
        bool operator () (const pair<int, int> &lhs, const pair<int, int> &rhs) const {
            return lhs.first == rhs.first
                   && lhs.second == rhs.second;
        }
    };

    struct cmp {
        bool operator() (const pair<int, double>& a, const pair<int, double>& b) {
            return a.second < b.second || (a.second == b.second && a.first > b.first);
        }
    };

    CrashSim(string file, double eps, double c, double delta) {
        g = Graph(file);
        //g.outputGraph();
        c_val = c;

        //反向游走的长度可以不用很长，大概为5即可
        lmax = min((int)((1 + sqrt(c)) / (1 - sqrt(c)) / (1 - sqrt(c))), 5);
        cout << "lmax = " << lmax << endl;
        double eps_t = pow(sqrt(c), lmax);
        double p = 1 - eps_t;
        double ex = eps - p * eps_t;

        //nr = (int)(3 * c / ex / ex * log(g.n / delta) / log(2));
        // ProbeSim
        nr = (int)(0.5 / eps / eps * log(g.n) / log(2));//保证要有nr次估计
        cout << "nr = " << nr << endl;

        sim = new double[g.n_t + 1];
    }
    ~CrashSim() {
        delete sim;
    }

    /**
     * 计算图G剩下的顶点与源点u的SimRank值
     * @param u
     */
    void CalculateSimRank(int u) {
        //
        sim[u] = 1;//顶点自身相似度为1

        //矩阵U
        unordered_map<pair<int, int>, double, HashFunc, EqualKey> U = revReach(u, lmax);
        //for(auto it = U.begin(); it != U.end(); it++)
        //    cout << it->first.first << " " << it->first.second << " " << it->second << endl;
        int* nodeList = new int[lmax + 1];//反向游走路径

        for(int v = 0; v <= g.n_t; v++) {
            if(g.indegree[v] == 0 && g.outdegree[v] == 0)//无效顶点
                continue;
            if(v == u)//不考虑单源顶点u
                continue;
            //nr次实验
            for(int k = 1; k <= nr; k++) {
                int l = 0;//反向游走步数
                SimRank_Walk(v, l, nodeList);//从顶点v反向游走l步

                for(int i = 1; i < l; i++)
                    sim[v] += U[make_pair(i, nodeList[i])];
            }
            //取平均值
            sim[v] = sim[v] / nr;
        }
    }

    /**
     * 从顶点v反向游走l步，
     * @param v 出发顶点
     * @param l 反向游走步数
     * @param nodeList 反向游走集合
     */
    void SimRank_Walk(int v, int& l, int* nodeList) {
        int tempNode = v;
        nodeList[0] = v;
        l = 1;

        //有sqrt(c)的概率会进入入度邻居顶点
        while(R.drand() <= sqrt(c_val)) {
            int len = g.indegree[tempNode];
            if(len == 0)
                break;

            //随机生成子顶点
            int row = R.generateRandom() % len;
            int newNode = g.inAdjList[tempNode][row];

            nodeList[l] = newNode;
            tempNode = newNode;
            l++;

            //生成路径长度大于最长长度时退出循环
            if(l > lmax)
                break;
        }
    }

    /**
     *
     * @param u 单源顶点
     * @param lmax 反向游走最长长度
     * @return 矩阵U,坐标(step,v)表示从顶点u反向游走step到达v的概率
     */
    unordered_map<pair<int, int>, double, HashFunc, EqualKey> revReach(int u, int lmax) {
        //U为矩阵
        unordered_map<pair<int, int>, double, HashFunc, EqualKey> U;
        U[make_pair(0, u)] = 1;
        //
        queue<pair<int, int>> Q;
        Q.push(make_pair(0, u));
        queue<int> PR;
        PR.push(-1);

        while(!Q.empty()) {
            pair<int, int> p = Q.front();
            Q.pop();
            int tl = p.first, tu = p.second;

            int tpr = PR.front();
            PR.pop();

            if(tl >= lmax)//反向游走长度超过最大长度
                continue;
            for(int i = 0; i < g.indegree[tu]; i++) {
                int v = g.inAdjList[tu][i];
                if(v == tpr)// || g.indegree[v] == 0)
                    continue;

                Q.push(make_pair(tl + 1, v));
                PR.push(v);
                U[make_pair(tl + 1, v)] += sqrt(c_val) / g.indegree[tu] * U[p];
            }
        }
        return U;
    }

    void findTop_k(string file, int k, int queryNode) {
        priority_queue<pair<int,  double>, vector<pair<int, double>>, cmp> pq;

        for(int i = 0; i <= g.n_t; i++) {
            if((g.indegree[i] == 0 && g.outdegree[i] == 0) || i == queryNode)//无效顶点
                continue;
            pq.emplace(i, sim[i]);
        }

        ofstream f(file);
        if(!f)
            return;
        //写入文件
        while(k-- && !pq.empty()) {
            auto p = pq.top();
            pq.pop();
            f << p.first << " " << p.second << endl;
        }
        f.close();
    }
};
#endif //CRASHSIM_CRASHSIM_H
