# 🎨 VectorForge — 2D Vector Design & Generative Art Studio

**VectorForge** is a high-performance desktop application built in modern **C++ (C++17)** using **Qt6** for graphics rendering and UI components. It merges a production-grade 2D vector drawing & CAD tool with a real-time parametric generative art engine, custom spatial partitioning index, and hand-crafted JSON persistence layer.

Designed as an architecture-first portfolio showcase demonstrating clean software design patterns, custom data structures (Quadtree), and modern C++ fundamentals.

---

## 🌟 Key Features

1. **Interactive 2D Vector Drawing**:
   - Create, select, translate, scale, rotate, and style vector primitive shapes.
   - **Supported Primitive & Composite Shapes**: `Circle`, `Rectangle` (with rounded corner support), `Polygon` (regular N-gons, stars), `BezierCurve` (cubic curve control points), `GroupShape` (hierarchical shape tree containers).
   - **Styling**: Custom stroke color, fill color, stroke width, pen dash styles, opacity.
   - **Canvas Navigation**: Infinite pan (`Middle Click` or `Shift + Left Drag`), smooth zoom (`Ctrl + Mouse Wheel`).
   - **Interactive Selection Gizmo**: 8 transform resize handles and rotation control stem.

2. **$O(\log N)$ Spatial Indexing**:
   - Custom **Quadtree** spatial partitioning structure for accelerated $O(\log N)$ point hit testing and rectangular marquee selection queries.
   - Live visual spatial index debugger.

3. **Parametric Generative Art Studio**:
   - **L-System Fractals**: Recursive string grammar expansion with Turtle graphics (Dragon Curve, Koch Snowflake, Sierpinski Arrowhead, Fern Tree, Penrose Mandala).
   - **Spirograph (Hypotrochoid)**: Parametric mathematical epitrochoid and hypotrochoid gear curves.
   - **Fractal Branching Tree**: Recursive geometric branching with decay & spread angle.
   - **Rhodonea Rose Curves**: Mathematical rose petals ($r = a \cdot \cos(k \cdot \theta)$).
   - **Perlin Noise Flow Field**: Smooth streamline vector field integration.
   - **Sacred Geometry Mandala**: Concentric ring guides with rotational $N$-fold symmetry petals.
   - Generative outputs inject directly into editable vector layers for post-generation styling and export.

4. **Hierarchical Layer Management**:
   - Tree-structured layer system with root and child nested layers.
   - Per-layer visibility (`👁`), locking (`🔒`), reordering (`⬆`/`⬇`), and inherited parent transform properties.

5. **Command Pattern Undo / Redo Engine**:
   - Dual-stack operation history (`Ctrl + Z` to Undo, `Ctrl + Y` to Redo).

6. **Persistence & Export**:
   - Direct export to scalable vector graphics (**SVG**).
   - Hand-crafted lightweight JSON parser/lexer (`.vfg` project files) without third-party JSON library overhead.

---

## 📐 Software Architecture & Design Patterns

- **Polymorphism & Shape Hierarchy**:
  An abstract base class `VectorForge::Shape` defines virtual interfaces (`draw()`, `contains()`, `boundingRect()`, `translate()`, `scale()`, `rotate()`, `clone()`, `toJson()`). All canvas operations operate on `std::shared_ptr<Shape>` polymorphic containers.
- **Command Pattern**:
  Actions (Add, Delete, Transform, Style Change, Group, Ungroup, Layer operations) encapsulate behavior into `Command` instances. Managed by a `CommandStack` with configurable max history.
- **Strategy Pattern**:
  `IGenerativeAlgorithm` defines an interchangeable strategy interface for generative engines.
- **Composite Pattern**:
  `GroupShape` encapsulates multiple `Shape` objects into unified container sub-trees.
- **Spatial Indexing Pattern**:
  `Quadtree` partitions 2D canvas space into recursive bounding regions for accelerated point and range queries.
- **Factory Pattern**:
  `ShapeFactory` decouples UI tool drag gestures from concrete shape instantiations.

---

## 🛠️ Building & Running VectorForge

### Prerequisites
- **CMake** (v3.16 or newer)
- **C++17 Compiler** (GCC 9+, Clang 10+, or MSVC 2019+)
- **Qt6** or **Qt5** (Widgets, Svg, Core, Gui components)

---

### Option A: Windows (MSYS2 UCRT64 / MinGW)

1. Install dependencies in **MSYS2 UCRT64**:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-qt6-svg mingw-w64-ucrt-x86_64-ninja
   ```

2. Build and Run:
   ```bash
   cmake -B build -G "Ninja"
   cmake --build build
   ./build/VectorForge.exe
   ```

3. Run Unit Tests:
   ```bash
   ./build/VectorForgeTests.exe
   ```

---

### Option B: Windows (Visual Studio 2022)

1. Open **x64 Native Tools Command Prompt for VS 2022**:
   ```cmd
   cd path\to\VectorForge
   cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.6.0\msvc2019_64"
   cmake --build build --config Release
   .\build\Release\VectorForge.exe
   ```

---

### Option C: Linux (Ubuntu / Debian)

1. Install dependencies:
   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake qt6-base-dev qt6-svg-dev
   ```

2. Build and Run:
   ```bash
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ./VectorForge
   ```

3. Run Unit Tests:
   ```bash
   ./VectorForgeTests
   ```

---

### Option D: macOS (Homebrew)

1. Install dependencies:
   ```bash
   brew install cmake qt
   ```

2. Build and Run:
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_PREFIX_PATH=$(brew --prefix qt) ..
   make -j$(sysctl -n hw.logicalcpu)
   ./VectorForge.app/Contents/MacOS/VectorForge
   ```

---

## ⌨️ Shortcuts & Controls

| Shortcut | Description |
|---|---|
| **Left Click + Drag** | Draw active shape / Select & transform shapes |
| **Middle Click Drag** | Pan canvas |
| **Shift + Left Drag** | Pan canvas |
| **Ctrl + Mouse Wheel** | Zoom in / Zoom out |
| **Delete / Backspace** | Delete selected shapes |
| **Ctrl + Z** | Undo |
| **Ctrl + Y** | Redo |
| **Ctrl + G** | Group selected shapes |
| **Ctrl + Shift + G** | Ungroup selected shape |
| **Ctrl + E** | Export artwork to SVG |

---

## 📄 License
Distributed under the MIT License.
