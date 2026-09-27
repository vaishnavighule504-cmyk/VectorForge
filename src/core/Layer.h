#ifndef VECTORFORGE_LAYER_H
#define VECTORFORGE_LAYER_H

#include "Shape.h"
#include <vector>
#include <memory>
#include <string>
#include <QJsonObject>
#include <QJsonArray>

namespace VectorForge {

class Layer {
public:
    Layer(const std::string& name = "Layer 1");
    ~Layer() = default;

    std::string name() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    bool isVisible() const { return m_isVisible; }
    void setVisible(bool visible) { m_isVisible = visible; }

    bool isLocked() const { return m_isLocked; }
    void setLocked(bool locked) { m_isLocked = locked; }

    const std::vector<std::shared_ptr<Shape>>& shapes() const { return m_shapes; }
    
    void addShape(const std::shared_ptr<Shape>& shape);
    void insertShape(size_t index, const std::shared_ptr<Shape>& shape);
    bool removeShape(const std::shared_ptr<Shape>& shape);
    void clear();

    int indexOf(const std::shared_ptr<Shape>& shape) const;
    
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    std::string m_name{"Layer 1"};
    bool m_isVisible{true};
    bool m_isLocked{false};
    std::vector<std::shared_ptr<Shape>> m_shapes;
};

} // namespace VectorForge

#endif // VECTORFORGE_LAYER_H
