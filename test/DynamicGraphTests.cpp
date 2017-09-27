#include "catch.hpp"

#include "../DynamicGraph.h"

TEST_CASE("dynamic graphs work fine on simple tests", "[dg]"){
    SECTION("simple triangle test") {
        dgraph::DynamicGraph graph(3);
        REQUIRE(!graph.is_connected(0, 1));
        REQUIRE(!graph.is_connected(1, 2));
        REQUIRE(!graph.is_connected(0, 2));
        auto token = graph.add(0, 1);
        REQUIRE(graph.is_connected(0, 1));
        REQUIRE(!graph.is_connected(0, 2));
        graph.add(1, 2);
        graph.add(0, 2);
        REQUIRE(graph.is_connected(0, 1));
        REQUIRE(graph.is_connected(1, 2));
        REQUIRE(graph.is_connected(0, 2));
        graph.remove(std::move(token));
        REQUIRE(graph.is_connected(0, 1));
        REQUIRE(graph.is_connected(1, 2));
        REQUIRE(graph.is_connected(0, 2));
    }
}
