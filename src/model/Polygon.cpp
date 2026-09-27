#include "Polygon.h"
#include <QJsonArray>
#include <cmath>

namespace VectorForge {

Polygon::Polygon() {
    m_name = "Polygon";
}

Polygon::Polygon(const QPolygonF& points, bool isClosed)
    : m_points(points), m_isClosed(isClosed) {
    m_name = "Polygon";
}

Polygon::Polygon(const std::vector<QPointF>& points, bool isClosed)
    : m_isClosed(isClosed) {
    m_name = "Polygon";
    for (const auto& pt : points) {
        m_points.append(pt);
    }
}

void Polygon::draw(QPainter& painter) const {
    if (!m_isVisible || m_points.isEmpty()) return;

    painter.save();
    painter.setTransform(m_transform, true);
    setupPainterStyle(painter);

    if (m_isClosed) {
        painter.drawPolygon(m_points);
    } else {
        painter.drawPolyline(m_points);
    }

    painter.restore();
}

QPainterPath Polygon::shapePath() const {
    QPainterPath path;
    if (m_points.isEmpty()) return path;

    path.moveTo(m_points.first());
    for (int i = 1; i < m_points.size(); ++i) {
        path.lineTo(m_points[i]);
    }
    if (m_isClosed && m_points.size() > 2) {
        path.closeSubpath();
    }
    return m_transform.map(path);
}

bool Polygon::contains(const QPointF& point) const {
    if (!m_isVisible || m_points.isEmpty()) return false;
    
    QPainterPath path = shapePath();
    if (!m_isClosed) {
        // For unclosed polylines, test proximity to stroke
        QPainterPathStroker stroker;
        stroker.setWidth(m_strokeWidth > 0 ? m_strokeWidth + 4.0 : 6.0);
        return stroker.createStroke(path).contains(point);
    }
    return path.contains(point);
}

QRectF Polygon::boundingRect() const {
    return shapePath().boundingRect();
}

void Polygon::translate(double dx, double dy) {
    m_transform.translate(dx, dy);
}

void Polygon::scale(double sx, double sy, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.scale(sx, sy);
    m_transform.translate(-origin.x(), -origin.y());
}

void Polygon::rotate(double angleDegrees, const QPointF& origin) {
    m_transform.translate(origin.x(), origin.y());
    m_transform.rotate(angleDegrees);
    m_transform.translate(-origin.x(), -origin.y());
}

std::shared_ptr<Shape> Polygon::clone() const {
    return std::make_shared<Polygon>(*this);
}

std::shared_ptr<Polygon> Polygon::createRegularPolygon(const QPointF& center, double radius, int sides) {
    if (sides < 3) sides = 3;
    QPolygonF pts;
    double angleStep = (2.0 * M_PI) / sides;
    for (int i = 0; i < sides; ++i) {
        double angle = i * angleStep - M_PI / 2.0; // Start at top
        pts.append(QPointF(center.x() + radius * std::cos(angle),
                           center.y() + radius * std::sin(angle)));
    }
    auto poly = std::make_shared<Polygon>(pts, true);
    poly->setName("Regular Polygon");
    return poly;
}

std::shared_ptr<Polygon> Polygon::createStar(const QPointF& center, double outerRadius, double innerRadius, int points) {
    if (points < 3) points = 3;
    QPolygonF pts;
    double angleStep = M_PI / points;
    for (int i = 0; i < points * 2; ++i) {
        double r = (i % 2 == 0) ? outerRadius : innerRadius;
        double angle = i * angleStep - M_PI / 2.0;
        pts.append(QPointF(center.x() + r * std::cos(angle),
                           center.y() + r * std::sin(angle)));
    }
    auto star = std::make_shared<Polygon>(pts, true);
    star->setName("Star");
    return star;
}

QJsonObject Polygon::toJson() const {
    QJsonObject json = Shape::toJson();
    json["isClosed"] = m_isClosed;
    
    QJsonArray ptsArray;
    for (const auto& pt : m_points) {
        QJsonObject ptObj;
        ptObj["x"] = pt.x();
        ptObj["y"] = pt.y();
        ptsArray.append(ptObj);
    }
    json["points"] = ptsArray;
    return json;
}

void Polygon::fromJson(const QJsonObject& json) {
    Shape::fromJson(json);
    m_isClosed = json["isClosed"].toBool(true);
    m_points.clear();

    if (json.contains("points")) {
        QJsonArray ptsArray = json["points"].toArray();
        for (const auto& val : ptsArray) {
            QJsonObject ptObj = val.toObject();
            m_points.append(QPointF(ptObj["x"].toDouble(), ptObj["y"].toDouble()));
        }
    }
}

} // namespace VectorForge
