#ifndef VECTORFORGE_PROJECTSERIALIZER_H
#define VECTORFORGE_PROJECTSERIALIZER_H

#include "../core/Document.h"
#include <QString>

namespace VectorForge {

class ProjectSerializer {
public:
    static bool saveToFile(const Document& doc, const QString& filePath);
    static bool loadFromFile(Document& doc, const QString& filePath);
};

} // namespace VectorForge

#endif // VECTORFORGE_PROJECTSERIALIZER_H
