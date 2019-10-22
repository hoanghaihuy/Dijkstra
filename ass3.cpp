#include <iostream>
#include <fstream>
using namespace std;

const int EDGE_ARRAY_SIZE = 100;
const int VERTICES_ARRAY_SIZE = 100;
const int CONVERT_ASCII = 97;

struct Edge {
    int weight;
    int destination;
    Edge() {};
    Edge(int, char);
    void display();
};

Edge::Edge(int _weight, char _destination) : weight(_weight), destination(_destination) {};
void Edge::display() { cout << char(destination + CONVERT_ASCII) << '(' << weight << ')'; };

struct Vertex {
    int name;
    int x;
    int y;
    int numberOfEdge;
    Edge edgeArray[EDGE_ARRAY_SIZE];
    Vertex() : numberOfEdge(0) {};
    Vertex(char, int, int);
    void addEdge(Edge edge);
    void display();
};

Vertex::Vertex(char _name, int _x, int _y) : name(_name), x(_x), y(_y), numberOfEdge(0) {};
void Vertex::addEdge(Edge edge) {
    edgeArray[numberOfEdge] = edge;
    numberOfEdge++;
};
void Vertex::display() {
    cout << char(name + CONVERT_ASCII) << ":";
    for (int i = 0; i < numberOfEdge; i++) {
        cout << "  ";
        edgeArray[i].display();
        if (i == numberOfEdge - 1) cout << endl;
    }
};

Vertex vertexArray[VERTICES_ARRAY_SIZE];

int nodeVisited = 0;
int length = 0;

void printPath(int parent[], int j, int nodesVisit[]);
int minDistance(int dist[], bool sptSet[], int numberOfVertices);
void dijkstra(Vertex array[], int numberOfVertices, int src, int des, int parent[]);

int main() {
    const string FILE_NAME = "ass3.txt";
    fstream inFile;
    inFile.open(FILE_NAME.c_str());

    if (!inFile) {
        cout << "Unable to open file " << FILE_NAME << endl;
        exit(1);
    }

    int numberOfVertices;
    int numberOfEdges;
    inFile >> numberOfVertices >> numberOfEdges;

    char name;
    int x, y;
    for (int i = 0; i < numberOfVertices; i++) {
        inFile >> name >> x >> y;
        vertexArray[int(name - CONVERT_ASCII)] = Vertex(int(name - CONVERT_ASCII), x, y);
    }

    name = ' ';

    char destination;
    int weight;
    int currentIndex;
    int currentVertex;
    for (int i = 0; i < numberOfEdges; i++) {
        inFile >> name >> destination >> weight;
        Edge edge(weight, int(destination - CONVERT_ASCII));
        vertexArray[int(name - CONVERT_ASCII)].addEdge(edge);
    }

    // for (int i = 0; i < 5; i++) {
    //     vertexArray[i].display();
    // }
    char start, end;
    inFile >> start >> end;
    cout << "Start and end vertex: " << start << " " << end << endl;

    // parent array to store shortest path
    int parent[numberOfVertices];
    // nodesVisit contains the reverse shortest path in int
    int nodesVisit[numberOfVertices];

    // find the shortest path
    dijkstra(vertexArray, numberOfVertices, int(start - CONVERT_ASCII), int(end - CONVERT_ASCII), parent);
    cout << "\nShortest path using Dijkstra alg:\nPath: " << start << " ";
    printPath(parent, int(end - CONVERT_ASCII), nodesVisit);
    cout << "\nPath distance: " << length << endl;
    cout << "Number of vertices visited: " << nodeVisited << endl;
    nodesVisit[nodeVisited - 1] = int(start - CONVERT_ASCII);

    // find the second shortest path
    int secondShortest = INT_MAX;
    int secondShortestParent[numberOfVertices];
    for (int i = nodeVisited - 1; i > 0; i--) {
        int weightTemp = 0;
        int removeIndex = -1;
        int index = nodesVisit[i];
        for (int j = 0; j < vertexArray[index].numberOfEdge; j++) {
            if (vertexArray[index].edgeArray[j].destination == nodesVisit[i - 1]) {
                weightTemp = vertexArray[index].edgeArray[j].weight;
                vertexArray[index].edgeArray[j].weight = 0; // weight = 0 ~ no edge between those vertices
                removeIndex = j;
                break;
            }
        }

        dijkstra(vertexArray, numberOfVertices, int(start - CONVERT_ASCII), int(end - CONVERT_ASCII), parent);
        if (length < secondShortest) {
            secondShortest = length;
            for (int j = 0; j < numberOfVertices; j++) {
                secondShortestParent[j] = parent[j];
            }
        }

        if (weightTemp > 0 && removeIndex != -1) {
            // reset the weight between vertices
            vertexArray[index].edgeArray[removeIndex].weight = weightTemp;
        }
    }
    nodeVisited = 0;
    cout << "\nSecond shortest path using Dijkstra alg:\nPath: " << start << " ";
    printPath(parent, int(end - CONVERT_ASCII), nodesVisit);
    cout << "\nPath distance: " << secondShortest << endl;
    cout << "Number of vertices visited: " << nodeVisited << endl;

    inFile.close();
    return 0;
}

void dijkstra(Vertex array[], int numberOfVertices, int src, int des, int parent[]) {
    // dist[i] hold the shortest distance from src to i
    // i is corresponding to the index of vertexArray since each character has been converted to int
    // and used the number as index in vertexArray (a-t ~ 0-19)
    int dist[numberOfVertices];
    // sptSet[i] will be true if vertex i is includedin shortest path tree
    // or shortest distance from src to i is finalized
    bool sptSet[numberOfVertices];


    for (int i = 0; i < numberOfVertices; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
        parent[i] = -1;
    }

    dist[src] = 0;

    for (int i = 0; i < numberOfVertices - 1; i++) {
        // get the minimum distance vertex's index from the set of vertices not yet processed
        int u = minDistance(dist, sptSet, numberOfVertices);
        sptSet[u] = true;

        for (int j = 0; j < array[u].numberOfEdge; j++) {
            int destination = array[u].edgeArray[j].destination;
            if (!sptSet[destination] && array[u].edgeArray[j].weight && dist[u] != INT_MAX
                && dist[u] + array[u].edgeArray[j].weight < dist[destination]) {
                    dist[destination] = dist[u] + array[u].edgeArray[j].weight;
                    parent[destination] = u;
            }
        }
    }

    length = dist[des];
}

int minDistance(int dist[], bool sptSet[], int numberOfVertices) {
    // Initialize min value
    int min = INT_MAX, min_index;

    for (int i = 0; i < numberOfVertices; i++)
        if (sptSet[i] == false && dist[i] <= min)
            min = dist[i], min_index = i;

    return min_index;
}

void printPath(int parent[], int j, int nodesVisit[]) {
    if (parent[j] != -1) nodesVisit[nodeVisited] = j;
    nodeVisited++;

    // Base Case : If j is source
    if (parent[j] == -1)
        return;

    printPath(parent, parent[j], nodesVisit);

    cout << char(j + CONVERT_ASCII) << " ";
}
