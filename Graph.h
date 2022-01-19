
#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <string>
#include <vector>

// Compile time constants
#define MAX_EDGE_PER_VERTEX 8
#define INVALID_INDEX -1
// Large number can be use to set initial weights.
// Weights of the edges are guaranteed to not exceed this value
#define LARGE_NUMBER 0x7FFFFFFF

struct GraphVertex
{
    int edgeIds[MAX_EDGE_PER_VERTEX]; // Adjacency List
    int edgeCount;                    // Current edge count
    std::string name;                 // Name of the vertex
};

struct GraphEdge
{
    int weight;    // Weight of the edge (used on shortest path)
    bool masked;   // Whether this edge is masked or not
    int vertexId0; // Information about which two vertices this edge
    int vertexId1; // connects.
};

struct StringPair
{
    std::string s0;
    std::string s1;

    StringPair(const std::string s0, const std::string s1)
    {
        this->s0 = s0;
        this->s1 = s1;
    }
};

// This struct can be used on priority queue
struct DistanceVertexIdPair
{
    int vId;
    int currentWeight;

    // Constructor
    DistanceVertexIdPair(int vId, int currentWeight)
    {
        this->vId = vId;
        this->currentWeight = currentWeight;
    }

    // This is implemented for "priority_queue"
    // priority queue uses this to determine which pair is "smaller"
    // than another pair
    bool operator<(const DistanceVertexIdPair &p) const
    {
        return currentWeight > p.currentWeight;
    }
};

class Graph
{
private:
    // HELPER FUNCTIONS
    int HELPER_ID_FINDER_VERTEX(const std::string &vertexName) const;
    int HELPER_ID_FINDER_EDGE(int id1, int id2) const;
    void HELPER_MaskEdges(const StringPair &vertexNames, bool mask_situation);

    std::vector<GraphVertex> vertexList;
    std::vector<GraphEdge> edgeList;

    // Friend class that will access private members etc.
    // This will be used for testing. Do not remove/edit these
    // statements.
    friend class PA3_GRH_TEST_CASE;
    friend class PA3_MAPS_TEST_CASE;

protected:
public:
    Graph();
    Graph(const std::string &filePath);
    void InsertVertex(const std::string &vertexName);
    bool ConnectVertices(const std::string &fromVertexName, const std::string &toVertexName, int weight);
    bool ShortestPath(std::vector<int> &orderedVertexIdList, const std::string &from, const std::string &to) const;
    int MultipleShortPaths(std::vector<std::vector<int>> &orderedVertexIdList, const std::string &from, const std::string &to, int numberOfShortestPaths);
    void MaskEdges(const std::vector<StringPair> &vertexNames);
    void UnMaskEdges(const std::vector<StringPair> &vertexNames);
    void UnMaskAllEdges();
    void MaskVertexEdges(const std::string &name);
    void UnMaskVertexEdges(const std::string &name);
    void ModifyEdge(const std::string &vName0, const std::string &vName1, float newWeight);
    void ModifyEdge(int vId0, int vId1, float newWeight);
    void PrintAll() const;
    void PrintPath(const std::vector<int> &orderedVertexIdList, bool sameLine = false) const;
    int TotalVertexCount() const;
    int TotalEdgeCount() const;
    std::string VertexName(int vertexId) const;
    int TotalWeightInBetween(std::vector<int> &orderedVertexIdList);
};

#endif //__GRAPH_H__