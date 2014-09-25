#ifndef _JSON_CXX_H_
#define _JSON_CXX_H_

extern "C" {
#include "json.h"
}

#include <vector>
#include <string>
#include <map>

#define JSON_ASSERT_MESSAGE 

namespace Json {

    enum ValueType
    {
        unknowValue = -1,
        nullValue = 0, ///< 'null' value
        intValue,      ///< signed integer value
        uintValue,     ///< unsigned integer value
        realValue,     ///< double value
        stringValue,   ///< string value
        booleanValue,  ///< bool value
        arrayValue,    ///< array value (ordered list)
        objectValue    ///< object value (collection of name/value pairs).
    };

    class Value
    {
    public:
        Value();
        Value( json_object *obj );
        ~Value();

        void SetObj( json_object *obj );

        int asInt() const;
        double asDouble() const;
        bool asBool() const;
        const char *asString();
        const char *asCString();

        bool isNull() const;
        bool isBool() const;
        bool isInt() const;
        bool isDouble() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;

        /// Number of values in array or object
        unsigned int size() const;

        /// \brief Return true if empty array, empty object, or null;
        /// otherwise, false.
        bool empty() const;

        /// Remove all object members and array elements.
        void clear();

        ValueType type() const;

        bool isMember( const std::string &sKey ) const;

        Value operator[]( const std::string &key );
        Value operator[]( const unsigned int i );

        std::vector<std::string> getAllKeys();

    public:
        std::string m_strErrMsg;

    protected:
        ValueType init_type() const;

        json_object *m_objValue;
        ValueType m_eType;
        std::vector<std::string> m_allKeys;
        std::map<std::string, json_object*> m_valueMap;
    };

    class Reader
    {
    public:
        Reader();

        bool parse(const std::string &document, Value &root);
    protected:

    };

}

#endif
