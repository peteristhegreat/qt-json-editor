#include "jsoncxx.h"
#include <cstdio>
#include <cstring>

namespace Json {

    Reader::Reader()
    {
    }

    bool Reader::parse( const std::string &document, Value &root )
    {
        json_object *obj = json_tokener_parse( document.data() );

        if( is_error(obj) )
            return false;

        root.SetObj( obj );

        return true;
    }

    Value::Value()
    {
        m_objValue = NULL;
        m_eType = unknowValue;
    }

    Value::Value( json_object *obj )
    {
        m_objValue = json_object_get( obj );
        m_eType = init_type();

        if (m_eType == objectValue)
        {
            json_object_object_foreach( m_objValue, key, value ){
                m_allKeys.push_back(key);
                m_valueMap.insert(std::pair<std::string, json_object*>(key, value));
            }
        }
    }

    Value::~Value()
    {
        json_object_put( m_objValue );
    }

    void Value::SetObj( json_object *obj )
    {
        json_object_put( m_objValue );

        m_objValue = json_object_get( obj );
        m_eType = init_type();

        if (m_eType == objectValue)
        {
            json_object_object_foreach( m_objValue, key, value ){
                m_allKeys.push_back(key);
                m_valueMap.insert(std::pair<std::string, json_object*>(key, value));
            }
        }
    }

    ValueType Value::init_type() const
    {
        json_type type = json_object_get_type( m_objValue );
        switch( type )
        {
        case json_type_null:
            return nullValue;
        case json_type_boolean:
            return booleanValue;
        case json_type_double:
            return realValue;
        case json_type_int:
            return intValue;
        case json_type_object:
            return objectValue;
        case json_type_array:
            return arrayValue;
        case json_type_string:
            return stringValue;
        default:
            return nullValue;
        }
        return nullValue;
    }

    ValueType Value::type() const
    {
        if( m_eType == unknowValue )
            return init_type();
        return m_eType;
    }

    int Value::asInt() const
    {
        switch ( m_eType )
        {
        case nullValue:
            return 0;
        case intValue:
            return json_object_get_int(m_objValue);
        case uintValue:
            return json_object_get_int(m_objValue);
        case realValue:
            return (int)json_object_get_double(m_objValue);
        case booleanValue:
            return json_object_get_boolean(m_objValue)? 1 : 0;
        case stringValue:
        case arrayValue:
        case objectValue:
            //JSON_ASSERT_MESSAGE( false, "Type is not convertible to int" );
        default:
            break;
        }
        return 0; // unreachable;
    }

    const char *Value::asString()
    {
        char* str = new char[500];
        switch ( m_eType )
        {
        case nullValue:
            strcpy(str, "Null");
            break;
        case stringValue:
            strcpy(str, json_object_get_string(m_objValue));
            break;
        case intValue:
        case uintValue:
            sprintf(str, "%i", json_object_get_int(m_objValue));
            break;
        case realValue:
            sprintf(str, "%lf", json_object_get_double(m_objValue));
            break;
        case booleanValue:
            json_object_get_boolean(m_objValue) == true ? strcpy(str, "true") : strcpy(str, "false");
            break;
        case arrayValue:
            //strcpy(str, "[array]");
            strcpy(str, "");
            break;
        case objectValue:
            //strcpy(str, "");
            strcpy(str, "");
            break;
        default:
            return "[null]";
            break;
        }
        return str;
    }

    const char *Value::asCString()
    {
        switch ( m_eType )
        {
        case nullValue:
            return "";
        case stringValue:
            return json_object_get_string(m_objValue);
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
        case arrayValue:
        case objectValue:
        default:
            return "";
            break;
        }
        return "";
    }

    double Value::asDouble() const
    {
        switch ( m_eType )
        {
        case nullValue:
            return 0.0;
        case intValue:
        case uintValue:
        case realValue:
            return json_object_get_double(m_objValue);
        case booleanValue:
            return (int)json_object_get_double(m_objValue) ? 1.0 : 0.0;
        case stringValue:
        case arrayValue:
        case objectValue:
            //JSON_ASSERT_MESSAGE( false, "Type is not convertible to double" );
        default:
            return 0.0;
        }
        return 0; // unreachable;
    }

    bool Value::asBool() const
    {
        switch ( m_eType )
        {
        case nullValue:
            return false;
        case intValue:
        case uintValue:
            return json_object_get_int(m_objValue) != 0;
        case realValue:
            return json_object_get_double(m_objValue) != 0.0;
        case booleanValue:
            return json_object_get_boolean(m_objValue)?true:false;
        case stringValue:
            return json_object_get_string(m_objValue)  &&  json_object_get_string(m_objValue)[0] != 0;
		case arrayValue:
		case objectValue:
            return this->size() != 0;
		default:
            break;
        }
        return false; // unreachable;
    }

    unsigned int Value::size() const
    {
        switch ( m_eType )
        {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
        case stringValue:
            return 0;
        case arrayValue:
            return json_object_array_length(m_objValue);
        case objectValue:
            return m_allKeys.size();
        default:
            break;
        }
        return 0;
    }

    Value Value::operator[]( const std::string &sKey )
    {
        /*
        json_object_object_foreach( m_objValue, key, value ){
            if( sKey == key )
            {
                return Value(value);
            }
        }

        return Value();
        */
        return Value(m_valueMap[sKey]);
    }

    Value Value::operator[]( const unsigned int i )
    {
        return Value( json_object_array_get_idx( m_objValue, i ) );
    }

    bool Value::isMember( const std::string &sKey ) const
    {
        json_object_object_foreach( m_objValue, key, value ){
            if( sKey == key )
            {
                return true;
            }
        }
        return false;
    }

    bool Value::isInt() const
    {
        return true;
    }
    bool Value::isString() const
    {
        return true;
    }
    bool Value::isObject() const
    {
        return true;
    }
    void Value::clear()
    {
        /*
        if (m_allKeys.size() > 0)
        {
            json_object_object_foreach( m_objValue, key, value )
            {
                json_object_object_del(value, key);
            }
        }
        */

        m_allKeys.clear();
        m_valueMap.clear();
        delete m_objValue;
        m_objValue = NULL;
        m_eType = unknowValue;
    }

    std::vector<std::string> Value::getAllKeys()
    {
        return m_allKeys;
    }
}

