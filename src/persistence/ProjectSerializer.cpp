#include "ProjectSerializer.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace VectorForge {

std::string ProjectSerializer::serializeToString(const Document& doc) {
    QJsonObject docJson = doc.toJson();
    JsonValue jsonValue = JsonValue::fromQJsonObject(docJson);
    return jsonValue.serialize(2);
}

bool ProjectSerializer::deserializeFromString(Document& doc, const std::string& jsonString) {
    try {
        JsonLexer lexer(jsonString);
        auto tokens = lexer.tokenize();

        JsonParser parser(tokens);
        JsonValue ast = parser.parse();

        QJsonObject qobj = ast.toQJsonObject();
        return doc.fromJson(qobj);
    } catch (const std::exception& e) {
        std::cerr << "[ProjectSerializer] Parsing error: " << e.what() << std::endl;
        return false;
    }
}

bool ProjectSerializer::saveToFile(const Document& doc, const std::string& filePath) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "[ProjectSerializer] Failed to open file for writing: " << filePath << std::endl;
        return false;
    }

    std::string jsonStr = serializeToString(doc);
    outFile << jsonStr;
    outFile.close();
    return true;
}

bool ProjectSerializer::loadFromFile(Document& doc, const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "[ProjectSerializer] Failed to open file for reading: " << filePath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    inFile.close();

    return deserializeFromString(doc, buffer.str());
}

} // namespace VectorForge
