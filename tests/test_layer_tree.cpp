#include "catch.hpp"
#include "model/Layer.h"
#include "model/Circle.h"

using namespace VectorForge;

TEST_CASE("Hierarchical Layer Tree Structure & Inheritance", "[model][layer]") {
    auto root = std::make_shared<Layer>("Root Layer");
    auto child = std::make_shared<Layer>("Child Layer");
    root->addChild(child);

    REQUIRE(child->parent().lock() == root);
    REQUIRE(root->children().size() == 1);

    SECTION("Visibility and Lock Inheritance") {
        root->setVisible(false);
        REQUIRE_FALSE(root->isVisible());
        REQUIRE(child->isVisible()); // Direct visibility flag
        REQUIRE_FALSE(child->isEffectiveVisible()); // Inherited visibility is false

        root->setVisible(true);
        root->setLocked(true);
        REQUIRE(child->isEffectiveLocked()); // Inherited lock is true
    }

    SECTION("Shape Addition and Reordering") {
        auto circle1 = std::make_shared<Circle>(QPointF(10, 10), 5);
        auto circle2 = std::make_shared<Circle>(QPointF(20, 20), 10);

        child->addShape(circle1);
        child->addShape(circle2);

        REQUIRE(child->indexOfShape(circle1) == 0);
        REQUIRE(child->indexOfShape(circle2) == 1);

        child->moveShapeDown(0); // Move circle1 down
        REQUIRE(child->indexOfShape(circle1) == 1);
        REQUIRE(child->indexOfShape(circle2) == 0);
    }
}
