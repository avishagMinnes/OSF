#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstring>
#include <algorithm>  // Include algorithm for remove_if
#include <queue>
#include <memory>
#include <arpa/inet.h>
#include <unistd.h>
#include "MSTFactory.cpp"  // Include the MST Factory for Boruvka/Prim algorithms

#define PORT 8080
#define NUM_THREADS 4

using namespace std;
using namespace std::chrono;

// Task class for representing work that needs to be done by a thread in the thread pool
class Task {
public:
    virtual void execute() = 0;
    virtual ~Task() {}
};

// Graph class representing an undirected graph
class Graph {
public:
    Graph(int n) : n(n) {
        graph.resize(n + 1);  // 1-based indexing
    }

    void addEdge(int u, int v, double weight) {
        graph[u].push_back({v, weight});
        graph[v].push_back({u, weight}); // Undirected graph
        edges.push_back({u, {v, weight}});
    }

    void removeEdge(int u, int v) {
        graph[u].remove_if([v](const pair<int, double>& edge) { return edge.first == v; });
        graph[v].remove_if([u](const pair<int, double>& edge) { return edge.first == u; });
        edges.erase(remove_if(edges.begin(), edges.end(), [&](const pair<int, pair<int, double>>& edge) {
            return (edge.first == u && edge.second.first == v) || (edge.first == v && edge.second.first == u);
        }), edges.end());
    }

    vector<pair<int, pair<int, double>>> getEdges() const {
        return edges;
    }

    int getNumVertices() const {
        return n;
    }

private:
    int n;
    vector<list<pair<int, double>>> graph;
    vector<pair<int, pair<int, double>>> edges; // To store edges for MST
};

// Helper function to convert MST result to string
string convertMSTToString(const list<pair<int, int>>& mstEdges) {
    string result;
    for (const auto& edge : mstEdges) {
        result += to_string(edge.first) + " - " + to_string(edge.second) + "\n";
    }
    return result;
}

// Task class that handles a specific client request (graph operations)
class GraphTask : public Task {
public:
    GraphTask(int clientSocket, Graph* graph, string command) 
        : clientSocket(clientSocket), graph(graph), command(command) {}

    void execute() override {
        // Stage 1: Parse the command
        if (command.find("Newgraph") == 0) {
            createGraph();
        } else if (command.find("Boruvka") == 0 || command.find("Prim") == 0) {
            calculateMST();
        } else if (command.find("Newedge") == 0) {
            addEdge();
        } else if (command.find("Removeedge") == 0) {
            removeEdge();
        }

        close(clientSocket);  // Close connection after task is completed
    }

private:
    int clientSocket;
    Graph* graph;
    string command;

    void createGraph() {
        // Parse and create the graph
        int n, m;
        sscanf(command.c_str(), "Newgraph %d,%d", &n, &m);
        delete graph;
        graph = new Graph(n);
        send(clientSocket, "Graph created\n", strlen("Graph created\n"), 0);
    }

    void calculateMST() {
        // Calculate the MST using Boruvka or Prim
        string algorithmType = (command.find("Boruvka") == 0) ? "Boruvka" : "Prim";
        unique_ptr<IMSTSolver> solver = MSTFactory::createSolver(algorithmType);
        vector<pair<int, pair<int, double>>> edges = graph->getEdges();

        auto start = high_resolution_clock::now();
        list<pair<int, int>> mstEdges = solver->solve(graph->getNumVertices(), edges);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start).count();

        string result = "MST:\n" + convertMSTToString(mstEdges);
        result += "Time taken: " + to_string(duration) + " ms\n";
        send(clientSocket, result.c_str(), result.size(), 0);
    }

    void addEdge() {
        // Add a new edge
        int u, v;
        double weight;
        sscanf(command.c_str(), "Newedge %d,%d,%lf", &u, &v, &weight);
        graph->addEdge(u, v, weight);
        send(clientSocket, "Edge added\n", strlen("Edge added\n"), 0);
    }

    void removeEdge() {
        // Remove an edge
        int u, v;
        sscanf(command.c_str(), "Removeedge %d,%d", &u, &v);
        graph->removeEdge(u, v);
        send(clientSocket, "Edge removed\n", strlen("Edge removed\n"), 0);
    }
};

// Thread pool class (Leader-Follower)
class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    Task* task;

                    {   // Lock the queue and wait for a task
                        unique_lock<mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

                        if (this->stop && this->tasks.empty())
                            return;

                        task = this->tasks.front();
                        this->tasks.pop();
                    }

                    // Execute the task
                    task->execute();
                    delete task;  // Clean up after task execution
                }
            });
        }
    }

    // Add a new task to the thread pool
    void enqueue(Task* task) {
        {
            unique_lock<mutex> lock(queueMutex);
            tasks.push(task);
        }
        condition.notify_one();  // Notify a worker thread
    }

    // Destructor to join all threads
    ~ThreadPool() {
        {
            unique_lock<mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (thread &worker : workers)
            worker.join();
    }

private:
    vector<thread> workers;
    queue<Task*> tasks;

    mutex queueMutex;
    condition_variable condition;
    bool stop;
};

// Server function to handle incoming connections and create tasks
void serverThread(ThreadPool &pool, Graph* &g) {
    int server_fd, newSocket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming client connections
    while (true) {
        cout << "Waiting for a connection..." << endl;
        if ((newSocket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        cout << "Connection accepted" << endl;

        // Read command from client
        char buffer[1024] = {0};
        read(newSocket, buffer, 1024);
        string command(buffer);

        // Create a task for the incoming request
        Task* task = new GraphTask(newSocket, g, command);

        // Enqueue the task into the thread pool
        pool.enqueue(task);
    }

    close(server_fd);
}

int main() {
    ThreadPool pool(NUM_THREADS);
    Graph* g = nullptr;

    // Launch the server on a separate thread
    thread server(serverThread, ref(pool), ref(g));

    // Wait for the server thread to finish
    server.join();

    delete g;
    return 0;
}


