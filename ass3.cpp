#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

const int EDGE_ARRAY_SIZE = 100;
const int VERTICES_ARRAY_SIZE = 100;
const int CONVERT_ASCII = 96;

struct Edge {
    int weight;
    int destination;
    int desX;
    int desY;
    Edge() : weight(INT_MAX), destination(0), desX(0), desY(0) {};
    Edge(int, int, int, int);
    void display();
};

Edge::Edge(int _weight, int _destination, int _desX, int _desY) : weight(_weight), destination(_destination), desX(_desX), desY(_desY) {};
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

int nodeVisitedOnPath = 0;
int nodeVisited = 0;
int length = 0;

void swap(Edge* a, Edge* b);
void siftup(Edge heap[], int i);
void siftdown(Edge heap[], int i, int ctr);
Edge getMinDist(Edge heap[], int &ctr);
void makeheap(Edge heap[], int ctr);

void printPath(int parent[], int j, int nodesVisit[]);
void dijkstra(Vertex array[], int numberOfVertices, int src, int des, int parent[]);
void AStar(Vertex array[], int numberOfVertices, int src, int des, int parent[]);

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
        int des = int(destination - CONVERT_ASCII);
        Edge edge(weight, des, vertexArray[des].x, vertexArray[des].y);
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
    nodesVisit[nodeVisitedOnPath] = int(start - CONVERT_ASCII);

    // find the second shortest path
    nodeVisited = 0;
    int secondShortest = INT_MAX;
    int secondShortestParent[numberOfVertices];
    for (int i = nodeVisitedOnPath; i > 1; i--) {
        int weightTemp = 0;
        int removeIndex = -1;
        int index = nodesVisit[i];

        // binary search
        int startBS = 0;
        int endBS = vertexArray[index].numberOfEdge;
        for (int j = startBS; j < endBS; j++) {
            int mid = startBS + (endBS - startBS) / 2;
            if (vertexArray[index].edgeArray[mid].destination == nodesVisit[i - 1]) {
                weightTemp = vertexArray[index].edgeArray[mid].weight;
                vertexArray[index].edgeArray[mid].weight = 0; // weight = 0 ~ no edge between those vertices
                removeIndex = mid;
                break;
            } else if (nodesVisit[i - 1] > vertexArray[index].edgeArray[mid].destination) {
                startBS = mid + 1;
            } else {
                endBS = mid - 1;
            }

            if (startBS > endBS) break;
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
    // reduce the duplicate destination counts (the number of duplicate counts would be number of nodes on the shortest path - 1,
    // the number of vertices visited includes the destination so get 1 from the number of duplicate counts)
    int duplicateDes = nodeVisitedOnPath - 2;
    nodeVisitedOnPath = 0;
    cout << "\nSecond shortest path using Dijkstra alg:\nPath: " << start << " ";
    printPath(parent, int(end - CONVERT_ASCII), nodesVisit);
    cout << "\nPath distance: " << secondShortest << endl;
    cout << "Number of vertices visited: " << nodeVisited - duplicateDes << endl;

    // A* algorithm
    nodeVisited = 0;
    nodeVisitedOnPath = 0;
    AStar(vertexArray, numberOfVertices, int(start - CONVERT_ASCII), int(end - CONVERT_ASCII), parent);
    cout << "\nShortest path using A* alg:\nPath: " << start << " ";
    printPath(parent, int(end - CONVERT_ASCII), nodesVisit);
    cout << "\nPath distance: " << length << endl;
    cout << "Number of vertices visited: " << nodeVisited << endl;
    nodesVisit[nodeVisitedOnPath] = int(start - CONVERT_ASCII);

    // find second shortest path
    secondShortest = INT_MAX;
    nodeVisited = 0;
    for (int i = nodeVisitedOnPath; i > 1; i--) {
        int weightTemp = 0;
        int removeIndex = -1;
        int index = nodesVisit[i];

        // binary search
        int startBS = 0;
        int endBS = vertexArray[index].numberOfEdge;
        for (int j = startBS; j < endBS; j++) {
            int mid = startBS + (endBS - startBS) / 2;
            if (vertexArray[index].edgeArray[mid].destination == nodesVisit[i - 1]) {
                weightTemp = vertexArray[index].edgeArray[mid].weight;
                vertexArray[index].edgeArray[mid].weight = 0; // weight = 0 ~ no edge between those vertices
                removeIndex = mid;
                break;
            } else if (nodesVisit[i - 1] > vertexArray[index].edgeArray[mid].destination) {
                startBS = mid + 1;
            } else {
                endBS = mid - 1;
            }

            if (startBS > endBS) break;
        }

        AStar(vertexArray, numberOfVertices, int(start - CONVERT_ASCII), int(end - CONVERT_ASCII), parent);
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
    duplicateDes = nodeVisitedOnPath - 2;
    nodeVisitedOnPath = 0;
    cout << "\nSecond shortest path using A* alg:\nPath: " << start << " ";
    printPath(parent, int(end - CONVERT_ASCII), nodesVisit);
    cout << "\nPath distance: " << secondShortest << endl;
    cout << "Number of vertices visited: " << nodeVisited - duplicateDes << endl;

    inFile.close();
    return 0;
}

void swap(Edge* a, Edge* b) {
    Edge temp = *a;
    *a = *b;
    *b = temp;
}

void siftup(Edge heap[], int i) {
    if (i == 1) return;
    int position = i / 2;

    if (heap[position].weight > heap[i].weight) {
        swap(&heap[i], & heap[position]);
        siftup(heap, position);
    }
}

void siftdown(Edge heap[], int i, int ctr) {
    int position = i * 2;
    if (position < ctr && heap[position].weight > heap[position + 1].weight) position++;

    if (position <= ctr && heap[i].weight > heap[position].weight) {
        swap(&heap[i], &heap[position]);
        siftdown(heap, position, ctr);
    }
}

Edge getMinDist(Edge heap[], int &ctr) {
    Edge edge = heap[1];
    swap(&heap[1], &heap[ctr]);
    ctr--;
    siftdown(heap, 1, ctr);
    return edge;
}

void makeheap(Edge heap[], int ctr) {
    for (int i = round(ctr / 2); i >= 1; i--) {
        siftdown(heap, i, ctr);
    }
}

void dijkstra(Vertex array[], int numberOfVertices, int src, int des, int parent[]) {
    // dist[i] hold the shortest distance from src to i
    // i is corresponding to the index of vertexArray since each character has been converted to int
    // and used the number as index in vertexArray (a-t ~ 0-19)
    // edges is a min heap which contains the distance
    Edge edges[numberOfVertices];
    int dist[numberOfVertices];
    // sptSet[i] will be true if vertex i is includedin shortest path tree
    // or shortest distance from src to i is finalized
    bool sptSet[numberOfVertices];

    for (int i = 1; i <= numberOfVertices; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
        parent[i] = -1;
    }

    int ctr = 0;
    ctr++;
    edges[ctr].weight = 0;
    edges[ctr].destination = src;
    dist[src] = 0;

    bool isDes = false;
    while (ctr > 0) {
        Edge u = getMinDist(edges, ctr);
        if (u.destination == des && !isDes) {
            isDes = true;
            nodeVisited++;
        } else if (!isDes) {
            nodeVisited++;
        }
        sptSet[u.destination] = true;

        for (int i = 0; i < array[u.destination].numberOfEdge; i++) {
            int destination = array[u.destination].edgeArray[i].destination;
            if (!sptSet[destination] && array[u.destination].edgeArray[i].weight && dist[u.destination] != INT_MAX
                && dist[u.destination] + array[u.destination].edgeArray[i].weight < dist[destination]) {
                    dist[destination] = dist[u.destination] + array[u.destination].edgeArray[i].weight;
                    parent[destination] = u.destination;
                    ctr++;
                    edges[ctr].destination = destination;
                    edges[ctr].weight = dist[destination];
                    siftup(edges, ctr);
            }
        }
    }

    length = dist[des];
}

void AStar(Vertex array[], int numberOfVertices, int src, int des, int parent[]) {
    // dist[i] hold the shortest distance from src to i
    // i is corresponding to the index of vertexArray since each character has been converted to int
    // and used the number as index in vertexArray (a-t ~ 0-19)
    // edges is a min heap which contains the distance
    Edge edges[numberOfVertices];
    int dist[numberOfVertices];
    // sptSet[i] will be true if vertex i is includedin shortest path tree
    // or shortest distance from src to i is finalized
    bool sptSet[numberOfVertices];

    for (int i = 1; i <= numberOfVertices; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
        parent[i] = -1;
    }

    int ctr = 0;
    ctr++;
    edges[ctr].weight = 0;
    edges[ctr].destination = src;
    dist[src] = 0;

    bool isDes = false;
    while (ctr > 0) {
        Edge u = getMinDist(edges, ctr);
        if (u.destination == des && !isDes) {
            isDes = true;
            nodeVisited++;
        } else if (!isDes) {
            nodeVisited++;
        }
        sptSet[u.destination] = true;

        for (int i = 0; i < array[u.destination].numberOfEdge; i++) {
            int destination = array[u.destination].edgeArray[i].destination;
            double power = 2;
            double startX = array[u.destination].x;
            double startY = array[u.destination].y;
            double desX = array[u.destination].edgeArray[i].desX;
            double desY = array[u.destination].edgeArray[i].desY;

            double euclideanDist = sqrt(pow(startX - desX, power) + pow(startY - desY, power));

            if (!sptSet[destination] && array[u.destination].edgeArray[i].weight && dist[u.destination] != INT_MAX
                && dist[u.destination] + array[u.destination].edgeArray[i].weight + euclideanDist < dist[destination]) {
                    dist[destination] = dist[u.destination] + array[u.destination].edgeArray[i].weight + euclideanDist;
                    parent[destination] = u.destination;
                    ctr++;
                    edges[ctr].destination = destination;
                    edges[ctr].weight = dist[destination];
                    siftup(edges, ctr);
            }
        }
    }

    length = dist[des];
}

void printPath(int parent[], int j, int nodesVisit[]) {
    nodeVisitedOnPath++;
    if (parent[j] != -1) nodesVisit[nodeVisitedOnPath] = j;

    // Base Case : If j is source
    if (parent[j] == -1)
        return;

    printPath(parent, parent[j], nodesVisit);

    cout << char(j + CONVERT_ASCII) << " ";
}

// - To find the shortest path using Dijkstra alg, I have used the min heap to contain the distances to neighbour nodes of
// the current node then the first element in the heap is the shortest path. Beside, I also have an int array which contain
// the shortest path to go to a node from the src. This method is used for A* alg but in the calculation of weight, it also
// adds the Euclidean distance
// - There is an array which contains the nodes visited. Get each pair of node (src and destination - get all neighbour nodes
// of the src and using binary search to find the destination), set the weight between these nodes to 0 (no weight or no path
// between these nodes), then using the alg to find the shortest path, save this result as temporary, loop through all pair of
// nodes in the array of nodes visited and compare the shortest path. The final result is the second shortest path.
// - The proposed solution to find the second shortest path is always correct; if the graph contains cycles, it will not effect
// the result of the second shortest path, because what we delete as proposed solution is the shortest path from the src to the
// destination, so it means that if between src and des contains cycles, there are a path from src to des and a path from des to
// src, but the path from des to src will not in used since the path must be from src to des; The same explanation could be used
// for the graph that contains undirected path, the path that go from des to src will not be certain; The second shortest path is
// always longer than or equal the shortest path since one of the path in the shortest path has been deleted, then the alg will
// find the shortest weight to make a move. So if it is required that the second shortest path must be longer than the shortest path
// we will have to modify the alg to eliminate the result and redo if the second shortest path is equal to the shortest path.
