#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <stack>

template<typename NodePayload>
class Graph {
public:
    typedef size_t NodeHandle;
    typedef std::pair<size_t, size_t> EdgeHandle;
    typedef std::function<void(NodeHandle const &)> NodeVisitor;
    typedef std::function<void(EdgeHandle const &)> EdgeVisitor;
public:
    // create empty graph
    Graph() {
        vertexN = 0;
    }

    // destroy
    virtual ~Graph() {}

    // load graph from filename path
    void loadFromFile(const std::string &filename) {
        std::ifstream fin(filename);
        fin >> vertexN;
        payloads.resize(vertexN);
        for (int i = 0; i < vertexN; i++) {
            fin >> payloads[i];
        }
        edges.resize(vertexN);
        int n;
        for (int i = 0; i < vertexN; i++) {
            fin >> n;
            edges[i].resize(n);
            for (int j = 0; j < n; j++) {
                fin >> edges[i][j];
            }
        }
    }

    // save graph to filename file
    void saveToFile(const std::string &filename) {
        std::ofstream fout(filename);
        fout << vertexN << std::endl;
        for (auto it = payloads.begin(); it != payloads.end(); it++) {
            fout << *it << " ";
        }
        fout << std::endl;
        for (auto it1 = edges.begin(); it1 != edges.end(); it1++) {
            fout << (*it1).size() << " ";
            for (auto it2 = (*it1).begin(); it2 != (*it1).end(); it2++) {
                fout << *it2 << " ";
            }
            fout << std::endl;
        }
    }

    // add node with no edges and default payload
    NodeHandle addNode() {
        vertexN++;
        payloads.push_back(emptyPayload);
        edges.push_back(std::vector<size_t>());
        return vertexN - 1;
    }

    // connect a and b nodes with edge
    void addEdge(const NodeHandle &a, const NodeHandle &b) {
        edges[a].push_back(b);
    }

    // get payload from node
    NodePayload& operator [](NodeHandle const & node) {
        return payloads[node];
    }

    // run visitior function for each node
    void forEachNode(const std::function<void (const NodeHandle &)> &visitor) const {
        for (size_t i = 0; i < vertexN; i++) {
            visitor(i);
        }
    }

    // run visitor function for each edge connectned with source node
    void forEachEdge(NodeHandle const & source, EdgeVisitor const & visitor) {
        for (auto it = edges[source].begin(); it != edges[source].end(); it++) {
            visitor(std::make_pair(source, *it));
        }
    }

    // get number of nodes
    size_t getNodesCount() const {
        return vertexN;
    }

    // run dfs
    // call startNode function for each node before iterating through its neighborhood
    // call endNode fucntion for each node when we're done with it
    // call discoverNode function for each node when it was discovered as adjacent to visited
    void dfs(NodeVisitor const & startNode, NodeVisitor const & endNode, NodeVisitor const & discoverNode) {
        std::vector<bool> used;
        used.resize(vertexN);
        for (int i = 0; i < used.size(); i++) {
            used[i] = false;
        }

        std::stack<NodeHandle> end;
        NodeHandle curr;
        for (NodeHandle i = 0; i < edges.size(); i++) {
            if (!used[i]) {
                std::stack< std::pair< NodeHandle, std::stack<NodeHandle> > > st;
                end.push(i);
                st.push(make_pair(i, end));
                while (!st.empty()) {
                    curr = st.top().first;
                    end = st.top().second;
                    st.pop();
                    startNode(curr);

/*                    std::stack<NodeHandle> copy = end;
                    std::cout << curr << "\n";
                    while (!copy.empty()) {
                        std::cout << copy.top() << " ";
                        copy.pop();
                    }
                    std::cout << "\n";*/

                    bool discovered = false;
                    for (auto it = edges[curr].begin(); it != edges[curr].end(); it++) {
                        discoverNode(*it);
                        if (!used[*it]) {
                            used[*it] = true;
                            // this call will be executed last
                            // have to end this node after executing
                            if (!discovered) {
                                end.push(*it);
                                st.push(make_pair(*it, end));
                                discovered = true;
                            }
                            else {
                                end = std::stack<NodeHandle>();
                                end.push(*it);
                                st.push(make_pair(*it, end));
                            }
                        }
                    }
//                    std::cout << discovered << "\n\n";


                    if (!discovered) {
                        while(!end.empty()) {
                            endNode(end.top());
                            end.pop();
                        }
                    }
                }
            }
        }
    }

    // returns second node of edge with first origin node
    NodeHandle move(const NodeHandle &origin, const EdgeHandle &edge) {
        if (origin == edge.first)
            return edge.second;
        return edge.first;
    }

private:
    std::vector<NodePayload> payloads;
    std::vector< std::vector<size_t> > edges;
    size_t vertexN;
    NodePayload emptyPayload;
};
