#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using std::cout, std::cin, std::cerr;

struct Tree
{
    uint32_t root;
    struct PointProperties
    {
        std::vector<uint32_t> neighbours;
        uint32_t DiscoveryTime;
        uint32_t Low;
        uint32_t parent; // = UINT32_MAX for root
        PointProperties() { DiscoveryTime = Low = UINT32_MAX; }
        PointProperties(uint32_t DiscoveryTime, uint32_t parent = UINT32_MAX, uint32_t Low = UINT32_MAX)
        {
            this->DiscoveryTime = DiscoveryTime;
            this->Low = Low;
            this->parent = parent;
        }
    };
    struct DiscoveredBackEdge
    {
        uint32_t vertex1;
        uint32_t vertex2;
        uint32_t DiscoveryTime;
        bool operator<( DiscoveredBackEdge &a) const
        {
            return DiscoveryTime < a.DiscoveryTime;
        }

        DiscoveredBackEdge(uint32_t vertex1, uint32_t vertex2, uint32_t DiscoveryTime)
        {
            this->DiscoveryTime = DiscoveryTime; // dicovery time of vertex 2
            this->vertex1 = vertex1;
            this->vertex2 = vertex2;
        }
    };
#ifdef DEBUG
    std::unordered_map<uint32_t, PointProperties> AdjMap;
#else
    std::unordered_map<uint32_t, PointProperties> AdjMap;
#endif
    std::unordered_set<uint32_t> ArticulationPoints;
    std::vector<DiscoveredBackEdge> BackEdge;
    Tree(uint32_t root) { this->root = root; }
};

struct Graph
{
    uint32_t n, m;
    std::vector<std::vector<uint32_t>> AdjList; // the graph itself
    std::vector<Tree> DFSForest;                // a spanning forest, stored as a list of adjacency lists
    std::vector<uint32_t> TreeNum;              // TreeNum[i] is the tree number that vertex i belongs to

    Graph(uint32_t n, uint32_t m = 0)
    {
        this->n = n;
        this->m = m;
        AdjList.resize(n);
        TreeNum.resize(n, UINT32_MAX);
    }
};

bool IsWhitespace(const std::string &str)
{
    if (str.find_first_not_of(" \t\n\v\f\r") == std::string::npos)
        return true;
    return false;
}

void SkipNLines(std::ifstream &InputFile, uint64_t n, std::ios::seekdir whence = std::ios::cur,
                std::streamoff offset = 0)
{
    InputFile.seekg(offset, whence);
    std::string line;
    for (uint64_t i = 0; i < n; ++i)
    {
        getline(InputFile, line);
    }
}

void OpenInputFile(std::ifstream &InputFile, char *path)
{
    InputFile.open(path);
    if (!InputFile.is_open())
    {
        cout << "Could not open " << path << "\n";
        exit(1);
    }
}
// Comparator function to sort pairs
// according to second value
bool cmp(const Tree::DiscoveredBackEdge &a, const Tree::DiscoveredBackEdge &b)
{
    return a.DiscoveryTime < b.DiscoveryTime;
}
/* std::vector<std::pair<uint32_t,Tree::PointProperties> > sortBackEdge(const std::unordered_map<uint32_t,Tree::PointProperties> & M)
{

    // Declare vector of pairs
    std::vector<std::pair<uint32_t,Tree::PointProperties> > A;

    // Copy key-value pair from Map
    // to vector of pairs
    for (auto& it : M) {
        A.push_back(it);
    }

    // Sort using comparator function
    sort(A.begin(), A.end(), cmp);

    return A;
} */

template <typename T>
void PrintStack(std::stack<T> stack)
{
    while (!stack.empty())
    {
        cout << stack.top() << " ";
        stack.pop();
    }
    cout << "\n";
}

void PrintGraph(const Graph &graph, bool MatchInputFormat = false)
{
#ifdef DEBUG
    if (!MatchInputFormat)
    {
        cout << "GRAPH\n";
        cout << "Number of vertices = " << graph.n << "\n";
        cout << "Number of edges = " << graph.m << "\n";
    }
    for (uint32_t i = 0; i < graph.n; ++i)
    {
        if (!MatchInputFormat)
            cout << i << " | ";
        for (uint32_t j = 0; j < graph.AdjList[i].size(); ++j)
        {
            cout << graph.AdjList[i][j];
            if (j < graph.AdjList[i].size() - 1)
            {
                cout << " ";
            }
        }
        cout << "\n";
    }
    cout << "\n";
#endif
}

void PrintForest(const std::vector<Tree> &Forest, const Graph &graph)
{
#ifdef DEBUG
    for (uint32_t i = 0; i < Forest.size(); ++i)
    {
        cout << "TREE " << i << "\n";
        cout << "Root: " << Forest[i].root << "\n";
        for (auto &it : Forest[i].AdjMap)
        {
            cout << it.first << " | d=" << it.second.DiscoveryTime << " | TreeNum=" << graph.TreeNum.at(it.first)
                 << " | low=" << it.second.Low << " | ";
            for (auto &v : it.second.neighbours)
            {
                cout << v << " ";
            }
            cout << "\n";
        }

        cout << "Articulation Points: ";
        for (auto &it : Forest[i].ArticulationPoints)
        {
            cout << it << " ";
        }
        cout << "\n\n";
    }
#endif
}

void PrintLevelOrderTraversal(const std::vector<uint32_t> &LevelOrder)
{
#ifdef DEBUG
    cout << "Level-Order Traversal: ";
    for (auto &it : LevelOrder)
    {
        cout << it << " ";
    }
    cout << "\n";
#endif
}

#endif