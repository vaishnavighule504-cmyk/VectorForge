#ifndef VECTORFORGE_PROJECTSERIALIZER_H
#define VECTORFORGE_PROJECTSERIALIZER_H

#include "../model/Document.h"
#include "LightweightJson.h"
#include <string>

namespace VectorForge {

/**
 * @brief Handles saving and loading VectorForge design projects to disk.
 * 
 * Uses raw C++ std::ofstream and std::ifstream streams coupled with the custom
 * LightweightJson lexer/parser engine to demonstrate I/O and data modeling.
 */
class ProjectSerializer {
public:
    static bool saveToFile(const Document& doc, const std::string& filePath);
    static bool loadFromFile(Document& doc, const std::string& filePath);

    static std::string serializeToString(const Document& doc);
    static bool deserializeFromString(Document& doc, const std::string& jsonString);
};

} // namespace VectorForge

#endif // VECTORFORGE_PROJECTSERIALIZER_H
