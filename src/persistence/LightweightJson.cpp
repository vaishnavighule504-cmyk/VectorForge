#include "LightweightJson.h"
#include <cctype>
#include <sstream>
#include <iomanip>

namespace VectorForge {

// Static dummy empty value for out-of-bounds indexing
static const JsonValue s_nullValue;

JsonValue& JsonValue::operator[](const std::string& key) {
    if (m_type != Type::Object) {
        m_type = Type::Object;
        m_object.clear();
    }
    return m_object[key];
}

const JsonValue& JsonValue::operator[](const std::string& key) const {
    if (m_type == Type::Object) {
        auto it = m_object.find(key);
        if (it != m_object.end()) {
            return it->second;
        }
    }
    return s_nullValue;
}

QJsonObject JsonValue::toQJsonObject() const {
    QJsonObject obj;
    if (isObject()) {
        for (const auto& [k, v] : m_object) {
            obj[QString::fromStdString(k)] = v.toQJsonValue();
        }
    }
    return obj;
}

JsonValue JsonValue::fromQJsonObject(const QJsonObject& qobj) {
    ObjectType obj;
    for (auto it = qobj.begin(); it != qobj.end(); ++it) {
        obj[it.key().toStdString()] = fromQJsonValue(it.value());
    }
    return JsonValue(obj);
}

QJsonArray JsonValue::toQJsonArray() const {
    QJsonArray arr;
    if (isArray()) {
        for (const auto& v : m_array) {
            arr.append(v.toQJsonValue());
        }
    }
    return arr;
}

JsonValue JsonValue::fromQJsonArray(const QJsonArray& qarr) {
    ArrayType arr;
    for (const auto& val : qarr) {
        arr.push_back(fromQJsonValue(val));
    }
    return JsonValue(arr);
}

QJsonValue JsonValue::toQJsonValue() const {
    switch (m_type) {
        case Type::Null: return QJsonValue(QJsonValue::Null);
        case Type::Boolean: return QJsonValue(m_bool);
        case Type::Number: return QJsonValue(m_number);
        case Type::String: return QJsonValue(QString::fromStdString(m_string));
        case Type::Array: return QJsonValue(toQJsonArray());
        case Type::Object: return QJsonValue(toQJsonObject());
    }
    return QJsonValue(QJsonValue::Null);
}

JsonValue JsonValue::fromQJsonValue(const QJsonValue& qval) {
    if (qval.isNull()) return JsonValue();
    if (qval.isBool()) return JsonValue(qval.toBool());
    if (qval.isDouble()) return JsonValue(qval.toDouble());
    if (qval.isString()) return JsonValue(qval.toString().toStdString());
    if (qval.isArray()) return fromQJsonArray(qval.toArray());
    if (qval.isObject()) return fromQJsonObject(qval.toObject());
    return JsonValue();
}

std::string JsonValue::serialize(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::ostringstream ss;

    switch (m_type) {
        case Type::Null:
            return "null";
        case Type::Boolean:
            return m_bool ? "true" : "false";
        case Type::Number: {
            ss << std::setprecision(10) << m_number;
            return ss.str();
        }
        case Type::String: {
            std::string escaped = "\"";
            for (char c : m_string) {
                if (c == '"') escaped += "\\\"";
                else if (c == '\\') escaped += "\\\\";
                else if (c == '\n') escaped += "\\n";
                else if (c == '\r') escaped += "\\r";
                else if (c == '\t') escaped += "\\t";
                else escaped += c;
            }
            escaped += "\"";
            return escaped;
        }
        case Type::Array: {
            if (m_array.empty()) return "[]";
            ss << "[\n";
            for (size_t i = 0; i < m_array.size(); ++i) {
                ss << std::string((indent + 1) * 2, ' ') << m_array[i].serialize(indent + 1);
                if (i + 1 < m_array.size()) ss << ",";
                ss << "\n";
            }
            ss << indentStr << "]";
            return ss.str();
        }
        case Type::Object: {
            if (m_object.empty()) return "{}";
            ss << "{\n";
            size_t count = 0;
            for (const auto& [key, val] : m_object) {
                ss << std::string((indent + 1) * 2, ' ') << "\"" << key << "\": " << val.serialize(indent + 1);
                if (++count < m_object.size()) ss << ",";
                ss << "\n";
            }
            ss << indentStr << "}";
            return ss.str();
        }
    }
    return "null";
}

// ============================================================================
// JsonLexer Implementation
// ============================================================================

JsonLexer::JsonLexer(const std::string& input) : m_input(input) {}

void JsonLexer::skipWhitespace() {
    while (m_cursor < m_input.length()) {
        char c = m_input[m_cursor];
        if (c == ' ' || c == '\t' || c == '\r') {
            m_cursor++;
            m_column++;
        } else if (c == '\n') {
            m_cursor++;
            m_line++;
            m_column = 1;
        } else {
            break;
        }
    }
}

JsonToken JsonLexer::lexString() {
    int startCol = m_column;
    m_cursor++; // Skip open quote
    m_column++;

    std::string text = "";
    bool escaped = false;

    while (m_cursor < m_input.length()) {
        char c = m_input[m_cursor++];
        m_column++;

        if (escaped) {
            if (c == '"') text += '"';
            else if (c == '\\') text += '\\';
            else if (c == '/') text += '/';
            else if (c == 'n') text += '\n';
            else if (c == 'r') text += '\r';
            else if (c == 't') text += '\t';
            else text += c;
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (c == '"') {
            return {JsonTokenType::String, text, m_line, startCol};
        } else {
            text += c;
        }
    }

    throw std::runtime_error("Unterminated string literal at line " + std::to_string(m_line));
}

JsonToken JsonLexer::lexNumber() {
    int startCol = m_column;
    size_t startPos = m_cursor;

    if (m_input[m_cursor] == '-') {
        m_cursor++;
        m_column++;
    }

    while (m_cursor < m_input.length() && (std::isdigit(m_input[m_cursor]) || m_input[m_cursor] == '.' || m_input[m_cursor] == 'e' || m_input[m_cursor] == 'E' || m_input[m_cursor] == '+' || m_input[m_cursor] == '-')) {
        m_cursor++;
        m_column++;
    }

    std::string numStr = m_input.substr(startPos, m_cursor - startPos);
    return {JsonTokenType::Number, numStr, m_line, startCol};
}

JsonToken JsonLexer::lexKeyword() {
    int startCol = m_column;
    size_t startPos = m_cursor;

    while (m_cursor < m_input.length() && std::isalpha(m_input[m_cursor])) {
        m_cursor++;
        m_column++;
    }

    std::string kw = m_input.substr(startPos, m_cursor - startPos);
    if (kw == "true") return {JsonTokenType::BooleanTrue, kw, m_line, startCol};
    if (kw == "false") return {JsonTokenType::BooleanFalse, kw, m_line, startCol};
    if (kw == "null") return {JsonTokenType::NullValue, kw, m_line, startCol};

    throw std::runtime_error("Unknown keyword '" + kw + "' at line " + std::to_string(m_line));
}

std::vector<JsonToken> JsonLexer::tokenize() {
    std::vector<JsonToken> tokens;

    while (m_cursor < m_input.length()) {
        skipWhitespace();
        if (m_cursor >= m_input.length()) break;

        char c = m_input[m_cursor];

        if (c == '{') {
            tokens.push_back({JsonTokenType::LeftBrace, "{", m_line, m_column++});
            m_cursor++;
        } else if (c == '}') {
            tokens.push_back({JsonTokenType::RightBrace, "}", m_line, m_column++});
            m_cursor++;
        } else if (c == '[') {
            tokens.push_back({JsonTokenType::LeftBracket, "[", m_line, m_column++});
            m_cursor++;
        } else if (c == ']') {
            tokens.push_back({JsonTokenType::RightBracket, "]", m_line, m_column++});
            m_cursor++;
        } else if (c == ':') {
            tokens.push_back({JsonTokenType::Colon, ":", m_line, m_column++});
            m_cursor++;
        } else if (c == ',') {
            tokens.push_back({JsonTokenType::Comma, ",", m_line, m_column++});
            m_cursor++;
        } else if (c == '"') {
            tokens.push_back(lexString());
        } else if (std::isdigit(c) || c == '-') {
            tokens.push_back(lexNumber());
        } else if (std::isalpha(c)) {
            tokens.push_back(lexKeyword());
        } else {
            throw std::runtime_error("Unexpected character '" + std::string(1, c) + "' at line " + std::to_string(m_line));
        }
    }

    tokens.push_back({JsonTokenType::EndOfFile, "", m_line, m_column});
    return tokens;
}

// ============================================================================
// JsonParser Implementation
// ============================================================================

JsonParser::JsonParser(const std::vector<JsonToken>& tokens) : m_tokens(tokens) {}

const JsonToken& JsonParser::peek() const {
    if (m_cursor < m_tokens.size()) {
        return m_tokens[m_cursor];
    }
    return m_tokens.back();
}

JsonToken JsonParser::advance() {
    const JsonToken& token = peek();
    if (m_cursor < m_tokens.size()) {
        m_cursor++;
    }
    return token;
}

bool JsonParser::match(JsonTokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

void JsonParser::expect(JsonTokenType type, const std::string& errorMessage) {
    if (!match(type)) {
        const auto& tok = peek();
        throw std::runtime_error(errorMessage + " at line " + std::to_string(tok.line) + ", col " + std::to_string(tok.column));
    }
}

JsonValue JsonParser::parseObject() {
    JsonValue::ObjectType obj;

    if (match(JsonTokenType::RightBrace)) {
        return JsonValue(obj);
    }

    while (true) {
        const auto& keyToken = peek();
        if (keyToken.type != JsonTokenType::String) {
            throw std::runtime_error("Expected object string key at line " + std::to_string(keyToken.line));
        }
        advance();
        std::string key = keyToken.text;

        expect(JsonTokenType::Colon, "Expected ':' after key in object");

        JsonValue value = parseValue();
        obj[key] = value;

        if (match(JsonTokenType::RightBrace)) {
            break;
        }

        expect(JsonTokenType::Comma, "Expected ',' or '}' in object");
    }

    return JsonValue(obj);
}

JsonValue JsonParser::parseArray() {
    JsonValue::ArrayType arr;

    if (match(JsonTokenType::RightBracket)) {
        return JsonValue(arr);
    }

    while (true) {
        arr.push_back(parseValue());

        if (match(JsonTokenType::RightBracket)) {
            break;
        }

        expect(JsonTokenType::Comma, "Expected ',' or ']' in array");
    }

    return JsonValue(arr);
}

JsonValue JsonParser::parseValue() {
    const auto& tok = peek();

    switch (tok.type) {
        case JsonTokenType::LeftBrace:
            advance();
            return parseObject();
        case JsonTokenType::LeftBracket:
            advance();
            return parseArray();
        case JsonTokenType::String:
            advance();
            return JsonValue(tok.text);
        case JsonTokenType::Number:
            advance();
            return JsonValue(std::stod(tok.text));
        case JsonTokenType::BooleanTrue:
            advance();
            return JsonValue(true);
        case JsonTokenType::BooleanFalse:
            advance();
            return JsonValue(false);
        case JsonTokenType::NullValue:
            advance();
            return JsonValue();
        default:
            throw std::runtime_error("Unexpected token '" + tok.text + "' at line " + std::to_string(tok.line));
    }
}

JsonValue JsonParser::parse() {
    return parseValue();
}

} // namespace VectorForge
