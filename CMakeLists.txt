cmake_minimum_required(VERSION 3.16)
project(VectorForge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg Core Gui)

set(QT_LIBRARIES Qt6::Widgets Qt6::Svg Qt6::Core Qt6::Gui)

# Core Library Sources
set(ENGINE_SOURCES
    src/model/Shape.cpp
    src/model/Circle.cpp
    src/model/Rectangle.cpp
    src/model/Polygon.cpp
    src/model/BezierCurve.cpp
    src/model/GroupShape.cpp
    src/model/ShapeFactory.cpp
    src/model/Layer.cpp
    src/model/Document.cpp
    src/spatial/Quadtree.cpp
    src/command/CommandStack.cpp
    src/command/ShapeCommands.cpp
    src/command/LayerCommands.cpp
    src/utils/PerlinNoise.cpp
    src/generative/LSystemGenerator.cpp
    src/generative/SpirographGenerator.cpp
    src/generative/FractalTreeGenerator.cpp
    src/generative/RoseCurveGenerator.cpp
    src/generative/FlowFieldGenerator.cpp
    src/generative/MandalaGenerator.cpp
    src/generative/GenerativeEngine.cpp
    src/persistence/LightweightJson.cpp
    src/persistence/ProjectSerializer.cpp
    src/persistence/SvgExporter.cpp
)

set(UI_SOURCES
    src/ui/CanvasView.cpp
    src/ui/PropertyInspector.cpp
    src/ui/GenerativeDock.cpp
    src/ui/LayerPanel.cpp
    src/ui/MainWindow.cpp
)

# Application Executable
add_executable(VectorForge src/main.cpp ${ENGINE_SOURCES} ${UI_SOURCES})
target_link_libraries(VectorForge PRIVATE ${QT_LIBRARIES})
target_include_directories(VectorForge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Unit Tests Executable
add_executable(VectorForgeTests
    tests/main.cpp
    tests/test_quadtree.cpp
    tests/test_command_stack.cpp
    tests/test_layer_tree.cpp
    tests/test_lsystem.cpp
    tests/test_json.cpp
    ${ENGINE_SOURCES}
)
target_link_libraries(VectorForgeTests PRIVATE ${QT_LIBRARIES})
target_include_directories(VectorForgeTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/tests)

if (MSVC)
    target_compile_options(VectorForge PRIVATE /W4 /MP)
    target_compile_options(VectorForgeTests PRIVATE /W4 /MP)
else()
    target_compile_options(VectorForge PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(VectorForgeTests PRIVATE -Wall -Wextra -Wpedantic)
endif()
