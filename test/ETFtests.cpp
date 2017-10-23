#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../EulerTourForest.h"

TEST_CASE("Euler tour forest works fine on simple tests", "[etf]"){
    dgraph::EulerTourForest forest(5);

    SECTION("linking vertices"){
        forest.link(0, 1);
        forest.link(2, 3);
        REQUIRE(forest.str() == "01\n23\n4\n");

        SECTION("linking leaf vertices"){
            dgraph::TreeEdge link = forest.link(1, 3);
            REQUIRE(forest.str() == "101323\n4\n");
            forest.link(4, 3);
            REQUIRE(forest.str() == "43231013\n");

            SECTION("cutting and linking back"){
                forest.cut(std::move(link));
                REQUIRE(forest.str() == "10\n4323\n");
                forest.link(4, 1);
                REQUIRE(forest.is_connected(4, 0));
            }

            SECTION("checking iterator"){
                forest.change_edges(2, 1);
                forest.change_edges(4, 1);
                forest.change_edges(2, 0);
                auto iterator = forest.iterator(1);
                std::set<int> vs;
                while(iterator.hasNext()){
                    vs.insert(*iterator);
                    ++iterator;
                }
                REQUIRE(vs.size() == 1);
            }
        }
    }
}
