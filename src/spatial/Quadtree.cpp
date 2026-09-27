#include "Quadtree.h"
#include <algorithm>
#include <unordered_set>

namespace VectorForge {

// ============================================================================
// QuadtreeNode Implementation
// ============================================================================

QuadtreeNode::QuadtreeNode(const QRectF& nodeBounds, int nodeDepth, int nodeMaxShapes, int nodeMaxDepth)
    : bounds(nodeBounds), depth(nodeDepth), maxShapes(nodeMaxShapes), maxDepth(nodeMaxDepth) {
}

void QuadtreeNode::subdivide() {
    double halfWidth = bounds.width() / 2.0;
    double halfHeight = bounds.height() / 2.0;
    double x = bounds.x();
    double y = bounds.y();

    // 0: Top-Left (NW)
    children[0] = std::make_unique<QuadtreeNode>(QRectF(x, y, halfWidth, halfHeight), depth + 1, maxShapes, maxDepth);
    // 1: Top-Right (NE)
    children[1] = std::make_unique<QuadtreeNode>(QRectF(x + halfWidth, y, halfWidth, halfHeight), depth + 1, maxShapes, maxDepth);
    // 2: Bottom-Left (SW)
    children[2] = std::make_unique<QuadtreeNode>(QRectF(x, y + halfHeight, halfWidth, halfHeight), depth + 1, maxShapes, maxDepth);
    // 3: Bottom-Right (SE)
    children[3] = std::make_unique<QuadtreeNode>(QRectF(x + halfWidth, y + halfHeight, halfWidth, halfHeight), depth + 1, maxShapes, maxDepth);
}

int QuadtreeNode::getQuadrant(const QRectF& shapeBounds) const {
    double midX = bounds.x() + bounds.width() / 2.0;
    double midY = bounds.y() + bounds.height() / 2.0;

    bool fitsTop = (shapeBounds.top() >= bounds.top() && shapeBounds.bottom() <= midY);
    bool fitsBottom = (shapeBounds.top() >= midY && shapeBounds.bottom() <= bounds.bottom());
    bool fitsLeft = (shapeBounds.left() >= bounds.left() && shapeBounds.right() <= midX);
    bool fitsRight = (shapeBounds.left() >= midX && shapeBounds.right() <= bounds.right());

    if (fitsTop) {
        if (fitsLeft) return 0;  // NW
        if (fitsRight) return 1; // NE
    } else if (fitsBottom) {
        if (fitsLeft) return 2;  // SW
        if (fitsRight) return 3; // SE
    }

    return -1; // Spans split line, stays in parent node
}

void QuadtreeNode::insert(const std::shared_ptr<Shape>& shape) {
    if (!shape) return;

    QRectF shapeBounds = shape->boundingRect();

    if (!isLeaf()) {
        int quad = getQuadrant(shapeBounds);
        if (quad != -1) {
            children[quad]->insert(shape);
            return;
        }
    }

    shapes.push_back(shape);

    if (isLeaf() && static_cast<int>(shapes.size()) > maxShapes && depth < maxDepth) {
        subdivide();

        auto oldShapes = std::move(shapes);
        shapes.clear();

        for (const auto& s : oldShapes) {
            int quad = getQuadrant(s->boundingRect());
            if (quad != -1) {
                children[quad]->insert(s);
            } else {
                shapes.push_back(s);
            }
        }
    }
}

void QuadtreeNode::queryPoint(const QPointF& point, std::vector<std::shared_ptr<Shape>>& outResult) const {
    if (!bounds.contains(point)) return;

    for (const auto& shape : shapes) {
        if (shape && shape->boundingRect().contains(point)) {
            outResult.push_back(shape);
        }
    }

    if (!isLeaf()) {
        int quad = getQuadrant(QRectF(point.x(), point.y(), 0.0001, 0.0001));
        if (quad != -1) {
            children[quad]->queryPoint(point, outResult);
        } else {
            for (const auto& child : children) {
                if (child) child->queryPoint(point, outResult);
            }
        }
    }
}

void QuadtreeNode::queryRange(const QRectF& range, std::vector<std::shared_ptr<Shape>>& outResult) const {
    if (!bounds.intersects(range)) return;

    for (const auto& shape : shapes) {
        if (shape) {
            QRectF sb = shape->boundingRect();
            if (range.intersects(sb) || range.contains(sb)) {
                outResult.push_back(shape);
            }
        }
    }

    if (!isLeaf()) {
        for (const auto& child : children) {
            if (child) child->queryRange(range, outResult);
        }
    }
}

void QuadtreeNode::queryCollisions(const std::shared_ptr<Shape>& targetShape, std::vector<std::shared_ptr<Shape>>& outResult) const {
    if (!targetShape) return;
    QRectF targetBounds = targetShape->boundingRect();

    if (!bounds.intersects(targetBounds)) return;

    for (const auto& shape : shapes) {
        if (shape && shape != targetShape) {
            QRectF sb = shape->boundingRect();
            if (targetBounds.intersects(sb)) {
                outResult.push_back(shape);
            }
        }
    }

    if (!isLeaf()) {
        for (const auto& child : children) {
            if (child) child->queryCollisions(targetShape, outResult);
        }
    }
}

void QuadtreeNode::drawDebug(QPainter& painter) const {
    QPen pen(QColor(0, 230, 255, std::max(40, 220 - depth * 25)), 1.0, Qt::DashLine);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(bounds);

    if (!isLeaf()) {
        for (const auto& child : children) {
            if (child) child->drawDebug(painter);
        }
    }
}

// ============================================================================
// Quadtree Manager Implementation
// ============================================================================

Quadtree::Quadtree(const QRectF& bounds, int maxShapes, int maxDepth)
    : m_bounds(bounds), m_maxShapes(maxShapes), m_maxDepth(maxDepth) {
    clear();
}

void Quadtree::setBounds(const QRectF& bounds) {
    m_bounds = bounds;
    clear();
}

void Quadtree::clear() {
    m_root = std::make_unique<QuadtreeNode>(m_bounds, 0, m_maxShapes, m_maxDepth);
}

void Quadtree::insert(const std::shared_ptr<Shape>& shape) {
    if (m_root && shape) {
        m_root->insert(shape);
    }
}

void Quadtree::rebuild(const QRectF& bounds, const std::vector<std::shared_ptr<Shape>>& shapes) {
    m_bounds = bounds;
    clear();
    for (const auto& shape : shapes) {
        if (shape && shape->isVisible()) {
            insert(shape);
        }
    }
}

std::vector<std::shared_ptr<Shape>> Quadtree::queryPoint(const QPointF& point) const {
    std::vector<std::shared_ptr<Shape>> candidates;
    if (m_root) {
        m_root->queryPoint(point, candidates);
    }
    return candidates;
}

std::vector<std::shared_ptr<Shape>> Quadtree::queryRange(const QRectF& range) const {
    std::vector<std::shared_ptr<Shape>> candidates;
    if (m_root) {
        m_root->queryRange(range, candidates);
    }
    return candidates;
}

std::vector<std::shared_ptr<Shape>> Quadtree::queryCollisions(const std::shared_ptr<Shape>& targetShape) const {
    std::vector<std::shared_ptr<Shape>> candidates;
    if (m_root && targetShape) {
        m_root->queryCollisions(targetShape, candidates);
    }
    return candidates;
}

std::vector<std::pair<std::shared_ptr<Shape>, std::shared_ptr<Shape>>> Quadtree::findAllCollisions() const {
    std::vector<std::pair<std::shared_ptr<Shape>, std::shared_ptr<Shape>>> collisions;
    if (!m_root) return collisions;

    // Collect all shapes across tree
    std::vector<std::shared_ptr<Shape>> allShapes;
    m_root->queryRange(m_bounds, allShapes);

    // Deduplicate shape pointers
    std::unordered_set<Shape*> seen;
    std::vector<std::shared_ptr<Shape>> uniqueShapes;
    for (const auto& s : allShapes) {
        if (s && seen.find(s.get()) == seen.end()) {
            seen.insert(s.get());
            uniqueShapes.push_back(s);
        }
    }

    // Broad phase spatial query for collision pairs
    for (size_t i = 0; i < uniqueShapes.size(); ++i) {
        const auto& s1 = uniqueShapes[i];
        auto nearby = queryCollisions(s1);
        for (const auto& s2 : nearby) {
            if (s1.get() < s2.get()) { // Ensure unique ordered pairs
                collisions.push_back({s1, s2});
            }
        }
    }

    return collisions;
}

void Quadtree::drawDebug(QPainter& painter) const {
    if (m_root) {
        m_root->drawDebug(painter);
    }
}

} // namespace VectorForge
