#include "catch.hpp"
#include "command/CommandStack.h"
#include "command/ShapeCommands.h"
#include "model/Document.h"
#include "model/Rectangle.h"

using namespace VectorForge;

TEST_CASE("CommandStack Undo and Redo Pipeline", "[command][undo]") {
    Document doc(800, 600);
    CommandStack cmdStack;

    REQUIRE_FALSE(cmdStack.canUndo());
    REQUIRE_FALSE(cmdStack.canRedo());

    auto rect = std::make_shared<Rectangle>(QRectF(100, 100, 50, 50));

    SECTION("Push and Execute AddShapeCommand") {
        cmdStack.pushAndExecute(std::make_unique<AddShapeCommand>(&doc, rect));

        REQUIRE(doc.activeLayer()->shapes().size() == 1);
        REQUIRE(cmdStack.canUndo());
        REQUIRE_FALSE(cmdStack.canRedo());
        REQUIRE(cmdStack.undoDescription() == "Add Rectangle");

        SECTION("Undo AddShapeCommand") {
            cmdStack.undo();
            REQUIRE(doc.activeLayer()->shapes().empty());
            REQUIRE_FALSE(cmdStack.canUndo());
            REQUIRE(cmdStack.canRedo());

            SECTION("Redo AddShapeCommand") {
                cmdStack.redo();
                REQUIRE(doc.activeLayer()->shapes().size() == 1);
                REQUIRE(cmdStack.canUndo());
                REQUIRE_FALSE(cmdStack.canRedo());
            }
        }
    }
}
