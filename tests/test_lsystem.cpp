#include "catch.hpp"
#include "generative/LSystemGenerator.h"

using namespace VectorForge;

TEST_CASE("L-System String Expansion Grammar", "[generative][lsystem]") {
    std::string axiom = "FX";
    std::map<char, std::string> rules;
    rules['X'] = "X+YF+";
    rules['Y'] = "-FX-Y";

    SECTION("0 Iterations") {
        std::string res = LSystemGenerator::expandLSystem(axiom, rules, 0);
        REQUIRE(res == "FX");
    }

    SECTION("1 Iteration") {
        std::string res = LSystemGenerator::expandLSystem(axiom, rules, 1);
        REQUIRE(res == "FX+YF+");
    }

    SECTION("2 Iterations") {
        std::string res = LSystemGenerator::expandLSystem(axiom, rules, 2);
        REQUIRE(res == "FX+YF++-FX-YF+");
    }

    SECTION("Turtle Graphics Polygon Generation") {
        ParameterMap params;
        params["preset"] = 0; // Dragon Curve
        params["depth"] = 2;
        params["stepLength"] = 10.0;
        params["angle"] = 90.0;
        params["symmetry"] = 1;
        params["strokeWidth"] = 1.0;
        params["startColor"] = QColor(255, 0, 0);
        params["endColor"] = QColor(0, 0, 255);

        LSystemGenerator generator;
        auto shapes = generator.generate(params, QPointF(100, 100));
        REQUIRE_FALSE(shapes.empty());
    }
}
