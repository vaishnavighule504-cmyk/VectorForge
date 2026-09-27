#include "catch.hpp"
#include "persistence/LightweightJson.h"

using namespace VectorForge;

TEST_CASE("Hand-Crafted Lightweight JSON Lexer & Parser", "[persistence][json]") {
    std::string sampleJson = R"({
        "name": "VectorForge Project",
        "version": 1.5,
        "active": true,
        "layers": [
            { "name": "Layer 1", "visible": true },
            { "name": "Layer 2", "visible": false }
        ]
    })";

    JsonLexer lexer(sampleJson);
    auto tokens = lexer.tokenize();
    REQUIRE_FALSE(tokens.empty());

    JsonParser parser(tokens);
    JsonValue ast = parser.parse();

    REQUIRE(ast.isObject());
    REQUIRE(ast["name"].asString() == "VectorForge Project");
    REQUIRE(ast["version"].asNumber() == 1.5);
    REQUIRE(ast["active"].asBool() == true);
    REQUIRE(ast["layers"].isArray());
    REQUIRE(ast["layers"].asArray().size() == 2);
    REQUIRE(ast["layers"].asArray()[0]["name"].asString() == "Layer 1");
    REQUIRE(ast["layers"].asArray()[1]["visible"].asBool() == false);

    SECTION("Round-trip Serialization") {
        std::string serialized = ast.serialize();
        JsonLexer lexer2(serialized);
        JsonParser parser2(lexer2.tokenize());
        JsonValue ast2 = parser2.parse();

        REQUIRE(ast2["name"].asString() == "VectorForge Project");
        REQUIRE(ast2["layers"].asArray().size() == 2);
    }
}
