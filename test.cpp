#include "gtest/gtest.h"
#include "dfs.h"
#include <string>
#include <stdlib.h>
#include <time.h>

namespace {
    const size_t fullSize = 1000, N = 100;

    Graph<int> fullGraph() {
        Graph<int> g;

        for (size_t i = 0; i < fullSize; i++) {
            g[g.addNode()] = i;
        }
        for (size_t i = 0; i < fullSize; i++) {
            for (size_t j = i + 1; j < fullSize; j++) {
                g.addEdge(i, j);
            }
        }

        return g;
    }

    Graph<std::string> chainGraph() {
        Graph<std::string> g;

        for (size_t i = 0; i < fullSize; i++) {
            g[g.addNode()] = std::to_string(i);
        }
        for (size_t i = 0; i < fullSize - 1; i++) {
            g.addEdge(i, i + 1);
        }

        return g;
    }

    size_t cnt = 0, cnt1 = 0;
    Graph <std::string> chain;
    void chainVisitor(Graph<int>::EdgeHandle const & x) {
        chain.forEachEdge(chain.move(x.first, x), chainVisitor);
        cnt++;
    }

    void chainNodeVisitor(Graph<std::string>::NodeHandle const & x) {
        EXPECT_EQ(std::to_string(x), chain[x]);
        cnt++;
    }

    bool used[N];
    Graph <int> disChain;
    Graph <int> disconnectedChain() {
        srand (time(NULL));
        cnt = 0;

        Graph<int> g;

        for (size_t i = 0; i < N; i++) {
            g[g.addNode()] = i;
        }
        for (size_t i = 0; i < N - 1; i++) {
            if (rand() % 10 > 2) {
                g.addEdge(i, i + 1);
            }
            else {
                cnt++;
            }
        }

        return g;
    }

    void startNode(Graph<int>::NodeHandle const & node) {
        cnt1 += !used[node];
        used[node] = true;
    }

    void discoverNode(Graph<int>::NodeHandle const & node) {
        used[node] = true;
    }

    void doNothing(Graph<int>::NodeHandle const & ignored) {}

    int correctOrder[] = {3, 2, 1, 0};
    int order = 0;
    void checkOrder(Graph<int>::NodeHandle const & justEnded) {
        EXPECT_EQ(correctOrder[order], justEnded);
        order++;
    }
}

TEST(correctness, fullGraph) {
    Graph<int> g = fullGraph();
    for (size_t i = 0; i < fullSize; i++) {
        EXPECT_EQ(g[i], i);
    }
}

TEST(correctness, saveLoadFull) {
    Graph<int> g1 = fullGraph();
    Graph<int> g2;
    g1.saveToFile("test.out");
    g2.loadFromFile("test.out");
    for (int i = 0; i < fullSize; i++) {
        EXPECT_EQ(g1[i], g2[i]);
    }
}

TEST(correctness, saveLoadChainVisitor) {
    Graph<std::string> g = chainGraph();
    g.saveToFile("test1.out");
    chain.loadFromFile("test1.out");
    chain.forEachNode(chainNodeVisitor);
    EXPECT_EQ(cnt, fullSize);
}

TEST(correctness, visitorAndMove) {
    chain = chainGraph();
    chain.forEachEdge(0, chainVisitor);
    EXPECT_EQ(cnt, fullSize * 2 - 1);
}

TEST(correctness, connectedComponentsN) {
    disChain = disconnectedChain();
    for (int i = 0; i < N; i++) {
        used[i] = false;
    }
    disChain.dfs(startNode, doNothing, discoverNode);
    EXPECT_EQ(cnt, cnt1 - 1);
}

TEST(correctness, endTest) {
    Graph<int> g;
    Graph<int>::NodeHandle n1 = g.addNode();
    Graph<int>::NodeHandle n2 = g.addNode();
    Graph<int>::NodeHandle n3 = g.addNode();
    Graph<int>::NodeHandle n4 = g.addNode();
    g.addEdge(n1, n2);
    g.addEdge(n2, n3);
    g.addEdge(n2, n4);
    g.dfs(doNothing, checkOrder, doNothing);
}
