#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../EulerTourForest.h"

TEST_CASE("Euler tour forest works fine on simple tests", "[etf]"){
    dgraph::EulerTourForest forest(5);

    SECTION("linking vertices"){
        forest.link(0, 1);
        forest.link(2, 3);
        REQUIRE(to_string(forest) == "010\n232\n4\n");

        SECTION("linking leaf vertices"){
            auto link = forest.link(1, 3);
            REQUIRE(to_string(forest) == "0132310\n4\n");
            forest.link(4, 3);
            REQUIRE(to_string(forest) == "423101324\n");

            SECTION("cutting and linking back"){
                forest.cut(link.first, link.second);
                REQUIRE(to_string(forest) == "101\n42324\n");
                forest.link(4, 1);
                REQUIRE(forest.is_connected(4, 0));
            }

            SECTION("checking iterator"){
                auto iterator = forest.iterator(1);
                std::set<int> vs;
                while(iterator.hasNext()){
                    vs.insert(*iterator);
                    ++iterator;
                }
                REQUIRE(vs.size() == 5);
            }
        }
    }
}
