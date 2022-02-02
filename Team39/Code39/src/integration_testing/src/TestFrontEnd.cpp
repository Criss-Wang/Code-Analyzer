#include "Utility/TNode.h"
#include "catch.hpp"
using namespace std;
void RequirePkb(bool b) {
    REQUIRE(b);
}

TEST_CASE("1st Test") {

    TNode T;
    RequirePkb(1 == 1);
}

