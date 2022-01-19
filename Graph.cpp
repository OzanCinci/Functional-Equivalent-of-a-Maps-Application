#include "Graph.h"
#include "GraphExceptions.h"

#include <iostream>
#include <iomanip>
#include <queue>
#include <fstream>
#include <sstream>
#include <cstdlib>

// HELPER FUNCTION TO GET ID OF EDGE BY TRAVERSING THE ENTIRE EDGE LIST (RETURNS -1 IF NOT EXIST)
int Graph::HELPER_ID_FINDER_EDGE(int id1, int id2) const
{
    for (int i = 0; i < edgeList.size(); i++)
    {
        if (edgeList[i].vertexId0 == id1 && edgeList[i].vertexId1 == id2)
            return i;
        if (edgeList[i].vertexId1 == id1 && edgeList[i].vertexId0 == id2)
            return i;
    }
    return -1;
}

// HELPER FUNCTION TO GET ID OF A VERTEX BY TRAVERSING THE ENTIRE VERTEX LIST (RETURNS -1 IF NOT EXIST)
int Graph::HELPER_ID_FINDER_VERTEX(const std::string &vertexName) const
{
    for (int i = 0; i < vertexList.size(); i++)
    {
        if (vertexList[i].name == vertexName)
            return i;
    }
    return -1;
}

// Literally do nothing here
// default constructors of the std::vector is enough
Graph::Graph() {}

Graph::Graph(const std::string &filePath)
{
    // Tokens
    std::string tokens[3];

    std::ifstream mapFile(filePath.c_str());
    // Read line by line
    std::string line;
    while (std::getline(mapFile, line))
    {
        // Empty Line Skip
        if (line.empty())
            continue;
        // Comment Skip
        if (line[0] == '#')
            continue;

        // Tokenize the line
        int i = 0;
        std::istringstream stream(line);
        while (stream >> tokens[i])
            i++;

        // Single token (Meaning it is a vertex)
        if (i == 1)
        {
            InsertVertex(tokens[0]);
        }
        // Exactly three tokens (Meaning it is an edge)
        else if (i == 3)
        {
            int weight = std::atoi(tokens[0].c_str());
            if (!ConnectVertices(tokens[1], tokens[2], weight))
            {
                std::cerr << "Duplicate edge on "
                          << tokens[0] << "-"
                          << tokens[1] << std::endl;
            }
        }
        else
            std::cerr << "Token Size Mismatch" << std::endl;
    }
}

void Graph::InsertVertex(const std::string &vertexName)
{
    int size_of_vertices = vertexList.size();

    for (int i = 0; i < size_of_vertices; i++)
        if (vertexList[i].name == vertexName)
            throw DuplicateVertexNameException();

    GraphVertex new_vertex;
    new_vertex.edgeCount = 0;
    new_vertex.name = vertexName;

    vertexList.push_back(new_vertex);
}

bool Graph::ConnectVertices(const std::string &fromVertexName, const std::string &toVertexName, int weight)
{
    if (fromVertexName == toVertexName)
        return false;

    int size_of_edges = edgeList.size();
    int size_of_vertices = vertexList.size();
    int id0 = -1;
    int id1 = -1;
    int i = 0;
    int j = 0;

    for (; i < size_of_vertices; i++)
    {
        if (vertexList[i].name == fromVertexName)
            id0 = i;
        else if (vertexList[i].name == toVertexName)
            id1 = i;
    }

    if (id0 == -1 || id1 == -1)
        throw VertexNotFoundException();

    for (; j < size_of_edges; j++)
    {
        if (edgeList[j].vertexId0 == id0 && edgeList[j].vertexId1 == id1)
            return false;
        if (edgeList[j].vertexId1 == id0 && edgeList[j].vertexId0 == id1)
            return false;
    }

    if (vertexList[id0].edgeCount == MAX_EDGE_PER_VERTEX || vertexList[id1].edgeCount == MAX_EDGE_PER_VERTEX)
        throw TooManyEdgeOnVertexExecption();

    GraphEdge new_edge;
    new_edge.weight = weight;
    new_edge.vertexId0 = id0;
    new_edge.vertexId1 = id1;
    new_edge.masked = false;
    edgeList.push_back(new_edge);

    vertexList[id0].edgeIds[(vertexList[id0].edgeCount)] = edgeList.size() - 1;
    vertexList[id1].edgeIds[(vertexList[id1].edgeCount)] = edgeList.size() - 1;

    vertexList[id0].edgeCount++;
    vertexList[id1].edgeCount++;

    return true;
}

bool Graph::ShortestPath(std::vector<int> &orderedVertexIdList, const std::string &from, const std::string &to) const
{
    int from_id = HELPER_ID_FINDER_VERTEX(from);
    int to_id = HELPER_ID_FINDER_VERTEX(to);
    if (from_id == -1 || to_id == -1)
        throw VertexNotFoundException();

    if (from_id == to_id)
    {
        orderedVertexIdList.push_back(from_id);
        return true;
    }

    std::priority_queue<DistanceVertexIdPair> min_heap;
    std::vector<int> distance;
    std::vector<int> previous_vertex_id;
    for (int i = 0; i < TotalVertexCount(); i++)
    {
        distance.push_back(LARGE_NUMBER);
        previous_vertex_id.push_back(-1);
    }

    distance[from_id] = 0;
    for (int i = 0; i < vertexList[from_id].edgeCount; i++)
    {
        if (edgeList[vertexList[from_id].edgeIds[i]].masked == false)
        {
            int other_id;
            int agirlik = edgeList[vertexList[from_id].edgeIds[i]].weight;

            if (edgeList[vertexList[from_id].edgeIds[i]].vertexId0 == from_id)
                other_id = edgeList[vertexList[from_id].edgeIds[i]].vertexId1;
            else
                other_id = edgeList[vertexList[from_id].edgeIds[i]].vertexId0;
            distance[other_id] = agirlik;
            previous_vertex_id[other_id] = from_id;

            min_heap.push(DistanceVertexIdPair(other_id, agirlik));
        }
    }

    while (!min_heap.empty())
    {
        DistanceVertexIdPair top_element = min_heap.top();
        min_heap.pop();
        int from_id = top_element.vId;
        int prev_agirlik = top_element.currentWeight;

        for (int i = 0; i < vertexList[from_id].edgeCount; i++)
        {
            if (edgeList[vertexList[from_id].edgeIds[i]].masked == false)
            {
                int other_id;
                int agirlik = prev_agirlik + edgeList[vertexList[from_id].edgeIds[i]].weight;

                if (edgeList[vertexList[from_id].edgeIds[i]].vertexId0 == from_id)
                    other_id = edgeList[vertexList[from_id].edgeIds[i]].vertexId1;
                else
                    other_id = edgeList[vertexList[from_id].edgeIds[i]].vertexId0;

                if (distance[other_id] > agirlik)
                {
                    distance[other_id] = agirlik;
                    previous_vertex_id[other_id] = from_id;
                    min_heap.push(DistanceVertexIdPair(other_id, agirlik));
                }
            }
        }
    }

    from_id = HELPER_ID_FINDER_VERTEX(from);
    to_id = HELPER_ID_FINDER_VERTEX(to);

    if (previous_vertex_id[to_id] == -1)
        return false;

    std::vector<int> reversed_path;
    reversed_path.push_back(to_id);

    int prev_recursive = previous_vertex_id[to_id];

    while (prev_recursive != -1)
    {
        reversed_path.push_back(prev_recursive);
        prev_recursive = previous_vertex_id[prev_recursive];
    }

    for (int index = reversed_path.size() - 1; index > -1; index--)
        orderedVertexIdList.push_back(reversed_path[index]);

    return true;
}

int Graph::MultipleShortPaths(std::vector<std::vector<int>> &orderedVertexIdList,
                              const std::string &from,
                              const std::string &to,
                              int numberOfShortestPaths)
{
    int from_id = HELPER_ID_FINDER_VERTEX(from);
    int to_id = HELPER_ID_FINDER_VERTEX(to);
    if (from_id == -1 || to_id == -1)
        throw VertexNotFoundException();

    UnMaskAllEdges();

    std::vector<int> temp_shortest_path;
    ShortestPath(temp_shortest_path, from, to);
    if (temp_shortest_path.size() == 0)
        return 0;

    orderedVertexIdList.push_back(temp_shortest_path);

    numberOfShortestPaths--;
    int EdgeIdWithMaxWeight;
    int maxWeight = 0;
    int counter = 1;

    for (int index = 0; index < numberOfShortestPaths; index++)
    {
        std::vector<int> temp_path;

        if (index > orderedVertexIdList.size() - 1)
            break;

        std::vector<int> last_path = orderedVertexIdList[index];
        int tmp_edge_id;
        for (int k = 0; k < last_path.size() - 1; k++)
        {
            tmp_edge_id = HELPER_ID_FINDER_EDGE(last_path[k], last_path[k + 1]);
            if (edgeList[tmp_edge_id].weight > maxWeight)
            {
                maxWeight = edgeList[tmp_edge_id].weight;
                EdgeIdWithMaxWeight = tmp_edge_id;
            }
        }
        edgeList[EdgeIdWithMaxWeight].masked = true;

        if (ShortestPath(temp_path, from, to))
            orderedVertexIdList.push_back(temp_path);

        if (temp_path.size() == 0)
            break;

        counter++;
        maxWeight = 0;
    }
    UnMaskAllEdges();
    return counter;
}

void Graph::MaskEdges(const std::vector<StringPair> &vertexNames)
{
    int size = vertexNames.size();

    for (int i = 0; i < size; i++)
        HELPER_MaskEdges(vertexNames[i], true);
}

void Graph::HELPER_MaskEdges(const StringPair &vertexNames, bool mask_situation)
{
    int id1 = HELPER_ID_FINDER_VERTEX(vertexNames.s0);
    int id2 = HELPER_ID_FINDER_VERTEX(vertexNames.s1);

    if (id1 == -1 || id2 == -1)
        throw VertexNotFoundException();

    if (id1 == id2)
        return;

    int edge_id = HELPER_ID_FINDER_EDGE(id1, id2);

    if (edge_id == -1)
        return;

    edgeList[edge_id].masked = mask_situation;
}

void Graph::UnMaskEdges(const std::vector<StringPair> &vertexNames)
{
    int size = vertexNames.size();

    for (int i = 0; i < size; i++)
        HELPER_MaskEdges(vertexNames[i], false);
}

void Graph::UnMaskAllEdges()
{
    int size = edgeList.size();

    for (int i = 0; i < size; i++)
        edgeList[i].masked = false;
}

void Graph::MaskVertexEdges(const std::string &name)
{
    int id_of_vertex = HELPER_ID_FINDER_VERTEX(name);

    if (id_of_vertex == -1)
        throw VertexNotFoundException();

    for (int i = 0; i < vertexList[id_of_vertex].edgeCount; i++)
        edgeList[vertexList[id_of_vertex].edgeIds[i]].masked = true;
}

void Graph::UnMaskVertexEdges(const std::string &name)
{
    int id_of_vertex = HELPER_ID_FINDER_VERTEX(name);

    if (id_of_vertex == -1)
        throw VertexNotFoundException();

    for (int i = 0; i < vertexList[id_of_vertex].edgeCount; i++)
        edgeList[vertexList[id_of_vertex].edgeIds[i]].masked = false;
}

void Graph::ModifyEdge(const std::string &vName0, const std::string &vName1, float newWeight)
{
    int id0 = HELPER_ID_FINDER_VERTEX(vName0);
    int id1 = HELPER_ID_FINDER_VERTEX(vName1);

    if (id0 == -1 || id1 == -1)
        throw VertexNotFoundException();

    if (id0 == id1)
        return;

    int edge_id = HELPER_ID_FINDER_EDGE(id0, id1);

    if (edge_id == -1)
        return;

    edgeList[edge_id].weight = newWeight;
}

void Graph::ModifyEdge(int vId0, int vId1, float newWeight)
{
    int id0 = vId0;
    int id1 = vId1;

    if (id0 == id1)
        return;
    int edge_id = HELPER_ID_FINDER_EDGE(id0, id1);

    if (edge_id == -1)
        return;

    edgeList[edge_id].weight = newWeight;
}

void Graph::PrintAll() const
{
    for (size_t i = 0; i < vertexList.size(); i++)
    {
        const GraphVertex &v = vertexList[i];
        std::cout << v.name << "\n";
        for (int j = 0; j < v.edgeCount; j++)
        {
            int edgeId = v.edgeIds[j];
            const GraphEdge &edge = edgeList[edgeId];
            // Skip printing this edge if it is masked
            if (edge.masked)
                continue;

            // List the all vertex names and weight
            std::cout << "-" << std::setfill('-')
                      << std::setw(2) << edge.weight
                      << "-> ";
            int neigVertexId = (static_cast<int>(i) == edge.vertexId0)
                                   ? edge.vertexId1
                                   : edge.vertexId0;
            std::cout << vertexList[neigVertexId].name << "\n";
        }
    }
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

void Graph::PrintPath(const std::vector<int> &orderedVertexIdList, bool sameLine) const
{
    for (size_t i = 0; i < orderedVertexIdList.size(); i++)
    {
        int vertexId = orderedVertexIdList[i];
        if (vertexId >= static_cast<int>(vertexList.size()))
            throw VertexNotFoundException();

        const GraphVertex &vertex = vertexList[vertexId];
        std::cout << vertex.name;
        if (!sameLine)
            std::cout << "\n";
        // Only find and print the weight if next is available
        if (i == orderedVertexIdList.size() - 1)
            break;
        int nextVertexId = orderedVertexIdList[i + 1];
        if (nextVertexId >= static_cast<int>(vertexList.size()))
            throw VertexNotFoundException();

        // Find the edge between these two vertices
        int edgeId = INVALID_INDEX;
        if (vertexId < static_cast<int>(vertexList.size()) &&
            nextVertexId < static_cast<int>(vertexList.size()))
        {
            // Check all of the edges of vertex
            // and try to find
            const GraphVertex &fromVert = vertexList[vertexId];
            for (int i = 0; i < fromVert.edgeCount; i++)
            {
                int eId = fromVert.edgeIds[i];
                // Since the graph is not directional
                // check the both ends
                if ((edgeList[eId].vertexId0 == vertexId &&
                     edgeList[eId].vertexId1 == nextVertexId) ||
                    (edgeList[eId].vertexId0 == nextVertexId &&
                     edgeList[eId].vertexId1 == vertexId))
                {
                    edgeId = eId;
                    break;
                }
            }
        }
        if (edgeId != INVALID_INDEX)
        {
            const GraphEdge &edge = edgeList[edgeId];
            std::cout << "-" << std::setfill('-')
                      << std::setw(2)
                      << edge.weight << "->";
        }
        else
        {
            std::cout << "-##-> ";
        }
    }
    // Print endline on the last vertex if same line is set
    if (sameLine)
        std::cout << "\n";
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

int Graph::TotalVertexCount() const
{
    return vertexList.size();
}

int Graph::TotalEdgeCount() const
{
    return edgeList.size();
}

std::string Graph::VertexName(int vertexId) const
{
    if (vertexId >= (int)vertexList.size())
        return "";
    return vertexList[vertexId].name;
}

int Graph::TotalWeightInBetween(std::vector<int> &orderedVertexIdList)
{
    int not_exist;
    int result = 0;
    for (int index = 0; index < orderedVertexIdList.size() - 1; index++)
    {
        not_exist = HELPER_ID_FINDER_EDGE(orderedVertexIdList[index], orderedVertexIdList[index + 1]);
        if (not_exist == -1)
            throw VertexNotFoundException();
        result += edgeList[not_exist].weight;
    }
    return result;
}