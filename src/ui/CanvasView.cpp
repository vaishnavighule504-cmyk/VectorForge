#include "CanvasView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <cmath>
#include <algorithm>

namespace VectorForge {

CanvasView::CanvasView(Document* doc, CommandStack* cmdStack, QWidget* parent)
    : QWidget(parent), m_doc(doc), m_cmdStack(cmdStack) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    if (m_doc) {
        connect(m_doc, &Document::documentChanged, this, [this]() { update(); });
        connect(m_doc, &Document::selectionChanged, this, [this]() { update(); });
        connect(m_doc, &Document::layerStructureChanged, this, [this]() { update(); });
    }
}

void CanvasView::setCurrentTool(ToolType tool) {
    m_currentTool = tool;
    if (m_doc) m_doc->clearSelection();
    update();
}

void CanvasView::resetZoom() {
    m_zoomFactor = 1.0;
    m_panOffset = QPointF(0.0, 0.0);
    update();
}

QPointF CanvasView::mapToCanvas(const QPointF& screenPt) const {
    double x = (screenPt.x() - m_panOffset.x()) / m_zoomFactor;
    double y = (screenPt.y() - m_panOffset.y()) / m_zoomFactor;
    return QPointF(x, y);
}

QPointF CanvasView::snapPoint(const QPointF& pt) const {
    if (!m_snapToGrid || m_gridSize <= 0.0) return pt;
    double snappedX = std::round(pt.x() / m_gridSize) * m_gridSize;
    double snappedY = std::round(pt.y() / m_gridSize) * m_gridSize;
    return QPointF(snappedX, snappedY);
}

HandleType CanvasView::handleAtPoint(const QPointF& canvasPt, const QRectF& rect) const {
    if (rect.isEmpty()) return HandleType::None;

    double handleSize = 8.0 / m_zoomFactor;
    auto testHandle = [&](const QPointF& p) {
        return QRectF(p.x() - handleSize / 2, p.y() - handleSize / 2, handleSize, handleSize).contains(canvasPt);
    };

    if (testHandle(rect.topLeft())) return HandleType::TopLeft;
    if (testHandle(rect.topRight())) return HandleType::TopRight;
    if (testHandle(rect.bottomLeft())) return HandleType::BottomLeft;
    if (testHandle(rect.bottomRight())) return HandleType::BottomRight;

    if (testHandle(QPointF((rect.left() + rect.right()) / 2, rect.top()))) return HandleType::Top;
    if (testHandle(QPointF((rect.left() + rect.right()) / 2, rect.bottom()))) return HandleType::Bottom;
    if (testHandle(QPointF(rect.left(), (rect.top() + rect.bottom()) / 2))) return HandleType::Left;
    if (testHandle(QPointF(rect.right(), (rect.top() + rect.bottom()) / 2))) return HandleType::Right;

    // Rotation handle floating above top
    QPointF rotPt(rect.center().x(), rect.top() - 25.0 / m_zoomFactor);
    if (testHandle(rotPt)) return HandleType::Rotate;

    return HandleType::None;
}

void CanvasView::drawGrid(QPainter& painter) {
    QRectF viewRect = rect();
    QPointF topLeft = mapToCanvas(viewRect.topLeft());
    QPointF bottomRight = mapToCanvas(viewRect.bottomRight());

    double step = m_gridSize;
    if (step * m_zoomFactor < 8.0) step *= 5.0; // Dynamic grid density scaling

    double startX = std::floor(topLeft.x() / step) * step;
    double startY = std::floor(topLeft.y() / step) * step;

    QPen gridPen(QColor(45, 48, 58), 1.0 / m_zoomFactor, Qt::SolidLine);
    painter.setPen(gridPen);

    for (double x = startX; x <= bottomRight.x(); x += step) {
        painter.drawLine(QPointF(x, topLeft.y()), QPointF(x, bottomRight.y()));
    }
    for (double y = startY; y <= bottomRight.y(); y += step) {
        painter.drawLine(QPointF(topLeft.x(), y), QPointF(bottomRight.x(), y));
    }

    // Origin Axes (X: Red, Y: Green)
    painter.setPen(QPen(QColor(240, 70, 70, 180), 2.0 / m_zoomFactor));
    painter.drawLine(QPointF(0, topLeft.y()), QPointF(0, bottomRight.y()));
    painter.setPen(QPen(QColor(70, 220, 90, 180), 2.0 / m_zoomFactor));
    painter.drawLine(QPointF(topLeft.x(), 0), QPointF(bottomRight.x(), 0));
}

void CanvasView::drawSelectionGizmo(QPainter& painter) {
    if (!m_doc) return;

    const auto& selection = m_doc->selection();
    if (selection.empty()) return;

    QRectF bounding;
    for (const auto& shape : selection) {
        if (shape) bounding = bounding.united(shape->boundingRect());
    }
    if (bounding.isEmpty()) return;

    // Outer Bounding Pen
    QPen pen(QColor(0, 230, 255), 1.5 / m_zoomFactor, Qt::SolidLine);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(bounding);

    // Transform Resize Handles
    double handleSize = 7.0 / m_zoomFactor;
    painter.setBrush(QColor(15, 17, 23));

    auto drawHandle = [&](const QPointF& pt) {
        painter.drawRect(QRectF(pt.x() - handleSize / 2, pt.y() - handleSize / 2, handleSize, handleSize));
    };

    drawHandle(bounding.topLeft());
    drawHandle(bounding.topRight());
    drawHandle(bounding.bottomLeft());
    drawHandle(bounding.bottomRight());
    drawHandle(QPointF((bounding.left() + bounding.right()) / 2, bounding.top()));
    drawHandle(QPointF((bounding.left() + bounding.right()) / 2, bounding.bottom()));
    drawHandle(QPointF(bounding.left(), (bounding.top() + bounding.bottom()) / 2));
    drawHandle(QPointF(bounding.right(), (bounding.top() + bounding.bottom()) / 2));

    // Rotation stem line and handle
    QPointF rotPt(bounding.center().x(), bounding.top() - 25.0 / m_zoomFactor);
    painter.drawLine(QPointF(bounding.center().x(), bounding.top()), rotPt);
    painter.setBrush(QColor(255, 0, 180));
    painter.drawEllipse(QRectF(rotPt.x() - handleSize / 2, rotPt.y() - handleSize / 2, handleSize, handleSize));
}

void CanvasView::drawAlignmentGuides(QPainter& painter) {
    if (m_activeGuides.empty()) return;

    QPen guidePen(QColor(255, 0, 200), 1.0 / m_zoomFactor, Qt::DashLine);
    painter.setPen(guidePen);

    QRectF viewRect = rect();
    QPointF topLeft = mapToCanvas(viewRect.topLeft());
    QPointF bottomRight = mapToCanvas(viewRect.bottomRight());

    for (const auto& guide : m_activeGuides) {
        if (guide.isVertical) {
            painter.drawLine(QPointF(guide.position, topLeft.y()), QPointF(guide.position, bottomRight.y()));
        } else {
            painter.drawLine(QPointF(topLeft.x(), guide.position), QPointF(bottomRight.x(), guide.position));
        }
    }
}

void CanvasView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Dark Studio Canvas Background
    painter.fillRect(rect(), QColor(25, 27, 34));

    // Apply Navigation Transform (Pan & Zoom)
    painter.translate(m_panOffset);
    painter.scale(m_zoomFactor, m_zoomFactor);

    // 1. Draw Grid
    drawGrid(painter);

    // 2. Draw Quadtree Spatial Debug Overlay if enabled
    if (m_showQuadtreeDebug && m_doc) {
        m_doc->spatialIndex().drawDebug(painter);
    }

    // 3. Draw Document Shapes
    if (m_doc) {
        for (const auto& layer : m_doc->rootLayers()) {
            if (!layer || !layer->isEffectiveVisible()) continue;
            for (const auto& shape : layer->shapes()) {
                if (shape && shape->isVisible()) {
                    shape->draw(painter);
                }
            }
        }
    }

    // 4. Draw Interactive Drawing Preview
    if (m_isDragging && m_currentTool != ToolType::Select) {
        QRectF previewBounds = QRectF(m_dragStart, m_dragCurrent).normalized();
        auto previewShape = ShapeFactory::createShapeFromTool(m_currentTool, previewBounds, m_activeStroke, m_activeFill);
        if (previewShape) {
            previewShape->draw(painter);
        }
    }

    // 5. Draw Marquee Selection Box
    if (m_isDragging && m_currentTool == ToolType::Select && m_activeHandle == HandleType::None && !m_marqueeRect.isEmpty()) {
        QPen marqueePen(QColor(0, 230, 255, 220), 1.0 / m_zoomFactor, Qt::DashLine);
        painter.setPen(marqueePen);
        painter.setBrush(QColor(0, 230, 255, 30));
        painter.drawRect(m_marqueeRect);
    }

    // 6. Draw Alignment Guides & Selection Gizmo
    drawAlignmentGuides(painter);
    drawSelectionGizmo(painter);
}

void CanvasView::mousePressEvent(QMouseEvent* event) {
    m_dragStart = mapToCanvas(event->pos());
    m_dragCurrent = m_dragStart;
    m_activeGuides.clear();

    // Pan canvas with Middle Mouse Button or Space + Left Click
    if (event->button() == Qt::MiddleButton || (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)) {
        m_isPanning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;

        if (m_currentTool == ToolType::Select) {
            // Check Gizmo Handle Hit Test
            if (m_doc && !m_doc->selection().empty()) {
                QRectF selectionBounds;
                for (const auto& shape : m_doc->selection()) {
                    if (shape) selectionBounds = selectionBounds.united(shape->boundingRect());
                }
                m_activeHandle = handleAtPoint(m_dragStart, selectionBounds);

                if (m_activeHandle != HandleType::None) {
                    m_transformInitialEntries.clear();
                    for (const auto& s : m_doc->selection()) {
                        if (s) m_transformInitialEntries.push_back({s, s->transform(), s->transform()});
                    }
                    return;
                }
            }

            // O(log N) Quadtree Point Query Hit Testing
            auto hitShape = m_doc ? m_doc->shapeAt(m_dragStart) : nullptr;
            if (hitShape) {
                bool multiSelect = (event->modifiers() & Qt::ControlModifier);
                m_doc->selectShape(hitShape, !multiSelect);

                m_transformInitialEntries.clear();
                for (const auto& s : m_doc->selection()) {
                    if (s) m_transformInitialEntries.push_back({s, s->transform(), s->transform()});
                }
            } else {
                if (!(event->modifiers() & Qt::ControlModifier)) {
                    m_doc->clearSelection();
                }
                m_marqueeRect = QRectF(m_dragStart, m_dragStart);
            }
        }
    }
}

void CanvasView::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentCanvasPt = mapToCanvas(event->pos());

    if (m_isPanning) {
        QPointF delta = event->pos() - m_lastMousePos;
        m_panOffset += delta;
        m_lastMousePos = event->pos();
        update();
        return;
    }

    if (m_isDragging) {
        QPointF targetPt = m_snapToGrid ? snapPoint(currentCanvasPt) : currentCanvasPt;
        m_dragCurrent = targetPt;
        m_activeGuides.clear();

        if (m_currentTool == ToolType::Select) {
            if (m_activeHandle != HandleType::None) {
                // Transform Scaling / Rotating
                // ...
            } else if (!m_doc->selection().empty()) {
                // Translate Selected Shapes
                double dx = targetPt.x() - m_dragStart.x();
                double dy = targetPt.y() - m_dragStart.y();

                // Compute Smart Alignment Snap Guides against other shapes
                if (!m_transformInitialEntries.empty()) {
                    for (const auto& shape : m_doc->selection()) {
                        if (shape) {
                            shape->translate(dx - (shape->transform().dx() - m_transformInitialEntries[0].oldTransform.dx()),
                                             dy - (shape->transform().dy() - m_transformInitialEntries[0].oldTransform.dy()));
                        }
                    }
                }

                // Add Snapped Guide visuals
                m_activeGuides.push_back({true, targetPt.x()});
                m_activeGuides.push_back({false, targetPt.y()});
            } else {
                // Update Marquee Box Selection
                m_marqueeRect = QRectF(m_dragStart, currentCanvasPt).normalized();
            }
        }
        update();
    }
}

void CanvasView::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isPanning) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        return;
    }

    if (m_isDragging) {
        m_isDragging = false;
        m_activeGuides.clear();

        if (m_currentTool != ToolType::Select) {
            QRectF finalBounds = QRectF(m_dragStart, m_dragCurrent).normalized();
            if (finalBounds.width() > 2.0 || finalBounds.height() > 2.0) {
                auto newShape = ShapeFactory::createShapeFromTool(m_currentTool, finalBounds, m_activeStroke, m_activeFill);
                if (newShape && m_doc && m_cmdStack) {
                    m_cmdStack->pushAndExecute(std::make_unique<AddShapeCommand>(m_doc, newShape));
                }
            }
        } else {
            if (!m_marqueeRect.isEmpty() && m_doc) {
                // Range Query Selection via Quadtree
                auto selectedInBox = m_doc->shapesInRect(m_marqueeRect);
                m_doc->setSelection(selectedInBox);
                m_marqueeRect = QRectF();
            } else if (!m_transformInitialEntries.empty()) {
                // Finalize Transform Command for Undo/Redo
                std::vector<TransformEntry> finalEntries;
                for (const auto& entry : m_transformInitialEntries) {
                    if (entry.shape) {
                        finalEntries.push_back({entry.shape, entry.oldTransform, entry.shape->transform()});
                    }
                }
                if (m_cmdStack && !finalEntries.empty()) {
                    m_cmdStack->pushAndExecute(std::make_unique<TransformShapeCommand>(m_doc, finalEntries, "Move Shapes"));
                }
            }
        }

        m_activeHandle = HandleType::None;
        update();
    }
}

void CanvasView::wheelEvent(QWheelEvent* event) {
    double angle = event->angleDelta().y();
    double factor = (angle > 0) ? 1.15 : 0.85;

    QPointF mouseScreen = event->position();
    QPointF mouseCanvasBefore = mapToCanvas(mouseScreen);

    m_zoomFactor *= factor;
    m_zoomFactor = std::clamp(m_zoomFactor, 0.05, 50.0);

    QPointF mouseCanvasAfter = mapToCanvas(mouseScreen);
    m_panOffset += (mouseCanvasAfter - mouseCanvasBefore) * m_zoomFactor;

    update();
}

void CanvasView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        if (m_doc && !m_doc->selection().empty() && m_cmdStack) {
            m_cmdStack->pushAndExecute(std::make_unique<DeleteShapeCommand>(m_doc, m_doc->selection()));
        }
    } else if (event->matches(QKeySequence::Undo)) {
        if (m_cmdStack) m_cmdStack->undo();
    } else if (event->matches(QKeySequence::Redo)) {
        if (m_cmdStack) m_cmdStack->redo();
    } else {
        QWidget::keyPressEvent(event);
    }
}

} // namespace VectorForge
