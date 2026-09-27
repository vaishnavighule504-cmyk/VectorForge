#ifndef VECTORFORGE_CANVASVIEW_H
#define VECTORFORGE_CANVASVIEW_H

#include <QWidget>
#include "../model/Document.h"
#include "../model/ShapeFactory.h"
#include "../command/CommandStack.h"
#include "../command/ShapeCommands.h"

namespace VectorForge {

enum class HandleType {
    None,
    TopLeft, Top, TopRight,
    Right, BottomRight, Bottom, BottomLeft, Left,
    Rotate
};

struct AlignmentGuide {
    bool isVertical; // True: X line, False: Y line
    double position;
};

class CanvasView : public QWidget {
    Q_OBJECT
public:
    explicit CanvasView(Document* doc, CommandStack* cmdStack, QWidget* parent = nullptr);
    ~CanvasView() override = default;

    ToolType currentTool() const { return m_currentTool; }
    void setCurrentTool(ToolType tool);

    QColor activeStrokeColor() const { return m_activeStroke; }
    void setActiveStrokeColor(const QColor& color) { m_activeStroke = color; }

    QColor activeFillColor() const { return m_activeFill; }
    void setActiveFillColor(const QColor& color) { m_activeFill = color; }

    double zoomFactor() const { return m_zoomFactor; }
    void resetZoom();

    bool snapToGrid() const { return m_snapToGrid; }
    void setSnapToGrid(bool snap) { m_snapToGrid = snap; update(); }

    bool showQuadtreeDebug() const { return m_showQuadtreeDebug; }
    void setShowQuadtreeDebug(bool show) { m_showQuadtreeDebug = show; update(); }

    double gridSize() const { return m_gridSize; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void drawGrid(QPainter& painter);
    void drawSelectionGizmo(QPainter& painter);
    void drawAlignmentGuides(QPainter& painter);
    QPointF mapToCanvas(const QPointF& screenPt) const;
    QPointF snapPoint(const QPointF& pt) const;
    HandleType handleAtPoint(const QPointF& canvasPt, const QRectF& selectionRect) const;

    Document* m_doc;
    CommandStack* m_cmdStack;

    ToolType m_currentTool{ToolType::Select};
    QColor m_activeStroke{QColor(0, 230, 255)};
    QColor m_activeFill{QColor(100, 150, 240, 100)};

    // Navigation state (Pan & Zoom)
    double m_zoomFactor{1.0};
    QPointF m_panOffset{0.0, 0.0};
    bool m_isPanning{false};
    QPointF m_lastMousePos;

    // Grid & Guide options
    bool m_snapToGrid{true};
    double m_gridSize{20.0};
    bool m_showQuadtreeDebug{false};
    std::vector<AlignmentGuide> m_activeGuides;

    // Interaction state
    bool m_isDragging{false};
    QPointF m_dragStart;
    QPointF m_dragCurrent;

    // Marquee Selection Box
    QRectF m_marqueeRect;

    // Gizmo Transformation state
    HandleType m_activeHandle{HandleType::None};
    std::vector<TransformEntry> m_transformInitialEntries;
};

} // namespace VectorForge

#endif // VECTORFORGE_CANVASVIEW_H
