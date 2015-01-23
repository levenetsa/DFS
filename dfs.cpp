#include "dfs.h"
#include <fstream>
#include <functional>
#include <vector>
#include <stack>
#include <string>
#include <iostream>

template <typename NodePayload>
Graph<NodePayload>::Graph() {
    vertexN = 0;
}

template <typename NodePayload>
Graph<NodePayload>::~Graph() {}

template <typename NodePayload>
void Graph<NodePayload>::saveToFile(const std::string &filename) {
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

template <typename NodePayload>
void Graph<NodePayload>::loadFromFile(const std::string &filename) {
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

template <typename NodePayload>
Graph<size_t>::NodeHandle Graph<NodePayload>::addNode() {
    vertexN++;
    payloads.push_back(emptyPayload);
    edges.push_back(std::vector<size_t>());
    return vertexN - 1;
}

template <typename NodePayload>
void Graph<NodePayload>::addEdge(const NodeHandle &a, const NodeHandle &b) {
    edges[a].push_back(b);
}

template<typename NodePayload>
NodePayload& Graph<NodePayload>::operator [](NodeHandle const & node) {
    return payloads[node];
}

template<typename NodePayload>
void Graph<NodePayload>::forEachNode(const std::function<void (const NodeHandle &)> &visitor) const {
    for (size_t i = 0; i < vertexN; i++) {
        visitor(i);
    }
}

template <typename NodePayload>
void Graph<NodePayload>::forEachEdge(NodeHandle const & source, EdgeVisitor const & visitor) {
    for (auto it = edges[source].begin(); it != edges[source].end(); it++) {
        visitor(std::make_pair(source, *it));
    }
}

template <typename NodePayload>
size_t Graph<NodePayload>::getNodesCount() const {
    return vertexN;
}

template <typename NodePayload>
void Graph<NodePayload>::dfs(NodeVisitor const & startNode, NodeVisitor const & endNode, NodeVisitor const & discoverNode) {
    std::vector<bool> used;
    used.resize(vertexN);
    for (int i = 0; i < used.size(); i++) {
        used[i] = false;
    }

    NodeHandle curr;
    for (NodeHandle i = 0; i < edges.size(); i++) {
        if (!used[i]) {
            std::stack<NodeHandle> st;
            st.push(i);
            while (!st.empty()) {
                curr = st.top();
                st.pop();
                startNode(curr);

                for (auto it = edges[curr].begin(); it != edges[curr].end(); it++) {
                    discoverNode(*it);
                    if (!used[*it]) {
                        used[*it] = true;
                        st.push(*it);
                    }
                }

                endNode(curr);
            }
        }
    }
}

template <typename NodePayload>
Graph<int>::NodeHandle Graph<NodePayload>::move(const NodeHandle &origin, const EdgeHandle &edge) {
    if (origin == edge.first)
        return edge.second;
    return edge.first;
}
