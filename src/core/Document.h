#ifndef VECTORFORGE_DOCUMENT_H
#define VECTORFORGE_DOCUMENT_H

#include "Layer.h"
#include <QObject>
#include <vector>
#include <memory>

namespace VectorForge {

class Document : public QObject {
    Q_OBJECT
public:
    explicit Document(QObject* parent = nullptr);
    ~Document() override = default;

    double width() const { return m_width; }
    double height() const { return m_height; }
    void setDimensions(double width, double height);

    // Layer Management
    const std::vector<std::shared_ptr<Layer>>& layers() const { return m_layers; }
    std::shared_ptr<Layer> activeLayer() const;
    size_t activeLayerIndex() const { return m_activeLayerIndex; }
    void setActiveLayerIndex(size_t index);

    std::shared_ptr<Layer> addLayer(const std::string& name = "");
    bool removeLayer(size_t index);

    // All shapes in document across layers (top-to-bottom or bottom-to-top)
    std::vector<std::shared_ptr<Shape>> allShapes() const;

    // Hit Testing (Top-most visible and unlocked shape at point)
    std::shared_ptr<Shape> shapeAt(const QPointF& point) const;
    std::vector<std::shared_ptr<Shape>> shapesInRect(const QRectF& rect) const;

    // Selection Model
    const std::vector<std::shared_ptr<Shape>>& selection() const { return m_selection; }
    void setSelection(const std::vector<std::shared_ptr<Shape>>& selection);
    void selectShape(const std::shared_ptr<Shape>& shape, bool clearPrevious = true);
    void deselectShape(const std::shared_ptr<Shape>& shape);
    void clearSelection();
    bool isSelected(const std::shared_ptr<Shape>& shape) const;

    // Find layer containing shape
    std::shared_ptr<Layer> layerOf(const std::shared_ptr<Shape>& shape) const;

    // Document JSON Serialization
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject& json);

signals:
    void documentChanged();
    void selectionChanged();
    void layerStructureChanged();

private:
    double m_width{1920.0};
    double m_height{1080.0};
    std::vector<std::shared_ptr<Layer>> m_layers;
    size_t m_activeLayerIndex{0};
    std::vector<std::shared_ptr<Shape>> m_selection;
};

} // namespace VectorForge

#endif // VECTORFORGE_DOCUMENT_H
