#include "ProjectSerializer.h"
#include <QFile>
#include <QJsonDocument>

namespace VectorForge {

bool ProjectSerializer::saveToFile(const Document& doc, const QString& filePath) {
    QJsonObject json = doc.toJson();
    json["version"] = "1.0.0";
    json["app"] = "VectorForge";

    QJsonDocument jsonDoc(json);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool ProjectSerializer::loadFromFile(Document& doc, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        return false;
    }

    return doc.fromJson(jsonDoc.object());
}

} // namespace VectorForge
