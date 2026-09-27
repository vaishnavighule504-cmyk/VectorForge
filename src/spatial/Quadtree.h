#ifndef VECTORFORGE_QUADTREE_H
#define VECTORFORGE_QUADTREE_H

#include "../model/Shape.h"
#include <QRectF>
#include <QPointF>
#include <QPainter>
#include <vector>
#include <memory>
#include <array>
#include <utility>

namespace VectorForge {

/**
 * @brief Node within the Quadtree spatial partitioning structure.
 * 
 * Each node represents a 2D rectangular region (bounds). If shape density exceeds
 * maxShapes and depth is below maxDepth, the node subdivides into 4 equal child quadrants:
 * 0: Top-Left (NW), 1: Top-Right (NE), 2: Bottom-Left (SW), 3: Bottom-Right (SE).
 */
struct QuadtreeNode {
    QRectF bounds;
    int depth{0};
    int maxShapes{8};
    int maxDepth{8};

    std::vector<std::shared_ptr<Shape>> shapes;
    std::array<std::unique_ptr<QuadtreeNode>, 4> children;

    explicit QuadtreeNode(const QRectF& nodeBounds, int nodeDepth = 0, int nodeMaxShapes = 8, int nodeMaxDepth = 8);
    ~QuadtreeNode() = default;

    // Disable copy, allow move
    QuadtreeNode(const QuadtreeNode&) = delete;
    QuadtreeNode& operator=(const QuadtreeNode&) = delete;
    QuadtreeNode(QuadtreeNode&&) noexcept = default;
    QuadtreeNode& operator=(QuadtreeNode&&) noexcept = default;

    bool isLeaf() const { return children[0] == nullptr; }
    void subdivide();
    int getQuadrant(const QRectF& shapeBounds) const;

    void insert(const std::shared_ptr<Shape>& shape);
    void queryPoint(const QPointF& point, std::vector<std::shared_ptr<Shape>>& outResult) const;
    void queryRange(const QRectF& range, std::vector<std::shared_ptr<Shape>>& outResult) const;
    void queryCollisions(const std::shared_ptr<Shape>& targetShape, std::vector<std::shared_ptr<Shape>>& outResult) const;
    void drawDebug(QPainter& painter) const;
};

/**
 * @brief Spatial index manager using a 2D Quadtree.
 * 
 * Provides O(log N) average time complexity for spatial point queries (hit testing),
 * range queries (marquee box selection), and O(N log N) collision/overlap detection.
 */
class Quadtree {
public:
    explicit Quadtree(const QRectF& bounds = QRectF(0, 0, 1920, 1080), int maxShapes = 8, int maxDepth = 8);
    ~Quadtree() = default;

    void setBounds(const QRectF& bounds);
    QRectF bounds() const { return m_bounds; }

    void clear();
    void insert(const std::shared_ptr<Shape>& shape);
    void rebuild(const QRectF& bounds, const std::vector<std::shared_ptr<Shape>>& shapes);

    std::vector<std::shared_ptr<Shape>> queryPoint(const QPointF& point) const;
    std::vector<std::shared_ptr<Shape>> queryRange(const QRectF& range) const;
    std::vector<std::shared_ptr<Shape>> queryCollisions(const std::shared_ptr<Shape>& targetShape) const;
    std::vector<std::pair<std::shared_ptr<Shape>, std::shared_ptr<Shape>>> findAllCollisions() const;

    void drawDebug(QPainter& painter) const;

private:
    QRectF m_bounds;
    int m_maxShapes{8};
    int m_maxDepth{8};
    std::unique_ptr<QuadtreeNode> m_root;
};

} // namespace VectorForge

#endif // VECTORFORGE_QUADTREE_H
