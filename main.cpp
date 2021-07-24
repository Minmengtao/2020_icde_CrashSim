#include "iostream"
#include "vector"
#include "unordered_map"
#include "CrashSim.h"

using namespace std;

void readfile(vector<int>& queryNode, string file) {
    ifstream infile(file);

    //读取文件记录查询顶点
    int node;
    while(infile.good()) {
        infile >> node;
        queryNode.push_back(node);
    }
}

int main() {

    string prefix = "/Users/minmengtao/CLionProjects/CrashSim";
    string dataset = "/dataset/";
    string query = "/query/";
    string result = "/result/";
    string suffix = ".txt";
    vector<string> filePath = {"facebook", "g1", "smallgraph", "wiki-Vote", "tx"};// "ca-HepPh", "ca-AstroPh", "CA-CondMat", "email-Enron", "Epinions", "brightkite", "Slashdot"};

    double eps = 0.0125;//保证模拟值与实际值之间的误差小于等于eps
    double delta = 0.0001;//保证估计模拟值与实际值之间的误差小于等于eps的概率大于等于1-delta
    double c = 0.25;//衰减因子
    int k = 100;//查找的Top-k的节点

    //for(int i = 0; i < filePath.size(); i++) {
        string filename = prefix + dataset + filePath[2] + suffix;
        CrashSim sim = CrashSim(filename, eps, c, delta);

        vector<int> queryNode;//给定的查询节点集

        string queryname = prefix + query + filePath[2] + suffix;
        readfile(queryNode, queryname);
        for (int j = 0; j < queryNode.size(); j++) {
            sim.CalculateSimRank(queryNode[j]);

            string resultname = prefix + result + filePath[2] + "/" + to_string(queryNode[j]) + suffix;
            cout << resultname << endl;
            sim.findTop_k(resultname, k, queryNode[j]);
        }
    //}
    return 0;
}
//g++ -march=core2 -pthread -std=c++11 -DSFMT_MEXP=607 -I SFMT-src-1.4.1 -O3 -o CrashSim SFMT-src-1.4.1/SFMT.c main.cpp
//./CrashSim
//0.142857142857143