#include "utils.h"
#include "Timer.h"
using std::cerr;
using std::cin;
using std::cout;


void CheckArgs(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./tarjan file_path.in\n";
        exit(1);
    }
    else if (!(strlen(argv[1]) >= 3 && !strcmp(argv[1] + strlen(argv[1]) - 3, ".in")))
    {
        cout << "The file must end in .in\n";
        exit(1);
    }
}

void MakeDFSGraphWithBackEdges(Graph &graph)
{
    std::unordered_set<uint32_t> unvisited; // unvisited nodes
    unvisited.reserve(graph.n);
    for (uint32_t i = 0; i < graph.n; ++i)
    {
        unvisited.insert(i);
    }

    while (!unvisited.empty())
    {
        auto root = unvisited.begin();
        graph.DFSForest.push_back(Tree(*root)); // create a new tree
        auto &curTree = graph.DFSForest.back();
        uint32_t DiscoveryTime = 1;
        // find curTree, a DFS tree rooted at *root
        std::stack<std::pair<uint32_t, uint32_t>> s; // stack<pair<cur, prev>> to get the DFS tree
        s.push({*root, UINT32_MAX});
        while (!s.empty())
        {
            uint32_t v = s.top().first;
            uint32_t prev = s.top().second;
            s.pop();
            if (unvisited.find(v) != unvisited.end())
            {
                unvisited.erase(v); // mark v as visited
                if (prev == UINT32_MAX)
                {
                    // v is the root
                    curTree.AdjMap.insert({v, Tree::PointProperties(DiscoveryTime)});

                    ++DiscoveryTime;

                    graph.TreeNum.at(v) = static_cast<uint32_t>(graph.DFSForest.size() - 1);
                }
                else
                {
                    // v is not the root
                    if (curTree.AdjMap.count(prev) == 0)
                    {
                        cerr << "Error: prev not present in the map!\n";
                        exit(1);
                    }
                    try
                    {
                        curTree.AdjMap.at(prev).neighbours.push_back(v);
                    }
                    catch (const std::out_of_range &e)
                    {
                        std::cerr << "Error: Prev (" << prev << ") not present in the map: " << e.what() << "\n";
                        exit(1);
                    }
                    if (curTree.AdjMap.count(v) == 0)
                    {
                        curTree.AdjMap.insert({v, Tree::PointProperties(DiscoveryTime)});
                        ++DiscoveryTime;

                        graph.TreeNum.at(v) = static_cast<uint32_t>(graph.DFSForest.size() - 1);
                    }
                    try
                    {
                        curTree.AdjMap.at(v).parent = prev;
                    }
                    catch (const std::out_of_range &e)
                    {
                        std::cerr << "Error: V (" << v << ") not present in the map: " << e.what() << "\n";
                        exit(1);
                    }
                }

                for (uint32_t i = 0; i < graph.AdjList.at(v).size(); ++i)
                {
                    if (unvisited.find(graph.AdjList[v][i]) != unvisited.end())
                        s.push({graph.AdjList[v][i], v}); // push w for all edges vw
                }
            }
            else
            {
                try
                {
                    // curTree.AdjMap.at(prev).parent = v;
                    curTree.BackEdge.push_back(Tree::DiscoveredBackEdge(prev, v, curTree.AdjMap.at(prev).DiscoveryTime, curTree.AdjMap.at(v).DiscoveryTime));
                }
                catch (const std::out_of_range &e)
                {
                    std::cerr << "Error: V (" << v << ") not present in the map: " << e.what() << "\n";
                    exit(1);
                }
            }
        }
    }
}

void LoadGraph(std::ifstream &InputFile, Graph &graph)
{
    SkipNLines(InputFile, 1, std::ios::beg);
    std::string line;
    line.reserve(graph.n * 2);

    uint32_t i = 0;
    while (std::getline(InputFile, line))
    {
        std::stringstream stream(line);
        uint32_t j = 0;
        while (stream >> j)
        {
            graph.AdjList.at(i).push_back(j);
            ++graph.m;
        }
        ++i;
    }

    assert(graph.AdjList.size() == graph.n);
}

struct atrcuate_bridge
{
    std::unordered_set<uint32_t> atriculate;
    std::vector<std::vector<uint32_t>> bridge;
};
atrcuate_bridge CheckBiconnectivity(std::vector<Tree> &Forest, const Graph &graph)
{
    cout << "--------------------EARS___________________________";
    std::vector<std::vector<uint32_t>> EarRemovedAdjList = graph.AdjList;
    std::unordered_set<uint32_t> articulatep(graph.n);
    std::unordered_set<uint32_t> visited;
    for (uint32_t i = 0; i < Forest.size(); ++i)
    {
        uint32_t keep_count = Forest[i].AdjMap.size();

        // sorted by discovery time
        std::sort(Forest[i].BackEdge.begin(), Forest[i].BackEdge.end());
        std::vector<std::vector<uint32_t>> ears;
        uint32_t ear_num = 0;
        for (uint32_t j = 0; j < Forest[i].BackEdge.size(); ++j)
        {
            std::vector<uint32_t> ear{Forest[i].BackEdge[j].vertex1};
            cout << "\nEar" << ear_num << ":";
            cout << Forest[i].BackEdge[j].vertex1 << "-" << Forest[i].BackEdge[j].vertex2;
            ear_num += 1;
            if (ear_num == 1)
            {
                cout << "\nEar" << ear_num << ":";
                cout << Forest[i].BackEdge[j].vertex2;
            }

            visited.insert(Forest[i].BackEdge[j].vertex1);
            keep_count--;
            ear.push_back(Forest[i].BackEdge[j].vertex2);
            uint32_t v1 = Forest[i].BackEdge[j].vertex1;
            uint32_t v2 = Forest[i].BackEdge[j].vertex2;
            EarRemovedAdjList[v1].erase(std::remove(EarRemovedAdjList[v1].begin(), EarRemovedAdjList[v1].end(), v2), EarRemovedAdjList[v1].end());
            EarRemovedAdjList[v2].erase(std::remove(EarRemovedAdjList[v2].begin(), EarRemovedAdjList[v2].end(), v1), EarRemovedAdjList[v2].end());
            while (visited.find(ear.back()) == visited.end())
            {
                visited.insert(ear.back());
                keep_count--;
                EarRemovedAdjList[ear.back()].erase(std::remove(EarRemovedAdjList[ear.back()].begin(), EarRemovedAdjList[ear.back()].end(), Forest[i].AdjMap.at(ear.back()).parent), EarRemovedAdjList[ear.back()].end());
                uint32_t par = Forest[i].AdjMap.at(ear.back()).parent;
                // cout << "[" << par << ear.back() << "]";
                if (par < graph.n)
                    EarRemovedAdjList[par].erase(std::remove(EarRemovedAdjList[par].begin(), EarRemovedAdjList[par].end(), ear.back()), EarRemovedAdjList[par].end());
                ear.push_back(Forest[i].AdjMap.at(ear.back()).parent);

                cout << "-" << ear.back();
            }

            if (j != 0 && ear.front() == ear.back())
            {
                articulatep.insert(ear.front());
            }
            ears.push_back(ear);
        }
        if (keep_count > 0 && keep_count < graph.n) // if unvisited node in the connected Tree
        {
            cout << "\nConnected component " << i << " whose DFS Root is " << Forest[i].root << " don't have biconnectivity ";
        }
        else
        {
            cout << "\nConnected component " << i << " whose DFS Root is " << Forest[i].root << " is biconnected!";
        }

        for (auto i : Forest[i].AdjMap)
        {
            for (uint32_t r = 0; r < i.second.neighbours.size(); r++)
            {
                if (visited.find(i.second.neighbours[r]) == visited.end() && i.second.neighbours.size() != 1)
                {
                    articulatep.insert(i.first);
                }
            }
        }
    }
    // my logic not sure....need to verify
    for (uint32_t x = 0; x < graph.n; ++x)
    {
        for (uint32_t r = 0; r < graph.AdjList[x].size(); ++r)
        {
            if (visited.find(graph.AdjList[x][r]) == visited.end() && graph.AdjList[x].size() != 1)
            {
                articulatep.insert(x);
            }
        }
    }

    cout << "\n Articulate points: ";
    for (auto i = articulatep.begin(); i != articulatep.end(); ++i)
    {
        std::cout << (*i) << " ";
    }
    cout << "\n Bridges: ";
    for (uint32_t x = 0; x < graph.n; ++x)
    {
        for (uint32_t r = 0; r < EarRemovedAdjList[x].size(); ++r)
        {
            if (x < EarRemovedAdjList[x][r])
                cout << x << "-" << EarRemovedAdjList[x][r] << ",";
        }
    }
    return atrcuate_bridge{articulatep, EarRemovedAdjList};
}

void DFS(uint32_t v,uint32_t par,uint32_t b_num,std::unordered_set<uint32_t> &unvisited,std::unordered_set<uint32_t>articulatePoint, std::vector<std::vector< std::pair <uint32_t,uint32_t> >> &biconn,std::vector<std::vector<uint32_t>> Biconnected)
{
    // Mark the current node as visited and
    // print it
    if(par < Biconnected.size())
        biconn[b_num].push_back(std::make_pair(v,par));
    // Recur for all the vertices adjacent
    // to this vertex
    
    for (auto i = Biconnected[v].begin(); i != Biconnected[v].end(); ++i){
        if (unvisited.find(*i)!=unvisited.end() && articulatePoint.find(*i) == articulatePoint.end() )
            DFS(*i,v,b_num,unvisited,articulatePoint,biconn,Biconnected);
        else{
            biconn[b_num].push_back(std::make_pair(*i,par));
        }
    }        
}
void findBiconnectedComponent(Graph &graph)
{
    atrcuate_bridge s = CheckBiconnectivity(graph.DFSForest, graph);
    std::vector<std::vector<uint32_t>> Biconnected = graph.AdjList;

    //remove bridges from original graph
    for (uint32_t x = 0; x < graph.n; ++x)
    {
        for (uint32_t r = 0; r < s.bridge[x].size(); ++r)
        {
            if (x < s.bridge[x][r])
            {
                s.bridge[x].erase(std::remove(s.bridge[x].begin(), s.bridge[x].end(), s.bridge[x][r]), s.bridge[x].end());
                s.bridge[ s.bridge[x][r]].erase(std::remove(s.bridge[ s.bridge[x][r]].begin(), s.bridge[ s.bridge[x][r]].end(),x), s.bridge[s.bridge[x][r]].end());
            }
        }
    }
    std::unordered_set<uint32_t> unvisited; // unvisited nodes
    std::vector<std::vector< std::pair <uint32_t,uint32_t> >> biconn;
    unvisited.reserve(graph.n);
    biconn.reserve(graph.n);
    uint32_t b_num=0;
    for (uint32_t i = 0; i < graph.n; ++i)
    {
        unvisited.insert(i);
    }

     while (!unvisited.empty())
    {   
        auto root = unvisited.begin();
        if(s.atriculate.find(*root) == s.atriculate.end()){
            DFS(*root,graph.n,b_num,unvisited,s.atriculate, biconn,Biconnected);
        }
        b_num++;

    }   
    //find biconnected component



}
int main(int argc, char *argv[])
{
    CheckArgs(argc, argv);

    std::ifstream InputFile;
    OpenInputFile(InputFile, argv[1]);

    uint32_t n;                // number of vertices
    InputFile >> n >> std::ws; // skip the whitespace at the end of the line before switching to line-based reading using getline
    Timer t;

    Graph graph(n);
    LoadGraph(InputFile, graph);
    // PrintGraph(graph);

    MakeDFSGraphWithBackEdges(graph);
    CheckBiconnectivity(graph.DFSForest, graph);
    //findBiconnectedComponent(graph);
    // PrintForest(graph.DFSForest, graph);

    InputFile.close();
    auto duration = t.Stop();
    cout<<"TimeTaken"<<duration;
    return 0;
}
