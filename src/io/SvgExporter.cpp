#include "SvgExporter.h"
#include <QSvgGenerator>
#include <QPainter>
#include <QFile>

namespace VectorForge {

bool SvgExporter::exportToSvg(const Document& doc, const QString& filePath) {
    QSvgGenerator generator;
    generator.setFileName(filePath);
    generator.setSize(QSize(static_cast<int>(doc.width()), static_cast<int>(doc.height())));
    generator.setViewBox(QRectF(0, 0, doc.width(), doc.height()));
    generator.setTitle("VectorForge Generative Artwork");
    generator.setDescription("Generated with VectorForge 2D Vector Design & Generative Art Studio");

    QPainter painter;
    if (!painter.begin(&generator)) {
        return false;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Draw dark canvas background
    painter.fillRect(QRectF(0, 0, doc.width(), doc.height()), QColor(25, 27, 34));

    // Render all shapes polymorphically loop over Shape* references
    for (const auto& layer : doc.layers()) {
        if (!layer || !layer->isVisible()) continue;

        for (const auto& shape : layer->shapes()) {
            if (shape && shape->isVisible()) {
                shape->draw(painter);
            }
        }
    }

    painter.end();
    return true;
}

} // namespace VectorForge
