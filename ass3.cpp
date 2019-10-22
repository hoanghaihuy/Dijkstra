#include <iostream>
#include <fstream>
using namespace std;

const int EDGE_ARRAY_SIZE = 100;
const int VERTICES_ARRAY_SIZE = 100;
const int CONVERT_ASCII = 97;

struct Edge {
    int weight;
    int destination;
    bool isProcessed;
    Edge() : weight(INT_MAX), destination(0), isProcessed(false) {};
    Edge(int, int);
    void display();
};

Edge::Edge(int _weight, int _destination) : weight(_weight), destination(_destination), isProcessed(false) {};
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

void swap(Edge *a, Edge *b);
void siftup(Edge heap[], int i);
void siftdown(Edge heap[], int i, int max);
int getMinDist(Edge heap[], int i);
void makeheap(Edge heap[], int max);

void printPath(int parent[], int j);
int minDistance(int dist[], bool sptSet[], int numberOfVertices);
void dijkstra(int numberOfVertices, int src, int des);

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
    dijkstra(numberOfVertices, int(start - CONVERT_ASCII), int(end - CONVERT_ASCII));
    cout << "Number of nodes visited: " << nodeVisited << endl;

    inFile.close();
    return 0;
}

void dijkstra(int numberOfVertices, int src, int des) {
    // dist[i] hold the shortest distance from src to i
    // i is corresponding to the index of vertexArray since each character has been converted to int
    // and used the number as index in vertexArray (a-t ~ 0-19)
    Edge *dist = new Edge[numberOfVertices];
    // sptSet[i] will be true if vertex i is includedin shortest path tree
    // or shortest distance from src to i is finalized
    bool sptSet[numberOfVertices];
    // parent array to store shortest path
    int parent[numberOfVertices];
    // cout << numberOfVertices << endl;

    for (int i = 0; i < numberOfVertices; i++) {
        dist[i] = Edge(INT_MAX, i);
        sptSet[i] = false;
        parent[i] = -1;
    }

    dist[src] = Edge(0, src);

    for (int i = 0; i < numberOfVertices - 1; i++) {
        // get the minimum distance vertex's index from the set of vertices not yet processed
        // int u = minDistance(dist, sptSet, numberOfVertices);
        int u = getMinDist(dist, i);
        // sptSet[u] = true;
        dist[u].isProcessed = true;

        for (int j = 0; j < vertexArray[u].numberOfEdge; j++) {
            int destination = vertexArray[u].edgeArray[j].destination;
            if (!dist[destination].isProcessed && dist[u].weight != INT_MAX
                && dist[u].weight + vertexArray[u].edgeArray[j].weight < dist[destination].weight) {
                    // cout << char(u + CONVERT_ASCII) << " ";
                    dist[destination].weight = dist[u].weight + vertexArray[u].edgeArray[j].weight;
                    parent[destination] = u;
                    makeheap(dist, numberOfVertices - 1);
            }
        }

    }

    cout << char(src + CONVERT_ASCII) << " ";
    printPath(parent, des);
    cout << "\nLength of shortest path: " << dist[des].weight << endl;
}

void makeheap(Edge heap[], int max) {
    for (int i = max / 2; i >= 1; i--) {
        siftdown(heap, i, max);
    }
    // for (int i = 1; i <= max; i++) {
    //     heap[i].display();
    // }
}

int getMinDist(Edge heap[], int i) {
    int des;
    if (i == 0) {
        des = heap[0].destination;
        // heap[0].weight = INT_MAX;
    } else {
        des = heap[1].destination;
        heap[1].weight = INT_MAX;
        siftdown(heap, 1, 20);
    }
    return des;
}

void swap(Edge *a, Edge *b) {
    Edge temp = *a;
    *a = *b;
    *b = temp;
}

void siftup(Edge heap[], int i) {
    if (i == 1) return;
    int position = i / 2;

    if (heap[position].weight > heap[i].weight) {
        swap(&heap[position], &heap[i]);
        siftup(heap, position);
    }
}

void siftdown(Edge heap[], int i, int max) {
    int position = i * 2;
    if (position < max && heap[position].weight > heap[position + 1].weight) position++;

    if (position <= max && heap[i].weight > heap[position].weight) {
        swap(&heap[position], &heap[i]);
        siftdown(heap, position, max);
    }
}

int minDistance(int dist[], bool sptSet[], int numberOfVertices) {
    // Initialize min value
    int min = INT_MAX, min_index;

    for (int i = 0; i < numberOfVertices; i++)
        if (sptSet[i] == false && dist[i] <= min)
            min = dist[i], min_index = i;

    return min_index;
}

void printPath(int parent[], int j) {
    nodeVisited++;
    // Base Case : If j is source
    if (parent[j] == -1)
        return;

    printPath(parent, parent[j]);

    cout << char(j + CONVERT_ASCII) << " ";
}
