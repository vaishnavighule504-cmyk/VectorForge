#include "GroupShape.h"
#include <QJsonArray>
#include <algorithm>

namespace VectorForge {

GroupShape::GroupShape() {
    m_name = "Group";
}

GroupShape::GroupShape(const std::vector<std::shared_ptr<Shape>>& children)
    : m_children(children) {
    m_name = "Group";
}

void GroupShape::addChild(const std::shared_ptr<Shape>& child) {
    if (child) {
        m_children.push_back(child);
    }
}

void GroupShape::removeChild(const std::shared_ptr<Shape>& child) {
    m_children.erase(
        std::remove(m_children.begin(), m_children.end(), child),
        m_children.end()
    );
}

void GroupShape::clear() {
    m_children.clear();
}

void GroupShape::draw(QPainter& painter) const {
    if (!m_isVisible) return;

    painter.save();
    painter.setTransform(m_transform, true);

    for (const auto& child : m_children) {
        if (child) {
            child->draw(painter);
        }
    }

    painter.restore();
}

QPainterPath GroupShape::shapePath() const {
    QPainterPath compositePath;
    for (const auto& child : m_children) {
        if (child) {
            compositePath.addPath(child->shapePath());
        }
    }
    return m_transform.map(compositePath);
}

bool GroupShape::contains(const QPointF& point) const {
    if (!m_isVisible) return false;
    for (const auto& child : m_children) {
        if (child && child->contains(point)) {
            return true;
        }
    }
    return false;
}

QRectF GroupShape::boundingRect() const {
    if (m_children.empty()) return QRectF();
    QRectF rect;
    for (const auto& child : m_children) {
        if (child) {
            rect = rect.united(child->boundingRect());
        }
    }
    return m_transform.mapRect(rect);
}

void GroupShape::translate(double dx, double dy) {
    m_transform.translate(dx, dy);
}

void GroupShape::scale(double sx, double sy, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.scale(sx, sy);
    m_transform.translate(-origin.x(), -origin.y());
}

void GroupShape::rotate(double angleDegrees, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.rotate(angleDegrees);
    m_transform.translate(-origin.x(), -origin.y());
}

std::shared_ptr<Shape> GroupShape::clone() const {
    auto groupCopy = std::make_shared<GroupShape>();
    groupCopy->m_name = m_name;
    groupCopy->m_transform = m_transform;
    groupCopy->m_isVisible = m_isVisible;
    groupCopy->m_isLocked = m_isLocked;
    groupCopy->m_opacity = m_opacity;

    for (const auto& child : m_children) {
        if (child) {
            groupCopy->addChild(child->clone());
        }
    }
    return groupCopy;
}

QJsonObject GroupShape::toJson() const {
    QJsonObject json = Shape::toJson();
    QJsonArray childrenArray;
    for (const auto& child : m_children) {
        if (child) {
            childrenArray.append(child->toJson());
        }
    }
    json["children"] = childrenArray;
    return json;
}

void GroupShape::fromJson(const QJsonObject& json) {
    Shape::fromJson(json);
    m_children.clear();
}

} // namespace VectorForge
