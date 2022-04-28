#include "utils.h"

using std::cout, std::cin, std::cerr;

void CheckArgs(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: ./tarjan file_path.in\n";
        exit(1);
    } else if (!(strlen(argv[1]) >= 3 && !strcmp(argv[1] + strlen(argv[1]) - 3, ".in"))) {
        cout << "The file must end in .in\n";
        exit(1);
    }
}

void MakeDFSForest(Graph& graph) {
    std::unordered_set<uint32_t> unvisited;  // unvisited nodes
    unvisited.reserve(graph.n);
    for (uint32_t i = 0; i < graph.n; ++i) {
        unvisited.insert(i);
    }

    while (!unvisited.empty()) {
        auto root = unvisited.begin();
        graph.DFSForest.push_back(Tree(*root));  // create a new tree
        auto& curTree = graph.DFSForest.back();
        uint32_t DiscoveryTime = 1;

        // find curTree, a DFS tree rooted at *root
        std::stack<std::pair<uint32_t, uint32_t>> s;  // stack<pair<cur, prev>> to get the DFS tree
        s.push({*root, UINT32_MAX});
        while (!s.empty()) {
            uint32_t v = s.top().first;
            uint32_t prev = s.top().second;
            s.pop();
            if (unvisited.find(v) != unvisited.end()) {
                unvisited.erase(v);  // mark v as visited
                if (prev == UINT32_MAX) {
                    // v is the root
                    curTree.AdjMap.insert({v, Tree::PointProperties(DiscoveryTime)});

                    ++DiscoveryTime;

                    graph.TreeNum.at(v) = static_cast<uint32_t>(graph.DFSForest.size() - 1);
                } else {
                    // v is not the root
                    if (curTree.AdjMap.count(prev) == 0) {
                        cerr << "Error: prev not present in the map!\n";
                        exit(1);
                    }

                    try {
                        curTree.AdjMap.at(prev).neighbours.push_back(v);
                    } catch (const std::out_of_range& e) {
                        std::cerr << "Error: Prev (" << prev << ") not present in the map: " << e.what() << "\n";
                        exit(1);
                    }

                    if (curTree.AdjMap.count(v) == 0) {
                        curTree.AdjMap.insert({v, Tree::PointProperties(DiscoveryTime)});
                        ++DiscoveryTime;

                        graph.TreeNum.at(v) = static_cast<uint32_t>(graph.DFSForest.size() - 1);
                    }
                }

                for (uint32_t i = 0; i < graph.AdjList.at(v).size(); ++i) {
                    if (unvisited.find(graph.AdjList[v][i]) != unvisited.end())
                        s.push({graph.AdjList[v][i], v});  // push w for all edges vw
                }
            }
        }
    }
}

void LoadGraph(std::ifstream& InputFile, Graph& graph) {
    SkipNLines(InputFile, 1, std::ios::beg);
    std::string line;
    line.reserve(graph.n * 2);

    uint32_t i = 0;
    while (std::getline(InputFile, line)) {
        std::stringstream stream(line);
        uint32_t j = 0;
        while (stream >> j) {
            graph.AdjList.at(i).push_back(j);
            ++graph.m;
        }
        ++i;
    }

    assert(graph.AdjList.size() == graph.n);
}

void LevelOrderTraversal(const Tree& tree, std::vector<uint32_t>& LevelOrder) {
    // Performs BFS
    LevelOrder.reserve(tree.AdjMap.size());
    std::queue<uint32_t> q;

    uint32_t root = tree.root;
    q.push(root);

    while (!q.empty()) {
        auto& v = q.front();
        LevelOrder.push_back(v);
        q.pop();
        for (auto& neighbour : tree.AdjMap.at(v).neighbours) {
            q.push(neighbour);
        }
    }

    assert(LevelOrder.size() == tree.AdjMap.size());
}

void FindLowTree(Tree& tree, const Graph& graph) {
    std::vector<uint32_t> LevelOrder;
    LevelOrderTraversal(tree, LevelOrder);
    PrintLevelOrderTraversal(LevelOrder);

    for (auto node = LevelOrder.rbegin(); node != LevelOrder.rend(); ++node) {
        auto& CurNode = tree.AdjMap.at(*node);
        CurNode.Low = CurNode.DiscoveryTime;

        // iterate over children of CurNode
        uint32_t MaxLowOfChildren = 0;
        for (auto& child : CurNode.neighbours) {
            auto& ChildNode = tree.AdjMap.at(child);
            CurNode.Low = std::min(CurNode.Low, ChildNode.Low);
            MaxLowOfChildren = std::max(MaxLowOfChildren, ChildNode.Low);
        }
        if (MaxLowOfChildren >= CurNode.DiscoveryTime &&
            CurNode.neighbours.size() > 0 &&
            !(tree.root == *node && CurNode.neighbours.size() == 1)
            /* not a node with only 1 neighbour */) {
            tree.ArticulationPoints.insert(*node);
        }

        // iterate over back edges of CurNode
        auto& children = tree.AdjMap.at(*node).neighbours;
        auto& neighbours = graph.AdjList.at(*node);
        for (auto& neighbour : neighbours) {
            auto& ChildrenOfNeighbour = tree.AdjMap.at(neighbour).neighbours;
            if (
                std::find(children.begin(), children.end(), neighbour) == children.end()
                /* is a neighbour but a not a child */
                &&
                std::find(ChildrenOfNeighbour.begin(), ChildrenOfNeighbour.end(), *node) == ChildrenOfNeighbour.end()
                /* is not a parent */) {
                // CurNode is a back edge
                // note: the edge to the parent is *NOT* a back edge as the graph is undirected
                CurNode.Low = std::min(CurNode.Low, tree.AdjMap.at(neighbour).DiscoveryTime);
            }
        }
    }
}

void FindLow(Graph& graph) {
    for (auto& tree : graph.DFSForest) {
        FindLowTree(tree, graph);
    }
}

int main(int argc, char* argv[]) {
    CheckArgs(argc, argv);

    std::ifstream InputFile;
    OpenInputFile(InputFile, argv[1]);

    uint32_t n;                 // number of vertices
    InputFile >> n >> std::ws;  // skip the whitespace at the end of the line before switching to line-based reading using getline

    Graph graph(n);
    LoadGraph(InputFile, graph);
    PrintGraph(graph);

    MakeDFSForest(graph);
    FindLow(graph);

    PrintForest(graph.DFSForest, graph);

    InputFile.close();
    return 0;
}