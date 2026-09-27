#ifndef VECTORFORGE_SHAPE_H
#define VECTORFORGE_SHAPE_H

#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QPointF>
#include <QColor>
#include <QTransform>
#include <QJsonObject>
#include <memory>
#include <string>
#include <vector>

namespace VectorForge {

enum class ShapeType {
    Circle,
    Rectangle,
    Polygon,
    BezierCurve,
    Group
};

class Shape : public std::enable_shared_from_this<Shape> {
public:
    Shape();
    virtual ~Shape() = default;

    // Polymorphic Pure Virtual Core Interface
    virtual void draw(QPainter& painter) const = 0;
    virtual bool contains(const QPointF& point) const = 0;
    virtual QRectF boundingRect() const = 0;
    virtual QPainterPath shapePath() const = 0;

    // Transformations (Move / Scale / Rotate)
    virtual void translate(double dx, double dy) = 0;
    virtual void scale(double sx, double sy, const QPointF& origin) = 0;
    virtual void rotate(double angleDegrees, const QPointF& origin) = 0;

    // Cloning & Metadata
    virtual std::shared_ptr<Shape> clone() const = 0;
    virtual ShapeType type() const = 0;
    virtual std::string typeName() const = 0;
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& json);

    // Style Accessors & Mutators
    QColor fillColor() const { return m_fillColor; }
    void setFillColor(const QColor& color) { m_fillColor = color; }

    QColor strokeColor() const { return m_strokeColor; }
    void setStrokeColor(const QColor& color) { m_strokeColor = color; }

    double strokeWidth() const { return m_strokeWidth; }
    void setStrokeWidth(double width) { m_strokeWidth = width; }

    Qt::PenStyle strokeStyle() const { return m_strokeStyle; }
    void setStrokeStyle(Qt::PenStyle style) { m_strokeStyle = style; }

    double opacity() const { return m_opacity; }
    void setOpacity(double opacity) { m_opacity = opacity; }

    bool isVisible() const { return m_isVisible; }
    void setVisible(bool visible) { m_isVisible = visible; }

    bool isLocked() const { return m_isLocked; }
    void setLocked(bool locked) { m_isLocked = locked; }

    std::string id() const { return m_id; }
    void setId(const std::string& id) { m_id = id; }

    std::string name() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    // Transform Matrix
    QTransform transform() const { return m_transform; }
    void setTransform(const QTransform& transform) { m_transform = transform; }

protected:
    void setupPainterStyle(QPainter& painter) const;

    QColor m_fillColor{QColor(100, 150, 240, 100)}; // Default semi-transparent fill
    QColor m_strokeColor{QColor(240, 240, 240)};    // Default bright stroke
    double m_strokeWidth{2.0};
    Qt::PenStyle m_strokeStyle{Qt::SolidLine};
    double m_opacity{1.0};
    bool m_isVisible{true};
    bool m_isLocked{false};
    std::string m_id;
    std::string m_name{"Shape"};
    QTransform m_transform;
};

} // namespace VectorForge

#endif // VECTORFORGE_SHAPE_H
