#ifndef VECTORFORGE_SHAPECOMMANDS_H
#define VECTORFORGE_SHAPECOMMANDS_H

#include "Command.h"
#include "../model/Document.h"
#include "../model/GroupShape.h"
#include <memory>
#include <vector>

namespace VectorForge {

// 1. Add Shape Command
class AddShapeCommand : public Command {
public:
    AddShapeCommand(Document* doc, const std::shared_ptr<Shape>& shape, std::shared_ptr<Layer> layer = nullptr);
    void execute() override;
    void undo() override;
    std::string description() const override { return "Add " + (m_shape ? m_shape->name() : "Shape"); }

private:
    Document* m_doc;
    std::shared_ptr<Shape> m_shape;
    std::shared_ptr<Layer> m_layer;
    int m_insertedIndex{-1};
};

// 2. Delete Shape Command
struct DeletedShapeEntry {
    std::shared_ptr<Shape> shape;
    std::shared_ptr<Layer> layer;
    int index;
};

class DeleteShapeCommand : public Command {
public:
    DeleteShapeCommand(Document* doc, const std::vector<std::shared_ptr<Shape>>& shapes);
    void execute() override;
    void undo() override;
    std::string description() const override { return "Delete Shapes"; }

private:
    Document* m_doc;
    std::vector<DeletedShapeEntry> m_entries;
};

// 3. Transform Shape Command (Move, Scale, Rotate)
struct TransformEntry {
    std::shared_ptr<Shape> shape;
    QTransform oldTransform;
    QTransform newTransform;
};

class TransformShapeCommand : public Command {
public:
    TransformShapeCommand(Document* doc, const std::vector<TransformEntry>& entries, const std::string& actionName = "Transform Shapes");
    void execute() override;
    void undo() override;
    std::string description() const override { return m_actionName; }

private:
    Document* m_doc;
    std::vector<TransformEntry> m_entries;
    std::string m_actionName;
};

// 4. Style Change Command
struct StyleState {
    QColor fillColor;
    QColor strokeColor;
    double strokeWidth;
    Qt::PenStyle strokeStyle;
    double opacity;
};

struct StyleEntry {
    std::shared_ptr<Shape> shape;
    StyleState oldStyle;
    StyleState newStyle;
};

class StyleChangeCommand : public Command {
public:
    StyleChangeCommand(Document* doc, const std::vector<StyleEntry>& entries, const std::string& name = "Change Style");
    void execute() override;
    void undo() override;
    std::string description() const override { return m_name; }

private:
    Document* m_doc;
    std::vector<StyleEntry> m_entries;
    std::string m_name;
};

// 5. Group Shapes Command
class GroupShapesCommand : public Command {
public:
    GroupShapesCommand(Document* doc, const std::vector<std::shared_ptr<Shape>>& shapesToGroup);
    void execute() override;
    void undo() override;
    std::string description() const override { return "Group Shapes"; }

private:
    Document* m_doc;
    std::vector<DeletedShapeEntry> m_childEntries;
    std::shared_ptr<GroupShape> m_groupShape;
    std::shared_ptr<Layer> m_targetLayer;
};

// 6. Ungroup Shapes Command
class UngroupShapesCommand : public Command {
public:
    UngroupShapesCommand(Document* doc, const std::shared_ptr<GroupShape>& groupShape);
    void execute() override;
    void undo() override;
    std::string description() const override { return "Ungroup Shapes"; }

private:
    Document* m_doc;
    std::shared_ptr<GroupShape> m_groupShape;
    std::shared_ptr<Layer> m_layer;
    int m_groupIndex{-1};
    std::vector<std::shared_ptr<Shape>> m_children;
};

} // namespace VectorForge

#endif // VECTORFORGE_SHAPECOMMANDS_H
