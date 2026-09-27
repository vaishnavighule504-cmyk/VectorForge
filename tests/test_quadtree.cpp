#include "catch.hpp"
#include "spatial/Quadtree.h"
#include "model/Circle.h"
#include "model/Rectangle.h"

using namespace VectorForge;

TEST_CASE("Quadtree Spatial Index Insertion & Query", "[spatial][quadtree]") {
    Quadtree tree(QRectF(0, 0, 1000, 1000), 4, 6);

    auto rect1 = std::make_shared<Rectangle>(QRectF(50, 50, 40, 40));
    auto rect2 = std::make_shared<Rectangle>(QRectF(800, 800, 50, 50));
    auto circle1 = std::make_shared<Circle>(QPointF(60, 60), 20);

    REQUIRE(tree.insert(rect1));
    REQUIRE(tree.insert(rect2));
    REQUIRE(tree.insert(circle1));
    REQUIRE(tree.totalShapesCount() == 3);

    SECTION("Point Hit Test Query") {
        auto hits = tree.queryPoint(QPointF(60, 60));
        REQUIRE(hits.size() >= 2); // Should hit both rect1 and circle1

        auto emptyHits = tree.queryPoint(QPointF(500, 500));
        REQUIRE(emptyHits.empty());
    }

    SECTION("Range Box Query") {
        auto boxHits = tree.queryRange(QRectF(0, 0, 100, 100));
        REQUIRE(boxHits.size() == 2); // rect1 and circle1

        auto farHits = tree.queryRange(QRectF(700, 700, 250, 250));
        REQUIRE(farHits.size() == 1);
        REQUIRE(farHits.front() == rect2);
    }

    SECTION("Clear and Rebuilding") {
        tree.clear();
        REQUIRE(tree.totalShapesCount() == 0);
        REQUIRE(tree.queryPoint(QPointF(60, 60)).empty());
    }
}
