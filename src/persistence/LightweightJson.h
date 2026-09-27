#ifndef VECTORFORGE_LIGHTWEIGHTJSON_H
#define VECTORFORGE_LIGHTWEIGHTJSON_H

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <stdexcept>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace VectorForge {

/**
 * @brief Custom lightweight JSON AST representation.
 * 
 * Hand-crafted to demonstrate parsing, tokenization, and recursion fundamentals
 * without relying on black-box external JSON parsing libraries.
 */
class JsonValue {
public:
    enum class Type {
        Null,
        Boolean,
        Number,
        String,
        Array,
        Object
    };

    using ObjectType = std::map<std::string, JsonValue>;
    using ArrayType = std::vector<JsonValue>;

    JsonValue() : m_type(Type::Null) {}
    JsonValue(bool val) : m_type(Type::Boolean), m_bool(val) {}
    JsonValue(double val) : m_type(Type::Number), m_number(val) {}
    JsonValue(int val) : m_type(Type::Number), m_number(static_cast<double>(val)) {}
    JsonValue(const std::string& val) : m_type(Type::String), m_string(val) {}
    JsonValue(const char* val) : m_type(Type::String), m_string(val) {}
    JsonValue(const ArrayType& val) : m_type(Type::Array), m_array(val) {}
    JsonValue(const ObjectType& val) : m_type(Type::Object), m_object(val) {}

    Type type() const { return m_type; }

    bool isNull() const { return m_type == Type::Null; }
    bool isBool() const { return m_type == Type::Boolean; }
    bool isNumber() const { return m_type == Type::Number; }
    bool isString() const { return m_type == Type::String; }
    bool isArray() const { return m_type == Type::Array; }
    bool isObject() const { return m_type == Type::Object; }

    bool asBool(bool defaultVal = false) const { return isBool() ? m_bool : defaultVal; }
    double asNumber(double defaultVal = 0.0) const { return isNumber() ? m_number : defaultVal; }
    std::string asString(const std::string& defaultVal = "") const { return isString() ? m_string : defaultVal; }
    const ArrayType& asArray() const { return m_array; }
    const ObjectType& asObject() const { return m_object; }

    ArrayType& asArray() { return m_array; }
    ObjectType& asObject() { return m_object; }

    // Key indexing for Objects
    JsonValue& operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;

    // Conversion to and from Qt JSON classes for seamless UI/Model integration
    QJsonObject toQJsonObject() const;
    static JsonValue fromQJsonObject(const QJsonObject& qobj);

    QJsonArray toQJsonArray() const;
    static JsonValue fromQJsonArray(const QJsonArray& qarr);

    QJsonValue toQJsonValue() const;
    static JsonValue fromQJsonValue(const QJsonValue& qval);

    // Serialization
    std::string serialize(int indent = 0) const;

private:
    Type m_type{Type::Null};
    bool m_bool{false};
    double m_number{0.0};
    std::string m_string;
    ArrayType m_array;
    ObjectType m_object;
};

/**
 * @brief TokenType enumeration for Lexer.
 */
enum class JsonTokenType {
    LeftBrace,      // {
    RightBrace,     // }
    LeftBracket,    // [
    RightBracket,   // ]
    Colon,          // :
    Comma,          // ,
    String,         // "text"
    Number,         // 123.45
    BooleanTrue,    // true
    BooleanFalse,   // false
    NullValue,      // null
    EndOfFile
};

struct JsonToken {
    JsonTokenType type;
    std::string text;
    int line{1};
    int column{1};
};

/**
 * @brief Hand-crafted Lexer tokenizer for JSON strings.
 */
class JsonLexer {
public:
    explicit JsonLexer(const std::string& input);
    std::vector<JsonToken> tokenize();

private:
    void skipWhitespace();
    JsonToken lexString();
    JsonToken lexNumber();
    JsonToken lexKeyword();

    std::string m_input;
    size_t m_cursor{0};
    int m_line{1};
    int m_column{1};
};

/**
 * @brief Hand-crafted Recursive Descent Parser for JSON tokens.
 */
class JsonParser {
public:
    explicit JsonParser(const std::vector<JsonToken>& tokens);
    JsonValue parse();

private:
    JsonValue parseValue();
    JsonValue parseObject();
    JsonValue parseArray();

    const JsonToken& peek() const;
    JsonToken advance();
    bool match(JsonTokenType type);
    void expect(JsonTokenType type, const std::string& errorMessage);

    std::vector<JsonToken> m_tokens;
    size_t m_cursor{0};
};

} // namespace VectorForge

#endif // VECTORFORGE_LIGHTWEIGHTJSON_H
