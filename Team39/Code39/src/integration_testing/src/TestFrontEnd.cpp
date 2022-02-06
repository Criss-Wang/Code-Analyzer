#include "catch.hpp"
using namespace std;
void RequirePkb(bool b) {
    REQUIRE(b);
}

TEST_CASE("1st Test") {


    RequirePkb(1 == 1);
}

