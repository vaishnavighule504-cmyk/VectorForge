#ifndef VECTORFORGE_SVGEXPORTER_H
#define VECTORFORGE_SVGEXPORTER_H

#include "../model/Document.h"
#include <QString>

namespace VectorForge {

class SvgExporter {
public:
    static bool exportToSvg(const Document& doc, const QString& filePath);
};

} // namespace VectorForge

#endif // VECTORFORGE_SVGEXPORTER_H
