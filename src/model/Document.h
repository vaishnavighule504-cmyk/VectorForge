#ifndef VECTORFORGE_DOCUMENT_H
#define VECTORFORGE_DOCUMENT_H

#include "Layer.h"
#include "../spatial/Quadtree.h"
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

    // Root Layer Hierarchy
    const std::vector<std::shared_ptr<Layer>>& rootLayers() const { return m_layers; }
    std::shared_ptr<Layer> activeLayer() const;
    size_t activeLayerIndex() const { return m_activeLayerIndex; }
    void setActiveLayerIndex(size_t index);

    std::shared_ptr<Layer> addLayer(const std::string& name = "");
    bool removeLayer(size_t index);
    bool moveLayerUp(size_t index);
    bool moveLayerDown(size_t index);

    // Shapes across all layers
    std::vector<std::shared_ptr<Shape>> allShapes() const;

    // Spatial Index Quadtree integration
    void updateSpatialIndex() const;
    const Quadtree& spatialIndex() const;

    // Hit Testing & Spatial Queries using Quadtree
    std::shared_ptr<Shape> shapeAt(const QPointF& point) const;
    std::vector<std::shared_ptr<Shape>> shapesInRect(const QRectF& rect) const;
    std::vector<std::shared_ptr<Shape>> shapesCollidingWith(const std::shared_ptr<Shape>& shape) const;
    std::vector<std::pair<std::shared_ptr<Shape>, std::shared_ptr<Shape>>> findAllCollisions() const;

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

    mutable Quadtree m_spatialIndex;
    mutable bool m_spatialIndexDirty{true};
};

} // namespace VectorForge

#endif // VECTORFORGE_DOCUMENT_H
