#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../EulerTourForest.h"

TEST_CASE("Euler tour forest works fine on simple tests", "[etf]"){
    dgraph::EulerTourForest forest(5);

    SECTION("linking vertices"){
        forest.link(0, 1);
        forest.link(2, 3);
        REQUIRE (forest.is_connected(0, 1));
        REQUIRE (forest.is_connected(2, 3));
        REQUIRE (!forest.is_connected(1, 3));
    }

    SECTION("linking leaf vertices"){
        forest.link(1, 3);
        forest.link(4, 2);
        REQUIRE(forest.is_connected(0, 1));
    }

    SECTION("cutting and linking back"){
        forest.cut(1);
        REQUIRE(!forest.is_connected(1, 2));
        forest.link(4, 1);
        REQUIRE(forest.is_connected(4, 0));
    }

    SECTION("checking iterator"){
        auto iterator = forest.iterator(1);
        std::set<int> vs;
        while(iterator.hasNext()){
            vs.insert(*iterator);
        }
        REQUIRE(vs.size() == 5);
    }
}
