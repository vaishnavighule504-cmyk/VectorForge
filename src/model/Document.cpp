#include "Document.h"
#include <algorithm>

namespace VectorForge {

Document::Document(QObject* parent)
    : QObject(parent), m_spatialIndex(QRectF(0, 0, m_width, m_height)) {
    addLayer("Layer 1");
}

void Document::setDimensions(double width, double height) {
    m_width = width;
    m_height = height;
    m_spatialIndexDirty = true;
    emit documentChanged();
}

std::shared_ptr<Layer> Document::activeLayer() const {
    if (m_layers.empty()) return nullptr;
    if (m_activeLayerIndex < m_layers.size()) {
        return m_layers[m_activeLayerIndex];
    }
    return m_layers.front();
}

void Document::setActiveLayerIndex(size_t index) {
    if (index < m_layers.size()) {
        m_activeLayerIndex = index;
        emit layerStructureChanged();
    }
}

std::shared_ptr<Layer> Document::addLayer(const std::string& name) {
    std::string layerName = name;
    if (layerName.empty()) {
        layerName = "Layer " + std::to_string(m_layers.size() + 1);
    }
    auto layer = std::make_shared<Layer>(layerName);
    m_layers.push_back(layer);
    m_activeLayerIndex = m_layers.size() - 1;
    m_spatialIndexDirty = true;
    emit layerStructureChanged();
    emit documentChanged();
    return layer;
}

bool Document::removeLayer(size_t index) {
    if (m_layers.size() <= 1 || index >= m_layers.size()) {
        return false;
    }
    m_layers.erase(m_layers.begin() + index);
    if (m_activeLayerIndex >= m_layers.size()) {
        m_activeLayerIndex = m_layers.size() - 1;
    }
    clearSelection();
    m_spatialIndexDirty = true;
    emit layerStructureChanged();
    emit documentChanged();
    return true;
}

bool Document::moveLayerUp(size_t index) {
    if (index > 0 && index < m_layers.size()) {
        std::swap(m_layers[index], m_layers[index - 1]);
        if (m_activeLayerIndex == index) m_activeLayerIndex--;
        else if (m_activeLayerIndex == index - 1) m_activeLayerIndex++;
        m_spatialIndexDirty = true;
        emit layerStructureChanged();
        emit documentChanged();
        return true;
    }
    return false;
}

bool Document::moveLayerDown(size_t index) {
    if (index + 1 < m_layers.size()) {
        std::swap(m_layers[index], m_layers[index + 1]);
        if (m_activeLayerIndex == index) m_activeLayerIndex++;
        else if (m_activeLayerIndex == index + 1) m_activeLayerIndex--;
        m_spatialIndexDirty = true;
        emit layerStructureChanged();
        emit documentChanged();
        return true;
    }
    return false;
}

std::vector<std::shared_ptr<Shape>> Document::allShapes() const {
    std::vector<std::shared_ptr<Shape>> result;
    for (const auto& layer : m_layers) {
        if (layer) {
            auto layerShapes = layer->allShapesRecursive();
            result.insert(result.end(), layerShapes.begin(), layerShapes.end());
        }
    }
    return result;
}

void Document::updateSpatialIndex() const {
    m_spatialIndex.rebuild(QRectF(0, 0, m_width, m_height), allShapes());
    m_spatialIndexDirty = false;
}

const Quadtree& Document::spatialIndex() const {
    if (m_spatialIndexDirty) {
        updateSpatialIndex();
    }
    return m_spatialIndex;
}

std::shared_ptr<Shape> Document::shapeAt(const QPointF& point) const {
    if (m_spatialIndexDirty) updateSpatialIndex();

    // Query spatial candidates from Quadtree in O(log N)
    auto candidates = m_spatialIndex.queryPoint(point);
    if (candidates.empty()) return nullptr;

    // Filter top-most shape based on Z-order (top-to-bottom across layers)
    for (auto layerIt = m_layers.rbegin(); layerIt != m_layers.rend(); ++layerIt) {
        const auto& layer = *layerIt;
        if (!layer || !layer->isEffectiveVisible() || layer->isEffectiveLocked()) continue;

        const auto& shapes = layer->shapes();
        for (auto shapeIt = shapes.rbegin(); shapeIt != shapes.rend(); ++shapeIt) {
            const auto& shape = *shapeIt;
            if (shape && shape->isVisible() && !shape->isLocked()) {
                if (std::find(candidates.begin(), candidates.end(), shape) != candidates.end()) {
                    if (shape->contains(point)) {
                        return shape;
                    }
                }
            }
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Shape>> Document::shapesInRect(const QRectF& rect) const {
    if (m_spatialIndexDirty) updateSpatialIndex();

    auto candidates = m_spatialIndex.queryRange(rect);
    std::vector<std::shared_ptr<Shape>> result;

    for (const auto& layer : m_layers) {
        if (!layer || !layer->isEffectiveVisible() || layer->isEffectiveLocked()) continue;

        for (const auto& shape : layer->shapes()) {
            if (shape && shape->isVisible() && !shape->isLocked()) {
                if (std::find(candidates.begin(), candidates.end(), shape) != candidates.end()) {
                    result.push_back(shape);
                }
            }
        }
    }
    return result;
}

std::vector<std::shared_ptr<Shape>> Document::shapesCollidingWith(const std::shared_ptr<Shape>& shape) const {
    if (m_spatialIndexDirty) updateSpatialIndex();
    return m_spatialIndex.queryCollisions(shape);
}

std::vector<std::pair<std::shared_ptr<Shape>, std::shared_ptr<Shape>>> Document::findAllCollisions() const {
    if (m_spatialIndexDirty) updateSpatialIndex();
    return m_spatialIndex.findAllCollisions();
}

void Document::setSelection(const std::vector<std::shared_ptr<Shape>>& selection) {
    m_selection = selection;
    emit selectionChanged();
}

void Document::selectShape(const std::shared_ptr<Shape>& shape, bool clearPrevious) {
    if (!shape) return;

    if (clearPrevious) {
        m_selection.clear();
    }
    if (!isSelected(shape)) {
        m_selection.push_back(shape);
        emit selectionChanged();
    }
}

void Document::deselectShape(const std::shared_ptr<Shape>& shape) {
    auto it = std::find(m_selection.begin(), m_selection.end(), shape);
    if (it != m_selection.end()) {
        m_selection.erase(it);
        emit selectionChanged();
    }
}

void Document::clearSelection() {
    if (!m_selection.empty()) {
        m_selection.clear();
        emit selectionChanged();
    }
}

bool Document::isSelected(const std::shared_ptr<Shape>& shape) const {
    return std::find(m_selection.begin(), m_selection.end(), shape) != m_selection.end();
}

std::shared_ptr<Layer> Document::layerOf(const std::shared_ptr<Shape>& shape) const {
    for (const auto& layer : m_layers) {
        if (layer) {
            const auto& shapes = layer->shapes();
            if (std::find(shapes.begin(), shapes.end(), shape) != shapes.end()) {
                return layer;
            }
        }
    }
    return nullptr;
}

QJsonObject Document::toJson() const {
    QJsonObject json;
    json["width"] = m_width;
    json["height"] = m_height;
    json["activeLayerIndex"] = static_cast<int>(m_activeLayerIndex);

    QJsonArray layersArray;
    for (const auto& layer : m_layers) {
        if (layer) {
            layersArray.append(layer->toJson());
        }
    }
    json["layers"] = layersArray;
    return json;
}

bool Document::fromJson(const QJsonObject& json) {
    m_width = json["width"].toDouble(1920.0);
    m_height = json["height"].toDouble(1080.0);
    m_activeLayerIndex = json["activeLayerIndex"].toInt(0);

    m_layers.clear();
    m_selection.clear();

    if (json.contains("layers")) {
        QJsonArray arr = json["layers"].toArray();
        for (const auto& val : arr) {
            auto layer = std::make_shared<Layer>();
            layer->fromJson(val.toObject());
            m_layers.push_back(layer);
        }
    }

    if (m_layers.empty()) {
        addLayer("Layer 1");
    }

    m_spatialIndexDirty = true;
    emit layerStructureChanged();
    emit selectionChanged();
    emit documentChanged();
    return true;
}

} // namespace VectorForge
