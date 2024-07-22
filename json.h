// json.h
#ifndef JSON_H
#define JSON_H

#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <stdbool.h>

#ifdef JSON_DEBUG_MODE
#define _jdbg_print(_fmt, ...) printf(_fmt, __VA_ARGS__)
#else
#define _jdbg_print(_fmt, ...)
#endif

#define JSON_MAX_POOL_SIZE 16384
#define JSON_MAX_PROPERTIES 100
#define JSON_MAX_ARRAY_ELEMENTS 100

#define JVALUE_GET(_JVal,_Type) _JVal##.V.##_Type##_value
#define JVALUE_REF_GET(_JVal,_Type) _JVal##->V.##_Type##_value

typedef struct _S_JMemoryPool {
    char data[JSON_MAX_POOL_SIZE];
    size_t used;
} JMemoryPool;

typedef struct _S_JPoolManager {
    JMemoryPool*    pools;
    size_t          pool_count;
    size_t          current_pool;
} JPoolManager;

typedef enum _E_JValueType {
    JSON_VALUE_TYPE_STRING,
    JSON_VALUE_TYPE_NULL,
    JSON_VALUE_TYPE_BOOLEAN,
    JSON_VALUE_TYPE_INTEGER,
    JSON_VALUE_TYPE_REAL,
    JSON_VALUE_TYPE_ARRAY,
    JSON_VALUE_TYPE_OBJECT
} JValueType;

typedef struct _S_JValue {
    JValueType T;
    union {
        char* string_value;
        bool boolean_value;
        int64_t integer_value;
        double real_value;
        struct _S_JArray* array_value;
        struct _S_JObject* object_value;
    } V;
} JValue;

typedef struct _S_JProperty {
    char* key;
    JValue value;
} JProperty;

typedef struct _S_JObject {
    JProperty properties[JSON_MAX_PROPERTIES];
    size_t property_count;
} JObject;

typedef struct _S_JArray {
    JValue elements[JSON_MAX_ARRAY_ELEMENTS];
    size_t element_count;
} JArray;

void    json_pool_manager_init(JPoolManager* manager, size_t pool_count);
void*   json_pool_alloc(JPoolManager* manager, size_t size);
int     json_pool_manager_free_pool(JPoolManager* manager, size_t index);
void    json_pool_manager_free_pools(JPoolManager* manager);
void    json_skip_whitespace(const char** str);
char*   json_parse_string(JPoolManager* manager, const char** str);
int     json_parse_null(const char** str);
int     json_parse_bool(const char** str, bool* value);
int     json_parse_int(const char** str, int64_t* value);
int     json_parse_float(const char** str, double* value);
int     json_parse_property(JPoolManager* manager, JObject* obj, const char** str);
int     json_parse_object(JPoolManager* manager, JObject* obj, const char** str);
int     json_parse_array(JPoolManager* manager, JArray* array, const char** str);
int     json_parse_value(JPoolManager* manager, JValue* value, const char** str);
void    json_serialize_object(FILE* file, JObject* obj);
void    json_serialize_array(FILE* file, JArray* array);
void    json_serialize_value(FILE* file, JValue* value);

#endif // JSON_H