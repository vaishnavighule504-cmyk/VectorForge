#ifndef VECTORFORGE_LAYER_H
#define VECTORFORGE_LAYER_H

#include "Shape.h"
#include <vector>
#include <memory>
#include <string>
#include <QJsonObject>
#include <QJsonArray>

namespace VectorForge {

/**
 * @brief Hierarchical Layer Tree node.
 * 
 * Each Layer can contain shapes and sub-layers, supporting reordering,
 * recursive show/hide, recursive lock/unlock, and nested layer operations.
 */
class Layer : public std::enable_shared_from_this<Layer> {
public:
    explicit Layer(const std::string& name = "Layer 1");
    ~Layer() = default;

    std::string name() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    std::string id() const { return m_id; }
    void setId(const std::string& id) { m_id = id; }

    // Direct and Recursive Visibility
    bool isVisible() const { return m_isVisible; }
    void setVisible(bool visible) { m_isVisible = visible; }
    bool isEffectiveVisible() const;

    // Direct and Recursive Lock
    bool isLocked() const { return m_isLocked; }
    void setLocked(bool locked) { m_isLocked = locked; }
    bool isEffectiveLocked() const;

    // Parent/Child Hierarchy Access
    std::shared_ptr<Layer> parent() const { return m_parent.lock(); }
    void setParent(const std::shared_ptr<Layer>& parentLayer) { m_parent = parentLayer; }

    const std::vector<std::shared_ptr<Layer>>& children() const { return m_children; }
    
    std::shared_ptr<Layer> createSubLayer(const std::string& name = "");
    void addChildLayer(const std::shared_ptr<Layer>& childLayer);
    bool removeChildLayer(const std::shared_ptr<Layer>& childLayer);
    
    // Layer Reordering
    bool moveChildUp(size_t index);
    bool moveChildDown(size_t index);
    bool setChildIndex(size_t currentIndex, size_t newIndex);

    // Shape Management
    const std::vector<std::shared_ptr<Shape>>& shapes() const { return m_shapes; }
    void addShape(const std::shared_ptr<Shape>& shape);
    void insertShape(size_t index, const std::shared_ptr<Shape>& shape);
    bool removeShape(const std::shared_ptr<Shape>& shape);
    void clearShapes();

    int indexOfShape(const std::shared_ptr<Shape>& shape) const;
    std::vector<std::shared_ptr<Shape>> allShapesRecursive() const;

    // JSON Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    std::string m_id;
    std::string m_name{"Layer 1"};
    bool m_isVisible{true};
    bool m_isLocked{false};

    std::weak_ptr<Layer> m_parent;
    std::vector<std::shared_ptr<Layer>> m_children;
    std::vector<std::shared_ptr<Shape>> m_shapes;
};

} // namespace VectorForge

#endif // VECTORFORGE_LAYER_H
